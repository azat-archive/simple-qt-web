QMAKE_CXXFLAGS += -Wall -Werror -std=c++0x
QMAKE_LFLAGS += -Wall -Werror -std=c++0x
QT += network \
    webkit
CONFIG += silent
TARGET = web
TEMPLATE = app

OBJECTS_DIR = build/.obj
MOC_DIR = build/.moc

SOURCES += src/main.cpp src/lambda.cpp src/qwebpage.cpp src/connector.cpp
HEADERS += src/lambda.h src/qwebpage.h src/connector.h

