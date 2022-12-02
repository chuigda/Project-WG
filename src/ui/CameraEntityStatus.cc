#include "ui/CameraEntityStatus.h"

#include "cwglx/Vertex.h"
#include "cwglx/GLImpl.h"

void CameraEntityStatus::ApplyCameraTransformation(GLFunctions *f) const noexcept {
  f->glTranslatef(-cameraX, -cameraY, -cameraZ);
}

void CameraEntityStatus::ApplyEntityTransformation(GLFunctions *f) const noexcept {
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
