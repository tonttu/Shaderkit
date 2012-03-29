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
version.depend_command = $$RUBY ../update_version.rb --dep
version.commands = $$RUBY ../update_version.rb
version.output = ../version.hpp
version.variable_out = HEADERS
QMAKE_EXTRA_COMPILERS += version

INCLUDEPATH += ../app
