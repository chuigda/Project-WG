#ifndef PROJECT_WG_GL_H
#define PROJECT_WG_GL_H


#ifndef NDEBUG
class QOpenGLFunctions_4_5_Compatibility;
using GLFunctions = QOpenGLFunctions_4_5_Compatibility;
#else
class QOpenGLFUnctions_3_3_Compatibility;
using GLFunctions = QOpenGLFUnctions_3_3_Compatibility;
#endif

#endif // PROJECT_WG_GL_H
