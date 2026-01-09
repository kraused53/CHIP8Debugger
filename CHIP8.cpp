#include "CHIP8.h"

CHIP8::CHIP8( void ) {
	Logger::Log( "Initializing CHIP 8 system" );
	// Resize memory -> 4K system memory
	memory.resize( 4 * 1024 );

	reset();
}

CHIP8::~CHIP8( void ) {
	// Nothing to do yet
}

void CHIP8::clear_display( void ) {
	for ( int i = 0; i < ( 64 * 32 ); i++ ) {
		display[ i ] = 0x00000000;
	}
	draw_ready = true;
}

void CHIP8::reset( void ) {

	int i = 0;

	// Clear memory
	for ( i = 0; i < ( 4 * 1024 ); i++ ) {
		memory[ i ] = 0x00;
	}

	// Clear display ram and flag redraw
	clear_display();

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
		V[ i ] = 0x00;
	}

	// Clear pointers and counters
	PC = 0x0200;
	IP = 0x0000;
	SP = 0x00;
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

void CHIP8::emulate( void ) {
	switch ( get_INDEX() ) {
		case 0x0: SYSCALL(); break;
		case 0x1: PC = get_NNN(); break;
		case 0x6: V[ get_X() ] = get_NN(); break;
		case 0x7: V[ get_X() ] += get_NN(); break;
		case 0xA: IP = get_NNN(); break;
		case 0xD: DRAW(); break;
		default: UNI(); break;
	}
}

void CHIP8::step( void ) {
	// Fetch next opcode
	opcode = ( memory[ PC ] << 8 ) | ( memory[ PC + 1 ] );
	PC += 2;

	//DBG();

	emulate();
}

void CHIP8::UNI( void ) {
	error = true;
	Logger::Err( std::format( "ERROR at ${:04X} opcode 0x{:04X} not implemented", PC - 2, opcode ) );
}

void CHIP8::DRAW( void ) {
	uint8_t xPos = V[ get_X() ] % 64;
	uint8_t yPos = V[ get_Y() ] % 32;
	V[ 0xF ] = 0x00;

	for ( unsigned int row = 0; row < get_N(); ++row ) {
		uint8_t spriteByte = memory[ IP + row ];

		for ( unsigned int col = 0; col < 8; ++col ) {
			uint8_t spritePixel = spriteByte & ( 0x80u >> col );
			uint32_t* screenPixel = &display[ ( yPos + row ) * 64 + ( xPos + col ) ];

			// Sprite pixel is on
			if ( spritePixel ) {
				// Screen pixel also on - collision
				if ( *screenPixel == PIXEL_COLOR ) {
					V[ 0xF ] = 1;
				}

				// Effectively XOR with the sprite pixel
				*screenPixel ^= PIXEL_COLOR;
			}
		}
	}

	draw_ready = true;
}

void CHIP8::SYSCALL( void ) {
	switch ( get_NNN() ) {
		case 0x00E0: clear_display(); break;
		case 0x00EE: UNI(); break;
		default: UNI(); break;
	}
}

bool CHIP8::load_rom( const char* rom ) {
	std::ifstream inputFile( rom, std::ios_base::binary );

	// Determine the length of the file by seeking
	// to the end of the file, reading the value of the
	// position indicator, and then seeking back to the beginning.
	inputFile.seekg( 0, std::ios_base::end );
	auto length = inputFile.tellg();
	inputFile.seekg( 0, std::ios_base::beg );

	// Make a buffer of the exact size of the file and read the data into it.
	std::vector<uint8_t> buffer( length );
	inputFile.read( reinterpret_cast< char* >( buffer.data() ), length );
	inputFile.close();

	for ( int i = 0; i < length; i++ ) {
		memory[ 0x0200 + i ] = buffer[ i ];
	}

	return true;
}

