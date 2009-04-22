#include <QCoreApplication>
#include "qt4.h"

int main(int argc, char** argv) {
  QCoreApplication app(argc, argv);
  qt4 foo;
  printf("This is not that crappy!\n");
  printf("So, it is running now!\n");
  return app.exec();
}
