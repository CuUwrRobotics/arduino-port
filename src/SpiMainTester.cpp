#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "Ardu_SPI.h"

int step = 1;
int transactionNum = 1; \

uint16_t tx = 0b1010010010100100; // Transmit data for testing
uint16_t txLsbFirst = 0x2525; // Backwards tx, for lsb first settings

uint32_t clockSpeedA = 15200; // 15.2 kHz
uint32_t clockSpeedB = 488000; // 488 kHz
// TODO: These are untested due to a lack of equipment during development
uint32_t clockSpeedC = 7800000; // 7.8 MHz
uint32_t clockSpeedD = 125000000; // Maximum value for the RPi = 125 MHz

uint8_t csPin = 25; // For testing with different CS pins. Uses WiringPi pins

// reversed data = 0x01, 0x04, 0x01...
// uint8_t data[8] = {0x80, 0x02, 0x80, 0x02, 0x80, 0x02, 0x80, 0x02};
// uint16_t data16[4] = {0x8002, 0x8002, 0x8002, 0x8002};

int main() {
	// SPI settings for tests
	SPISettings settingsMode0(500000, MSBFIRST, SPI_MODE0);
	SPISettings settingsMode1(500000, MSBFIRST, SPI_MODE1);
	SPISettings settingsMode2(500000, MSBFIRST, SPI_MODE2);
	SPISettings settingsMode3(500000, MSBFIRST, SPI_MODE3);

	SPISettings settingsClkA(clockSpeedA, MSBFIRST, SPI_MODE0);
	SPISettings settingsClkB(clockSpeedB, MSBFIRST, SPI_MODE0);
	SPISettings settingsClkC(clockSpeedC, MSBFIRST, SPI_MODE0);
	SPISettings settingsClkD(clockSpeedD, MSBFIRST, SPI_MODE0);

	SPISettings settingsMsbFirst(500000, MSBFIRST, SPI_MODE0);
	SPISettings settingsLsbFirst(500000, LSBFIRST, SPI_MODE0);

	SPISettings settingsCsLow(500000, MSBFIRST, SPI_MODE0, false);
	SPISettings settingsCsHigh(500000, MSBFIRST, SPI_MODE0, true);
	SPISettings settingsCustomCsLow(500000, MSBFIRST, SPI_MODE0, false, csPin);
	SPISettings settingsCustomCsHigh(500000, MSBFIRST, SPI_MODE0, true, csPin);

	printf("%d: Initializing SPI.\n", step++);
	SPI.begin();

	printf("%d: Starting transactions. Data for transactions: 0x%4x = 0b",
	       step++, tx);
	for (int i = 0; i < 16; i++)
		printf("%d", (tx >> i) & 0x0001);
	printf("\n");

	// Arduino clock mode tests (see arduino docs)
	printf("Transaction #%d: clock mode 0\n", transactionNum++);
	SPI.beginTransaction(settingsMode0);
	SPI.transfer16(tx);
	SPI.endTransaction();
	printf("Transaction #%d: clock mode 1\n", transactionNum++);
	SPI.beginTransaction(settingsMode1);
	SPI.transfer16(tx);
	SPI.endTransaction();
	printf("Transaction #%d: clock mode 2\n", transactionNum++);
	SPI.beginTransaction(settingsMode2);
	SPI.transfer16(tx);
	SPI.endTransaction();
	printf("Transaction #%d: clock mode 3\n", transactionNum++);
	SPI.beginTransaction(settingsMode3);
	SPI.transfer16(tx);
	SPI.endTransaction();

	// Clock speed tests
	printf("Transaction #%d: clock speed: \n", transactionNum++, clockSpeedA);
	SPI.beginTransaction(settingsClkA);
	SPI.transfer16(tx);
	SPI.endTransaction();
	printf("Transaction #%d: clock speed: \n", transactionNum++, clockSpeedB);
	SPI.beginTransaction(settingsClkB);
	SPI.transfer16(tx);
	SPI.endTransaction();
	printf("Transaction #%d: clock speed: \n", transactionNum++, clockSpeedC);
	SPI.beginTransaction(settingsClkC);
	SPI.transfer16(tx);
	SPI.endTransaction();
	printf("Transaction #%d: clock speed: \n", transactionNum++, clockSpeedD);
	SPI.beginTransaction(settingsClkD);
	SPI.transfer16(tx);
	SPI.endTransaction();

	// LSB tests
	printf("Transaction #%d: MSB first (default, no change)\n", transactionNum++);
	SPI.beginTransaction(settingsMsbFirst);
	SPI.transfer16(tx);
	SPI.endTransaction();
	printf(
		"Transaction #%d: LSB first (sending LSB first version of normal data; actual output on SPI bus should be identical to other tests!)\n",
		transactionNum++);
	SPI.beginTransaction(settingsLsbFirst);
	SPI.transfer16(txLsbFirst); // Send reversed data so that output is inverted
	SPI.endTransaction();

	// CS Pin tests
	printf("Transaction #%d: Default CS pin, active low (default)\n",
	       transactionNum++);
	SPI.beginTransaction(settingsCsLow);
	SPI.transfer16(tx);
	SPI.endTransaction();
	printf("Transaction #%d: Default CS pin, active high\n", transactionNum++);
	SPI.beginTransaction(settingsCsHigh);
	SPI.transfer16(tx);
	SPI.endTransaction();
	printf("Transaction #%d: Non-default CS pin = %d, active low\n",
	       transactionNum++, csPin);
	SPI.beginTransaction(settingsCustomCsLow);
	SPI.transfer16(tx);
	SPI.endTransaction();
	printf("Transaction #%d: Non-default CS pin = %d, active high\n",
	       transactionNum++, csPin);
	SPI.beginTransaction(settingsCustomCsHigh);
	SPI.transfer16(tx);
	SPI.endTransaction();

	printf("%d: Finished.\n", step++);
} // main