void CHIP8::DBG( void ) {
	Logger::Log( std::format( "${:04X} [ {:04X} ] ", PC - 2, opcode ) );

	switch ( opcode & 0xF000 ) {
		// System Codes
		case 0x0000:
			if ( opcode == 0x00E0 ) {
				Logger::Log( std::format( "Clear Screen" ) );
			} else if ( opcode == 0x00E0 ) {
				Logger::Log( std::format( "Return" ) );
			} else {
				Logger::Log( std::format( "Invalid Opcode" ) );
			}
			break;

			// JMP
		case 0x1000: Logger::Log( std::format( "JMP 0x{:04X}", get_NNN() ) ); break;

			// Call
		case 0x2000: Logger::Log( std::format( "CALL 0x{:04X}", get_NNN() ) ); break;

			// Skip if Vx equal nn
		case 0x3000: Logger::Log( std::format( "Skip if V[{:01X}] == 0x{:02X}", get_X(), get_NN() ) ); break;

			// Skip if Vx not equal nn
		case 0x4000: Logger::Log( std::format( "Skip if V[{:01X}] != 0x{:02X}", get_X(), get_NN() ) ); break;

			// Skip if Vx equal Vy
		case 0x5000: Logger::Log( std::format( "Skip if V[{:01X}] == V[{:01X}]", get_X(), get_Y() ) ); break;

			// Vx = nn
		case 0x6000: Logger::Log( std::format( "V[{:01X}] = 0x{:02X}", get_X(), get_NN() ) ); break;

			// Vx += nn ( no flag )
		case 0x7000: Logger::Log( std::format( "V[{:01X}] += 0x{:02X} ( No Flag )", get_X(), get_NN() ) ); break;

			// ALU
		case 0x8000:
			if ( get_N() == 0 ) {
				Logger::Log( std::format( "V[{:02X}] = V[{:02X}]", get_X(), get_Y() ) );
			} else if ( get_N() == 1 ) {
				Logger::Log( std::format( "V[{:02X}] |= V[{:02X}]", get_X(), get_Y() ) );
			} else if ( get_N() == 2 ) {
				Logger::Log( std::format( "V[{:02X}] &= V[{:02X}]", get_X(), get_Y() ) );
			} else if ( get_N() == 3 ) {
				Logger::Log( std::format( "V[{:02X}] ^= V[{:02X}]", get_X(), get_Y() ) );
			} else if ( get_N() == 4 ) {
				Logger::Log( std::format( "V[{:02X}] += V[{:02X}] ( With Overflow Flag )", get_X(), get_Y() ) );
			} else if ( get_N() == 5 ) {
				Logger::Log( std::format( "V[{:02X}] -= V[{:02X}] ( With Underflow Flag )", get_X(), get_Y() ) );
			} else if ( get_N() == 6 ) {
				Logger::Log( std::format( "V[{:02X}] >>= 1 ( V[F] = old LSB )", get_X(), get_Y() ) );
			} else if ( get_N() == 7 ) {
				Logger::Log( std::format( "V[{:02X}] = V[{:02X}] - V[{:02X}] ( With Overflow Flag )", get_X(), get_Y(), get_X() ) );
			} else if ( get_N() == 0xE ) {
				Logger::Log( std::format( "V[{:02X}] <<= 1 ( V[F] = old MSB )", get_X(), get_Y() ) );
			} else {
				Logger::Log( std::format( "Invalid Opcode" ) );
			}
			break;

			// Skip if Vx not equal Vy
		case 0x9000: Logger::Log( std::format( "Skip if V[{:01X}] != V[{:01X}]", get_X(), get_Y() ) ); break;

			// Set IP
		case 0xA000: Logger::Log( std::format( "I = 0x{:04X}", get_NNN() ) ); break;

			// JMP with offset
		case 0xB000: Logger::Log( std::format( "JMP {:04X} + V[0]", get_NNN() ) ); break;

			// Random number
		case 0xC000: Logger::Log( std::format( "V[{:01X}] = random number between 0 and 0x{:02X}", get_X(), get_NN() ) ); break;

			// Display
		case 0xD000: Logger::Log( std::format( "Draw {} bit wide sprite at location [ V[{:02X}], V[{:02X}] ]", get_N(), get_X(), get_Y() ) ); break;

			// Keys
		case 0xE000:
			if ( get_NN() == 0x9E ) {
				Logger::Log( std::format( "Skip if the key in V[{:02X}] is pressed", get_X() ) );
			} else if ( get_NN() == 0xA1 ) {
				Logger::Log( std::format( "Skip if the key in V[{:02X}] is not pressed", get_X() ) );
			} else {
				Logger::Log( std::format( "Invalid Opcode" ) );
			}
			break;

			// MISC
		case 0xF000:
			if ( get_NN() == 0x07 ) {
				Logger::Log( std::format( "V[{:02X}] = delay timer value", get_X() ) );
			} else if ( get_NN() == 0x0A ) {
				Logger::Log( std::format( "Wait until a key is pressed, then store it in V[{:02X}]", get_X() ) );
			} else if ( get_NN() == 0x15 ) {
				Logger::Log( std::format( "Delay timer value = V[{:02X}]", get_X() ) );
			} else if ( get_NN() == 0x18 ) {
				Logger::Log( std::format( "Sound timer value = V[{:02X}]", get_X() ) );
			} else if ( get_NN() == 0x1E ) {
				Logger::Log( std::format( "IP += V[{:02X}]", get_X() ) );
			} else if ( get_NN() == 0x29 ) {
				Logger::Log( std::format( "Set IP to location of font character ( {:01X} )", V[ get_X() & 0x0F ] ) );
			} else if ( get_NN() == 0x33 ) {
				Logger::Log( std::format( "Store V[{:02X}] in memory at ${:04X} in BCD", get_X(), IP ) );
			} else if ( get_NN() == 0x55 ) {
				Logger::Log( std::format( "Store registers in memory at ${:04X}", IP ) );
			} else if ( get_NN() == 0x65 ) {
				Logger::Log( std::format( "Read registers from memory at ${:04X}", IP ) );
			} else {
				Logger::Log( std::format( "Invalid Opcode" ) );
			}
			break;


		default:
			Logger::Log( std::format( "Invalid Opcode" ) );
			break;
	}
}