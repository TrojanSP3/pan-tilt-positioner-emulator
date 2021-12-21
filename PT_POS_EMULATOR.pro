TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lpthread

HEADERS += \
Sources/main.h \
Sources/TcpSocket/tcpsocketexception.h \
Sources/TcpSocket/socketstatus.h \
Sources/TcpSocket/tcpserversocket.h \
Sources/TcpSocket/tcpclientsocket.h \
Sources/utils.h \
Sources/log.h \
Sources/property.h \
Sources/config.h \
Sources/configfile.h \
Sources/drawing.h \
Sources/engine.h \
Sources/tcpserver.h \
Sources/mso_state.h \
Sources/protocol_base.h \
Sources/protocol_mso2.h \

SOURCES += \
Sources/main.cpp \
Sources/TcpSocket/tcpsocketexception.cpp \
Sources/TcpSocket/socketstatus.cpp \
Sources/TcpSocket/tcpserversocket.cpp \
Sources/TcpSocket/tcpclientsocket.cpp \
Sources/utils.cpp \
Sources/log.cpp \
Sources/property.cpp \
Sources/config.cpp \
Sources/configfile.cpp \
Sources/drawing.cpp \
Sources/engine.cpp \
Sources/tcpserver.cpp \
Sources/mso_state.cpp \
Sources/protocol_base.cpp \
Sources/protocol_mso2.cpp \
