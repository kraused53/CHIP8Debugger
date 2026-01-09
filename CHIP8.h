#pragma once

#include <cstdint>
#include <vector>

class CHIP8 {
	// All items are public to allow easy access for debugger
public:
	// System memory
	std::vector<uint8_t> memory;

	// Display RAM -> using 32 bit for easier SDL texture usage
	std::vector<uint32_t> display;

	// System registers
	uint8_t V[ 16 ];

	// Stack
	uint16_t S[ 16 ];

	// Keys
	bool keys[ 16 ];

	// System Pointers
	uint16_t PC;
	uint16_t IP;

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
	void reset( void );
	uint16_t get_NNN( void );
	uint8_t get_NN( void );
	uint8_t get_N( void );
	uint8_t get_X( void );
	uint8_t get_Y( void );
	uint8_t get_INDEX( void );
	void step( void );
};

