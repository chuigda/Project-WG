#ifndef PROJECT_WG_CAMERA_ENTITY_STATUS_H
#define PROJECT_WG_CAMERA_ENTITY_STATUS_H

#include <QtGui/qopengl.h>

#include "cwglx/GL.h"

class EntityStatus {
public:
  GLfloat translateX;
  GLfloat translateY;
  GLfloat translateZ;

  GLfloat entityRotateX;
  GLfloat entityRotateY;
  GLfloat entityRotateZ;

  explicit constexpr inline EntityStatus() noexcept
    : translateX(0.0f),
      translateY(25.0f),
      translateZ(75.0f),
      entityRotateX(0.0f),
      entityRotateY(0.0f),
      entityRotateZ(0.0f)
  {}

  constexpr void Reset() noexcept {
    *this = EntityStatus {};
  }

  void ApplyEntityTransformation(GLFunctions *f) const noexcept;
};

#endif // PROJECT_WG_CAMERA_ENTITY_STATUS_H
