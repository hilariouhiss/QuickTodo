#pragma once

#include "spdlog/common.h"
#include "spdlog/spdlog.h"

#include <utility>

namespace app::logging {
/**
 * @brief Configures the shared spdlog logger and its sinks.
 */
void initialize();

/**
 * @brief Routes Qt logging through the shared spdlog logger.
 */
void installQtMessageHandler();

/**
 * @brief Flushes and tears down global logging state.
 */
void shutdown();

template<typename... Args>
inline void debug(spdlog::format_string_t<Args...> fmt, Args &&...args)
{
    spdlog::debug(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void info(spdlog::format_string_t<Args...> fmt, Args &&...args)
{
    spdlog::info(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void warn(spdlog::format_string_t<Args...> fmt, Args &&...args)
{
    spdlog::warn(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void error(spdlog::format_string_t<Args...> fmt, Args &&...args)
{
    spdlog::error(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void critical(spdlog::format_string_t<Args...> fmt, Args &&...args)
{
    spdlog::critical(fmt, std::forward<Args>(args)...);
}
} // namespace app::logging
