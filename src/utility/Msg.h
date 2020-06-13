//
// Created by perci on 2020/6/13.
//

#ifndef UNTITLED8_SRC_UTILITY_MSG_H_
#define UNTITLED8_SRC_UTILITY_MSG_H_

#include <string>
class Msg {
 public:
  int msg_id;
  int client_id;
  std::string msg;
  Msg(int cid, std::string s) : client_id(cid), msg(std::move(s)) {};
};

#endif//UNTITLED8_SRC_UTILITY_MSG_H_
