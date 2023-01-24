#include "wgc0310/api/Attachment.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include "cwglx/GLImpl.h"
#include "cwglx/MeshLoader.h"
#include "cwglx/Material.h"
#include "cwglx/Texture.h"

struct EmitterStatus {
  enum class Status {
    Stopped,
    Starting,
    Started
  } status = Status::Stopped;
  std::size_t currentFrame = 0;
  bool stopStage = false;
};

class Emitter final : public wgc0310::WGAPIAttachment {
public:
  Emitter() : m_Widget(new QWidget(nullptr, Qt::Window)) {
    m_Widget->setWindowTitle("光线发射器 - 控制器");

    QHBoxLayout *layout = new QHBoxLayout();
    m_Widget->setLayout(layout);

    QPushButton *startButton = new QPushButton("开始");
    QPushButton *stopButton = new QPushButton("停止");
    layout->addWidget(startButton);

    QWidget *placeholder = new QWidget();
    placeholder->setFixedWidth(startButton->sizeHint().width());
    layout->addWidget(placeholder);

    layout->addWidget(stopButton);

    m_Widget->setFixedSize(m_Widget->sizeHint());

    QObject::connect(startButton, &QPushButton::clicked, m_Widget, [this] {
      if (m_Status.status != EmitterStatus::Status::Stopped) {
        return;
      }

      m_Status.status = EmitterStatus::Status::Starting;
      m_Status.currentFrame = 0;
    });
    QObject::connect(stopButton, &QPushButton::clicked, m_Widget, [this] {
      m_Status.status = EmitterStatus::Status::Stopped;
    });
  }

  ~Emitter() final {
    delete m_Widget;
  }

  const char *GetName() noexcept final {
    return "光线发射器";
  }

  QWidget *GetControlWidget() noexcept final {
    return m_Widget;
  }

  void NextTick() noexcept final {
    if (m_Status.status == EmitterStatus::Status::Stopped) {
      return;
    }

    m_Status.currentFrame += 1;
    if (m_Status.status == EmitterStatus::Status::Starting
        && m_Status.currentFrame >= 150) {
      m_Status.status = EmitterStatus::Status::Started;
      m_Status.currentFrame = 0;
    }
  }

  bool Initialize(GLFunctions *f) noexcept final {
    m_Emitter = cw::LoadMesh(":/emitter.common");
    m_SideCover = cw::LoadMesh(":/emitter-side-cover.common");
    m_PowerSource = cw::LoadMesh(":/emitter-power-source.common");

    m_Emitter->PreInitialize(f);
    m_SideCover->PreInitialize(f);
    m_PowerSource->PreInitialize(f);

    m_SpaciumTexture = std::make_unique<cw::Texture2D>(QImage(":/spacium.png"), f);

    m_PlasticMaterial = cw::GetPlasticMaterial();
    m_BlackPlasticMaterial = cw::GetBlackPlasticMaterial();
    return true;
  }

  void Draw(GLFunctions *f) noexcept final {
    f->glPushAttrib(GL_ALL_ATTRIB_BITS);
    f->glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);

    f->glColor3f(1.0f, 1.0f, 1.0f);
    m_PlasticMaterial->Apply(f);
    m_Emitter->Draw(f);

    if (m_Status.status == EmitterStatus::Status::Stopped) {
      m_BlackPlasticMaterial->Apply(f);
      m_SideCover->Draw(f);
      m_PowerSource->Draw(f);
    } else {
      f->glDisable(GL_LIGHTING);
      f->glColor3f(0.5f, 1.0f, 1.0f);
      m_SideCover->Draw(f);
      m_PowerSource->Draw(f);

      f->glDisable(GL_CULL_FACE);
      f->glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      std::size_t validFrame = m_Status.currentFrame % 150;
      // 第一阶段
      if ((m_Status.status == EmitterStatus::Status::Starting) || validFrame == 0) {
        GLfloat lightRear = 4.425f + m_Status.currentFrame * 2.0f;
        GLfloat texCoordMax = m_Status.currentFrame / 150.0f;

        m_SpaciumTexture->BeginTexture(f);
        f->glBegin(GL_QUADS);

        f->glTexCoord2f(0.0f, 0.0f);
        f->glVertex2f(4.0f, lightRear);

        f->glTexCoord2f(0.0f, 1.0f);
        f->glVertex2f(17.0f, lightRear);

        f->glTexCoord2f(texCoordMax, 1.0f);
        f->glVertex2f(17.0f, 4.425f);

        f->glTexCoord2f(texCoordMax, 0.0f);
        f->glVertex2f(4.0f, 4.425f);
        f->glEnd();
      } else {
        // 第二阶段
        GLfloat lightRear = 4.425f + 300.0f;
        GLfloat lightMid = 4.425f + validFrame * 2.0f;
        GLfloat texCoordMid = validFrame / 150.0f;

        m_SpaciumTexture->BeginTexture(f);
        // 远处的光线
        f->glBegin(GL_QUADS);
        {
          f->glTexCoord2f(texCoordMid, 0.0f);
          f->glVertex2f(4.0f, lightRear);

          f->glTexCoord2f(texCoordMid, 1.0f);
          f->glVertex2f(17.0f, lightRear);

          f->glTexCoord2f(1.0f, 1.0f);
          f->glVertex2f(17.0f, lightMid);

          f->glTexCoord2f(1.0f, 0.0f);
          f->glVertex2f(4.0f, lightMid);
        }
        f->glEnd();

        // 近处的光线
        f->glBegin(GL_QUADS);
        {
          f->glTexCoord2f(0.0f, 0.0f);
          f->glVertex2f(4.0f, lightMid);

          f->glTexCoord2f(0.0f, 1.0f);
          f->glVertex2f(17.0f, lightMid);

          f->glTexCoord2f(texCoordMid, 1.0f);
          f->glVertex2f(17.0f, 4.425f);

          f->glTexCoord2f(texCoordMid, 0.0f);
          f->glVertex2f(4.0f, 4.425f);
        }
        f->glEnd();
      }
    }
    f->glPopAttrib();
    f->glPopClientAttrib();
  }

  void Delete(GLFunctions *f) noexcept final {
    m_Emitter->Delete(f);
    m_SideCover->Delete(f);
    m_PowerSource->Delete(f);
    m_SpaciumTexture->DeleteTexture(f);
  }

private:
  QWidget *m_Widget;
  EmitterStatus m_Status;

  std::unique_ptr<cw::PlainMesh> m_Emitter;
  std::unique_ptr<cw::PlainMesh> m_SideCover;
  std::unique_ptr<cw::PlainMesh> m_PowerSource;
  std::unique_ptr<cw::Texture2D> m_SpaciumTexture;
  cw::StandardMaterial const* m_PlasticMaterial;
  cw::StandardMaterial const* m_BlackPlasticMaterial;
};

extern "C" {
std::uint32_t WGAPI GetWGAPIVersion() {
  return WGAPI_VERSION;
}

wgc0310::WGAPIAttachment * WGAPI LoadAttachment() {
  return new Emitter();
}
} // extern "C"
