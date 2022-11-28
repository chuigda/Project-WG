#include "cwglx/MeshLoader.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>

namespace cw {

#define PARSE_DOUBLE(v, s, r) \
  double v = (s).toDouble(&(r)); \
  if (!(r)) { \
    qWarning() << "error processing file:" \
               << meshFile      \
               << "line:"       \
               << lineNo        \
               << "(invalid v-line)"; \
    return; \
  }

#define PARSE_UINT(v, s, r) \
  std::size_t v = (s).toUInt(&(r)); \
  if (!(r)) { \
    qWarning() << "error processing file:" \
               << meshFile    \
               << "line:"     \
               << lineNo      \
               << "(invalid f-line)"; \
    return; \
  }

static void ProcessMeshLine(PlainTriangles* triangles,
                            std::vector<Vertex>& verticesPool,
                            const QString& meshLine,
                            const char *meshFile,
                            size_t lineNo)
{
  if (meshLine.isEmpty() || meshLine.startsWith('#')) {
    return;
  }

  QStringList parts = meshLine.split(' ');
  if (parts.length() < 0) {
    qWarning() << "error processing file:" << meshFile << "line:" << lineNo;
    return;
  }

  if (parts[0] == "v") {
    if (parts.length() != 4) {
      qWarning() << "error processing file:"
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

    verticesPool.emplace_back(x, y, z);
  } else if (parts[0] == "f") {
    if (parts.length() != 4) {
      qWarning() << "error processing file:"
                 << meshFile
                 << "line:"
                 << lineNo
                 << "(invalid f-line)";
      return;
    }

    bool success;
    PARSE_UINT(v1, parts[1], success)
    PARSE_UINT(v2, parts[2], success)
    PARSE_UINT(v3, parts[3], success)

    triangles->AddTriangle(Triangle {
      verticesPool[v1 - 1],
      verticesPool[v2 - 1],
      verticesPool[v3 - 1]
    });
  } else if (parts[0] == "s" || parts[0] == "o") {
    // do nothing, since we put only one object into one mesh file
  } else {
    qWarning() << "warning: when processing file:"
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

std::unique_ptr<PlainTriangles> LoadMesh(const char *meshFile) {
  QFile file(meshFile);
  if (!file.open(QIODevice::ReadOnly)) {
    qWarning() << "cannot open mesh file:" << meshFile;
    return nullptr;
  }

  QTextStream textStream(&file);
  std::unique_ptr<PlainTriangles> triangles =
    std::make_unique<PlainTriangles>();
  std::vector<Vertex> verticesPool;
  std::size_t lineNo = 1;
  while (!textStream.atEnd()) {
    QString line = textStream.readLine().trimmed();
    ProcessMeshLine(triangles.get(), verticesPool, line, meshFile, lineNo);
    lineNo += 1;
  }

  return triangles;
}

} // namespace cw
