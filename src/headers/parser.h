#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
// #include <wchar.h>
#include "game_content.h"

///Conversion of the original map to the appropriate types
class Parser {
  struct game_data {
  	  original_classes::header header;
      original_classes::lump* lumps;
  };
  ///Storage converted data
  game_content* content = nullptr;

  ///Load raw data from WAD
  ///@param file file from read data
  ///@param pointer the place from we read
  ///@return read data, one of the structures original_classes
  template<class T>
  T *load_lump(FILE *file, original_classes::lump pointer);
  ///Load maps
  ///@param file file from read
  ///@param gd data of lumps
  ///@param i
  void load_maps(FILE *file, const game_data &gd, uint32_t &i);
  ///Load pallet
  ///@param file file from read
  ///@param gd data of lumps
  ///@param i
  void load_pallet(FILE *file, const game_data &gd, uint32_t i);
  ///Load pallet
  ///@param file file from read
  ///@param gd data of lumps
  ///@param i
  void load_color_map(FILE *file, const game_data &gd, uint32_t i);
  ///Load names for patch
  ///@param file file from read
  ///@param gd data of lumps
  ///@param i
  void load_pname(FILE *file, const game_data &gd, uint32_t i);
  ///Load patch
  ///@param file file from read
  ///@param gd data of lumps
  ///@param i
  void load_patch(FILE *file, const game_data &gd, uint32_t i);

  ///Convert image index pallet to RGBA
  ///@param in image in with index pallet
  ///@param out image out, format RGBA
  ///@param pallet_idx index color pallet
  void convert_to_rgba(std::vector<std::vector<uint16_t>> &in, std::vector<std::vector<std::vector<uint8_t>>> &out, int pallet_idx);
  ///Convert image index pallet to RGB
  ///@param in image in with index pallet
  ///@param width
  ///@param height
  ///@param out image out, format RGB
  ///@param pallet_idx index color pallet
  void convert_to_rgb(unsigned char *in, int width, int height, unsigned char *out, int pallet_idx);
public:
  Parser() {
    content = new game_content;
  }

  ~Parser() {
    for (const auto& e: content->maps) {
      delete e;
    }
    delete[] content->color_palette[0];
    delete content;
  }


  ///Loads level into parser, load color pallet
  ///@param file_name defines the path to read file from
  void load_file(char *file_name);

  ///
  std::vector<std::string> get_levels() const;
  original_classes::palette* get_palette(int id) const;

  unsigned char *get_color_map(int id);
  //TODO - Convert an old map to new std::vector<map> get_level(int id);
};



#endif //PARSER_H
