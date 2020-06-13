//
// Created by perci on 2020/6/13.
//

#ifndef UNTITLED8_SRC_UI_WINDOW_MAINWINDOW_H_
#define UNTITLED8_SRC_UI_WINDOW_MAINWINDOW_H_

#include <src/include/Config.h>

#include <string>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QVBoxLayout>
#include <boost/asio.hpp>
class MainWindow {
 public:
  QMainWindow mw;
  QVBoxLayout vbLayout;
  QWidget centralWidget;

  MainWindow();
  int server_start();
 private:
  void setupUI();
  std::string make_daytime_string();
};

#endif//UNTITLED8_SRC_UI_WINDOW_MAINWINDOW_H_
