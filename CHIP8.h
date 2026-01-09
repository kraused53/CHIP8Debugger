#pragma once

#include <cstdint>
#include <vector>
#include <format>
#include <fstream>

#include "Logger.h"

const int PIXEL_COLOR = 0x00FF00FF;

class CHIP8 {
	// All items are public to allow easy access for debugger
public:
	// System memory
	std::vector<uint8_t> memory;

	// Display RAM -> using 32 bit for easier SDL texture usage
	uint32_t display[ 64 * 32 ];

	// System registers
	uint8_t V[ 16 ];

	// Stack
	uint16_t S[ 16 ];

	// Keys
	bool keys[ 16 ];

	// System Pointers
	uint16_t PC;
	uint16_t IP;
	uint8_t SP;

	// Timers
	uint8_t sound;
	uint8_t delay;

	// Constructors
	CHIP8( void );
	~CHIP8( void );

	// Emulation
	bool draw_ready;
	bool error;
	uint16_t opcode;
	void clear_display( void );
	void reset( void );
	uint16_t get_NNN( void );
	uint8_t get_NN( void );
	uint8_t get_N( void );
	uint8_t get_X( void );
	uint8_t get_Y( void );
	uint8_t get_INDEX( void );
	void emulate( void );
	void step( void );
	bool load_rom( const char* rom );
	void DBG( void );

	// Opcodes -> any opcode that is more than one line gets its own function
	void UNI( void );
	void DRAW( void );
	void SYSCALL( void );
	void CALL( void );
};

