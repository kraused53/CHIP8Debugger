#include "Debugger.h"

Debugger::Debugger( void ) {
	initialized = false;
	running = false;

	// Attempt to start SDL
	if ( SDL_Init( SDL_INIT_EVERYTHING ) != 0 ) {
		Logger::Err( std::format( "Could not initialize SDL subsystems: {}", SDL_GetError() ));
		return;
	}

	// Attempt to create window
	window = SDL_CreateWindow( "CHIP 8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH * WINDOW_SCALE, WINDOW_HEIGHT * WINDOW_SCALE, SDL_WINDOW_SHOWN );
	if ( !window ) {
		Logger::Err( std::format( "Could not initialize SDL window: {}", SDL_GetError() ) );
		return;
	}

	// Attempt to create renderer
	renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
	if ( !renderer ) {
		Logger::Err( std::format( "Could not initialize SDL renderer: {}", SDL_GetError() ) );
		return;
	}
	
	// Attempt to create texture
	texture = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT );
	if ( !texture ) {
		Logger::Err( std::format( "Could not initialize SDL texture: {}", SDL_GetError() ) );
		return;
	}

	Logger::Log( "Debugger initialized without error" );
	initialized = true;
}

Debugger::~Debugger( void ) {
	Logger::Log( "Exiting debugger..." );

	SDL_DestroyTexture( texture );
	SDL_DestroyRenderer( renderer );
	SDL_DestroyWindow( window );
	SDL_Quit();
}

void Debugger::setup( void ) {
	chip.reset();

	// Load ROM

	// No errors, start emulator
	running = true;
}

void Debugger::handle_press( SDL_Event* e ) {
	switch ( e->key.keysym.sym ) {
		// Press ESC to exit debugger
		case SDLK_ESCAPE: running = false; break;
		default: break;
	}
}

void Debugger::handle_release( SDL_Event* e ) {
	// TODO
}

void Debugger::input( void ) {
	SDL_Event e;

	while ( SDL_PollEvent( &e ) ) {
		switch ( e.type ) {
			case SDL_QUIT: running = false; break;
			case SDL_KEYDOWN: handle_press( &e ); break;
			case SDL_KEYUP: handle_release( &e ); break;
			default: break;
		}
	}
}

void Debugger::update( void ) {
	// Handle emulation
	chip.step();

	// Handle timing
}

void Debugger::render( void ) {
	// Render
}

void Debugger::run( void ) {
	// Load rom and start emulator
	setup();

	// Loop until running is false
	while ( running ) {
		// Handle user input
		input();
		// Update system
		update();
		// Render system state
		render();
	}
}