//
// Created by perci on 2020/6/13.
//

#ifndef UNTITLED8_SRC_UI_WINDOW_MAINWINDOW_H_
#define UNTITLED8_SRC_UI_WINDOW_MAINWINDOW_H_

#include <src/include/Config.h>

#include <Qt>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QVBoxLayout>
#include <boost/asio.hpp>
#include <list>
#include <queue>
#include <src/utility/Msg.h>
#include <string>
#include <vector>
class MainWindow : public QObject{
 Q_OBJECT
 public:
  QMainWindow mw;
  QVBoxLayout vbLayout;
  QWidget centralWidget;

  MainWindow();
  void server_start();
  void client_start();
 private:
  int client_count = 0;
  std::vector<Msg> to_send_msg;
  std::unique_ptr<QLineEdit> lep;
  int msg_id = 0;

  void setupUI();
  std::string make_daytime_string();

  public slots:
  void send_msg();

};

#endif//UNTITLED8_SRC_UI_WINDOW_MAINWINDOW_H_
