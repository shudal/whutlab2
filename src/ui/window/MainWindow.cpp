//
// Created by perci on 2020/6/13.
//

#include "MainWindow.h"
#include <ctime>
#include <QtWidgets/QLineEdit>
#include <thread>
#include <unordered_map>
#include <chrono>
#include <QtWidgets/QPushButton>
#include <QObject>
MainWindow::MainWindow() {
  setupUI();
  if (Config::GetConfig()->APP_TYPE == Config::GetConfig()->APP_TYPE_SERVER) {
    server_start();
  } else {
    client_start();
  }
}
void MainWindow::setupUI() {
  mw.resize(Config::GetConfig()->WINDOW_WIDTH, Config::GetConfig()->WINDOW_HEIGHT);

  std::string title = Config::GetConfig()->MAIN_TITLE;
  if (Config::GetConfig()->APP_TYPE == Config::GetConfig()->APP_TYPE_SERVER) {
    title = title + "服务端";
  } else {
    title = title + "客户端";
  }
  mw.setWindowTitle(QString(title.c_str()));

  centralWidget.setParent(&mw);
  mw.setCentralWidget(&centralWidget);

  lep = std::make_unique<QLineEdit>();
  vbLayout.addWidget(lep.get());
  centralWidget.setLayout(&vbLayout);

  auto send_btn = new QPushButton("发送", lep.get());
  vbLayout.addWidget(send_btn);
  connect(send_btn, SIGNAL(released()), this, SLOT(send_msg()));
}
void MainWindow::server_start() {
  std::thread t([&] () -> void {
    boost::asio::io_context io;
    boost::asio::ip::tcp::acceptor acceptor(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), Config::GetConfig()->SERVER_LISTEN_PORT));
    for (;;) {
      boost::asio::ip::tcp::socket st(io);
      acceptor.accept(st);
      std::thread t2([&](boost::asio::ip::tcp::socket st) -> void {
        client_count ++;
        int my_cid = client_count;
        boost::system::error_code ignored_err;

        std::string msg = make_daytime_string();
        msg = msg + "client count : " + std::to_string(client_count) + "\n";
        boost::asio::write(st, boost::asio::buffer(msg), ignored_err);

        std::unordered_map<int, bool> sended;
        std::vector<char> buf(Config::GetConfig()->BUFFER_SIZE, 'a');
        bool last_read_finished = true;
        while (true) {
          for (int i=0; i<to_send_msg.size(); i++) {
             if (sended[to_send_msg[i].msg_id]) continue;
             boost::asio::write(st, boost::asio::buffer(to_send_msg[i].msg), ignored_err);
             sended[to_send_msg[i].msg_id] = true;
          }
          if (last_read_finished) {
            last_read_finished = false;
            st.async_read_some(boost::asio::buffer(buf, buf.size()), [&](boost::system::error_code ec, std::size_t N) {
              std::string s(buf.begin(), buf.end());
              qDebug("server read:");
              qDebug(s.c_str());
              last_read_finished = true;
            });
          }
        }
      }, std::move(st));
      t2.detach();
    }
  });
  t.detach();
}
std::string MainWindow::make_daytime_string() {
  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);
  return ctime(&now);
}
void MainWindow::client_start() {
  std::thread t ([&]()-> void {
    boost::asio::io_context io;
    using namespace boost::asio::ip;
    tcp::socket st(io);
    st.connect(tcp::endpoint (address::from_string(Config::GetConfig()->SERVER_ADDR), Config::GetConfig()->SERVER_LISTEN_PORT));
    std::vector<char> buf(Config::GetConfig()->BUFFER_SIZE, 'a');
    boost::system::error_code ignored_err;
    std::unordered_map<int, bool> sended;
    bool last_read_finished = true;
    while (true) {
      if (last_read_finished) {
        last_read_finished = false;
        st.async_read_some(boost::asio::buffer(buf, Config::GetConfig()->BUFFER_SIZE), [&](boost::system::error_code ec, std::size_t N) {
          std::string s(buf.begin(), buf.end());
          qDebug("client read:");
          qDebug(s.c_str());
          last_read_finished = true;
        });
      }
      for (int i=0; i<to_send_msg.size(); i++) {
        if (sended[to_send_msg[i].msg_id]) continue;
        std::string msg = to_send_msg[i].msg;
        st.async_send(boost::asio::buffer(msg), [&](boost::system::error_code ec) {
          std::string s = "write msg:" + ec.message();
          qDebug(s.c_str());
        });
        sended[to_send_msg[i].msg_id] = true;
      }
    }
  });
  t.detach();
}
void MainWindow::send_msg() {
  std::string s = lep->text().toStdString();
  std::string s2 = "will send: " + s;
  qDebug(s2.c_str());
  Msg m (msg_id++, s);
  to_send_msg.push_back(m);
}
