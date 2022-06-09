#include "pl2/pl2q.h"

#include <QStringList>

namespace pl2 {

ProgramContext::~ProgramContext() = default;

int CommandHandler::Call(int commandIdx,
                         const QString &command,
                         const QStringList &arguments) const {
  return m_HandlerFn.value()(commandIdx, command, arguments);
}

bool CommandHandler::Test(int commandIdx,
                          const QString &command,
                          const QStringList &arguments) const {
  if (m_CommandName.has_value()) {
    return m_CommandName.value() == command;
  } else {
    return m_RouterFn.value()(commandIdx, command, arguments);
  }
}

} // namespace pl2
