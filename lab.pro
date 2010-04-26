QT += opengl

QMAKE_EXTRA_COMPILERS += bison
bison.output = glsl.tab.cpp glsl.tab.hpp
bison.commands = bison -v -d -t -o glsl.tab.cpp ${QMAKE_FILE_IN}
bison.input = BISON_FILES
bison.CONFIG = no_link
BISON_FILES = glsl.y

QMAKE_EXTRA_COMPILERS += flex
flex.output = lex.yy.c
flex.commands = flex ${QMAKE_FILE_IN}
flex.input = FLEX_FILES
flex.CONFIG = no_link
FLEX_FILES = glsl.l

HEADERS += forward.hpp \
    grammar.hpp \
    highlighter.hpp \
    lexer.hpp \
    watcher.hpp \
    ext/glut_teapot.hpp \
    glsl.tab.hpp

SOURCES += glsl.tab.cpp \
    lex.yy.c \
    grammar.cpp \
    highlighter.cpp \
    watcher.cpp \
    ext/glut_teapot.c \
    main.cpp

OTHER_FILES += doc/style.cpp \
    std/150.all \
    std/150.compat.all \
    std/150.compat.frag \
    std/150.compat.geom \
    std/150.compat.vert \
    std/150.frag \
    std/150.geom \
    std/150.vert

QMAKE_CXXFLAGS += -DGL_GLEXT_PROTOTYPES

include(qtpropertybrowser/src/qtpropertybrowser.pri)
