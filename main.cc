#include <atomic>
#include <thread>

#include <opencv2/highgui/highgui.hpp>
#include <QApplication>
#include <QMessageBox>

#include "ui/GLWidget.h"
#include "pose/DlibPoser.h"
#include "util/Channel.h"
#include "util/Logger.h"

int main(int argc, char *argv[]) {
  int qt_ret_value;

  // cv::VideoCapture cap(0);
  cv::VideoCapture cap("/home/icey/Videos/test_video.mkv");
  cw::DlibPoser dlibPoser;
  std::unique_ptr<cw::Sender<cw::HeadPose>> sender = nullptr;
  std::unique_ptr<cw::Receiver<cw::HeadPose>> receiver = nullptr;

  std::atomic_bool thread_done = false;

  enum class FaceCaptureStatus {
    Connected,
    CameraInitFailure,
    DlibInitFailure
  } fcs = FaceCaptureStatus::Connected;

  if (cap.isOpened()) {
    if (dlibPoser.Open()) {
      auto [tx, rx] = cw::Channel<cw::HeadPose>(50);
      sender = std::move(tx);
      receiver = std::move(rx);
    } else {
      fcs = FaceCaptureStatus::DlibInitFailure;
    }
  } else {
    fcs = FaceCaptureStatus::CameraInitFailure;
  }

  std::thread t {
    [&qt_ret_value,
     receiver = std::move(receiver),
     &argc,
     argv,
     fcs,
     &thread_done] () mutable {
      QApplication a { argc, argv };

      switch (fcs) {
        case FaceCaptureStatus::Connected:
          break;
        case FaceCaptureStatus::CameraInitFailure:
          QMessageBox::warning(nullptr,
                               "警告",
                               "摄像机初始化失败，面部捕捉将不可用");
          break;
        case FaceCaptureStatus::DlibInitFailure:
          QMessageBox::warning(nullptr,
                               "警告",
                               "DLIB 初始化失败，面部捕捉将不可用\r\n"
                               "请检查文件 <code>alchemy/dlib_sp_68points.dat</code> 是否正确放置\r\n");
          break;
      }

      GLWidget w { std::move(receiver), nullptr };
      w.show();
      qt_ret_value = a.exec();

      thread_done.store(true);
    }
  };


  if (sender) {
    cv::Mat image;
    // 出于某种奇怪的原因，dlib 似乎希望自己被运行在“主”线程上
    // 所以选择把 Qt 发送到另一个线程上运行
    while (!thread_done.load() && cap.read(image)) {
      sona::LogInfo("FaceCapture", "read 1 frame");
      auto [success, pose] = dlibPoser.EstimateHeadPose(image, false);
      if (success) {
        sona::LogInfo("FaceCapture",
                      "x=", pose.rotationX,
                      ", y=", pose.rotationY,
                      ", z=", pose.rotationZ);
        if (!sender->TrySend(std::move(pose))) {
          sona::LogWarn("FaceCapture",
                        "head pose frame queue full! is the renderer overloaded?");
        }
      } else {
        sona::LogWarn("FaceCapture",
                      "missing face in 1 frame");
      }
    }
  }
  t.join();
  return qt_ret_value;
}
