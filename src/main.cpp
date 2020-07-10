#include "includes/ArduinoPort.h"

/**
 * @return TODO
 */
int main(){
	Wire.begin();
	printf("created wire\n");
	SPI.begin();
	printf("created spi\n");
} // main
