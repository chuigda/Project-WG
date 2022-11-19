#ifndef LOGGER_H
#define LOGGER_H

#include <cstdio>
#include <cstdint>
#include <cinttypes>
#include <cassert>
#include <string>
#include <unistd.h>

inline thread_local pid_t tid = gettid();

namespace sona {

class Logger {
public:
  enum LogLevel {
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Critical = 5,
    Never = 6
  };

  static Logger& Instance() {
    static Logger instance;
    return instance;
  }

  void SetEnabledLogLevel(LogLevel enabledLogLevel = LogLevel::Warning) {
    m_EnabledLogLevel = enabledLogLevel;
  }

  void SetColorEnabled(bool enabled = true) {
    m_ColorEnabled = enabled;
  }

  template <typename ...Args>
  void Log(LogLevel level,
           const char *component,
           Args &&...args) {
    if (level < m_EnabledLogLevel) {
      return;
    }

    if (m_ColorEnabled) {
      switch (level) {
        case LogLevel::Debug:
          std::fprintf(stderr, "\033[36m[DEBUG] (%u) (%s): ", tid, component);
          break;

        case LogLevel::Info:
          std::fprintf(stderr, "\033[32m[INFO] (%u) (%s): ", tid, component);
          break;

        case LogLevel::Warning:
          std::fprintf(stderr, "\033[93m[WARN] (%u) (%s): ", tid, component);
          break;

        case LogLevel::Error:
          std::fprintf(stderr, "\033[31m[ERROR] (%u) (%s): ", tid, component);
          break;

        case LogLevel::Critical:
          std::fprintf(stderr, "\033[91m[CRITICAL] (%u) (%s): ", tid, component);
          break;

        default:
          assert(false && "Wrong option");
      }
    } else {
      switch (level) {
        case LogLevel::Debug:
          std::fprintf(stderr, "[DEBUG] (%u) (%s): ", tid, component);
          break;

        case LogLevel::Info:
          std::fprintf(stderr, "[INFO] (%u) (%s): ", tid, component);
          break;

        case LogLevel::Warning:
          std::fprintf(stderr, "[WARN] (%u) (%s): ", tid, component);
          break;

        case LogLevel::Error:
          std::fprintf(stderr, "[ERROR] (%u) (%s): ", tid, component);
          break;

        case LogLevel::Critical:
          std::fprintf(stderr, "[CRITICAL] (%u) (%s): ", tid, component);
          break;

        default:
          assert(false && "Wrong option");
      }
    }

    LogImpl(std::forward<Args>(args)...);

    if (m_ColorEnabled) {
      std::fputs("\033[39;49m", stderr);
    }
    std::fputc('\n', stderr);
  }

private:
  void LogOnce(const std::string& s) {
    std::fprintf(stderr, "%s", s.c_str());
  }

  void LogOnce(const char *s) {
    std::fprintf(stderr, "%s", s);
  }

  template <typename T>
  std::enable_if_t<std::is_integral<T>::value
                   && std::is_signed<T>::value, void>
  LogOnce(const T& t) {
    std::fprintf(stderr, "%" PRId64, static_cast<int64_t>(t));
  }

  template <typename T>
  std::enable_if_t<std::is_integral<T>::value
                   && std::is_unsigned<T>::value, void>
  LogOnce(const T& t) {
    std::fprintf(stderr, "%" PRIu64, static_cast<uint64_t>(t));
  }

  template <typename T>
  std::enable_if_t<std::is_floating_point<T>::value, void>
  LogOnce(const T& t) {
    std::fprintf(stderr, "%g", static_cast<double>(t));
  }

  template <typename T>
  std::void_t<std::is_same<std::string,
    decltype(DebugString(std::declval<T>()))>>
  LogOnce(T&& t) {
    std::fprintf(stderr, "%s", DebugString(std::forward<T>(t)).c_str());
  }

  template <typename T, typename... Args>
  void LogImpl(T &&t, Args &&...args) {
    LogOnce(std::forward<T>(t));
    LogImpl(std::forward<Args>(args)...);
  }

  void LogImpl() {}

  Logger() = default;

  LogLevel m_EnabledLogLevel = LogLevel::Info;
  bool m_ColorEnabled = false;
};

inline void LoggerSetColorEnabled(bool enabled = true) {
  Logger::Instance().SetColorEnabled(enabled);
}

inline void LoggerSetEnabledLogLevel(Logger::LogLevel level = Logger::Warning) {
  Logger::Instance().SetEnabledLogLevel(level);
}

template <typename ...Args>
void LogDebug(const char *component, Args &&...args) {
  Logger::Instance().Log(Logger::Debug, component,
                         std::forward<Args>(args)...);
}

template <typename ...Args>
void LogInfo(const char *component, Args &&...args) {
  Logger::Instance().Log(Logger::Info, component,
                         std::forward<Args>(args)...);
}

template <typename ...Args>
void LogWarn(const char *component, Args &&...args) {
  Logger::Instance().Log(Logger::Warning, component,
                         std::forward<Args>(args)...);
}

template <typename ...Args>
void LogError(const char *component, Args &&...args) {
  Logger::Instance().Log(Logger::Error, component,
                         std::forward<Args>(args)...);
}

template <typename ...Args>
void LogCritical(const char *component, Args &&...args) {
  Logger::Instance().Log(Logger::Critical, component,
                         std::forward<Args>(args)...);
}

} // namespace sona

#endif // LOGGER_H
