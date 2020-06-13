#include <iostream>

#include <QtWidgets/QApplication>
#include <src/ui/window/MainWindow.h>
int main(int argc, char* argv[]) {
  QApplication qa(argc, argv);
  MainWindow mw;
  mw.mw.show();
  return qa.exec();
}
