#include "cwglx/Object/Object.h"

#include <QDebug>
#include "cwglx/GL/GLImpl.h"

namespace cw {

void GLObjectContext::Delete(GLFunctions *f) {
  for (auto &[name, texture] : m_TextureLibrary) {
    texture->Delete(f);
  }
}

void GLObjectContext::AddTexture(QString texturePath,
                                 std::unique_ptr<cw::Texture2D> texture)
{
  m_TextureLibrary.insert(std::make_pair<>(
    std::move(texturePath),
    std::move(texture))
  );
}

void GLObjectContext::AddMaterial(QString materialName,
                                  glm::vec4 const& ambient,
                                  glm::vec4 const& diffuse,
                                  glm::vec4 const& specular,
                                  GLfloat shine,
                                  const QString &diffuseTexture,
                                  const QString &specularTexture,
                                  const QString &normalTexture)
{
  if (m_MaterialLibrary.contains(materialName)) {
    qWarning() << "GLObjectContext::AddMaterial(QString, ...):"
               << "material"
               << materialName
               << "has been added";
    return;
  }

  Texture2D const* diffuseTexturePtr = nullptr;
  if (!diffuseTexture.isNull()) {
    diffuseTexturePtr = GetTexture(diffuseTexture);
  }

  Texture2D const* specularTexturePtr = nullptr;
  if (!specularTexture.isNull()) {
    diffuseTexturePtr = GetTexture(specularTexture);
  }

  Texture2D const* normalTexturePtr = nullptr;
  if (!normalTexture.isNull()) {
    normalTexturePtr = GetTexture(normalTexture);
  }

  m_MaterialLibrary.insert(std::make_pair<>(
    std::move(materialName),
    std::make_unique<Material>(
      ambient,
      diffuse,
      specular,
      shine,
      diffuseTexturePtr,
      specularTexturePtr,
      normalTexturePtr,
      SecretInternalsDoNotUseOrYouWillBeFired
    )
  ));
}

Texture2D const *GLObjectContext::GetTexture(const QString &textureName) const {
  auto it = m_TextureLibrary.find(textureName);
  if (it == m_TextureLibrary.cend()) {
    qWarning() << "GLObjectContext::GetTexture(QString const&):"
               << "texture"
               << textureName
               << "has not been loaded";
    return nullptr;
  }
  return it->second.get();
}

Material const *GLObjectContext::GetMaterial(const QString &materialName) const {
  auto it = m_MaterialLibrary.find(materialName);
  if (it == m_MaterialLibrary.cend()) {
    qWarning() << "GLObjectContext::GetMaterial(QString const&):"
               << "material"
               << materialName
               << "has not been defined";
    return nullptr;
  }
  return it->second.get();
}

} // namespace cw
