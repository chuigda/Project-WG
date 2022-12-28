#include "cwglx/Object/WavefrontLoader.h"
#include "util/FileUtil.h"

#include <glm/vec3.hpp>
#include <QString>
#include <QList>
#include <QDebug>

namespace cw {

#define PARSE_DOUBLE(v, s, r) \
  double v = (s).toDouble(&(r)); \
  if (!(r)) { \
    warning << "error processing file:" \
               << meshFile      \
               << "line:"       \
               << lineNo        \
               << "(invalid v-line)"; \
    return; \
  }

#define PARSE_UINT(v, s, r) \
  std::size_t v = (s).toUInt(&(r)); \
  if (!(r)) { \
    warning << "error processing file:" \
            << meshFile    \
            << "line:"     \
            << lineNo      \
            << "(invalid f-line or l-line)"; \
    return; \
  }

static void ProcessMeshStmt(std::vector<SimpleVertex> *triangles,
                            std::vector<glm::vec3>& verticesPool,
                            const QString& meshLine,
                            const QString& meshFile,
                            size_t lineNo,
                            double ratio)
{
  if (meshLine.isEmpty() || meshLine.startsWith('#')) {
    return;
  }

  auto warning = qWarning().noquote();

  QStringList parts = meshLine.split(' ');
  if (parts.length() < 0) {
    warning << "error processing file:" << meshFile << "line:" << lineNo;
    return;
  }

  if (parts[0] == "v") {
    if (parts.length() != 4) {
      warning << "error processing file:"
              << meshFile
              << "line:"
              << lineNo
              << "(invalid v-line)";
      return;
    }

    bool success;
    PARSE_DOUBLE(x, parts[1], success)
    PARSE_DOUBLE(y, parts[2], success)
    PARSE_DOUBLE(z, parts[3], success)

    verticesPool.emplace_back(x * ratio, y * ratio, z * ratio);
  } else if (parts[0] == "f") {
    if (parts.length() != 4) {
      warning << "error processing file:"
              << meshFile
              << "line:"
              << lineNo
              << "(invalid f-line)";
      return;
    }

    bool success;
    PARSE_UINT(idx1, parts[1], success)
    PARSE_UINT(idx2, parts[2], success)
    PARSE_UINT(idx3, parts[3], success)

    glm::vec3 v1 = verticesPool[idx1 - 1];
    glm::vec3 v2 = verticesPool[idx2 - 1];
    glm::vec3 v3 = verticesPool[idx3 - 1];

    glm::vec3 vn = glm::cross(v2 - v1, v3 - v2);

    triangles->push_back(SimpleVertex { .vertexCoord = v1, .vertexNormal = vn });
    triangles->push_back(SimpleVertex { .vertexCoord = v2, .vertexNormal = vn });
    triangles->push_back(SimpleVertex { .vertexCoord = v3, .vertexNormal = vn });
  } else if (parts[0] == "s" || parts[0] == "o") {
    // do nothing, since we put only one object into one common file
  } else {
    warning << "warning: when processing file:"
            << meshFile
            << "line:"
            << lineNo
            << "(unknown command"
            << parts[0]
            << ")";
    return;
  }
}

static void ProcessLineMeshStmt(std::vector<PlainVertex> *lines,
                                std::vector<glm::vec3>& verticesPool,
                                const QString& meshLine,
                                const QString& meshFile,
                                size_t lineNo,
                                double ratio)
{
  if (meshLine.isEmpty() || meshLine.startsWith('#')) {
    return;
  }

  auto warning = qWarning().noquote();

  QStringList parts = meshLine.split(' ');
  if (parts.length() < 0) {
    warning << "error processing file:" << meshFile << "line:" << lineNo;
    return;
  }

  if (parts[0] == "v") {
    if (parts.length() != 4) {
      warning << "error processing file:"
              << meshFile
              << "line:"
              << lineNo
              << "(invalid v-line)";
      return;
    }

    bool success;
    PARSE_DOUBLE(x, parts[1], success)
    PARSE_DOUBLE(y, parts[2], success)
    PARSE_DOUBLE(z, parts[3], success)

    verticesPool.emplace_back(x * ratio, y * ratio, z * ratio);
  } else if (parts[0] == "l") {
    if (parts.length() != 3) {
      warning << "error processing file:"
              << meshFile
              << "line:"
              << lineNo
              << "(invalid l-line)";
      return;
    }

    bool success;
    PARSE_UINT(idx1, parts[1], success)
    PARSE_UINT(idx2, parts[2], success)

    lines->push_back(PlainVertex { .vertexCoord = verticesPool[idx1 - 1] });
    lines->push_back(PlainVertex { .vertexCoord = verticesPool[idx2 - 1] });
  } else if (parts[0] == "s" || parts[0] == "o") {
    // do nothing, since we put only one object into one common file
  } else {
    warning << "warning: when processing file:"
            << meshFile
            << "line:"
            << lineNo
            << "(unknown command"
            << parts[0]
            << ")";
    return;
  }
}

#undef PARSE_DOUBLE
#undef PARSE_INT

std::vector<SimpleVertex> LoadMesh(QString const& fileName, double ratio) {
  QStringList content = ReadToString(fileName).split('\n');
  std::vector<SimpleVertex> triangles;
  std::vector<glm::vec3> verticesPool;
  for (qsizetype i = 0; i < content.length(); ++i) {
    ProcessMeshStmt(&triangles,
                    verticesPool,
                    content[i],
                    fileName,
                    i,
                    ratio);
  }
  return triangles;
}

std::vector<PlainVertex> LoadLineMesh(QString const& fileName, double ratio) {
  QStringList content = ReadToString(fileName).split('\n');
  std::vector<PlainVertex> lines;
  std::vector<glm::vec3> verticesPool;
  for (qsizetype i = 0; i < content.length(); ++i) {
    ProcessLineMeshStmt(&lines,
                        verticesPool,
                        content[i],
                        fileName,
                        i,
                        ratio);
  }
  return lines;
}

} // namespace cw
