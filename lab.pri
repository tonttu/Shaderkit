INCLUDEPATH += \
    $$PWD \
    $$PWD/ext/qjson/src \
    $$PWD/ext/glew/include

QT += opengl

DEFINES += GLEW_STATIC QJSON_EXPORT=

linux-g++ {
  CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS += -O0
    QMAKE_CFLAGS += -O0
  }
}
