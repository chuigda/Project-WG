#ifndef PROJECT_WG_COMPOSITION_H
#define PROJECT_WG_COMPOSITION_H

#include <vector>
#include "cwglx/Drawable.h"
#include "cwglx/Vertex.h"
#include "cwglx/Material.h"
#include "util/Derive.h"

namespace cw {

class TranslationStep {
public:
  enum StepKind {
    Translation,
    Rotation
  };

  explicit TranslationStep(const Vector& translation) noexcept;

  TranslationStep(const Vector& rotationVector, GLfloat angle) noexcept;

  void ApplyTranslation(QOpenGLFunctions_2_0 *f) const noexcept;

private:
  StepKind m_StepKind;
  VectorF m_Vector;
  GLfloat m_RotationValue;
};

class PositionedDrawable final : Drawable {
public:
  PositionedDrawable(std::vector<TranslationStep>&& translationSteps,
                     Drawable const* drawable);

  ~PositionedDrawable() final;

  void Draw(QOpenGLFunctions_2_0 *f) const noexcept final;

  CW_DERIVE_UNCOPYABLE(PositionedDrawable)
  CW_DERIVE_UNMOVABLE(PositionedDrawable)

private:
  std::vector<TranslationStep> m_TranslationSteps;
  Drawable const* m_Drawable;
};

class MaterializedDrawable final : Drawable {
public:
  MaterializedDrawable(const Material* material,
                       std::vector<Drawable const*>&& drawables);

  ~MaterializedDrawable() final;

  void Draw(QOpenGLFunctions_2_0 *f) const noexcept final;

  CW_DERIVE_UNCOPYABLE(MaterializedDrawable)
  CW_DERIVE_UNMOVABLE(MaterializedDrawable)

private:
  Material const *m_Material;
  std::vector<Drawable const*> m_Drawables;
};

class Composition final : Drawable {
public:
  Composition(std::vector<TranslationStep>&& translationSteps,
              std::vector<Drawable const*>&& drawables);

  ~Composition() final;

  void Draw(QOpenGLFunctions_2_0 *f) const noexcept final;

  CW_DERIVE_UNCOPYABLE(Composition)
  CW_DERIVE_UNMOVABLE(Composition)

private:
  std::vector<TranslationStep> m_TranslationSteps;
  std::vector<Drawable const*> m_Drawables;
};

} // namespace cw

#endif // PROJECT_WG_COMPOSITION_H
