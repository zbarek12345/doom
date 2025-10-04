#include <iostream>
#include <ostream>

#include "src/headers/parser.h"

int main(int argc, char *argv[]) {
	Parser parser;
	char* filename = "../src/assets/HERETIC.WAD";
	parser.load_file(filename);

	auto res = parser.get_levels();
	for (auto& level : res) {
		std::cout << level << "\n";
	}

	return 0;
}
