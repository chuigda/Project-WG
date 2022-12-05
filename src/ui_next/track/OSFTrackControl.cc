#include "TrackControlImpl.h"

#include <QLabel>
#include <QGroupBox>
#include <QBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QUdpSocket>
#include <QMessageBox>
#include <QNetworkDatagram>
#include <QSpinBox>
#include "util/Derive.h"
#include "util/CircularBuffer.h"

class OSFTrackWorker : public QObject {
Q_OBJECT

public:
  explicit OSFTrackWorker(QObject *parent = nullptr)
    : QObject(parent),
      m_Socket(nullptr),
      m_Parameter(),
      m_SmoothBuffer { wgc0310::HeadStatus {} }
  {}

  CW_DERIVE_UNCOPYABLE(OSFTrackWorker)
  CW_DERIVE_UNMOVABLE(OSFTrackWorker)

signals:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
  void HeadPoseUpdated(wgc0310::HeadStatus headPose);
  void TrackingError(QString const& reason);
#pragma clang diagnostic pop

public slots:
  void StartListening(std::uint16_t port) {
    if (m_Socket) {
      emit TrackingError("已经有一个监听任务了\r\n"
                         "请先停止监听");
      return;
    }

    m_Socket = new QUdpSocket();
    if (!m_Socket->bind(QHostAddress::LocalHost, port)) {
      delete m_Socket;
      emit TrackingError("无法绑定到指定端口");
      return;
    }

    connect(m_Socket, &QUdpSocket::readyRead,
            this, &OSFTrackWorker::HandleData);
  }

  void StopListening() {
    if (!m_Socket) {
      return;
    }

    // maybe unnecessary, but I'm not really sure
    disconnect(m_Socket, &QUdpSocket::readyRead,
               this, &OSFTrackWorker::HandleData);

    delete m_Socket;
    m_Socket = nullptr;
  }

  void SetParameter(OSFTrackParameter2 parameter) {
    m_Parameter = parameter;
  }

private slots:
  void HandleData();

private:
  QUdpSocket *m_Socket;
  OSFTrackParameter2 m_Parameter;
  cw::CircularBuffer<wgc0310::HeadStatus, 128> m_SmoothBuffer;
};

// Packet 的格式定义在这里：
// https://github.com/emilianavt/OpenSeeFace/blob/40119c17971c019b892b047b457c8182190acb8c/facetracker.py#L276
// https://github.com/emilianavt/OpenSeeFace/blob/40119c17971c019b892b047b457c8182190acb8c/Unity/OpenSee.cs#L19
// 因为这个程序只在 x86_64 架构上运行，所以无视 alignment 问题是可以的
struct FacePacket {
  // 8
  double now;

  // 4
  int id;

  // 2 * 4
  float width;
  float height;

  // 2 * 4
  float eyeBlinkRight;
  float eyeBlinkLeft;

  // 1
  uint8_t success;

  // 4
  float pnpError;

  // 4 * 4
  std::array<float, 4> quaternion;

  // 3 * 4
  std::array<float, 3> euler;

  // 3 * 4
  std::array<float, 3> translation;

  // 68 * 4
  std::array<float, 68> lms_confidence;

  // 68 * 2 * 4
  std::array<std::array<float, 2>, 68> lms;

  // 70 * 4 * 3
  std::array<std::array<float, 3>, 70> pnpPoints;

  // 14 * 4
  float eyeLeft;
  float eyeRight;

  float eyeSteepnessLeft;
  float eyeUpDownLeft;
  float eyeQuirkLeft;

  float eyeSteepnessRight;
  float eyeUpDownRight;
  float eyeQuirkRight;

  float mouthCornerUpdownLeft;
  float mouthCornerInOutLeft;
  float mouthCornerUpdownRight;
  float mouthCornerInOutRight;

  float mouthOpen;
  float mouthWide;
} __attribute__((packed));

static_assert(sizeof(FacePacket) == 1785);

static void DownscaleToDeadZone(float &value, float deadZone) {
  if (value > deadZone) {
    value = deadZone;
  } else if (value < -deadZone) {
    value = -deadZone;
  }
}

