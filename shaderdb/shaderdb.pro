include(../subdirs.pri)

QMAKE_CXXFLAGS += -std=c++0x

HEADERS += \
    shaderdb.hpp \
    metainfo.hpp

SOURCES += \
    shaderdb.cpp \
    metainfo.cpp

INCLUDEPATH += ../app
