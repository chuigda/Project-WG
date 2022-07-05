#include "lib.h"

#include "cwglx/GLImpl.h"

extern "C" {

std::uint32_t WGAPI GetWGAPIVersion(void) {
  // WGAPI 0.1.0
  return 0x00'01'0000;
}

void* WGAPI InitContext(GLFunctions *f) {
  // TODO your codes are here

  Q_UNUSED(f)

  static int g_DummyValueAddr = 0;
  return &g_DummyValueAddr;
}

void WGAPI DestroyContext(void *context, GLFunctions *f) {
  // TODO your codes are here

  Q_UNUSED(context)
  Q_UNUSED(f)
}

bool WGAPI RewindContext(void *context, GLFunctions *f) {
  // TODO your codes are here

  Q_UNUSED(context)
  Q_UNUSED(f)
  return true;
}

bool WGAPI PlayAnimationFrame(void *context,
                              GLFunctions *f,
                              std::uint64_t frame) {
  Q_UNUSED(context)

  f->glPushMatrix();
  f->glRotatef(static_cast<GLfloat>(frame), 0.0f, 0.0f, 1.0f);

  f->glPushAttrib(GL_CURRENT_BIT);
  f->glBegin(GL_TRIANGLES);
  {
    f->glColor3f(1.0f, 0.0f, 0.0f);
    f->glVertex3f(0.0f, 240.0f, 0.0f);

    f->glColor3f(0.0f, 1.0f, 0.0f);
    f->glVertex3f(-320.0f, -240.0f, 0.0f);

    f->glColor3f(0.0f, 0.0f, 1.0f);
    f->glVertex3f(320.0f, -240.0f, 0.0f);
  }
  f->glEnd();
  f->glPopAttrib();
  f->glPopMatrix();

  return true;
}

} // extern "C"
