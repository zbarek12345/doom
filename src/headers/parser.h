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



class Parser {
  struct game_data {
  	  original_classes::header header;
      original_classes::lump* lumps;
  };
  game_content* content = nullptr;


  template<class T>
  T *load_lump(FILE *file, original_classes::lump pointer);
public:
  ~Parser() {
    delete content;
  }
  ///Loads level into parser,
  void load_file(char *file_name);

  void obj_export(int id, const char *filepath);

  void ply_export(int id, const char *filepath);

  std::vector<std::string> get_levels() const;
  //TODO - Convert an old map to new std::vector<map> get_level(int id);
};



#endif //PARSER_H
