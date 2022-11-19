#include "pose/DlibPoser.h"
#include "util/Constants.h"

#include <opencv2/opencv.hpp>
#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing.h>

#include <iostream>

namespace cw {

// this model comes from
// http://aifi.isr.uc.pt/Downloads/OpenGL/glAnthropometric3DModel.cpp
// an annotation can be found here
// https://github.com/lincolnhard/head-pose-estimation
cv::Point3d g_FaceModel[58] = {
  {-7.308957, 0.913869,  0.000000},
  {-6.775290, -0.730814, -0.012799},
  {-5.665918, -3.286078, 1.022951},
  {-5.011779, -4.876396, 1.047961},
  {-4.056931, -5.947019, 1.636229},
  {-1.833492, -7.056977, 4.061275},
  {0.000000,  -7.415691, 4.070434},
  {1.833492,  -7.056977, 4.061275},
  {4.056931,  -5.947019, 1.636229},
  {5.011779,  -4.876396, 1.047961},
  {5.665918,  -3.286078, 1.022951},
  {6.775290,  -0.730814, -0.012799},
  {7.308957,  0.913869,  0.000000},
  {5.311432,  5.485328,  3.987654},
  {4.461908,  6.189018,  5.594410},
  {3.550622,  6.185143,  5.712299},
  {2.542231,  5.862829,  4.687939},
  {1.789930,  5.393625,  4.413414},
  {2.693583,  5.018237,  5.072837},
  {3.530191,  4.981603,  4.937805},
  {4.490323,  5.186498,  4.694397},
  {-5.311432, 5.485328,  3.987654},
  {-4.461908, 6.189018,  5.594410},
  {-3.550622, 6.185143,  5.712299},
  {-2.542231, 5.862829,  4.687939},
  {-1.789930, 5.393625,  4.413414},
  {-2.693583, 5.018237,  5.072837},
  {-3.530191, 4.981603,  4.937805},
  {-4.490323, 5.186498,  4.694397},
  {1.330353,  7.122144,  6.903745},
  {2.533424,  7.878085,  7.451034},
  {4.861131,  7.878672,  6.601275},
  {6.137002,  7.271266,  5.200823},
  {6.825897,  6.760612,  4.402142},
  {-1.330353, 7.122144,  6.903745},
  {-2.533424, 7.878085,  7.451034},
  {-4.861131, 7.878672,  6.601275},
  {-6.137002, 7.271266,  5.200823},
  {-6.825897, 6.760612,  4.402142},
  {-2.774015, -2.080775, 5.048531},
  {-0.509714, -1.571179, 6.566167},
  {0.000000,  -1.646444, 6.704956},
  {0.509714,  -1.571179, 6.566167},
  {2.774015,  -2.080775, 5.048531},
  {0.589441,  -2.958597, 6.109526},
  {0.000000,  -3.116408, 6.097667},
  {-0.589441, -2.958597, 6.109526},
  {-0.981972, 4.554081,  6.301271},
  {-0.973987, 1.916389,  7.654050},
  {-2.005628, 1.409845,  6.165652},
  {-1.930245, 0.424351,  5.914376},
  {-0.746313, 0.348381,  6.263227},
  {0.000000,  0.000000,  6.763430},
  {0.746313,  0.348381,  6.263227},
  {1.930245,  0.424351,  5.914376},
  {2.005628,  1.409845,  6.165652},
  {0.973987,  1.916389,  7.654050},
  {0.981972,  4.554081,  6.301271}
};

// most implementation comes from:
// https://github.com/lincolnhard/head-pose-estimation/blob/master/video_test_shape.cpp

class DlibPoserPrivate {
public:
  DlibPoserPrivate() {
    try {
        m_FaceDetector = dlib::get_frontal_face_detector();
    } catch (dlib::serialization_error const& e) {
        std::cerr << e.what() << std::endl;
        std::abort();
    }

    constexpr std::size_t pointIndices[] = {
      33, 29, // left brow
      34, 38, // right brow
      13, 17, // left eye
      25, 21, // right eye
      55, 49, // nose
      43, 39, 45, // mouth
      6 // chin
    };

    constexpr std::size_t noEyePointIndices[] = {
      12, 0, 11, 1, // cheeks
      55, 49, // nose
      43, 39, 45, // mouth
      6 // chin
    };

    m_CameraMatrix = cv::Mat { 3, 3, CV_64FC1, K };
    m_DistCoeffs = cv::Mat { 5, 1, CV_64FC1, D };

    m_DefaultObjectPts.reserve(countof(pointIndices));
    for (std::size_t pointIndex : pointIndices) {
      m_DefaultObjectPts.push_back(g_FaceModel[pointIndex]);
    }

    m_NoEyeObjectPts.reserve(countof(noEyePointIndices));
    for (std::size_t pointIndex : noEyePointIndices) {
      m_NoEyeObjectPts.push_back(g_FaceModel[pointIndex]);
    }

    m_ImagePts.reserve(std::max(countof(pointIndices),
                                countof(noEyePointIndices)));
  }

