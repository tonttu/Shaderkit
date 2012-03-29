include(../subdirs.pri)

HEADERS += \
    shaderdb.hpp \
    metainfo.hpp

SOURCES += \
    shaderdb.cpp \
    metainfo.cpp

win32:RUBY=../ext/win32/bin/ruby.exe
!win32:RUBY=ruby

VERSION_SOURCES = ../version
version.input = VERSION_SOURCES
version.depend_command = $$RUBY $$IN_PWD/../update_version.rb $$IN_PWD/../version --dep
version.commands = $$RUBY $$IN_PWD/../update_version.rb $$IN_PWD/../version $$OUT_PWD/../version.hpp
version.output = $$OUT_PWD/../version.hpp
version.variable_out = HEADERS
QMAKE_EXTRA_COMPILERS += version

INCLUDEPATH += ../app $$OUT_PWD/..