void OSFTrackWorker::HandleData() {
  while (m_Socket->hasPendingDatagrams()) {
    QNetworkDatagram datagram = m_Socket->receiveDatagram();

    // Our story starts here
    QByteArray data = datagram.data();

    if (static_cast<std::size_t>(data.length()) < sizeof(FacePacket)) {
      // too short, ignore this packet.
      continue;
    }

    FacePacket const* facePacket =
      reinterpret_cast<FacePacket const*>(data.data());

    qDebug() << "rawData: rx =" << facePacket->euler[0]
             << "ry =" << facePacket->euler[1]
             << "rz =" << facePacket->euler[2]
             << "blinkL =" << facePacket->eyeQuirkLeft
             << "blinkR =" << facePacket->eyeQuirkRight
             << "mouthOpen =" << facePacket->mouthOpen
             << "mouthWide =" << facePacket->mouthWide;

    wgc0310::HeadStatus pose {
      .rotationX = facePacket->euler[0],
      .rotationY = facePacket->euler[1],
      .rotationZ = facePacket->euler[2],
      .mouthStatus = facePacket->mouthOpen > 0.05f ?
                     wgc0310::HeadStatus::MouthStatus::Open :
                     wgc0310::HeadStatus::MouthStatus::Close
    };

    if (pose.rotationX > 0)  {
      pose.rotationX = pose.rotationX - 180.0f;
    } else {
      pose.rotationX = pose.rotationX + 180.0f;
    }
    pose.rotationY = -pose.rotationY;
    pose.rotationZ -= 90.0f;

    pose.rotationX += m_Parameter.xRotationFix;
    pose.rotationY += m_Parameter.yRotationFix;
    pose.rotationZ += m_Parameter.zRotationFix;

    DownscaleToDeadZone(pose.rotationX, 30.0f);
    DownscaleToDeadZone(pose.rotationY, 15.0f);
    DownscaleToDeadZone(pose.rotationZ, 30.0f);

    while (m_SmoothBuffer.Size() > m_Parameter.smoothSteps) {
      m_SmoothBuffer.PopFront();
    }
    m_SmoothBuffer.PopFront();
    m_SmoothBuffer.PushBack(pose);
  }

  float xSum = 0.0f;
  float ySum = 0.0f;
  float zSum = 0.0f;
  int mouthStatus = 0;

  for (std::size_t i = 0; i < m_SmoothBuffer.Size(); i++) {
    wgc0310::HeadStatus status = m_SmoothBuffer.Get(i);
    xSum += status.rotationX;
    ySum += status.rotationY;
    zSum += status.rotationZ;
    mouthStatus += static_cast<int>(status.mouthStatus);
  }

  xSum /= static_cast<float>(m_SmoothBuffer.Size());
  ySum /= static_cast<float>(m_SmoothBuffer.Size());
  zSum /= static_cast<float>(m_SmoothBuffer.Size());
  mouthStatus /= static_cast<int>(m_SmoothBuffer.Size());

  emit HeadPoseUpdated(wgc0310::HeadStatus {
    xSum, ySum, zSum,
    mouthStatus > 0 ? wgc0310::HeadStatus::MouthStatus::Open
                    : wgc0310::HeadStatus::MouthStatus::Close
  });
}

static QDoubleSpinBox *createAngleSpinBox() {
  QDoubleSpinBox *ret = new QDoubleSpinBox();
  ret->setValue(0.0);
  ret->setMinimum(-45.0);
  ret->setMaximum(45.0);
  return ret;
}

