#include "pininfo.h"
#include <loglibrary.h>

std::string quoteString(std::string s){
    if (s[0] == '"' && s[s.length() - 1] == '"')
        return s;
    return "\"" + s + "\"";
}

bool parsePinRequiredResponse(std::string s){
    return s.find("CPIN: SIM PIN") != std::string::npos;
}

int parseRemainingPinResponse(std::string s){
    size_t start = s.find(",");
    size_t end = s.find(",", start + 1);
    std::string parsedNumber = s.substr(start + 1, end - start - 1);
    int ret;
    try {
        ret = std::stoi(parsedNumber);
    } catch (std::exception e) {
        ERROR("Could not parse {}! Error: {}", s, e.what());
        exit(1);
    }

    return ret;
}

bool parsePinEnterResponse(std::string s){
    // Examining the length of the response is not too scientific, but
    // upon success the whole response is just an "OK" with some newline
    // characters. I think it should do the trick.
    return s.find("OK") != std::string::npos && s.length() < 7;
}

void PinInfo::modemAvailable(sdbus::Signal& signal)
{
    bool modemPresent;
    signal >> modemPresent;
    if (modemPresent) {
        m_pinRequired = isPinRequired();
        m_remainingTries = getRemainingPinTries();

        LOG("Emit pinrequiredChanged");
        emit pinRequiredChanged();
        LOG("Emit remainingTries changed");
        emit remainingTriesChanged();
    }
}

bool PinInfo::isPinRequired()
{
    std::string s;
    auto method = dbusProxy->createMethodCall(DBUS_INTERFACE_NAME, "pin_query");
    auto response = dbusProxy->callMethod(method);
    response >> s;
    return parsePinRequiredResponse(s);
}

int PinInfo::getRemainingPinTries()
{
    std::string s;
    auto method = dbusProxy->createMethodCall(DBUS_INTERFACE_NAME, "set_pin_remainder");
    method << "\"SC\"";
    auto response = dbusProxy->callMethod(method);
    response >> s;
    return parseRemainingPinResponse(s);
}

PinInfo::PinInfo(QObject *parent)
    : QObject{parent}, m_pinRequired{false}
{
    dbusProxy = sdbus::createProxy(DBUS_SERVICE_NAME, DBUS_OBJECT_PATH);

    auto modemAvailableHandler = [&](sdbus::Signal& signal){modemAvailable(signal);};
    dbusProxy->registerSignalHandler(DBUS_INTERFACE_NAME, "present", modemAvailableHandler);
    dbusProxy->finishRegistration();
}

bool PinInfo::enterPin(QString pin)
{
    std::string s;
    auto method = dbusProxy->createMethodCall(DBUS_INTERFACE_NAME, "pin_enter");
    method << quoteString(pin.toStdString());
    auto response = dbusProxy->callMethod(method);
    response >> s;
    bool ret = parsePinEnterResponse(s);
    if (ret) {
        LOG("PIN accepted");
        m_pinRequired = false;
        emit pinRequiredChanged();
    }
    return ret;
}

bool PinInfo::pinRequired()
{
    return m_pinRequired;
}

int PinInfo::remainingTries()
{
    if (m_remainingTries < 0)
        m_remainingTries = getRemainingPinTries();
    return m_remainingTries;
}

