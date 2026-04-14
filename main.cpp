#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#include "app/AppContainer.h"
#include "infrastructure/logging/Logging.h"

/**
 * @brief Bootstraps logging, dependency wiring, and the QML entry point.
 * @param argc Command-line argument count passed to Qt.
 * @param argv Command-line argument array passed to Qt.
 * @return Application exit code returned by the Qt event loop.
 */
int main(int argc, char *argv[])
{
    app::logging::initialize();
    app::logging::installQtMessageHandler();
    app::logging::info("Application bootstrapping");

    QGuiApplication app(argc, argv);

    QQuickStyle::setStyle("Fusion");

    QQmlApplicationEngine engine;
    AppContainer appContainer;
    // Surface database initialization state before exposing view models to QML.
    if (appContainer.databaseReady()) {
        app::logging::info("Database ready");
    } else {
        app::logging::error("Database not ready: {}", appContainer.databaseError().toStdString());
    }
    // Exit asynchronously if the root QML object fails to instantiate.
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.rootContext()->setContextProperty("mainViewModel", appContainer.mainViewModel());
    app::logging::info("Loading QML module: quick_todo/Main");
    engine.loadFromModule("quick_todo", "Main");
    app::logging::info("Application started");

    const int exitCode = QCoreApplication::exec();
    app::logging::info("Application exiting with code {}", exitCode);
    app::logging::shutdown();
    return exitCode;
}
