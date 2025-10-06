#include <iostream>
#include <ostream>

#include "src/headers/parser.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "./src/headers/stb_image_write.h"

int main(int argc, char *argv[]) {
	Parser parser;
	char* filename = "../src/assets/HERETIC.WAD";
	parser.load_file(filename);

	// show bmp
	for (int i=0;i<34;i++) {
	 	auto p = parser.get_color_map(i);
	 	std::string name = "out"+std::to_string(i)+".bmp";
	 	stbi_write_bmp(name.c_str(), 16, 16, 3, p);
	}

	// auto res = parser.get_levels();
	// for (auto& level : res) {
	// 	std::cout << level << "\n";
	// }

	return 0;
}
