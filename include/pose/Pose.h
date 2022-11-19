#ifndef PROJECT_WG_POSE_H
#define PROJECT_WG_POSE_H

namespace cw {

struct HeadPose {
    double rotationX;
    double rotationY;
    double rotationZ;

    enum MouthStatus { Close, Open, OpenBig } mouthStatus;
};

} // namespace cw

#endif // PROJECT_WG_POSE_H
