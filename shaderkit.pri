INCLUDEPATH += \
    $$PWD \
    $$PWD/ext/qjson/src \
    $$PWD/ext/glew/include \
    $$PWD/ext/assimp/include \
    $$PWD/ext/glm

QT += opengl

DEFINES += GLEW_STATIC QJSON_EXPORT= GLM_FORCE_RADIANS GLM_FORCE_CXX11

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

*g++*: QMAKE_CFLAGS += -g
*g++*: QMAKE_CXXFLAGS += -std=c++0x -g
*clang*: QMAKE_CXXFLAGS += -std=c++11 -g
*msvc*: QMAKE_CXXFLAGS += -D_CRT_SECURE_NO_WARNINGS
