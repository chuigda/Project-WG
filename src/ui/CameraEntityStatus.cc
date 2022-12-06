#include "ui/EntityStatus.h"

#include "cwglx/Vertex.h"
#include "cwglx/GLImpl.h"

void EntityStatus::ApplyEntityTransformation(GLFunctions *f) const noexcept {
  f->glTranslatef(-translateX, -translateY, -translateZ);

  cw::VectorF entityRotation(entityRotateX, entityRotateY, entityRotateZ);
  cw::VectorF normalizedRotationVec = entityRotation.Normalize();
  GLdouble rotationVecLen = entityRotation.Length();
  if (rotationVecLen >= 0.0005) {
    f->glRotatef(static_cast<GLfloat>(rotationVecLen),
                 static_cast<GLfloat>(normalizedRotationVec.GetX()),
                 static_cast<GLfloat>(normalizedRotationVec.GetY()),
                 static_cast<GLfloat>(normalizedRotationVec.GetZ()));
  }
}
