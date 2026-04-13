#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "app/AppContainer.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    AppContainer appContainer;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.rootContext()->setContextProperty("mainViewModel", appContainer.mainViewModel());
    engine.loadFromModule("underlying_test_platform", "Main");

    return QCoreApplication::exec();
}
