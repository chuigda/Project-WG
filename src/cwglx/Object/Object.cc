#include "cwglx/Object/Object.h"

#include <QDebug>
#include "cwglx/GL/GLImpl.h"

namespace cw {

void GLObjectContext::Delete(GLFunctions *f) {
  for (auto &[name, texture] : m_TextureLibrary) {
    texture->Delete(f);
  }
}

bool GLObjectContext::HasTexture(QString const& texturePath) const {
  return m_TextureLibrary.contains(texturePath);
}

bool GLObjectContext::HasMaterial(QString const& materialName) const {
  return m_MaterialLibrary.contains(materialName);
}

Texture2D const*
GLObjectContext::AddTexture(QString texturePath,
                            std::unique_ptr<cw::Texture2D> &&texture)
{
  auto it = m_TextureLibrary.find(texturePath);

  if (it != m_TextureLibrary.cend()) {
    qWarning() << "GLObjectContext::AddTexture(QString, std::unique_ptr<Texture2D>&&):"
               << "texture"
               << texturePath
               << "has been added (same-path)";
    return it->second.get();
  }

  auto r = m_TextureLibrary.insert(std::make_pair<>(
    std::move(texturePath),
    std::move(texture))
  );
  return r.first->second.get();
}

Material const*
GLObjectContext::AddMaterial(QString materialName,
                             std::unique_ptr<Material> &&material)
{
  auto it = m_MaterialLibrary.find(materialName);

  if (it != m_MaterialLibrary.cend()) {
    qWarning() << "GLObjectContext::AddMaterial(QString, std::unique_ptr<Material>&&):"
               << "material"
               << materialName
               << "has been added (same-name)";
    return it->second.get();
  }

  auto r = m_MaterialLibrary.insert(std::make_pair<>(
    std::move(materialName),
    std::move(material)
  ));
  return r.first->second.get();
}

Texture2D const *GLObjectContext::GetTexture(const QString &texturePath) const {
  auto it = m_TextureLibrary.find(texturePath);
  if (it == m_TextureLibrary.cend()) {
    qWarning() << "GLObjectContext::GetTexture(QString const&):"
               << "texture"
               << texturePath
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
