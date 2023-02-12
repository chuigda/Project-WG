#include "cwglx/Object/WavefrontLoader.h"

#include "cwglx/Object/Object.h"
#include "cwglx/Object/Material.h"
#include "util/FileUtil.h"

#include <QString>
#include <QDebug>
#include <QImage>

namespace cw {

void LoadMaterialLibrary(GLObjectContext *ctx,
                         GLFunctions *f,
                         QString const &fileName) {
  QString currentMaterialName;
  std::unique_ptr<Material> currentMaterial;

  QString fileContent = cw::ReadToString(fileName);
  QStringList fileLines = fileContent.split('\n');

  qsizetype lineNo = 0;
  for (QString const &line: fileLines) {
    lineNo += 1;

    QString trimmed = line.trimmed();
    if (trimmed.isEmpty() || trimmed.startsWith('#')) {
      continue;
    }

    QStringList parts = trimmed.split(' ');
    if (parts.length() < 1) {
      qWarning() << "LoadMaterialLibrary(GLObjectContext*, GLFunctions*, QString const&):"
                 << "when parsing line "
                 << lineNo
                 << ":"
                 << "bad command (no valid command parts)";
      continue;
    }

    QString command = parts[0].toLower();

    if (command == "newmtl") {
      if (parts.length() != 2) {
        qWarning() << "LoadMaterialLibrary(GLObjectContext*, GLFunctions*, QString const&):"
                   << "when parsing line "
                   << lineNo
                   << ":"
                   << "bad command (newmtl expects 1 argument)";
        continue;
      }

      if (currentMaterial) {
        ctx->AddMaterial(currentMaterialName, std::move(currentMaterial));
      }

      currentMaterialName = parts[1];
      currentMaterial = std::make_unique<Material>();

      continue;
    }

    if (!currentMaterial) {
      qWarning() << "LoadMaterialLibrary(GLObjectContext*, GLFunctions*, QString const&):"
                 << "when parsing line "
                 << lineNo
                 << ":"
                 << "bad command (cannot set material properties without a live material)";
    }

    if (command == "ka" || command == "kd" || command == "ka") {
      glm::vec4 *portion;
      if (command == "ka") {
        portion = &currentMaterial->ambient;
      } else if (command == "kd") {
        portion = &currentMaterial->diffuse;
      } else {
        portion = &currentMaterial->specular;
      }

      if (parts.length() == 4) {
        portion->r = parts[1].toFloat();
        portion->g = parts[2].toFloat();
        portion->b = parts[3].toFloat();
      } else if (parts.length() == 5) {
        portion->r = parts[1].toFloat();
        portion->g = parts[2].toFloat();
        portion->b = parts[3].toFloat();
        portion->a = parts[4].toFloat();
      } else {
        qWarning() << "LoadMaterialLibrary(GLObjectContext*, GLFunctions*, QString const&):"
                   << "when parsing line "
                   << lineNo
                   << ":"
                   << "bad command (k commands expect 3 or 4 arguments)";
      }
    }
    else if (command == "d" || command == "tr") {
      if (parts.length() != 2) {
        qWarning() << "LoadMaterialLibrary(GLObjectContext*, GLFunctions*, QString const&):"
                   << "when parsing line "
                   << lineNo
                   << ":"
                   << "bad command (d/tr expects 1 argument)";
        continue;
      }

      float value = parts[1].toFloat();
      if (command == "tr") {
        value = 1.0f - value;
      }

      currentMaterial->ambient.a = value;
      currentMaterial->diffuse.a = value;
      currentMaterial->specular.a = value;
    }
    else if (command == "ns") {
      if (parts.length() != 2) {
        qWarning() << "LoadMaterialLibrary(GLObjectContext*, GLFunctions*, QString const&):"
                   << "when parsing line "
                   << lineNo
                   << ":"
                   << "bad command (ns expects 1 argument)";
        continue;
      }

      float ns = parts[1].toFloat();
      currentMaterial->shine = ns;
    }
    else if (command == "map_ka"
               || command == "map_kd"
               || command == "map_ks"
               || command == "map_bump"
               || command == "bump") {
      if (parts.length() != 2) {
        qWarning() << "LoadMaterialLibrary(GLObjectContext*, GLFunctions*, QString const&):"
                   << "when parsing line "
                   << lineNo
                   << ":"
                   << "bad command (map expects 1 argument)";
        continue;
      }

      cw::Texture2D const **portion;
      if (command == "map_ka") {
        qWarning() << "LoadMaterialLibrary(GLObjectContext*, GLFunctions*, QString const&):"
                   << "when parsing line "
                   << lineNo
                   << ":"
                   << "ambient mapping not supported yet";
        continue;
      } else if (command == "map_kd") {
        portion = &currentMaterial->diffuseTexture;
      } else if (command == "map_ks") {
        portion = &currentMaterial->specularTexture;
      } else {
        portion = &currentMaterial->normalTexture;
      }

      QString texturePath = parts[1];
      if (ctx->HasTexture(texturePath)) {
        *portion = ctx->GetTexture(texturePath);
      } else {
        QImage image;
        if (!image.load(texturePath)) {
          qWarning() << "LoadMaterialLibrary(GLObjectContext*, GLFunctions*, QString const&):"
                     << "when parsing line "
                     << lineNo
                     << ":"
                     << "cannot load texture: "
                     << texturePath;
          continue;
        }

        std::unique_ptr<Texture2D> texture = std::make_unique<Texture2D>(image, f);
        *portion = ctx->AddTexture(texturePath, std::move(texture));
      }
    }
    else {
      qWarning() << "LoadMaterialLibrary(GLObjectContext*, GLFunctions*, QString const&):"
                 << "when parsing line "
                 << lineNo
                 << ":"
                 << "unsupported command: "
                 << command;
    }
  }

  if (currentMaterial) {
    ctx->AddMaterial(currentMaterialName, std::move(currentMaterial));
  }
}

} // namespace cw
