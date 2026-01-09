#pragma once

#include <format>

// SDL
#include <SDL.h>
const int WINDOW_SCALE = 10;
const int WINDOW_WIDTH = 64;
const int WINDOW_HEIGHT = 32;

#include "Logger.h"
#include "CHIP8.h"

class Debugger {
private:
	// SDL Variables
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;

	// Emulation Variables
	bool running;
	bool initialized;

	// CHIP 8 instance
	CHIP8 chip;

public:
	// Construction
	Debugger( void );
	~Debugger( void );

	// Input Handlers
	void handle_press( SDL_Event* e );
	void handle_release( SDL_Event* e );

	// Emulation Loop
	void setup( void );
	void input( void );
	void update( void );
	void render( void );
	void run( void );
};

