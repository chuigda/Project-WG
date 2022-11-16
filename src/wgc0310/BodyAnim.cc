#include "wgc0310/BodyAnim.h"
#include "wgc0310/BodyStatus.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>

namespace wgc0310 {

AnimationSection::AnimationSection() noexcept
  : totalFrameCount(0)
{}

void AnimationSection::AddCommand(AnimationCommand command) noexcept {
  totalFrameCount = std::max(totalFrameCount, command.frameCount);
  commandList.push_back(command);
}

void AnimationSection::ApplyAnimationFrame(BodyStatus *bodyStatus,
                                           std::size_t frame) const noexcept {
  for (const auto &command : commandList) {
    if (frame >= command.frameCount) {
      continue;
    }

    ArmStatus &arm = command.isLeft ? bodyStatus->leftArmStatus
                                    : bodyStatus->rightArmStatus;
    arm.rotation[command.rotationAxisIndex] += command.frameRotation;
  }
}

#define PARSE_DOUBLE(v, s, r) \
  double v = (s).toDouble(&(r)); \
  if (!(r)) { \
    qDebug() << "error processing file:" \
             << fileName      \
             << "line:"       \
             << lineNo        \
             << "(invalid float value)"; \
    continue; \
  }

#define PARSE_UINT(v, s, r) \
  std::size_t v = (s).toUInt(&(r)); \
  if (!(r)) { \
    qDebug() << "error processing file:" \
             << fileName    \
             << "line:"     \
             << lineNo      \
             << "(invalid int value)"; \
    continue; \
  }

std::unique_ptr<BodyAnimation> LoadBodyAnimation(const char *fileName) {
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly)) {
    qDebug() << "cannot open animation file:" << fileName;
    return nullptr;
  }

  QTextStream textStream(&file);
  std::unique_ptr<BodyAnimation> animation = std::make_unique<BodyAnimation>();

  std::size_t lineNo = 0;
  while (!textStream.atEnd()) {
    QString line = textStream.readLine().trimmed();
    lineNo += 1;

    if (line.isEmpty() || line.startsWith('#')) {
      continue;
    }

    if (line == "s") {
      animation->emplace_back();
      continue;
    }

    QStringList parts = line.split(' ');
    if (parts.length() == 4 && (parts[0] == "l" || parts[0] == "r")) {
      if (animation->empty()) {
        qDebug() << "error loading animation:"
                 << fileName
                 << "line:"
                 << lineNo
                 << "(cannot define rotation without section)";
        continue;
      }

      bool isLeft = parts[0] == "l";
      bool success;
      PARSE_UINT(axisIndex, parts[1], success)
      PARSE_DOUBLE(rotation, parts[2], success)
      PARSE_UINT(frameCount, parts[3], success)

      animation->back().AddCommand(AnimationCommand {
        .isLeft = isLeft,
        .rotationAxisIndex = axisIndex,
        .frameRotation =
          static_cast<GLfloat>(rotation / static_cast<double>(frameCount)),
        .frameCount = frameCount
      });
    } else {
      qDebug() << "error loading animation:"
               << fileName
               << "line:"
               << lineNo
               << "(command not understood)";
    }
  }

  return animation;
}

#undef PARSE_DOUBLE
#undef PARSE_INT

} // namespace wgc0310
