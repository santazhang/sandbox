#include <QCoreApplication>
#include "qt4.h"

int main(int argc, char** argv) {
  QCoreApplication app(argc, argv);
  qt4 foo;
  printf("This is not that crappy!\n");
  return app.exec();
}
