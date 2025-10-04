#include <iostream>
#include <ostream>

#include "src/headers/parser.h"

int main(int argc, char *argv[]) {
	Parser parser;
	char* filename = "../wad/Doom1.WAD";
	parser.load_file(filename);
	auto res = parser.get_levels();
	for (auto& level : res) {
		std::cout << level << "\n";
	}
	return 0;
}
