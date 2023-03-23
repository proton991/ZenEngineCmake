#ifndef LOGGING_HPP
#define LOGGING_HPP
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>
#define SPDLOG_DEFAULT_PATTERN "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v"
#ifdef ZEN_DEBUG
#define VK_ASSERT(x)                                               \
  do {                                                             \
    if (!bool(x)) {                                                \
      spdlog::error("Vulkan error at {}:{}.", __FILE__, __LINE__); \
      abort();                                                     \
    }                                                              \
  } while (0)
#define VK_CHECK(err, op)                \
  do {                                   \
    if (err != VK_SUCCESS) {             \
      spdlog::error("Failed to {}", op); \
      abort();                           \
    }                                    \
  } while (0)
#else
#define VK_ASSERT(x) ((void)0)
#define VK_CHECK(err, op) ((void)0)
#endif

namespace zen::logger {
inline void set_default_pattern() {
  spdlog::set_pattern(SPDLOG_DEFAULT_PATTERN);
}

inline void set_list_pattern() {
  spdlog::set_pattern("\t\t\t\t %v");
}

template <typename T>
inline void trace(const T& msg) {
  spdlog::trace(msg);
}

template <typename... Args>
inline void trace(spdlog::format_string_t<Args...> fmt, Args&&... args) {
  spdlog::trace(fmt, std::forward<Args>(args)...);
}

template <typename T>
inline void info(const T& msg) {
  spdlog::info(msg);
}

template <typename... Args>
inline void info(spdlog::format_string_t<Args...> fmt, Args&&... args) {
  spdlog::info(fmt, std::forward<Args>(args)...);
}

template <typename T>
inline void warn(const T& msg) {
  spdlog::warn(msg);
}

template <typename... Args>
inline void warn(spdlog::format_string_t<Args...> fmt, Args&&... args) {
  spdlog::warn(fmt, std::forward<Args>(args)...);
}

template <typename T>
inline void error(const T& msg) {
  spdlog::error(msg);
}

template <typename... Args>
inline void error(spdlog::format_string_t<Args...> fmt, Args&&... args) {
  spdlog::error(fmt, std::forward<Args>(args)...);
}

template <typename T>
inline void critical(const T& msg) {
  spdlog::critical(msg);
}

template <typename... Args>
inline void critical(spdlog::format_string_t<Args...> fmt, Args&&... args) {
  spdlog::critical(fmt, std::forward<Args>(args)...);
}

template <typename T>
inline void LogList(const std::vector<T>& l) {
  logger::set_list_pattern();
  for (const auto& item : l) {
    spdlog::info("{}", item);
  }
  logger::set_default_pattern();
}
}  // namespace zen::logger

#endif  //LOGGING_HPP
