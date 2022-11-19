#include <QApplication>
#include "ui/GLWidget.h"

#include <thread>
#include "pose/DlibPoser.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>

int main(int argc, char *argv[]) {
    std::thread t {[&argc, argv] {
        QApplication a { argc, argv };
        GLWidget w;
        w.show();
        return a.exec();
    }};

    cw::DlibPoser dlibPoser;
    bool success = dlibPoser.Open();
    if (!success) {
        qDebug() << "fuck you!";
        abort();
    }

    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
        qDebug() << "cannot connect to camera";
        return 0;
    }

    while (1) {
        cv::Mat temp;
        cap >> temp;

        dlibPoser.EstimateHeadPose(temp, false);
    }
}
