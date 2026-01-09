#include "Debugger.h"

int main( int argc, char** argv ) {
	Debugger chip;

	chip.setup();
	chip.run();

	return 0;
}