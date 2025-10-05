#include <iostream>
#include <ostream>

#include "src/headers/parser.h"

int main(int argc, char *argv[]) {
	Parser parser;
	char* filename = "../wad/Doom2.wad";
	parser.load_file(filename);
	auto res = parser.get_levels();


	for (auto& level : res) {
		std::cout << level << "\n";
	}

	printf("Select level to export");

	auto id = -1;
	scanf("%d", &id);

	if (id < 0|| id > res.size()) {
		throw std::runtime_error("Invalid level id");
	}

	parser.obj_export(id, "par.obj");
	//parser.find_sector(0, {1376, -3360, 8});
	//parser.find_sector(0, {1376, -3360, -56});
	return 0;
}
