//
// Created by perci on 2020/6/13.
//

#include "MainWindow.h"

#include <QtWidgets/QLineEdit>

MainWindow::MainWindow() {
  setupUI();
}
void MainWindow::setupUI() {
  mw.resize(Config::GetConfig()->WINDOW_WIDTH, Config::GetConfig()->WINDOW_HEIGHT);
  mw.setWindowTitle(QString(Config::GetConfig()->MAIN_TITLE.c_str()));

  centralWidget.setParent(&mw);
  mw.setCentralWidget(&centralWidget);

  auto le = new QLineEdit();
  vbLayout.addWidget(le);
  centralWidget.setLayout(&vbLayout);


  boost::asio::io_context io;
  boost::asio::steady_timer t(io, boost::asio::chrono::seconds(5));
  t.wait();
  qDebug("timer expires");
}
