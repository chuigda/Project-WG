#include "TrackControlImpl.h"

#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

#include "wgc0310/HeadStatus.h"
#include "util/Derive.h"
#include "util/CircularBuffer.h"

class VTSTrackWorker : public QObject {
  Q_OBJECT

public:
  VTSTrackWorker() :
    m_Websocket(nullptr),
    m_Timer(nullptr),
    m_LastRequestId(0),
    m_LastResponseId(0),
    m_SmoothBuffer { wgc0310::HeadStatus {} }
  {}

  CW_DERIVE_UNCOPYABLE(VTSTrackWorker)
  CW_DERIVE_UNMOVABLE(VTSTrackWorker)

signals:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
  void HeadPoseUpdated(wgc0310::HeadStatus headPose);
  void TrackingError(QString const& reason);
#pragma clang diagnostic pop

public slots:
  void StartCommunication(std::uint16_t port) {
    if (m_Websocket) {
      emit TrackingError("已经有一个监听任务了，请先停止监听");
      return;
    }

    QString vtsHost = QStringLiteral("ws://127.0.0.1:%1").arg(port);
    m_Websocket = new QWebSocket();
    m_Websocket->open(QUrl(vtsHost));
    connect(m_Websocket, &QWebSocket::textMessageReceived,
            this, &VTSTrackWorker::ReceiveTokenPacket);
    connect(m_Websocket,
            QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
            this,
            [this] (QAbstractSocket::SocketError error) {
              StopCommunication();
              emit TrackingError(QStringLiteral("Websocket 错误 (%1)").arg(error));
            });
    connect(
      m_Websocket,
      &QWebSocket::connected,
      this,
      [this] {
        m_Websocket->sendTextMessage(R"json(
{
  "apiName": "VTubeStudioPublicAPI",
  "apiVersion": "1.0",
  "requestID": "StartCommunicationAuthRequest",
  "messageType": "AuthenticationTokenRequest",
  "data": {
    "pluginName": "Project-WG VTS Data Collector",
    "pluginDeveloper": "Chuigda WhiteGive"
  }
}
      )json");
    });
  }

  void StopCommunication() {
    delete m_Timer;
    delete m_Websocket;

    m_Timer = nullptr;
    m_Websocket = nullptr;
  }

private slots:
  #define PARSE_VTS_API_MESSAGE(REQUEST_ID, MESSAGE_TYPE) \
    QJsonDocument document = QJsonDocument::fromJson(message.toUtf8()); \
    if (!document.isObject()) { \
      emit TrackingError("VTS 服务器发送了无法理解的响应"); \
      StopCommunication(); \
      return; \
    } \
    QJsonObject object = document.object(); \
    if (!object["data"].isObject()) { \
      emit TrackingError("VTS 服务器发送了无法理解的响应"); \
      StopCommunication(); \
      return; \
    } \
    QJsonObject data = object["data"].toObject();         \
    QString requestId = object["requestID"].toString(""); \
    { \
      QString apiName = object["apiName"].toString(""); \
      if (apiName != "VTubeStudioPublicAPI") { \
        emit TrackingError("VTS 服务器发送了无法理解的响应"); \
        StopCommunication(); \
        return; \
      } \
      if (REQUEST_ID && requestId != REQUEST_ID) { \
        return; \
      } \
    } \
    { \
      QString messageType = object["messageType"].toString(""); \
      if (messageType == "APIError") {\
        QString errorMessage = data["message"].toString("未知错误"); \
        emit TrackingError(QStringLiteral("VTS 服务器返回了错误响应: %1").arg(errorMessage)); \
        StopCommunication(); \
        return; \
      } \
      if (MESSAGE_TYPE && messageType != MESSAGE_TYPE) { \
        return; \
      } \
    }

