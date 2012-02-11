INCLUDEPATH += \
    $$PWD \
    $$PWD/ext/qjson/src \
    $$PWD/ext/glew/include \
    $$PWD/ext/assimp/include \
    $$PWD/ext/eigen \
    $$PWD/ext/eigen/unsupported

QT += opengl

DEFINES += GLEW_STATIC QJSON_EXPORT= EIGEN_QT_SUPPORT

unix {
  CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS += -O0
    QMAKE_CFLAGS += -O0
  }
}

unix:exists(/usr/bin/ccache) {
  QMAKE_CXX=ccache $$QMAKE_CXX
  QMAKE_CC=ccache $$QMAKE_CC
}
