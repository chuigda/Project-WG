#include "cwglx/Object/WavefrontLoader.h"

#include <QString>
#include <QDebug>
#include <QImage>
#include <glm/geometric.hpp>
#include "cwglx/Object/Object.h"
#include "cwglx/Object/Material.h"
#include "util/FileUtil.h"

namespace cw {

static glm::vec2 ParseVertex2(QStringList const& commandBuf);
static glm::vec3 ParseVertex3(QStringList const& commandBuf);
static glm::vec4 ParseVertex4(QStringList const& commandBuf);

void LoadMaterialLibrary(GLObjectContext *ctx,
                         GLFunctions *f,
                         QString const &basePath,
                         QString const &fileName,
                         bool linearSampling,
                         bool anisotropyFilter) {
  QString currentMaterialName;
  std::unique_ptr<Material> currentMaterial;

  QString fileContent = cw::ReadToString(
    QStringLiteral("%1/%2")
      .arg(basePath)
      .arg(fileName)
  );
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
      qWarning() << "LoadMaterialLibrary(GLObjectContext*, GLFunctions*, QString const&, QString const&):"
                 << "when parsing file"
                 << fileName
                 << "line"
                 << lineNo
                 << ":"
                 << "bad command (no valid command parts)";
      continue;
    }

    QString command = parts[0].toLower();

