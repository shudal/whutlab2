//
// Created by perci on 2020/6/13.
//

#include "MainWindow.h"
#include <ctime>
#include <QtWidgets/QLineEdit>
#include <thread>
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

}
int MainWindow::server_start() {
  std::thread t([&] () -> void {
    boost::asio::io_context io;
    boost::asio::ip::tcp::acceptor acceptor(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), Config::GetConfig()->SERVER_LISTEN_PORT));
    for (;;) {
      boost::asio::ip::tcp::socket st(io);
      acceptor.accept(st);
      boost::system::error_code ignored_err;
      std::string msg = make_daytime_string();
      boost::asio::write(st, boost::asio::buffer(msg), ignored_err);
    }
  });
  t.detach();
  return 0;
}
std::string MainWindow::make_daytime_string() {
  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);
  return ctime(&now);
}
