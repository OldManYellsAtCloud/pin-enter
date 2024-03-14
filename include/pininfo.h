#ifndef PININFO_H
#define PININFO_H

#include <QObject>
#include <QQmlEngine>
#include <settingslib.h>
#include <sdbus-c++/sdbus-c++.h>

#define DBUS_SERVICE_NAME "org.gspine.modem"
#define DBUS_OBJECT_PATH "/org/gspine/modem"
#define DBUS_INTERFACE_NAME "org.gspine.modem"

class PinInfo : public QObject
{
    Q_OBJECT
    QML_ELEMENT
private:
    std::unique_ptr<sdbus::IProxy> dbusProxy;
    SettingsLib settings;
    bool m_pinRequired;
    int m_remainingTries = -1;
    Q_PROPERTY(bool pinRequired READ pinRequired NOTIFY pinRequiredChanged FINAL)
    Q_PROPERTY(int remainingTries READ remainingTries NOTIFY remainingTriesChanged FINAL)

    bool isPinRequired();
    int getRemainingPinTries();
    void configureDataAccess();
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
