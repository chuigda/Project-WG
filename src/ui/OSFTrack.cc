#include "ui/OSFTrack.h"

#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QMessageBox>
#include <QBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include <QSpinBox>
#include <QRadioButton>

OSFTrackReceiver::OSFTrackReceiver(QObject *parent)
  : QObject(parent),
    m_Socket(nullptr),
    m_Parameter(),
    m_SmoothBuffer(m_Parameter.smoothSteps)
{}

void OSFTrackReceiver::StartListening(std::uint16_t port) {
  if (m_Socket) {
    emit TrackingError("已经有一个监听任务了\r\n"
                       "请先停止监听");
    return;
  }

  qDebug() << "Start listening";

  m_Socket = new QUdpSocket();
  if (!m_Socket->bind(QHostAddress::LocalHost, port)) {
    delete m_Socket;
    emit TrackingError("无法绑定到指定端口");
    return;
  }

  connect(m_Socket, &QUdpSocket::readyRead,
          this, &OSFTrackReceiver::HandleData);
}

void OSFTrackReceiver::StopListening() {
  if (!m_Socket) {
    return;
  }

  qDebug() << "Stop listening";

  // maybe unnecessary, but I'm not really sure
  disconnect(m_Socket, &QUdpSocket::readyRead,
             this, &OSFTrackReceiver::HandleData);

  delete m_Socket;
  m_Socket = nullptr;
}

void OSFTrackReceiver::SetParameter(OSFTrackParameter parameter) {
  m_Parameter = parameter;
}

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

void OSFTrackReceiver::HandleData() {
  HeadPose::MouthStatus lastMouthStatus = HeadPose::Close;
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

    HeadPose pose {
      .rotationX = facePacket->euler[0],
      .rotationY = facePacket->euler[1],
      .rotationZ = facePacket->euler[2],
    };
    lastMouthStatus = facePacket->mouthOpen > 0.225f ?
          HeadPose::OpenBig :
          facePacket->mouthOpen > 0.1f ?
            HeadPose::Open :
            HeadPose::Close;
    qDebug() << "lastMouthStatus =" << lastMouthStatus;

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

    while (m_SmoothBuffer.size() - 1 > m_Parameter.smoothSteps) {
      m_SmoothBuffer.pop_front();
    }
    m_SmoothBuffer.push_back(pose);
  }

  float xSum = 0.0f;
  float ySum = 0.0f;
  float zSum = 0.0f;
  for (const auto &pose : m_SmoothBuffer) {
    xSum += pose.rotationX;
    ySum += pose.rotationY;
    zSum += pose.rotationZ;
  }

  xSum /= static_cast<float>(m_SmoothBuffer.size());
  ySum /= static_cast<float>(m_SmoothBuffer.size());
  zSum /= static_cast<float>(m_SmoothBuffer.size());

  emit HeadPoseUpdated(HeadPose { xSum, ySum, zSum, lastMouthStatus });
}

static QDoubleSpinBox *createAngleSpinBox() {
  QDoubleSpinBox *ret = new QDoubleSpinBox();
  ret->setValue(0.0);
  ret->setMinimum(-45.0);
  ret->setMaximum(45.0);
  return ret;
}

OSFTrackController::OSFTrackController(FaceTrackStatus *fcs,
                                       QWidget *parent)
  : QWidget(parent),
    m_FaceTrackStatus(fcs)
{
  OSFTrackReceiver *receiver = new OSFTrackReceiver();
  receiver->moveToThread(&m_WorkerThread);

  connect(this, &OSFTrackController::StartListening,
          receiver, &OSFTrackReceiver::StartListening);
  connect(this, &OSFTrackController::StopListening,
          receiver, &OSFTrackReceiver::StopListening);
  connect(this, &OSFTrackController::SetParameter,
          receiver, &OSFTrackReceiver::SetParameter);
  connect(receiver, &OSFTrackReceiver::HeadPoseUpdated,
          this, &OSFTrackController::HandlePoseUpdate);
  connect(receiver, &OSFTrackReceiver::TrackingError,
          this, &OSFTrackController::HandleError);
  m_WorkerThread.start();

  QVBoxLayout *layout = new QVBoxLayout(this);

  {
    QHBoxLayout *box = new QHBoxLayout();
    QLabel *labelUdpAddr = new QLabel("UDP 端口");
    QLineEdit *lineEdit = new QLineEdit("11573");
    QPushButton *startButton = new QPushButton("开始监听");
    QPushButton *stopButton = new QPushButton("停止监听");

    box->addWidget(labelUdpAddr);
    box->addWidget(lineEdit);
    box->addWidget(startButton);
    box->addWidget(stopButton);

    connect(
      startButton,
      &QPushButton::clicked,
      [this, lineEdit] {
      bool success;
      std::uint16_t port = lineEdit->text().toUInt(&success);
      if (!success) {
        QMessageBox::warning(this, "面部捕捉错误", "输入的端口号无效");
      }

      emit this->StartListening(port);
      }
    );

    connect(stopButton,
            &QPushButton::clicked,
            this,
            &OSFTrackController::StopListening);
    connect(
      this,
      &OSFTrackController::StopListening,
      [this] {
        this->m_FaceTrackStatus->pose.mouthStatus = HeadPose::Close;
      }
    );

    layout->addLayout(box);
  }

  {
    QGroupBox *groupBox = new QGroupBox("屏幕显示");
    QHBoxLayout *box = new QHBoxLayout();
    groupBox->setLayout(box);

    QRadioButton *screenCapture = new QRadioButton("面捕捕捉");
    screenCapture->setChecked(true);
    box->addWidget(screenCapture);

    box->addStretch();

    QRadioButton *volumeSignal = new QRadioButton("音频信号");
    box->addWidget(volumeSignal);

    connect(
      screenCapture,
      &QRadioButton::toggled,
      [this] (bool toggled) {
        if (toggled) {
          this->m_FaceTrackStatus->pose.screenControlStatus = HeadPose::Face;
        } else {
          this->m_FaceTrackStatus->pose.screenControlStatus = HeadPose::Soundwave;
        }
      }
    );

    layout->addWidget(groupBox);
  }

  {
    QGroupBox *groupBox = new QGroupBox("后处理选项");
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
        OSFTrackParameter parameter;
        parameter.smoothSteps = spinSmooth->value();
        parameter.xRotationFix = static_cast<float>(spinX->value());
        parameter.yRotationFix = static_cast<float>(spinY->value());
        parameter.zRotationFix = static_cast<float>(spinZ->value());

        emit this->SetParameter(parameter);
      }
    );

    layout->addWidget(groupBox);
  }
}

OSFTrackController::~OSFTrackController() {
  emit StopListening();

  m_WorkerThread.quit();
  m_WorkerThread.wait();
}

void OSFTrackController::HandleError(const QString& reason) {
  QMessageBox::warning(this, "面部捕捉错误", reason);
}

void OSFTrackController::HandlePoseUpdate(HeadPose pose) {
  m_FaceTrackStatus->pose.rotationX = pose.rotationX;
  m_FaceTrackStatus->pose.rotationY = pose.rotationY;
  m_FaceTrackStatus->pose.rotationZ = pose.rotationZ;
  m_FaceTrackStatus->pose.mouthStatus = pose.mouthStatus;
}
