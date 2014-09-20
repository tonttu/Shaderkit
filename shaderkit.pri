INCLUDEPATH += \
    $$PWD \
    $$PWD/ext/qjson/src \
    $$PWD/ext/glew/include \
    $$PWD/ext/assimp/include \
    $$PWD/ext/eigen \
    $$PWD/ext/eigen/unsupported \
    $$PWD/ext/glm

QT += opengl

DEFINES += GLEW_STATIC QJSON_EXPORT= EIGEN_QT_SUPPORT GLM_FORCE_RADIANS GLM_FORCE_CXX11

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

# See issue http://eigen.tuxfamily.org/bz/show_bug.cgi?id=83
# And fix in VC11 http://connect.microsoft.com/VisualStudio/feedback/details/692988/std-vector-resize-should-take-a-const-reference
# On broken implementations we just disable optimizations, boo hoo
no-static-align:DEFINES += EIGEN_DONT_ALIGN_STATICALLY=1
*msvc* {
  !contains(QMAKE_HOST.arch, x86_64) {
    win32-msvc2003:DEFINES += EIGEN_DONT_ALIGN_STATICALLY=1
    win32-msvc2005:DEFINES += EIGEN_DONT_ALIGN_STATICALLY=1
    win32-msvc2008:DEFINES += EIGEN_DONT_ALIGN_STATICALLY=1
    win32-msvc2010:DEFINES += EIGEN_DONT_ALIGN_STATICALLY=1
  }
}