  void ReceiveTokenPacket(QString const& message) {
    PARSE_VTS_API_MESSAGE("StartCommunicationAuthRequest", "AuthenticationTokenResponse")

    m_VTSAuthToken = data["authenticationToken"].toString("abababa");
    disconnect(m_Websocket, &QWebSocket::textMessageReceived,
               this, &VTSTrackWorker::ReceiveTokenPacket);
    connect(m_Websocket, &QWebSocket::textMessageReceived,
            this, &VTSTrackWorker::ReceiveAuthPacket);

    m_Websocket->sendTextMessage(QStringLiteral(R"json(
{
  "apiName": "VTubeStudioPublicAPI",
  "apiVersion": "1.0",
  "requestID": "DoAuthRequest",
  "messageType": "AuthenticationRequest",
  "data": {
    "pluginName": "Project-WG VTS Data Collector",
    "pluginDeveloper": "Chuigda WhiteGive",
    "authenticationToken": "%1"
  }
}
    )json").arg(m_VTSAuthToken));
  }

  void ReceiveAuthPacket(QString const& message) {
    PARSE_VTS_API_MESSAGE("DoAuthRequest", "AuthenticationResponse")

    if (!data["authenticated"].toBool()) {
      emit TrackingError("VTS 鉴权失败");
      StopCommunication();
      return;
    }

    disconnect(m_Websocket, &QWebSocket::textMessageReceived,
               this, &VTSTrackWorker::ReceiveAuthPacket);
    connect(m_Websocket, &QWebSocket::textMessageReceived,
            this, &VTSTrackWorker::ReceiveDataPacket);

    m_LastRequestId = 0;
    m_LastResponseId = 0;

    m_Timer = new QTimer;
    m_Timer->setInterval(1000 / 25);
    m_Timer->setTimerType(Qt::CoarseTimer);
    connect(m_Timer, &QTimer::timeout,
            this, &VTSTrackWorker::RequestDataPacket);
    m_Timer->start();
  }

  void RequestDataPacket() {
    m_LastRequestId += 1;
    m_Websocket->sendTextMessage(QStringLiteral(R"json(
{
  "apiName": "VTubeStudioPublicAPI",
  "apiVersion": "1.0",
  "requestID": "%1",
  "messageType": "InputParameterListRequest",
  "data": {
    "authenticationToken": "%2"
  }
}
    )json").arg(m_LastRequestId).arg(m_VTSAuthToken));
  }

  void ReceiveDataPacket(QString const& message) {
    PARSE_VTS_API_MESSAGE(nullptr, "InputParameterListResponse")

    bool isValidResponseId = false;
    std::uint64_t responseId = requestId.toULongLong(&isValidResponseId);
    if (!isValidResponseId) {
      return;
    }

    if (responseId <= m_LastResponseId) {
      return;
    }

    if (!data["defaultParameters"].isArray()) {
      return;
    }

    QJsonArray dataArray = data["defaultParameters"].toArray();
    wgc0310::HeadStatus headStatus;
    for (auto item : dataArray) {
      if (!item.isObject()) {
        continue;
      }

      QJsonObject itemObject = item.toObject();
      if (!itemObject["name"].isString()) {
        continue;
      }

      QString itemName = itemObject["name"].toString();

      if (itemName == "FaceAngleX") {
        headStatus.rotationY = itemObject["value"].toDouble();
      } else if (itemName == "FaceAngleY") {
        headStatus.rotationX = -itemObject["value"].toDouble();
      } else if (itemName == "FaceAngleZ") {
        headStatus.rotationZ = -itemObject["value"].toDouble();
      } else if (itemName == "MouthOpen") {
        headStatus.mouthStatus =
            itemObject["value"].toDouble() >= 0.5 ?
              wgc0310::HeadStatus::MouthStatus::Open :
              wgc0310::HeadStatus::MouthStatus::Close;
      }
    }
    m_SmoothBuffer.PopFront();
    m_SmoothBuffer.PushBack(headStatus);

    float xSum = 0.0f;
    float ySum = 0.0f;
    float zSum = 0.0f;
    float mouthStatus = 0;

    for (std::size_t i = 0; i < m_SmoothBuffer.Size(); i++) {
      wgc0310::HeadStatus status = m_SmoothBuffer.Get(i);
      xSum += status.rotationX;
      ySum += status.rotationY;
      zSum += status.rotationZ;
      mouthStatus += static_cast<float>(static_cast<int>(status.mouthStatus));
    }

    float smoothBufferSize = static_cast<float>(m_SmoothBuffer.Size());

    xSum /= smoothBufferSize;
    ySum /= smoothBufferSize;
    zSum /= smoothBufferSize;

    emit HeadPoseUpdated(wgc0310::HeadStatus {
      xSum,
      ySum,
      zSum,
      // leftEyeSum,
      /// rightEyeSum,
      1.0f, 1.0f,
      mouthStatus > 0 ? wgc0310::HeadStatus::MouthStatus::Open
                      : wgc0310::HeadStatus::MouthStatus::Close
    });

    m_LastResponseId = responseId;
  }

private:
  QWebSocket *m_Websocket;
  QTimer *m_Timer;

  QString m_VTSAuthToken;
  std::uint64_t m_LastRequestId;
  std::uint64_t m_LastResponseId;
  cw::CircularBuffer<wgc0310::HeadStatus, 4> m_SmoothBuffer;
};

VTSTrackControl::VTSTrackControl(wgc0310::HeadStatus *headStatus,
                                 QThread *workerThread,
                                 QWidget *parent)
  : QWidget(parent),
    m_HeadStatus(headStatus),
    m_WorkerThread(workerThread)
{
  VTSTrackWorker *worker = new VTSTrackWorker();
  worker->moveToThread(workerThread);

  connect(this, &VTSTrackControl::StartTracking,
          worker, &VTSTrackWorker::StartCommunication);
  connect(this, &VTSTrackControl::StopTracking,
          worker, &VTSTrackWorker::StopCommunication);
  connect(worker, &VTSTrackWorker::TrackingError,
          this, &VTSTrackControl::HandleError);
  connect(worker, &VTSTrackWorker::HeadPoseUpdated,
          this, &VTSTrackControl::HandleHeadStatus);

  QVBoxLayout *layout = new QVBoxLayout(this);

  {
    QLabel *introduction = new QLabel(
      "从 <a href=\"https://denchisoft.com/\">VTubeStudio (VTS)</a> 获取面部捕捉和姿态估计参数<br/>"
      "需要启动 VTS 并启用插件 API，本程序会作为一个插件接入 VTS"
    );
    introduction->setOpenExternalLinks(true);
    layout->addWidget(introduction);
  }

  {
    QGroupBox *vtsSettings = new QGroupBox("VTubeStudio 设置");
    layout->addWidget(vtsSettings);

    QHBoxLayout *vtsLayout = new QHBoxLayout();
    vtsSettings->setLayout(vtsLayout);

    QLabel *labelWSAddr = new QLabel("WebSocket 端口");
    QLineEdit *lineEdit = new QLineEdit("8001");
    QPushButton *startButton = new QPushButton("启动");
    QPushButton *stopButton = new QPushButton("停止");

    vtsLayout->addWidget(labelWSAddr);
    vtsLayout->addStretch();
    vtsLayout->addWidget(lineEdit);
    vtsLayout->addWidget(startButton);
    vtsLayout->addWidget(stopButton);

    connect(
      startButton,
      &QPushButton::clicked,
      this,
      [this, lineEdit] {
        bool success;
        std::uint16_t port = lineEdit->text().toUInt(&success);
        if (!success) {
          QMessageBox::warning(this, "VTS 面部捕捉错误", "输入的端口号无效");
        }

        emit this->StartTracking(port);
      }
    );

    connect(stopButton, &QPushButton::clicked, this, &VTSTrackControl::StopTracking);
  }
}

void VTSTrackControl::HandleError(const QString &error) {
  QMessageBox::warning(this, "VTS 面部捕捉错误", error);
}

void VTSTrackControl::HandleHeadStatus(wgc0310::HeadStatus headStatus) {
  *m_HeadStatus = headStatus;
}

#include "VTSTrackControl.moc"
