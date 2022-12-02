#ifndef PROJECT_WG_CAMERA_ENTITY_STATUS_H
#define PROJECT_WG_CAMERA_ENTITY_STATUS_H

#include <QtGui/qopengl.h>

#include "cwglx/GL.h"

class CameraEntityStatus {
public:
  GLfloat cameraX;
  GLfloat cameraY;
  GLfloat cameraZ;

  GLfloat entityX;
  GLfloat entityY;
  GLfloat entityZ;

  GLdouble entityRotateX;
  GLdouble entityRotateY;
  GLdouble entityRotateZ;

  explicit constexpr inline CameraEntityStatus() noexcept
    : cameraX(0.0f),
      cameraY(0.0f),
      cameraZ(0.0f),
      entityX(0.0f),
      entityY(-25.0f),
      entityZ(-75.0f),
      entityRotateX(0.0),
      entityRotateY(0.0),
      entityRotateZ(0.0)
  {}

  constexpr void Reset() noexcept {
    *this = CameraEntityStatus {};
  }

  void ApplyCameraTransformation(GLFunctions *f) const noexcept;
  void ApplyEntityTransformation(GLFunctions *f) const noexcept;
};

#endif // PROJECT_WG_CAMERA_ENTITY_STATUS_H
