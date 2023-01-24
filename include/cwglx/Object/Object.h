#ifndef PROJECT_GL2_OBJECT_H
#define PROJECT_GL2_OBJECT_H

#include <memory>
#include <QMap>
#include "cwglx/Object/Material.h"
#include "Vertex.h"

namespace cw {

class GLObjectContext final {
public:
  GLObjectContext() = default;
  ~GLObjectContext() = default;

  void Delete(GLFunctions *f);

  void AddTexture(QString texturePath,
                  std::unique_ptr<cw::Texture2D> texture);
  void AddMaterial(QString materialName,
                   glm::vec4 const& ambient = { 1.0f, 1.0f, 1.0f, 1.0f },
                   glm::vec4 const& diffuse = { 1.0f, 1.0f, 1.0f, 1.0f },
                   glm::vec4 const& specular = { 1.0f, 1.0f, 1.0f, 1.0f },
                   GLfloat shine = 0.0f,
                   QString const& diffuseTexture = QString(),
                   QString const& specularTexture = QString(),
                   QString const& normalTexture = QString());

  Texture2D const* GetTexture(QString const& textureName) const;
  Material const* GetMaterial(QString const& materialName) const;

private:
  std::unordered_map<QString, std::unique_ptr<Material>> m_MaterialLibrary;
  std::unordered_map<QString, std::unique_ptr<Texture2D>> m_TextureLibrary;
};

} // namespace cw

#endif // PROJECT_GL2_OBJECT_H
