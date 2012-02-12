include(../../shaderkit.pri)

QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../../app

SOURCES += parser_test.cpp
PRE_TARGETDEPS +=  ../../app/log.o ../../app/glsl_yacc.o ../../app/glsl_lex.o ../../app/pp_yacc.o ../../app/pp_lex.o ../../app/glsl_pp.o
LIBS += ../../app/log.o ../../app/glsl_yacc.o ../../app/glsl_lex.o ../../app/pp_yacc.o ../../app/pp_lex.o ../../app/glsl_pp.o
