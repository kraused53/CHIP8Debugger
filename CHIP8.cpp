#include "CHIP8.h"
#include "Logger.h"

CHIP8::CHIP8( void ) {
	Logger::Log( "Initializing CHIP 8 system" );
	// Resize memory -> 4K system memory
	memory.resize( 4 * 1024 );

	// Resize display ram -> 64 * 32 system memory
	display.resize( 64 * 32 );

	reset();
}

CHIP8::~CHIP8( void ) {
	// Nothing to do yet
}

void CHIP8::reset( void ) {

	int i = 0;

	// Clear memory
	for ( i = 0; i < ( 4 * 1024 ); i++ ) {
		memory[ i ] = 0x00;
	}

	// Clear display ram and flag redraw
	for ( i = 0; i < ( 64 * 32 ); i++ ) {
		display[ i ] = 0x00000000;
	}
	draw_ready = true;

	// Clear Stack
	for ( i = 0; i < 16; i++ ) {
		S[ i ] = 0x00;
	}

	// Clear Keys
	for ( i = 0; i < 16; i++ ) {
		keys[ i ] = false;
	}

	// Clear registers
	for ( i = 0; i < 16; i++ ) {
		V[i] = 0x00;
	}

	// Clear pointers and counters
	PC = 0x0200;
	IP = 0x0000;
	sound = 0x00;
	delay = 0x00;

	// Clear emulation data
	error = false;
	opcode = 0x0000;
}

uint16_t CHIP8::get_NNN( void ) { return opcode & 0x0FFF; }

uint8_t CHIP8::get_NN( void ) { return opcode & 0xFF; }

uint8_t CHIP8::get_N( void ) { return opcode & 0x0F; }

uint8_t CHIP8::get_X( void ) { return ( opcode & 0x0F00 ) >> 8; }

uint8_t CHIP8::get_Y( void ) { return ( opcode & 0x00F0 ) >> 4; }

uint8_t CHIP8::get_INDEX( void ) { return ( opcode & 0xF000 ) >> 12; }

void CHIP8::step( void ) {
	// Fetch next opcode
	opcode = ( memory[ PC ] << 8 ) | ( memory[ PC + 1 ] );
	//PC += 2;


}