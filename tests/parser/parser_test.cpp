#include "parser/glpp.hpp"
#include "core/log.hpp"

#include <QFile>

#include <fcntl.h>

int main(int argc, char* argv[]) {
  Log log;

  if (argc != 2) {
    Log::error("Usage: %s <file>", argv[0]);
    return 1;
  }

  int out = dup(1);
  close(1);
  int n = open("/dev/null", O_WRONLY);

  QFile file(argv[1]);
  if (file.open(QFile::ReadOnly)) {
    GLpp p;
    p.scan(file.readAll());
    write(out, p.out().c_str(), p.out().size());
  } else {
    Log::error("Failed to open %s", argv[1]);
  }

  close(out);
  close(n);
}
