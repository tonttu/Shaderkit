/**
 * Copyright 2010 Riku Palomäki.
 * This file is part of GLSL Lab.
 *
 * GLSL Lab is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3 as
 * published by the Free Software Foundation.
 *
 * GLSL Lab is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GLSL Lab.  If not, see <http://www.gnu.org/licenses/>.
 */

// Indentation is two spaces, no tabs.
// Max line length is 100 chars, when usually 80 is enough.

#include "own_files.hpp"

#include <libs>

namespace Foo {

class Foo : public QObject {
  Q_OBJECT

public:
  struct InnerStruct {
    InnerStruct(int foo);
    int foo; // no "m_"-prefix with public members
  };

  Foo();
  virtual ~Foo();

  const char* foo(double* d, int& i) const;

  void bar() {
    for (int y = 0; y < 100; ++y) {
      if (!foo(0.2, y)) break;
    }
  }

  void operator+=(int i);
  operator Bar() const;

  void functionName(int param_name);

  int m_var_name;

  int* m_a;       // Asterisk on left..
  int *m_b, *m_c; // .. with exception if there is a declaration
                  // with multiple variables in same line
};

Foo::Foo() : m_var_name(1.0) {}

Bar::Bar() : Foo(1.0), m_small(0.0) {
  do_stuff();
}

Zoo::Zoo()
  : SomeNamespace::SomeClass(Something::Long("zombie")),
    m_something(1.0), m_small(0.0) {
  do_stuff();
}

}
