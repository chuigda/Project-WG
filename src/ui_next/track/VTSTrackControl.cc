#include "TrackControlImpl.h"

#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include "util/Derive.h"

class VTSTrackWorker : public QObject {
  Q_OBJECT

public:
  VTSTrackWorker() :
    m_Websocket(nullptr),
    m_Timer(nullptr),
    m_LastRequestId(0),
    m_LastResponseId(0)
  {}

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
      StopCommunication();
      emit TrackingError("无法初始化到 VTS 服务器的 WebSocket 连接");
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
    QString requestId = object["requestId"].toString(""); \
    { \
      QString apiName = object["apiName"].toString(""); \
      \
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
        emit TrackingError(QStringLiteral("VTS 服务器返回了错误响应：%1").arg(errorMessage)); \
        StopCommunication(); \
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
      StopCommunication();
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
    m_WorkerThread(workerThread),
    m_VTSVersionLabel(new QLabel(""))
{
  VTSTrackWorker *worker = new VTSTrackWorker();
  worker->moveToThread(workerThread);

  connect(this, &VTSTrackControl::StartTracking,
          worker, &VTSTrackWorker::StartCommunication);
  connect(this, &VTSTrackControl::StopTracking,
          worker, &VTSTrackWorker::StopCommunication);
  connect(worker, &VTSTrackWorker::VTSVersionProbed,
          this, &VTSTrackControl::DisplayVTSVersion);
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

  m_VTSVersionLabel->setVisible(false);
  layout->addWidget(m_VTSVersionLabel);
}

void VTSTrackControl::DisplayVTSVersion(const QString &version) {
  m_VTSVersionLabel->setVisible(true);
  m_VTSVersionLabel->setText(QStringLiteral("VTS版本: %1").arg(version));
}

void VTSTrackControl::HandleError(const QString &error) {
  QMessageBox::warning(this, "VTS 面部捕捉错误", error);
}

void VTSTrackControl::HandleHeadStatus(wgc0310::HeadStatus headStatus) {
  *m_HeadStatus = headStatus;
}

#include "VTSTrackControl.moc"
