// #include "ArduinoPort.h"
#include "Ardu_Wire.h" // Arduino wire port
uint8_t i2cAddr = 0x3C;
uint8_t helloMsg[8] = "HELLO!\n";

/**
 * @return TODO
 */
int main(){
	printf("1. wire begin\n");
	Wire.begin();
	printf("2. Begin transmission\n");
	Wire.beginTransmission(i2cAddr);

	printf("3. writing data array into buffer: %d\n", Wire.write(helloMsg, 7));

	printf("4. ending transmission: %d\n", Wire.endTransmission());

	printf("5. Writing data directly to I2C lines individually: %d\n",
	       Wire.write('H'),
	       Wire.write('E'),
	       Wire.write('L'),
	       Wire.write('L'),
	       Wire.write('O'),
	       Wire.write('!'),
	       Wire.write('\n'));

	printf("6. Writing data array directly to I2C lines: %d\n",
	       Wire.write(helloMsg, 7));

	printf("7. Reading: ");
	while (Wire.available() > 0)
		Wire.read();
	// printf("0x%2x, ", Wire.read());
	printf("\n");
	Wire.requestFrom(i2cAddr, 1, 0x01, 1, 0);
	printf("8. Requsting 0x1: 0x%2x\n", Wire.read());
	Wire.requestFrom(i2cAddr, 1, 0x02, 1, 0);
	printf("9. Requsting 0x2: 0x%2x\n", Wire.read());

	printf("__ Finished wire\n");
	// SPI.begin();
	// printf("created spi\n");
} // main