  bool Open() {
    try {
      dlib::deserialize("./alchemy/dlib_sp_68points.dat") >> m_ShapePredictor;
      return true;
    } catch (dlib::serialization_error const& e) {
      return false;
    }
  }

  std::pair<bool, HeadPose> EstimateHeadPose(cv::Mat const& image) {
      dlib::cv_image<dlib::bgr_pixel> dlib_image(image);
      std::vector<dlib::rectangle> faces = (m_FaceDetector)(dlib_image);
      if (faces.size() == 0) {
          return std::make_pair(false, HeadPose {});
      }

      dlib::rectangle face = faces[0];
      dlib::full_object_detection shape = m_ShapePredictor(dlib_image, face);
      m_ImagePts.clear();

      m_ImagePts.emplace_back(shape.part(17).x(), shape.part(17).y()); //#17 left brow left corner
      m_ImagePts.emplace_back(shape.part(21).x(), shape.part(21).y()); //#21 left brow right corner
      m_ImagePts.emplace_back(shape.part(22).x(), shape.part(22).y()); //#22 right brow left corner
      m_ImagePts.emplace_back(shape.part(26).x(), shape.part(26).y()); //#26 right brow right corner
      m_ImagePts.emplace_back(shape.part(36).x(), shape.part(36).y()); //#36 left eye left corner
      m_ImagePts.emplace_back(shape.part(39).x(), shape.part(39).y()); //#39 left eye right corner
      m_ImagePts.emplace_back(shape.part(42).x(), shape.part(42).y()); //#42 right eye left corner
      m_ImagePts.emplace_back(shape.part(45).x(), shape.part(45).y()); //#45 right eye right corner
      m_ImagePts.emplace_back(shape.part(31).x(), shape.part(31).y()); //#31 nose left corner
      m_ImagePts.emplace_back(shape.part(35).x(), shape.part(35).y()); //#35 nose right corner
      m_ImagePts.emplace_back(shape.part(48).x(), shape.part(48).y()); //#48 mouth left corner
      m_ImagePts.emplace_back(shape.part(54).x(), shape.part(54).y()); //#54 mouth right corner
      m_ImagePts.emplace_back(shape.part(57).x(), shape.part(57).y()); //#57 mouth central bottom corner
      m_ImagePts.emplace_back(shape.part(8).x(), shape.part(8).y());   //#8 chin corner

      cv::Mat rotation; // 3x1 vector
      cv::Mat translation; // not important anyway

      cv::solvePnP(m_DefaultObjectPts, m_ImagePts, m_CameraMatrix, m_DistCoeffs, rotation, translation);
      return std::make_pair(true, HeadPose {
           rotation.at<double>(0) * 90.0,
           rotation.at<double>(1) * 90.0,
           rotation.at<double>(2) * 90.0
      });
  }

  ~DlibPoserPrivate() = default;

  CW_DERIVE_UNCOPYABLE(DlibPoserPrivate)
  CW_DERIVE_UNMOVABLE(DlibPoserPrivate)

private:
  cv::Mat m_CameraMatrix;
  cv::Mat m_DistCoeffs;

  dlib::frontal_face_detector m_FaceDetector;
  dlib::shape_predictor m_ShapePredictor;

  std::vector<cv::Point3d> m_DefaultObjectPts;
  std::vector<cv::Point3d> m_NoEyeObjectPts;
  std::vector<cv::Point2d> m_ImagePts;

  // Directly copied from the link above, I don't exactly know what they are
  // Theorically a calibration or so is required, but since I'm not using
  // translation matrix, I think this will be fine.

  // OpenCV would modify these values, so make a copy for every class instance
  double K[9] = {
      6.5308391993466671e+002,
      0.0,
      3.1950000000000000e+002,
      0.0,
      6.5308391993466671e+002,
      2.3950000000000000e+002,
      0.0,
      0.0,
      1.0
  };
  double D[5] = {
      7.0834633684407095e-002,
      6.9140193737175351e-002,
      0.0,
      0.0,
      -1.3073460323689292e+000
  };
};

DlibPoser::DlibPoser() : m_Private(new DlibPoserPrivate()) {}

DlibPoser::~DlibPoser() { delete m_Private; }

bool DlibPoser::Open() {
    return m_Private->Open();
}

std::pair<bool, HeadPose>
DlibPoser::EstimateHeadPose(cv::Mat const& frame, bool useEyeLandmarks) {
    (void)useEyeLandmarks;
    return m_Private->EstimateHeadPose(frame);
}

} // namespace cw
