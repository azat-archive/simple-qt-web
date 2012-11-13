QMAKE_CXXFLAGS += -Wall -Werror -std=c++0x
QMAKE_LFLAGS += -Wall -Werror -std=c++0x
QT += network \
    webkit
TARGET = main 
TEMPLATE = app
SOURCES += src/main.cpp src/lambda.cpp src/qwebpage.cpp src/connector.cpp
HEADERS += src/lambda.h src/qwebpage.h src/connector.h

