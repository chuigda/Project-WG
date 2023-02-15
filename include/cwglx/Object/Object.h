#ifndef PROJECT_GL2_OBJECT_H
#define PROJECT_GL2_OBJECT_H

#include <memory>
#include <QMap>
#include "cwglx/Object/Material.h"
#include "cwglx/Base/VertexArrayObject.h"
#include "cwglx/Object/Vertex.h"

namespace cw {

class ShaderProgram;

class GLObjectContext final {
public:
  GLObjectContext() = default;
  ~GLObjectContext() = default;

  void Delete(GLFunctions *f);

  bool HasTexture(QString const& texturePath) const;
  bool HasMaterial(QString const& materialName) const;

  Texture2D const* AddTexture(QString texturePath,
                              std::unique_ptr<cw::Texture2D> &&texture);
  Material const* AddMaterial(QString materialName,
                              std::unique_ptr<Material> &&material);

  Texture2D const* GetTexture(QString const& texturePath) const;
  Material const* GetMaterial(QString const& materialName) const;

private:
  std::unordered_map<QString, std::unique_ptr<Material>> m_MaterialLibrary;
  std::unordered_map<QString, std::unique_ptr<Texture2D>> m_TextureLibrary;
};

struct GLObject final {
  std::unique_ptr<VertexArrayObject> vao;
  std::unique_ptr<VertexVBO> vbo;
  GLsizei vertexCount;
  Material const* material;

  GLObject(std::unique_ptr<VertexArrayObject> &&vao,
           std::unique_ptr<VertexVBO> &&vbo,
           GLsizei vertexCount,
           Material const* material);

  void Draw(GLFunctions *f, ShaderProgram *shaderProgram) const;

  void Delete(GLFunctions *f) const;

  GLObject(GLObject&&) = default;
  GLObject& operator=(GLObject&&) = default;
};

} // namespace cw

#endif // PROJECT_GL2_OBJECT_H
