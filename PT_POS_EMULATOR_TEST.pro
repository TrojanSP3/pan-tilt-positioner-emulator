TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lpthread

HEADERS += \
Sources/Tests/Framework/globaltestlist.h \
Sources/Tests/Framework/testframework.h \
Sources/Tests/Framework/testclass.h \
Sources/Tests/Framework/testexception.h \
Sources/Tests/Framework/testcase.h \
Sources/Tests/Framework/testsuite.h \
Sources/Tests/Framework/testutilities.h \
Sources/Tests/testutils.h \
Sources/Tests/testtcpsocket.h \
Sources/Tests/testlog.h \
Sources/Tests/testproperty.h \
Sources/Tests/testconfigfile.h \
Sources/Tests/testdrawing.h \
Sources/Tests/testengine.h \
Sources/Tests/testtcpserver.h \
Sources/Tests/testprotocolmso2.h \
Sources/TcpSocket/tcpsocketexception.h \
Sources/TcpSocket/tcpsocket.h \
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
Sources/main_test.cpp \
Sources/Tests/Framework/testframework.cpp \
Sources/Tests/Framework/testclass.cpp \
Sources/Tests/Framework/testexception.cpp \
Sources/Tests/Framework/testcase.cpp \
Sources/Tests/Framework/testsuite.cpp \
Sources/Tests/Framework/testutilities.cpp \
Sources/Tests/testutils.cpp \
Sources/Tests/testtcpsocket.cpp \
Sources/Tests/testlog.cpp \
Sources/Tests/testproperty.cpp \
Sources/Tests/testconfigfile.cpp \
Sources/Tests/testdrawing.cpp \
Sources/Tests/testengine.cpp \
Sources/Tests/testtcpserver.cpp \
Sources/Tests/testprotocolmso2.cpp \
Sources/TcpSocket/tcpsocketexception.cpp \
Sources/TcpSocket/tcpsocket.cpp \
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