    if (command == "newmtl") {
      if (parts.length() != 2) {
        qWarning() << "LoadMaterialLibrary(GLObjectContext*, GLFunctions*, QString const&, QString const&):"
                   << "when parsing file"
                   << fileName
                   << "line"
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
      qWarning() << "LoadMaterialLibrary(GLObjectContext*, GLFunctions*, QString const&, QString const&):"
                 << "when parsing file"
                 << fileName
                 << "line"
                 << lineNo
                 << ":"
                 << "bad command (cannot set material properties without a live material)";
    }

    if (command == "ka" || command == "kd" || command == "ks") {
      glm::vec4 *portion;
      if (command == "ka") {
        portion = &currentMaterial->ambient;
      } else if (command == "kd") {
        portion = &currentMaterial->diffuse;
      } else {
        portion = &currentMaterial->specular;
      }

      if (parts.length() == 4) {
        *portion = glm::vec4 { ParseVertex3(parts), portion->a };
      } else if (parts.length() == 5) {
        *portion = ParseVertex4(parts);
      } else {
        qWarning() << "LoadMaterialLibrary(GLObjectContext*, GLFunctions*, QString const&, QString const&):"
                   << "when parsing file"
                   << fileName
                   << "line"
                   << lineNo
                   << ":"
                   << "bad command (k commands expect 3 or 4 arguments)";
      }
    } else if (command == "d" || command == "tr") {
      if (parts.length() != 2) {
        qWarning() << "LoadMaterialLibrary(GLObjectContext*, GLFunctions*, QString const&, QString const&):"
                   << "when parsing file"
                   << fileName
                   << "line"
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
    } else if (command == "ns") {
      if (parts.length() != 2) {
        qWarning() << "LoadMaterialLibrary(GLObjectContext*, GLFunctions*, QString const&, QString const&):"
                   << "when parsing file"
                   << fileName
                   << "line"
                   << lineNo
                   << ":"
                   << "bad command (ns expects 1 argument)";
        continue;
      }

      float ns = parts[1].toFloat();
      currentMaterial->shine = ns;
    } else if (command == "map_ka" || command == "map_kd" || command == "map_ks"
               || command == "map_bump" || command == "bump") {
      if (parts.length() != 2) {
        qWarning() << "LoadMaterialLibrary(GLObjectContext*, GLFunctions*, QString const&, QString const&):"
                   << "when parsing file"
                   << fileName
                   << "line"
                   << lineNo
                   << ":"
                   << "bad command (map expects 1 argument)";
        continue;
      }

      cw::Texture2D const **portion;
      if (command == "map_ka") {
        qWarning() << "LoadMaterialLibrary(GLObjectContext*, GLFunctions*, QString const&, QString const&):"
                   << "when parsing file"
                   << fileName
                   << "line"
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
        if (!image.load(QStringLiteral("%1/%2").arg(basePath).arg(texturePath))) {
          qWarning() << "LoadMaterialLibrary(GLObjectContext*, GLFunctions*, QString const&, QString const&):"
                     << "when parsing file"
                     << fileName
                     << "line"
                     << lineNo
                     << ":"
                     << "cannot load texture:"
                     << texturePath;
          continue;
        }

        std::unique_ptr<Texture2D> texture = std::make_unique<Texture2D>(
          image,
          f,
          linearSampling,
          anisotropyFilter
        );
        *portion = ctx->AddTexture(texturePath, std::move(texture));
      }
    } else {
      qWarning() << "LoadMaterialLibrary(GLObjectContext*, GLFunctions*, QString const&, QString const&):"
                 << "when parsing file"
                 << fileName
                 << "line"
                 << lineNo
                 << ":"
                 << "unsupported command:"
                 << command;
    }
  }

  if (currentMaterial) {
    ctx->AddMaterial(currentMaterialName, std::move(currentMaterial));
  }
}

GLObject LoadObject(GLObjectContext *ctx,
                    GLFunctions *f,
                    QString const &basePath,
                    QString const &fileName,
                    bool linearSampling,
                    bool anisotropyFilter,
                    std::unique_ptr<VertexArrayObject> &&vao,
                    std::unique_ptr<VertexVBO> &&vbo)
{
  std::vector<glm::vec3> vertexCoords;
  std::vector<glm::vec3> vertexNormals;
  std::vector<glm::vec2> texCoords;
  std::vector<glm::vec3> tangents;
  std::vector<glm::vec3> biTangents;

  std::vector<Vertex> vertices;
  Material const* material = nullptr;

  QString fileContent = cw::ReadToString(
    QStringLiteral("%1/%2")
      .arg(basePath)
      .arg(fileName)
  );
  QStringList fileLines = fileContent.split('\n');

  size_t lineNo = 0;
  for (QString const& line : fileLines) {
    lineNo += 1;

    QString trimmed = line.trimmed();
    if (trimmed.isEmpty() || trimmed.startsWith('#')) {
      continue;
    }

    QStringList parts = trimmed.split(' ');
    QString command = parts[0].toLower();
    if (command == "o" || command == "s") {
      // do nothing
    } else if (command == "mtllib") {
      if (parts.length() != 2) {
        qWarning() << "LoadObject(GLObjectContext*, GLFunctions*, QString const&, QString const&, std::unique_ptr<VertexArrayObject>&&, std::unique_ptr<VertexVBO>&&):"
                   << "when parsing file"
                   << fileName
                   << "line"
                   << lineNo
                   << ":"
                   << "bad command (mtllib expects 1 argument)";
        continue;
      }

      QString materialLibPath = parts[1];
      LoadMaterialLibrary(ctx, f, basePath, materialLibPath, linearSampling, anisotropyFilter);
    } else if (command == "usemtl") {
      if (parts.length() != 2) {
        qWarning() << "LoadObject(GLObjectContext*, GLFunctions*, QString const&, QString const&, std::unique_ptr<VertexArrayObject>&&, std::unique_ptr<VertexVBO>&&):"
                   << "when parsing file"
                   << fileName
                   << "line"
                   << lineNo
                   << ":"
                   << "bad command (mtllib expects 1 argument)";
        continue;
      }

      material = ctx->GetMaterial(parts[1]);
      if (!material) {
        qWarning() << "LoadObject(GLObjectContext*, GLFunctions*, QString const&, QString const&, std::unique_ptr<VertexArrayObject>&&, std::unique_ptr<VertexVBO>&&):"
                   << "when parsing file"
                   << fileName
                   << "line"
                   << lineNo
                   << ":"
                   << "material not found:"
                   << parts[1];
      }
    } else if (command == "v" || command == "vn") {
      if (parts.length() != 4) {
        qWarning() << "LoadObject(GLObjectContext*, GLFunctions*, QString const&, QString const&, std::unique_ptr<VertexArrayObject>&&, std::unique_ptr<VertexVBO>&&):"
                   << "bad command (v/vn expects 3 arguments)";
        continue;
      }

      if (command == "v") {
        vertexCoords.push_back(ParseVertex3(parts));
      } else {
        vertexNormals.push_back(ParseVertex3(parts));
      }
    } else if (command == "vt") {
      if (parts.length() != 3) {
        qWarning() << "LoadObject(GLObjectContext*, GLFunctions*, QString const&, QString const&, std::unique_ptr<VertexArrayObject>&&, std::unique_ptr<VertexVBO>&&):"
                   << "when parsing file"
                   << fileName
                   << "line"
                   << lineNo
                   << ":"
                   << "bad command (vt expects 2 arguments)";
        continue;
      }

      texCoords.push_back(ParseVertex2(parts));
    } else if (command == "f") {
      if (parts.length() != 4) {
        qWarning() << "LoadObject(GLObjectContext*, GLFunctions*, QString const&, QString const&, std::unique_ptr<VertexArrayObject>&&, std::unique_ptr<VertexVBO>&&):"
                   << "when parsing file"
                   << fileName
                   << "line"
                   << lineNo
                   << ":"
                   << "bad command (f expects 3 arguments (i.e. 3 faces))";
        continue;
      }

      std::array<Vertex, 3> triangle {};
      bool computeNormal = false;
      for (int i = 1; i < 4; ++i) {
        QStringList vertexParts = parts[i].split('/');
        if (vertexParts.length() != 3) {
          qWarning() << "LoadObject(GLObjectContext*, GLFunctions*, QString const&, QString const&, std::unique_ptr<VertexArrayObject>&&, std::unique_ptr<VertexVBO>&&):"
                     << "when parsing file"
                     << fileName
                     << "line"
                     << lineNo
                     << ":"
                     << "bad command (f expects each argument to have 3 pieces)";
          continue;
        }

        int vertexCoordIndex = vertexParts[0].toInt() - 1;
        int texCoordIndex = vertexParts[1].toInt() - 1;
        int normalIndex = vertexParts[2].toInt() - 1;

        glm::vec3 vertexCoord = vertexCoords[vertexCoordIndex];
        glm::vec2 texCoord = texCoords[texCoordIndex];
        glm::vec3 normal;
        if (normalIndex >= 0) {
          normal = vertexNormals[normalIndex];
        } else {
          computeNormal = true;
          normal = glm::vec3(0.0);
        }

        triangle[i - 1] = {
          vertexCoord,
          normal,
          texCoord,
          glm::vec3(),
          glm::vec3()
        };
      }

      glm::vec3 v0 = triangle[0].vertexCoord;
      glm::vec3 v1 = triangle[1].vertexCoord;
      glm::vec3 v2 = triangle[2].vertexCoord;

      glm::vec3 edge1 = v1 - v0;
      glm::vec3 edge2 = v2 - v0;

      glm::vec2 uv0 = triangle[0].texCoord;
      glm::vec2 uv1 = triangle[1].texCoord;
      glm::vec2 uv2 = triangle[2].texCoord;

      glm::vec2 deltaUV1 = uv1 - uv0;
      glm::vec2 deltaUV2 = uv2 - uv0;

      float f1 = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
      glm::vec3 tangent = f1 * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
      glm::vec3 biTangent = f1 * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);

      for (int i = 0; i < 3; ++i) {
        triangle[i].tangent = tangent;
        triangle[i].biTangent = biTangent;
      }

      if (computeNormal) {
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        for (int i = 0; i < 3; ++i) {
          if (triangle[i].vertexNormal == glm::vec3(0.0, 0.0, 0.0)) {
            triangle[i].vertexNormal = normal;
          }
        }
      }

      for (int i = 0; i < 3; ++i) {
        vertices.push_back(triangle[i]);
      }
    } else {
      qWarning() << "LoadObject(GLObjectContext*, GLFunctions*, QString const&, QString const&, std::unique_ptr<VertexArrayObject>&&, std::unique_ptr<VertexVBO>&&):"
                 << "when parsing file"
                 << fileName
                 << "line"
                 << lineNo
                 << ":"
                 << "unsupported command: "
                 << command;
    }
  }

  if (vao == nullptr) {
    vao = std::make_unique<VertexArrayObject>(f);
  }
  vao->Bind(f);

  if (vbo == nullptr) {
    vbo = std::make_unique<VertexVBO>(f);
  }
  vbo->Bind(f);
  vbo->BufferData(f, vertices.data(), vertices.size(), GL_STATIC_DRAW);

  vao->Unbind(f);

  return GLObject {
    std::move(vao),
    std::move(vbo),
    static_cast<GLsizei>(vertices.size()),
    material
  };
}

static glm::vec2 ParseVertex2(QStringList const& commandBuf) {
  return {
    commandBuf[1].toFloat(),
    commandBuf[2].toFloat(),
  };
}

static glm::vec3 ParseVertex3(QStringList const& commandBuf) {
  return {
    commandBuf[1].toFloat(),
    commandBuf[2].toFloat(),
    commandBuf[3].toFloat()
  };
}

static glm::vec4 ParseVertex4(QStringList const& commandBuf) {
  return {
    commandBuf[1].toFloat(),
    commandBuf[2].toFloat(),
    commandBuf[3].toFloat(),
    commandBuf[4].toFloat()
  };
}

} // namespace cw
