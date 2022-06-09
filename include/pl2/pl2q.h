#ifndef PROJECT_WG_PL2Q_H
#define PROJECT_WG_PL2Q_H

#include <functional>
#include <optional>

#include <QString>

namespace pl2 {

class ProgramContext {
public:
  virtual ~ProgramContext() = 0;
};

using RouterFunction =
    std::function<bool(int, const QString&, const QStringList&)>;
using HandlerFunction =
    std::function<int(int, const QString&, const QStringList&)>;

class CommandHandler {
public:
  CommandHandler(const QString& commandName,
                 std::optional<HandlerFunction> &&handlerFn)
    : m_CommandName(commandName),
      m_HandlerFn(std::move(handlerFn))
  {}

  CommandHandler(RouterFunction &&routerFunction,
                 std::optional<HandlerFunction> &&handlerFn)
    : m_RouterFn(std::move(routerFunction)),
      m_HandlerFn(std::move(handlerFn))
  {}

  [[nodiscard]]
  int Call(int commandIdx,
           const QString& command,
           const QStringList& arguments) const;

  [[nodiscard]]
  bool Test(int commandIdx,
            const QString& command,
            const QStringList& arguments) const;

private:
  std::optional<QString> m_CommandName;
  std::optional<RouterFunction> m_RouterFn;
  std::optional<HandlerFunction> m_HandlerFn;
};

} // namespace pl2

#endif // PROJECT_WG_PL2Q_H
