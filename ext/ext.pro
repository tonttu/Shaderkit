include(../subdirs.pri)

# TODO: move these to own subdirectory
HEADERS += \
    glut_teapot.hpp

SOURCES += \
    glut_teapot.c


# QJson, TODO: do this correctly
HEADERS += \
    qjson/src/json_parser.hh \
    qjson/src/json_scanner.h \
    qjson/src/location.hh \
    qjson/src/parser_p.h  \
    qjson/src/position.hh \
    qjson/src/qjson_debug.h  \
    qjson/src/stack.hh \
    qjson/src/parser.h \
    qjson/src/parserrunnable.h \
    qjson/src/qobjecthelper.h \
    qjson/src/serializer.h \
    qjson/src/serializerrunnable.h \
    qjson/src/qjson_export.h

SOURCES += \
    qjson/src/json_parser.cc \
    qjson/src/json_scanner.cpp \
    qjson/src/parser.cpp \
    qjson/src/parserrunnable.cpp \
    qjson/src/qobjecthelper.cpp \
    qjson/src/serializer.cpp \
    qjson/src/serializerrunnable.cpp

# GLEW
SOURCES += glew/src/glew.c

# assimp
include(assimp/assimp.pri)

# glm (header only library, this is mostly for QtCreator)
include(glm.pri)

# find -type f '!' -name CMakeLists.txt|sed "s/.*/    \0 \\\\/"
