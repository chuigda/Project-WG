#ifndef PROJECT_WG_CAMERA_ENTITY_STATUS_H
#define PROJECT_WG_CAMERA_ENTITY_STATUS_H

#include <QtGui/qopengl.h>

#include "cwglx/GL.h"

class CameraEntityStatus {
public:
  GLfloat cameraX;
  GLfloat cameraY;
  GLfloat cameraZ;

  GLfloat entityRotateX;
  GLfloat entityRotateY;
  GLfloat entityRotateZ;

  explicit constexpr inline CameraEntityStatus() noexcept
    : cameraX(0.0f),
      cameraY(25.0f),
      cameraZ(75.0f),
      entityRotateX(0.0f),
      entityRotateY(0.0f),
      entityRotateZ(0.0f)
  {}

  constexpr void Reset() noexcept {
    *this = CameraEntityStatus {};
  }

  void ApplyCameraTransformation(GLFunctions *f) const noexcept;
  void ApplyEntityTransformation(GLFunctions *f) const noexcept;
};

#endif // PROJECT_WG_CAMERA_ENTITY_STATUS_H
