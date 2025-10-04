#ifndef PARSER_H
#define PARSER_H

#include <vector>
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

  std::vector<std::string> get_levels() const;
  //TODO - Convert an old map to new std::vector<map> get_level(int id);
};



#endif //PARSER_H
