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

  void functionName(int param_name);

  int m_var_name;

  int* m_a;       // Asterisk on left..
  int *m_b, *m_c; // .. with exception if there is a declaration
                  // with multiple variables in same line
};

}
