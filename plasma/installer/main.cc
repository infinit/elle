
#include <QtGui/QApplication>

#include "MainWindow.hh"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainWindow w;
  w.show();

  return a.exec();
}
