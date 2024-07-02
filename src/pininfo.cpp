#include "pininfo.h"
#include <loglibrary.h>


std::string quoteString(std::string s){
    if (s[0] == '"' && s[s.length() - 1] == '"')
        return s;
    return "\"" + s + "\"";
}

PinInfo::PinInfo(QObject *parent)
    : QObject{parent}, m_pinRequired{false}, settings{"/etc"}
{
    dbusConnection = sdbus::createBusConnection(sdbus::ServiceName{"org.gspine.sim"});

    dbusObject = sdbus::createObject(*dbusConnection, sdbus::ObjectPath{"/org/gspine/sim"});

    dbusProxy = sdbus::createProxy(*dbusConnection, sdbus::ServiceName{DBUS_SERVICE_NAME}, sdbus::ObjectPath{DBUS_OBJECT_PATH});
    dbusObject->addVTable(sdbus::SignalVTableItem{sdbus::MethodName{"unlocked"}, {}, {}}).forInterface(sdbus::InterfaceName{DBUS_INTERFACE_NAME});
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

bool PinInfo::isPinRequired()
{
    std::string responseSuccess;
    std::string simState;
    auto method = dbusProxy->createMethodCall(sdbus::InterfaceName{"org.gspine.modem.sim"}, sdbus::MethodName{"get_pin_state"});
    auto response = dbusProxy->callMethod(method);
    response >> responseSuccess;
    if (responseSuccess != "OK")
        return false;

    response >> simState;
    return simState == "SIM PIN";
}

int PinInfo::getRemainingPinTries()
{
    int res;
    std::string requestStatus;
    auto method = dbusProxy->createMethodCall(sdbus::InterfaceName{"org.gspine.modem.sim"}, sdbus::MethodName{"get_pin_counter"});
    method << "\"SC\"";
    auto dbusResponse = dbusProxy->callMethod(method);
    dbusResponse >> requestStatus;
    if (requestStatus == "OK")
        dbusResponse >> res;
    else
        res = -1;
    return res;
}

void PinInfo::configureDataAccess()
{
    std::string apnSettings = settings.getValue("sim", "apn");
    auto method = dbusProxy->createMethodCall(sdbus::InterfaceName{"org.gspine.modem.hw"}, sdbus::MethodName{"set_low_power"});

    method << false;
    dbusProxy->callMethod(method);
    method = dbusProxy->createMethodCall(sdbus::InterfaceName{"org.gspine.modem.pd"}, sdbus::MethodName{"enable_pd"});
    method << false;
    dbusProxy->callMethod(method);

    method = dbusProxy->createMethodCall(sdbus::InterfaceName{"org.gspine.modem.general"}, sdbus::MethodName{"set_functionality_level"});
    method << "Disable";
    dbusProxy->callMethod(method);

    method = dbusProxy->createMethodCall(sdbus::InterfaceName{"org.gspine.modem.pd"}, sdbus::MethodName{"set_apn"});
    method << apnSettings;
    dbusProxy->callMethod(method);

    method = dbusProxy->createMethodCall(sdbus::InterfaceName{"org.gspine.modem.general"}, sdbus::MethodName{"set_functionality_level"});
    method << "Full";
    dbusProxy->callMethod(method);

    method = dbusProxy->createMethodCall(sdbus::InterfaceName{"org.gspine.modem.pd"}, sdbus::MethodName{"enable_pd"});
    method << true;
    dbusProxy->callMethod(method);
}

void PinInfo::sendUnlockedSignal()
{
    dbusObject->createSignal(sdbus::InterfaceName{"org.gspine.sim"}, sdbus::SignalName{"unlocked"}).send();
}

bool PinInfo::enterPin(QString pin)
{
    std::string result;
    std::string modemOutput;
    auto method = dbusProxy->createMethodCall(sdbus::InterfaceName{"org.gspine.modem.sim"}, sdbus::MethodName{"pin_enter"});
    method << pin.toStdString();
    auto response = dbusProxy->callMethod(method);
    response >> result;
    response >> modemOutput;
    LOG("PIN enter result: {}", result);

    if (result == "OK"){
        configureDataAccess();
        sendUnlockedSignal();
    }
    return result == "OK";
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

