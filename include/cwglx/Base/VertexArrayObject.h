#ifndef PROJECT_GL2_VAO_H
#define PROJECT_GL2_VAO_H

#include <memory>
#include <vector>
#include "include/cwglx/GL/GL.h"
#include "util/Sinkrate.h"
#include "util/Derive.h"

namespace cw {

class VertexArrayObject {
public:
  explicit VertexArrayObject(GLFunctions *f);

  VertexArrayObject(GLuint vao, Sinkrate);

  ~VertexArrayObject();

  void Bind(GLFunctions *f) const;

  void Unbind(GLFunctions *f) const;

  void Delete(GLFunctions *f);

  static std::vector<std::unique_ptr<VertexArrayObject>>
  CreateMany(GLFunctions *f, GLsizei count);

  CW_DERIVE_UNCOPYABLE(VertexArrayObject)
  CW_DERIVE_UNMOVABLE(VertexArrayObject)

private:
  GLuint m_VAO;
  bool m_Deleted;
};

}

#endif // PROJECT_GL2_VAO_H
