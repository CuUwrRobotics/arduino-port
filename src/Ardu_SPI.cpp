/**
 * @Author: Nick Steele
 * @Date:   19:49 Aug 10 2020
 * @Last modified by:   Nick Steele
 * @Last modified time: 19:21 Feb 13 2021
 */

// Modified by Nicholas Steele to help port Arduino libraries to Raspberry Pi
// Files modifications for port are marked with a # in comments
// Original files gotten from arduino AVR core github 7/5/2020.

#include "arduino_port_lib/Ardu_SPI.h"

SPIClass SPI;

char SPIClass::RED[10] = "\033[1;31m";
char SPIClass::NO_COLOR[7] = "\033[0m";

uint8_t SPIClass::mode = 0;
uint8_t SPIClass::bits = 0;
uint32_t SPIClass::speed = 0;
bool SPIClass::lsbFirst = 0;
uint8_t SPIClass::csPin = 0;
bool SPIClass::csHigh = 0;
uint8_t SPIClass::spiDeviceFile = 0;

uint8_t SPIClass::initialized = 0;
uint8_t SPIClass::interruptMode = 0;
uint8_t SPIClass::interruptMask = 0;
uint8_t SPIClass::interruptSave = 0;

void SPIClass::begin(){
} // SPIClass::begin

/**
 * -> Disable SPI if there are no more transactions.
 * -> Decrement initialized if it != 0
 */

void SPIClass::end() {
} // SPIClass::end

/*******************************************************************************
 # All the following SPI interfacing involves interrupts. If possible, interrupts
 # will be avoided to prevent interference in larger functions.
 */

void SPIClass::usingInterrupt(uint8_t interruptNumber)
{
	printf("%s%s in %s:%d This function is not implmented for the RPi port.%s",
	       RED, __PRETTY_FUNCTION__, __FILE__, __LINE__, NO_COLOR);
} // SPIClass::usingInterrupt

void SPIClass::notUsingInterrupt(uint8_t interruptNumber)
{
	printf("%s%s in %s:%d This function is not implmented for the RPi port.%s",
	       RED, __PRETTY_FUNCTION__, __FILE__, __LINE__, NO_COLOR);
} // SPIClass::notUsingInterrupt
