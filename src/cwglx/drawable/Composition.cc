#include "cwglx/drawable/Composition.h"

#include <QOpenGLFunctions_2_0>

namespace cw {

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"
TranslationStep::TranslationStep(const Vector &translation) noexcept
  : m_StepKind(Translation),
    m_Vector(VectorF::Downscale(translation))
{}
#pragma clang diagnostic pop

TranslationStep::TranslationStep(const Vector &rotationVector,
                                 GLfloat angle) noexcept
  : m_StepKind(Rotation),
    m_Vector(VectorF::Downscale(rotationVector)),
    m_RotationValue(angle)
{}

void TranslationStep::ApplyTranslation(QOpenGLFunctions_2_0 *f) const noexcept {
  const auto &[x, y, z] = m_Vector.GetRepr();
  if (m_StepKind == Translation) {
    f->glTranslatef(x, y, z);
  } else {
    f->glRotatef(m_RotationValue, x, y, z);
  }
}

PositionedDrawable::
PositionedDrawable(std::vector<TranslationStep> &&translationSteps,
                   const Drawable *drawable)
  : m_TranslationSteps(std::move(translationSteps)),
    m_Drawable(drawable)
{}

PositionedDrawable::~PositionedDrawable() = default;

void PositionedDrawable::Draw(QOpenGLFunctions_2_0 *f) const noexcept {
  f->glPushMatrix();
  for (const auto &step : m_TranslationSteps) {
    step.ApplyTranslation(f);
  }
  m_Drawable->Draw(f);
  f->glPopMatrix();
}

Composition::Composition(std::vector<TranslationStep> &&translationSteps,
                         std::vector<Drawable const*> &&drawables)
  : m_TranslationSteps(std::move(translationSteps)),
    m_Drawables(drawables)
{}

Composition::~Composition() = default;

void Composition::Draw(QOpenGLFunctions_2_0 *f) const noexcept {
  f->glPushMatrix();
  for (const auto &step : m_TranslationSteps) {
    step.ApplyTranslation(f);
  }

  for (const auto &drawable : m_Drawables) {
    drawable->Draw(f);
  }
  f->glPopMatrix();
}

} // namespace cw
