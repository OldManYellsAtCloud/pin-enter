#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "pininfo.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<PinInfo>("sgy.pine.modem", 1, 0, "PinInfo");

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/modem-network/qml/Main.qml"_qs);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
