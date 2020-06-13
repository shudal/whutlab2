//
// Created by perci on 2020/6/13.
//

#ifndef UNTITLED8_SRC_INCLUDE_CONFIG_H_
#define UNTITLED8_SRC_INCLUDE_CONFIG_H_

#include <string>
class Config {
 public:
  static Config* GetConfig() {
    static Config c;
    return &c;
  }

  const int WINDOW_WIDTH = 800;
  const int WINDOW_HEIGHT = 600;
  const std::string MAIN_TITLE = "点对点通信";
  const int SERVER_LISTEN_PORT = 8991;

};
#endif//UNTITLED8_SRC_INCLUDE_CONFIG_H_
