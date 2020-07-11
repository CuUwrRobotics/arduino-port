// #include "ArduinoPort.h"
#include "Ardu_Wire.h" // Arduino wire port

/**
 * @return TODO
 */
int main(){
	printf("1. wire begin\n");
	Wire.begin();
	printf("2. Begin transmission\n");
	Wire.beginTransmission(0x3C);
	printf("3. writing data into buffer: %d, %d, %d, %d, %d, %d, %d\n",
	       Wire.write('H'),
	       Wire.write('E'),
	       Wire.write('L'),
	       Wire.write('L'),
	       Wire.write('O'),
	       Wire.write('!'),
	       Wire.write('\n'));
	printf("4. ending transmission: %d\n", Wire.endTransmission());
	printf("5. writing data 'response' into buffer: %d, %d, %d, %d, %d, %d, %d\n",
	       Wire.write('H'),
	       Wire.write('E'),
	       Wire.write('L'),
	       Wire.write('L'),
	       Wire.write('O'),
	       Wire.write('!'),
	       Wire.write('\n'));
	// printf("5. Reading: ");
	// while (Wire.available() > 0)
	// 	printf("0x%2x, ", Wire.read());
	printf("\n5. Finished wire\n");
	// SPI.begin();
	// printf("created spi\n");
} // main
