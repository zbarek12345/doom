//
// Created by Wiktor on 30.05.2025.
//
#include <cstdint>
#include <vector>
#include <regex.h>
#include <cstdio>
#include <string>
#include <wchar.h>
#include "game_content.h"
#ifndef PARSER_H
#define PARSER_H



class parser {
  game_content* content;

  void load_file(const char* path);


public:
  std::vector<std::string> get_levels();
  std::vector<map> get_level(int id);
};



#endif //PARSER_H
