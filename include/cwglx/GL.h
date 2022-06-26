#ifndef PROJECT_WG_GL_H
#define PROJECT_WG_GL_H


#ifndef NDEBUG
class QOpenGLFunctions_4_5_Compatibility;
using GLFunctions = QOpenGLFunctions_4_5_Compatibility;
#else
class QOpenGLFUnctions_2_0;
using GLFunctions = QOpenGLFUnctions_2_0;
#endif

#endif // PROJECT_WG_GL_H
