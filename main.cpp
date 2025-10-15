#include <iostream>
#include <GL/glew.h>
#include <ostream>
#include <SDL2/SDL.h>
#include <gl/gl.h>
#include <src/headers/game.h>

#include "src/headers/parser.h"

// int main(int argc, char *argv[]) {
// 	Parser parser;
// 	char* filename = "../wad/Doom2.wad";
// 	parser.load_file(filename);
// 	auto res = parser.get_levels();
//
//
// 	for (auto& level : res) {
// 		std::cout << level << "\n";
// 	}
//
// 	printf("Select level to export");
//
// 	auto id = -1;
// 	scanf("%d", &id);
//
// 	if (id < 0|| id > res.size()) {
// 		throw std::runtime_error("Invalid level id");
// 	}
//
// 	parser.obj_export(id, "par.obj");
// 	//parser.find_sector(0, {1376, -3360, 8});
// 	//parser.find_sector(0, {1376, -3360, -56});
// 	return 0;
// }

void lookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ);

int main(int argc, char* argv[]) {

    Game game = Game("Doom1.WAD");
    game.Init();
    game.SelectMap(0);
    game.Run();

    // Parser* p = new Parser();
    // p->load_file("./wad/PAR.wad");
    // p->obj_export(1, "par_e1m2_triangles.obj");
}
