cmake_minimum_required(VERSION 3.16)

project(modem-network VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

find_package(Qt6 6.2 REQUIRED COMPONENTS Quick)
find_package(sdbus-c++ REQUIRED)
find_package(LogLibrary)

set(SOURCES src/pininfo.cpp)

set(HEADERS include/pininfo.h)

qt_add_executable(appmodem-network
    src/main.cpp
    ${SOURCES} ${HEADERS}
)

qt_add_qml_module(appmodem-network
    URI modem-network
    VERSION 1.0
    QML_FILES qml/Main.qml
    QML_FILES qml/PinScreen.qml
    RESOURCES qml/res/show-password.png
)

target_include_directories(appmodem-network
    PRIVATE include
)

target_link_libraries(appmodem-network
    PRIVATE Qt6::Quick SDBusCpp::sdbus-c++ LogLibrary
)

include(GNUInstallDirs)
install(TARGETS appmodem-network
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
