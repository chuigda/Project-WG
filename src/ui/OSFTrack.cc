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
#include <QDoubleSpinBox>

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
    emit TrackingError("并没有进行中的监听任务");
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
// 因为这个程序只在 x86_64 架构上运行，所以无视 alignment 问题是可以的
struct FacePacket {
  int id ;
  float width;
  float height;
  float eyeBlinkRight;
  float eyeBlinkLeft;
  uint8_t success; // unused
  float pnpError; // unused
  float quaternion[4];
  float euler[3];
  float translation[3];
} __attribute__((packed));

static void DownscaleToDeadZone(float &value, float deadZone) {
  if (value > deadZone) {
    value = deadZone;
  } else if (value < -deadZone) {
    value = -deadZone;
  }
}

void OSFTrackReceiver::HandleData() {
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

    HeadPose pose {
      .rotationX = facePacket->euler[0] * -180.0f + m_Parameter.xRotationFix,
      .rotationY = facePacket->euler[1] * 180.0f + m_Parameter.yRotationFix,
      // 不知道为什么，OSF 的输出唯独 Z 轴用的是角度制，还从 180.0f 开始
      .rotationZ = facePacket->euler[2] - 180.0f + m_Parameter.zRotationFix
      // not important anyway
      // .mouthStatus = HeadPose::MouthStatus::Close
    };

    DownscaleToDeadZone(pose.rotationX, 15.0f);
    DownscaleToDeadZone(pose.rotationY, 30.0f);
    DownscaleToDeadZone(pose.rotationZ, 15.0f);

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

  xSum /= m_SmoothBuffer.size();
  ySum /= m_SmoothBuffer.size();
  zSum /= m_SmoothBuffer.size();

  emit HeadPoseUpdated(HeadPose { xSum, ySum, zSum });
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
    m_FCS(fcs)
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

    layout->addLayout(box);
  }

  {
    QGroupBox *groupBox = new QGroupBox("后处理选项");
    QHBoxLayout *box = new QHBoxLayout();
    groupBox->setLayout(box);

    box->addWidget(new QLabel("平滑"));
    QSpinBox *spinSmooth = new QSpinBox();
    spinSmooth->setValue(4);
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
        parameter.xRotationFix = spinX->value();
        parameter.yRotationFix = spinY->value();
        parameter.zRotationFix = spinZ->value();

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

void OSFTrackController::HandleError(QString reason) {
  QMessageBox::warning(this, "面部捕捉错误", reason);
}

void OSFTrackController::HandlePoseUpdate(HeadPose pose) {
  m_FCS->pose.rotationX = pose.rotationX;
  m_FCS->pose.rotationY = pose.rotationY;
  m_FCS->pose.rotationZ = pose.rotationZ;
}
