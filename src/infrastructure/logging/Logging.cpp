#include "infrastructure/logging/Logging.h"

#include <QString>
#include <QtGlobal>

#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace {
bool g_initialized = false;

spdlog::level::level_enum parseLogLevel(const QByteArray &levelValue)
{
    const std::string level = levelValue.toLower().toStdString();
    if (level == "trace") {
        return spdlog::level::trace;
    }
    if (level == "debug") {
        return spdlog::level::debug;
    }
    if (level == "info") {
        return spdlog::level::info;
    }
    if (level == "warn" || level == "warning") {
        return spdlog::level::warn;
    }
    if (level == "error") {
        return spdlog::level::err;
    }
    if (level == "critical") {
        return spdlog::level::critical;
    }
    if (level == "off") {
        return spdlog::level::off;
    }
    return spdlog::level::info;
}

spdlog::level::level_enum resolveDefaultLevel()
{
    const QByteArray envLevel = qgetenv("APP_LOG_LEVEL");
    if (envLevel.isEmpty()) {
        return spdlog::level::info;
    }
    return parseLogLevel(envLevel);
}

void qtMessageToSpdlog(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    const QByteArray localMsg = msg.toLocal8Bit();
    const std::string_view payload(localMsg.constData(), static_cast<size_t>(localMsg.size()));
    const char *file = context.file != nullptr ? context.file : "unknown";
    const char *function = context.function != nullptr ? context.function : "unknown";

    switch (type) {
    case QtDebugMsg:
        spdlog::debug("[Qt] {} ({}:{} {})", payload, file, context.line, function);
        break;
    case QtInfoMsg:
        spdlog::info("[Qt] {} ({}:{} {})", payload, file, context.line, function);
        break;
    case QtWarningMsg:
        spdlog::warn("[Qt] {} ({}:{} {})", payload, file, context.line, function);
        break;
    case QtCriticalMsg:
        spdlog::error("[Qt] {} ({}:{} {})", payload, file, context.line, function);
        break;
    case QtFatalMsg:
        spdlog::critical("[Qt] {} ({}:{} {})", payload, file, context.line, function);
        spdlog::shutdown();
        std::abort();
    }
}
} // namespace

void app::logging::initialize()
{
    if (g_initialized) {
        return;
    }

    std::filesystem::create_directories("logs");

    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/app.log",
                                                                           5 * 1024 * 1024,
                                                                           3);
    std::vector<spdlog::sink_ptr> sinks{consoleSink, fileSink};

    auto logger = std::make_shared<spdlog::logger>("app", sinks.begin(), sinks.end());
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");
    logger->set_level(resolveDefaultLevel());
    logger->flush_on(spdlog::level::warn);

    spdlog::set_default_logger(std::move(logger));
    g_initialized = true;
}

void app::logging::installQtMessageHandler()
{
    qInstallMessageHandler(qtMessageToSpdlog);
}

void app::logging::shutdown()
{
    qInstallMessageHandler(nullptr);
    spdlog::shutdown();
    g_initialized = false;
}
