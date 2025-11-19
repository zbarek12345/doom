//
// Created by Wiktor on 30.05.2025.
//

#ifndef PARSER_H
#define PARSER_H
#include "new_models.h"
#include <cstdint>
#include <vector>
#include <regex.h>
#include <cstdio>
#include <string>
#include <wchar.h>
#include "game_content.h"

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

  NewModels::Map *generateMap(int id);

  void testExport(int id, const char *filepath);

  void find_sector(int id, svec3 vertex);

  void obj_export(int id, const char *filepath);

  void read_all_lumps(const char *filepath);

  void ply_export(int id, const char *filepath);

  std::vector<std::string> get_levels() const;
  //TODO - Convert an old map to new std::vector<map> get_level(int id);
};



#endif //PARSER_H
