#ifndef PININFO_H
#define PININFO_H

#include <QObject>
#include <QQmlEngine>
#include <sdbus-c++/sdbus-c++.h>

#define DBUS_SERVICE_NAME "sgy.pine.modem"
#define DBUS_OBJECT_PATH "/sgy/pine/modem"
#define DBUS_INTERFACE_NAME "sgy.pine.modem"

class PinInfo : public QObject
{
    Q_OBJECT
    QML_ELEMENT
private:
    std::unique_ptr<sdbus::IProxy> dbusProxy;
    bool m_pinRequired;
    int m_remainingTries = -1;
    void modemAvailable(sdbus::Signal& signal);
    Q_PROPERTY(bool pinRequired READ pinRequired NOTIFY pinRequiredChanged FINAL)
    Q_PROPERTY(int remainingTries READ remainingTries NOTIFY remainingTriesChanged FINAL)

    bool isPinRequired();
    int getRemainingPinTries();
public:
    explicit PinInfo(QObject *parent = nullptr);
    Q_INVOKABLE bool enterPin(QString pin);
    bool pinRequired();
    Q_INVOKABLE int remainingTries();


signals:
    void pinRequiredChanged();
    void remainingTriesChanged();
};

#endif // PININFO_H