OSFTrackControl::OSFTrackControl(wgc0310::HeadStatus *headStatus,
                                 QThread *workerThread,
                                 QWidget *parent)
  : QWidget(parent),
    m_HeadStatus(headStatus),
    m_WorkerThread(workerThread)
{
  OSFTrackWorker *worker = new OSFTrackWorker();
  worker->moveToThread(workerThread);

  connect(this, &OSFTrackControl::StartTracking,
          worker, &OSFTrackWorker::StartListening);
  connect(this, &OSFTrackControl::StopTracking,
          worker, &OSFTrackWorker::StopListening);
  connect(this, &OSFTrackControl::SetParameters,
          worker, &OSFTrackWorker::SetParameter);
  connect(worker, &OSFTrackWorker::TrackingError,
          this, &OSFTrackControl::HandleError);
  connect(worker, &OSFTrackWorker::HeadPoseUpdated,
          this, &OSFTrackControl::HandleHeadStatus);

  QVBoxLayout *layout = new QVBoxLayout(this);

  {
    QLabel *introduction = new QLabel(
      "使用开源的 <a href=\"https://github.com/emilianavt/OpenSeeFace\">OpenSeeFace (OSF)</a>"
      " 进行面部捕捉和姿态估计<br/>"
      "需要启动 OSF 并且通过 UDP 将面捕数据发送给 Project-WG 控制程序"
    );
    introduction->setOpenExternalLinks(true);
    layout->addWidget(introduction);
  }

  {
    QGroupBox *osfUdpSettings = new QGroupBox("OpenSeeFrace 设置");
    layout->addWidget(osfUdpSettings);

    QHBoxLayout *osfLayout = new QHBoxLayout();
    osfUdpSettings->setLayout(osfLayout);

    QLabel *labelUdpAddr = new QLabel("UDP 端口");
    QLineEdit *lineEdit = new QLineEdit("11573");
    QPushButton *startButton = new QPushButton("开始监听");
    QPushButton *stopButton = new QPushButton("停止监听");

    osfLayout->addWidget(labelUdpAddr);
    osfLayout->addStretch();
    osfLayout->addWidget(lineEdit);
    osfLayout->addWidget(startButton);
    osfLayout->addWidget(stopButton);

    connect(
      startButton,
      &QPushButton::clicked,
      this,
      [this, lineEdit] {
        bool success;
        std::uint16_t port = lineEdit->text().toUInt(&success);
        if (!success) {
          QMessageBox::warning(this, "面部捕捉错误", "输入的端口号无效");
        }

        emit this->StartTracking(port);
      }
    );

    connect(stopButton,
            &QPushButton::clicked,
            this,
            &OSFTrackControl::StopTracking);
  }

  {
    QGroupBox *groupBox = new QGroupBox("后处理选项");
    layout->addWidget(groupBox);

    QHBoxLayout *box = new QHBoxLayout();
    groupBox->setLayout(box);

    box->addWidget(new QLabel("平滑"));
    QSpinBox *spinSmooth = new QSpinBox();
    spinSmooth->setValue(8);
    spinSmooth->setMinimum(1);
    spinSmooth->setMaximum(20);
    box->addWidget(spinSmooth);
    box->addStretch();

    box->addWidget(new QLabel("X"));
    QDoubleSpinBox *spinX = createAngleSpinBox();
    box->addWidget(spinX);

    box->addWidget(new QLabel("Y"));
    QDoubleSpinBox *spinY = createAngleSpinBox();
    box->addWidget(spinY);

    box->addWidget(new QLabel("Z"));
    QDoubleSpinBox *spinZ = createAngleSpinBox();
    box->addWidget(spinZ);

    QPushButton *okButton = new QPushButton("设定完了");
    box->addWidget(okButton);

    connect(
      okButton,
      &QPushButton::clicked,
      [this, spinSmooth, spinX, spinY, spinZ] {
        OSFTrackParameter2 parameter;
        parameter.smoothSteps = spinSmooth->value();
        parameter.xRotationFix = static_cast<float>(spinX->value());
        parameter.yRotationFix = static_cast<float>(spinY->value());
        parameter.zRotationFix = static_cast<float>(spinZ->value());

        emit this->SetParameters(parameter);
      }
    );

    layout->addWidget(groupBox);
  }
}

OSFTrackControl::~OSFTrackControl() noexcept {
  emit StopTracking();
}

void OSFTrackControl::HandleError(const QString& error) {
  QMessageBox::warning(this, "OSF 面部捕捉错误", error);
}

void OSFTrackControl::HandleHeadStatus(wgc0310::HeadStatus headStatus) {
  *m_HeadStatus = headStatus;
  qDebug () << headStatus.rotationX
            << headStatus.rotationY
            << headStatus.rotationZ
            << static_cast<int>(headStatus.mouthStatus);
}

#include "OSFTrackControl.moc"
