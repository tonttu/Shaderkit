include(../lab.pri)

TARGET = ../lab

QMAKE_EXTRA_COMPILERS += bison
bison.output = shader/glsl.tab.cpp shader/glsl.tab.hpp
bison.commands = bison -v -d -t -o shader/glsl.tab.cpp ${QMAKE_FILE_IN}
bison.input = BISON_FILES
bison.CONFIG = no_link
BISON_FILES = shader/glsl.y

QMAKE_EXTRA_COMPILERS += flex
flex.output = shader/lex.yy.c
flex.commands = flex -o shader/lex.yy.c ${QMAKE_FILE_IN}
flex.input = FLEX_FILES
flex.CONFIG = no_link
FLEX_FILES = shader/glsl.l

HEADERS += \
    lab_tr1.hpp \
    forward.hpp \
    highlighter.hpp \
    watcher.hpp \
    state.hpp \
    light.hpp \
    camera.hpp \
    object3d.hpp \
    editor.hpp \
    renderpass.hpp \
    scene.hpp \
    project.hpp \
    glwidget.hpp \
    properties.hpp \
    mainwindow.hpp \
    opengl.hpp \
    texture.hpp \
    fbo.hpp \
    welcome.hpp \
    shader/grammar.hpp \
    shader/lexer.hpp \
    shader/glsl.tab.hpp \
    shader/error.hpp \
    shader/compiler_output_parser.hpp \
    shader/uniform.hpp \
    shader/shader.hpp \
    shader/program.hpp

SOURCES += \
    highlighter.cpp \
    watcher.cpp \
    main.cpp \
    state.cpp \
    light.cpp \
    camera.cpp \
    object3d.cpp \
    editor.cpp \
    renderpass.cpp \
    scene.cpp \
    project.cpp \
    glwidget.cpp \
    properties.cpp \
    mainwindow.cpp \
    opengl.cpp \
    texture.cpp \
    fbo.cpp \
    welcome.cpp \
    shader/glsl.tab.cpp \
    shader/lex.yy.c \
    shader/grammar.cpp \
    shader/error.cpp \
    shader/compiler_output_parser.cpp \
    shader/uniform.cpp \
    shader/shader.cpp \
    shader/program.cpp

FORMS += \
    mainwindow.ui \
    welcome.ui

include(../qtpropertybrowser/src/qtpropertybrowser.pri)

LIBS += ../ext/libext.a ../shaderdb/libshaderdb.a
