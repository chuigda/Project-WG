#ifndef PROJECT_WG_UINEXT_CAMERA_ENTITY_STATUS_H
#define PROJECT_WG_UINEXT_CAMERA_ENTITY_STATUS_H

#include <QtGui/qopengl.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "cwglx/GL/GL.h"

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

  void ToMatrix(glm::mat4 &matrix) const noexcept {
    matrix = glm::translate(matrix, glm::vec3(-translateX, -translateY, -translateZ));
    matrix = glm::rotate(matrix, glm::radians(entityRotateX), glm::vec3(1.0f, 0.0f, 0.0f));
    matrix = glm::rotate(matrix, glm::radians(entityRotateY), glm::vec3(0.0f, 1.0f, 0.0f));
    matrix = glm::rotate(matrix, glm::radians(entityRotateZ), glm::vec3(0.0f, 0.0f, 1.0f));
  }
};

#endif // PROJECT_WG_UINEXT_CAMERA_ENTITY_STATUS_H
