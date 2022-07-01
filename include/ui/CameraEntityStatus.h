#ifndef PROJECT_WG_CAMERA_ENTITY_STATUS_H
#define PROJECT_WG_CAMERA_ENTITY_STATUS_H

#include <QtGui/qopengl.h>

class CameraEntityStatus {
public:
  GLfloat cameraX;
  GLfloat cameraY;
  GLfloat cameraZ;

  GLdouble cameraRotateX;
  GLdouble cameraRotateY;
  GLdouble cameraRotateZ;

  GLfloat entityX;
  GLfloat entityY;
  GLfloat entityZ;

  GLdouble entityRotateX;
  GLdouble entityRotateY;
  GLdouble entityRotateZ;

  explicit constexpr inline CameraEntityStatus()
    : cameraX(0.0f),
      cameraY(0.0f),
      cameraZ(0.0f),
      cameraRotateX(0.0),
      cameraRotateY(0.0),
      cameraRotateZ(0.0),
      entityX(0.0f),
      entityY(0.0f),
      entityZ(-30.0f),
      entityRotateX(0.0),
      entityRotateY(0.0),
      entityRotateZ(0.0)
  {}
};

#endif // PROJECT_WG_CAMERA_ENTITY_STATUS_H
