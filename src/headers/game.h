#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_opengl.h>
#include <iostream>
#include "Parser.h" // Assume this defines Parser and Map
#include "Player.h"

class Game {
public:
	Game(char* file_path);
	~Game();
	int Init();
	void SelectMap(int id);
	void Run();

private:
	Parser* parser;
	NewModels::Map* current_map;
	Player* player;
	SDL_Window* window;
	SDL_GLContext gl_context;
};

#endif