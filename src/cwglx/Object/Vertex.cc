#include "include/cwglx/Object/Vertex.h"
#include "include/cwglx/Base/VertexBufferObjectImpl.h"

namespace cw {

template class CW_DEFINE_VBO_TYPE(PlainVertex, vertexCoord);
template class CW_DEFINE_VBO_TYPE(SimpleVertex, vertexCoord, vertexNormal);
template class CW_DEFINE_VBO_TYPE(Vertex, vertexCoord, vertexNormal, texCoord, tangent, biTangent);

} // namespace cw
