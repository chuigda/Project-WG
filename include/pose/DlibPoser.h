#ifndef PROJECT_WG_DLIB_POSER
#define PROJECT_WG_DLIB_POSER

#include "Pose.h"
#include "util/Derive.h"

namespace cv {
class Mat;
} // namespace cv

namespace cw {

class DlibPoserPrivate;

class DlibPoser {
public:
  DlibPoser();
  ~DlibPoser();

  HeadPose EstimateHeadPose(cv::Mat const& frame,
                            bool useEyeLandmarks);

  CW_DERIVE_UNCOPYABLE(DlibPoser)
  CW_DERIVE_UNMOVABLE(DlibPoser)

private:
  DlibPoserPrivate *m_Private;
};

} // namespace cw

#endif // PROJECT_WG_DLIB_POSER
