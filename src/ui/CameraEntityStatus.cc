#include "ui/CameraEntityStatus.h"

#include "cwglx/Vertex.h"
#include "cwglx/GLImpl.h"

void CameraEntityStatus::ApplyTransformation(GLFunctions *f) const {
  {
    f->glTranslatef(-cameraX, -cameraY, -cameraZ);
    cw::Vector cameraRotation(-cameraRotateX, -cameraRotateY, -cameraRotateZ);
    cw::Vector normalizedCRotationVec = cameraRotation.Normalize();
    GLdouble rotationCVecLen = cameraRotation.Length();
    if (rotationCVecLen >= 0.0005) {
      f->glRotatef(static_cast<GLfloat>(rotationCVecLen),
                   static_cast<GLfloat>(normalizedCRotationVec.GetX()),
                   static_cast<GLfloat>(normalizedCRotationVec.GetY()),
                   static_cast<GLfloat>(normalizedCRotationVec.GetZ()));
    }
  }

  {
    f->glTranslatef(entityX, entityY, entityZ);
    cw::Vector entityRotation(entityRotateX, entityRotateY, entityRotateZ);
    cw::Vector normalizedRotationVec = entityRotation.Normalize();
    GLdouble rotationVecLen = entityRotation.Length();
    if (rotationVecLen >= 0.0005) {
      f->glRotatef(static_cast<GLfloat>(rotationVecLen),
                   static_cast<GLfloat>(normalizedRotationVec.GetX()),
                   static_cast<GLfloat>(normalizedRotationVec.GetY()),
                   static_cast<GLfloat>(normalizedRotationVec.GetZ()));
    }
  }
}
