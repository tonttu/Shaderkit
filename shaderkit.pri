INCLUDEPATH += \
    $$PWD \
    $$PWD/ext/qjson/src \
    $$PWD/ext/glew/include \
    $$PWD/ext/assimp/include

QT += opengl

DEFINES += GLEW_STATIC QJSON_EXPORT=

unix {
  CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS += -O0
    QMAKE_CFLAGS += -O0
  }
}
