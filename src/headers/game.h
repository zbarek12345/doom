#ifndef GAME_H
#define GAME_H

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <iostream>
#include "parser.h" // Assume this defines Parser and Map
#include "Player.h"

class Game {
public:
	Game(char* file_path);

	Game(char *file_path, char **patch_files);

	~Game();
	int Init();
	void SelectMap(int id);
	void Run();

	static void GetScreenSize(uint16_t& width, uint16_t& height);

private:
	static uint16_t screen_width, screen_height;
	Parser* parser;
	NewModels::Map* current_map;
	Player* player;
	SDL_Window* window;
	SDL_GLContext gl_context;
};

#endif