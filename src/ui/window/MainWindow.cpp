//
// Created by perci on 2020/6/13.
//

#include "MainWindow.h"
#include <QObject>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <chrono>
#include <ctime>
#include <thread>
#include <unordered_map>
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
  std::string s5 = std::to_string(Config::GetConfig()->APP_TYPE).c_str();
  qDebug(s5.c_str());
  qDebug(title.c_str());
  mw.setWindowTitle(QString(title.c_str()));

  centralWidget.setParent(&mw);
  mw.setCentralWidget(&centralWidget);


  label = new QLabel();
  vbLayout.addWidget(label);

  lep = std::make_unique<QLineEdit>();
  vbLayout.addWidget(lep.get());
  centralWidget.setLayout(&vbLayout);

  auto send_btn = new QPushButton("发送", lep.get());
  send_btn->setStyleSheet("QPushButton {background-color: rgb(58,64,85); color:white; font-size:28px; border-radius:5px; width: 30px;} QPushButton:hover { background-color: rgb(82,89,111);}");
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

        std::vector<char> buf(Config::GetConfig()->BUFFER_SIZE, 'a');
        bool last_read_finished = true;
        std::thread read_thread([&]()->void {
          while (true) {
            int size = st.read_some(boost::asio::buffer(buf, Config::GetConfig()->BUFFER_SIZE), ignored_err);
            if (size > 0) {
              std::string s(buf.begin(), buf.begin() + size);
              add_rece_msg(s);
              qDebug("server read:");
              qDebug(s.c_str());
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
          }
        });
        std::thread send_thread([&]()->void {
          while (true) {
            for (int i=0; i<to_send_msg.size(); i++) {
              if (to_send_msg[i].sended) continue;
              boost::asio::write(st, boost::asio::buffer(to_send_msg[i].msg), ignored_err);
              to_send_msg[i].sended = true;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
          }
        });
        read_thread.join();
        send_thread.join();
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

    std::thread read_thread([&]()->void {
      while (true) {
        int size = st.read_some(boost::asio::buffer(buf, Config::GetConfig()->BUFFER_SIZE), ignored_err);
        if (size > 0) {
          std::string s(buf.begin(), buf.begin() + size);
          add_rece_msg(s);
          qDebug("client read:");
          qDebug(s.c_str());
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
      }
    });
    std::thread send_thread([&]()->void {
      while (true) {
        for (int i=0; i<to_send_msg.size(); i++) {
          if (to_send_msg[i].sended) continue;
          boost::asio::write(st, boost::asio::buffer(to_send_msg[i].msg), ignored_err);
          to_send_msg[i].sended = true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
      }
    });
    read_thread.join();
    send_thread.join();
  });
  t.detach();
}
void MainWindow::send_msg() {
  std::string s = lep->text().toStdString();
  int mid = msg_id++;
  print("will send: " + s + ", msg_id=" + std::to_string(mid));
  Msg m (mid, s);
  to_send_msg.push_back(m);
}
void MainWindow::print(std::string s) {
  qDebug(s.c_str());
}
void MainWindow::add_rece_msg(std::string s) {
  rece_msg.push_back(s);
  std::string ls = "收到消息：\n";
  for (auto s : rece_msg) {
    ls = ls + s + '\n';
  }
  label->setText(QString(ls.c_str()));
}
