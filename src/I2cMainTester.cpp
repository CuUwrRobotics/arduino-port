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

	printf("3. Writing data array '%s' into buffer\n", helloMsg);
	Wire.write(helloMsg, 7);

	printf("4. Ending transmission\n");
	Wire.endTransmission();

	printf("5. Writing data bytes 'HELLO!\\n' directly and individually\n");
	Wire.write('H');
	Wire.write('E');
	Wire.write('L');
	Wire.write('L');
	Wire.write('O');
	Wire.write('!');
	Wire.write('\n');

	printf("6. Writing data array '%s' directly\n", helloMsg);
	Wire.write(helloMsg, 7);

	printf("7. Reading data: ");
	while (Wire.available() > 0)
		Wire.read();
	printf("\n");

	uint8_t data = 0;
	printf("8. Requsting 0x01: ");
	Wire.requestFrom(i2cAddr, 1, 0x01, 1, 0);
	data = Wire.read();
	printf("0x%2x\n", data);
	printf("9. Requsting 0x02: ");
	Wire.requestFrom(i2cAddr, 1, 0x02, 1, 0);
	data = Wire.read();
	printf("0x%2x\n", data);

	printf("\nFinished wire. Closing.\n");
	Wire.end();
} // main
