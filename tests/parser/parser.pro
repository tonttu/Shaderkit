include(../../lab.pri)

INCLUDEPATH += ../../app

SOURCES += parser_test.cpp
PRE_TARGETDEPS +=  ../../app/log.o ../../app/glsl_yacc.o ../../app/glsl_lex.o ../../app/pp_yacc.o ../../app/pp_lex.o ../../app/glpp.o
LIBS += ../../app/log.o ../../app/glsl_yacc.o ../../app/glsl_lex.o ../../app/pp_yacc.o ../../app/pp_lex.o ../../app/glpp.o
