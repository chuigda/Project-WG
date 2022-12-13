#include "TrackControlImpl.h"

#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include "util/Derive.h"

class VTSTrackWorker : public QObject {
  Q_OBJECT

public:

  CW_DERIVE_UNCOPYABLE(VTSTrackWorker)
  CW_DERIVE_UNMOVABLE(VTSTrackWorker)

signals:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
  void VTSVersionProbed(QString const& version);
  void HeadPoseUpdated(wgc0310::HeadStatus headPose);
  void TrackingError(QString const& reason);
#pragma clang diagnostic pop

public slots:
  void StartCommunication(std::uint16_t port) {
    if (m_Websocket) {
      emit TrackingError("已经有一个监听任务了，请先停止监听");
      return;
    }

    QString vtsHost = QStringLiteral("ws://localhost:%1").arg(port);
    m_Websocket = new QWebSocket(vtsHost);
    if (!m_Websocket->isValid()) {
      emit TrackingError("无法初始化到 VTS 服务器的 WebSocket 连接");
      delete m_Websocket;
      return;
    }

    connect(m_Websocket, &QWebSocket::textMessageReceived,
            this, &VTSTrackWorker::ReceiveProbePacket);
    m_Websocket->sendTextMessage(R"json(
{
  "apiName": "VTubeStudioPublicAPI",
  "apiVersion": "1.0",
  "requestID": "StartCommunicationProbeRequest",
  "messageType": "APIStateRequest"
}
    )json");
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
      delete m_Websocket; \
      delete m_Timer; \
      return; \
    } \
    QJsonObject object = document.object(); \
    if (!object["data"].isObject()) { \
      emit TrackingError("VTS 服务器发送了无法理解的响应"); \
      delete m_Websocket; \
      delete m_Timer; \
      return; \
    } \
    QJsonObject data = object["data"].toObject();         \
    QString requestId = object["requestId"].toString(""); \
    { \
      QString apiName = object["apiName"].toString(""); \
      \
      if (apiName != "VTubeStudioPublicAPI") { \
        emit TrackingError("VTS 服务器发送了无法理解的响应"); \
        delete m_Websocket; \
      delete m_Timer; \
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
        emit TrackingError(QStringLiteral("VTS 服务器返回了错误响应：%1").arg(errorMessage)); \
        delete m_Websocket; \
        delete m_Timer; \
        return; \
      } \
      if (MESSAGE_TYPE && messageType != MESSAGE_TYPE) { \
        return; \
      } \
    }

  void ReceiveProbePacket(QString const& message) {
    PARSE_VTS_API_MESSAGE("StartCommunicationProbeRequest", "APIStateResponse")

    bool active = data["active"].toBool(false);
    if (!active) {
      emit TrackingError("VTS 服务器的 API 并未激活");
      delete m_Websocket;
      return;
    }

    QString vtsVersion = object["vTubeStudioVersion"].toString("未知版本");
    emit VTSVersionProbed(vtsVersion);

    disconnect(m_Websocket);
    connect(m_Websocket, &QWebSocket::textMessageReceived,
            this, &VTSTrackWorker::ReceiveAuthPacket);
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
  }

  void ReceiveAuthPacket(QString const& message) {
    PARSE_VTS_API_MESSAGE("StartCommunicationAuthRequest", "AuthenticationTokenResponse")

    m_VTSAuthToken = data["authenticationToken"].toString("abababa");
    disconnect(m_Websocket);
    connect(m_Websocket, &QWebSocket::textMessageReceived,
            this, &VTSTrackWorker::ReceiveDataPacket);

    m_LastRequestId = 0;
    m_LastResponseId = 0;

    m_Timer = new QTimer;
    m_Timer->setInterval(1000 / 25);
    m_Timer->setTimerType(Qt::CoarseTimer);
    connect(m_Timer, &QTimer::timeout,
            this, &VTSTrackWorker::RequestDataPacket);
  }

  void RequestDataPacket() {
    m_LastRequestId += 1;
    m_Websocket->sendTextMessage(QStringLiteral(R"json(

    )json").arg(m_LastRequestId));
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

    qDebug() << data["defaultParameters"];
    m_LastResponseId = responseId;
  }

private:
  QWebSocket *m_Websocket;
  QTimer *m_Timer;

  QString m_VTSAuthToken;
  std::uint64_t m_LastRequestId;
  std::uint64_t m_LastResponseId;
};

VTSTrackControl::VTSTrackControl(wgc0310::HeadStatus *headStatus,
                                 QThread *workerThread,
                                 QWidget *parent)
  : QWidget(parent),
    m_HeadStatus(headStatus),
    m_WorkerThread(workerThread)
{}

#include "VTSTrackControl.moc"
