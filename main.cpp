#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#include "app/AppContainer.h"
#include "infrastructure/logging/Logging.h"

int main(int argc, char *argv[])
{
    app::logging::initialize();
    app::logging::installQtMessageHandler();
    app::logging::info("Application bootstrapping");

    QGuiApplication app(argc, argv);

    QQuickStyle::setStyle("Fusion");

    QQmlApplicationEngine engine;
    AppContainer appContainer;
    if (appContainer.databaseReady()) {
        app::logging::info("Database ready");
    } else {
        app::logging::error("Database not ready: {}", appContainer.databaseError().toStdString());
    }
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.rootContext()->setContextProperty("mainViewModel", appContainer.mainViewModel());
    app::logging::info("Loading QML module: underlying_test_platform/Main");
    engine.loadFromModule("underlying_test_platform", "Main");
    app::logging::info("Application started");

    const int exitCode = QCoreApplication::exec();
    app::logging::info("Application exiting with code {}", exitCode);
    app::logging::shutdown();
    return exitCode;
}
