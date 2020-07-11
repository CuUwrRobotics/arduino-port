// // Modified by Nicholas Steele to help port Arduino libraries to Raspberry Pi
// // Files modifications for port are marked with a # in comments
//
// /*
//  * Copyright (c) 2010 by Cristian Maglie <c.maglie@arduino.cc>
//  * Copyright (c) 2014 by Paul Stoffregen <paul@pjrc.com> (Transaction API)
//  * Copyright (c) 2014 by Matthijs Kooijman <matthijs@stdin.nl> (SPISettings AVR)
//  * Copyright (c) 2014 by Andrew J. Kroll <xxxajk@gmail.com> (atomicity fixes)
//  * SPI Master library for arduino.
//  *
//  * This file is free software; you can redistribute it and/or modify
//  * it under the terms of either the GNU General Public License version 2
//  * or the GNU Lesser General Public License version 2.1, both as
//  * published by the Free Software Foundation.
//  */
//
// #include "ArduinoPort.h"
//
// SPIClass SPI;
//
// uint8_t SPIClass::initialized = 0;
// uint8_t SPIClass::interruptMode = 0;
// uint8_t SPIClass::interruptMask = 0;
// uint8_t SPIClass::interruptSave = 0;
//
// /**
//  * -> initialized++
//  * -> Set up any pin modes and states. Pins were all predefined
//  * -> SS set to high to deselect devices
//  *
//  * Original would setup pin modes and states.
//  */
//
// void SPIClass::begin()
// {
// 	// uint8_t sreg = SREG;
// 	// noInterrupts(); // Protect from a scheduler and prevent transactionBegin
// 	// if (!initialized) {
// 	// 	// Set SS to high so a connected chip will be "deselected" by default
// 	// 	uint8_t port = digitalPinToPort(SS);
// 	// 	uint8_t bit = digitalPinToBitMask(SS);
// 	// 	volatile uint8_t *reg = portModeRegister(port);
// 	//
// 	// 	// if the SS pin is not already configured as an output
// 	// 	// then set it high (to enable the internal pull-up resistor)
// 	// 	if (!(*reg & bit)) {
// 	// 		digitalWrite(SS, HIGH);
// 	// 	}
// 	//
// 	// 	// When the SS pin is set as OUTPUT, it can be used as
// 	// 	// a general purpose output port (it doesn't influence
// 	// 	// SPI operations).
// 	// 	pinMode(SS, OUTPUT);
// 	//
// 	// 	// Warning: if the SS pin ever becomes a LOW INPUT then SPI
// 	// 	// automatically switches to Slave, so the data direction of
// 	// 	// the SS pin MUST be kept as OUTPUT.
// 	// 	SPCR |= _BV(MSTR);
// 	// 	SPCR |= _BV(SPE);
// 	//
// 	// 	// Set direction register for SCK and MOSI pin.
// 	// 	// MISO pin automatically overrides to INPUT.
// 	// 	// By doing this AFTER enabling SPI, we avoid accidentally
// 	// 	// clocking in a single bit since the lines go directly
// 	// 	// from "input" to SPI control.
// 	// 	// http://code.google.com/p/arduino/issues/detail?id=888
// 	// 	pinMode(SCK, OUTPUT);
// 	// 	pinMode(MOSI, OUTPUT);
// 	// }
// 	// initialized++; // reference count
// 	// SREG = sreg;
// } // SPIClass::begin
//
// /**
//  * -> Disable SPI if there are no more transactions.
//  * -> Decrement initialized if it != 0
//  */
//
// void SPIClass::end() {
// 	// uint8_t sreg = SREG;
// 	// noInterrupts(); // Protect from a scheduler and prevent transactionBegin
// 	// // Decrease the reference counter
// 	// if (initialized)
// 	// 	initialized--;
// 	// // If there are no more references disable SPI
// 	// if (!initialized) {
// 	// 	SPCR &= ~_BV(SPE);
// 	// 	interruptMode = 0;
// 	// }
// 	// SREG = sreg;
// } // SPIClass::end
//
// /*******************************************************************************
//  # All the following SPI interfacing involves interrupts. If possible, it will
//  # be avoided to prevent interference in the board_interface package which handles
//  # all comms.
//  */
//
// // mapping of interrupt numbers to bits within SPI_AVR_EIMSK
// // #if defined(__AVR_ATmega32U4__)
// // 	#define SPI_INT0_MASK  (1 << INT0)
// // 	#define SPI_INT1_MASK  (1 << INT1)
// // 	#define SPI_INT2_MASK  (1 << INT2)
// // 	#define SPI_INT3_MASK  (1 << INT3)
// // 	#define SPI_INT4_MASK  (1 << INT6)
// // #elif defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__)
// // 	#define SPI_INT0_MASK  (1 << INT0)
// // 	#define SPI_INT1_MASK  (1 << INT1)
// // 	#define SPI_INT2_MASK  (1 << INT2)
// // 	#define SPI_INT3_MASK  (1 << INT3)
// // 	#define SPI_INT4_MASK  (1 << INT4)
// // 	#define SPI_INT5_MASK  (1 << INT5)
// // 	#define SPI_INT6_MASK  (1 << INT6)
// // 	#define SPI_INT7_MASK  (1 << INT7)
// // #elif defined(EICRA) && defined(EICRB) && defined(EIMSK)
// // 	#define SPI_INT0_MASK  (1 << INT4)
// // 	#define SPI_INT1_MASK  (1 << INT5)
// // 	#define SPI_INT2_MASK  (1 << INT0)
// // 	#define SPI_INT3_MASK  (1 << INT1)
// // 	#define SPI_INT4_MASK  (1 << INT2)
// // 	#define SPI_INT5_MASK  (1 << INT3)
// // 	#define SPI_INT6_MASK  (1 << INT6)
// // 	#define SPI_INT7_MASK  (1 << INT7)
// // #else // if defined(__AVR_ATmega32U4__)
// // 	#ifdef INT0
// // 	#define SPI_INT0_MASK  (1 << INT0)
// // 	#endif // ifdef INT0
// // 	#ifdef INT1
// // 	#define SPI_INT1_MASK  (1 << INT1)
// // 	#endif // ifdef INT1
// // 	#ifdef INT2
// // 	#define SPI_INT2_MASK  (1 << INT2)
// // 	#endif // ifdef INT2
// // #endif // if defined(__AVR_ATmega32U4__)
//
// /**
//  # Prints an error as this is not implmemented.
//  * @param interruptNumber TODO
//  * @return TODO
//  */
//
// void SPIClass::usingInterrupt(uint8_t interruptNumber)
// {
// 	ROS_ERROR(
// 		"ERROR: A DEVICE HAS ATTEMPTED TO USE INTERRUPTS IN THE ARDUINO PORT SPI CLASS, WHICH IS NOT IMPLEMENTED. ERROR FROM %s",
// 		__PRETTY_FUNCTION__);
// 	// uint8_t mask = 0;
// 	// uint8_t sreg = SREG;
// 	// noInterrupts(); // Protect from a scheduler and prevent transactionBegin
// 	// switch (interruptNumber) {
// 	// #ifdef SPI_INT0_MASK
// 	// case 0: mask = SPI_INT0_MASK; break;
// 	// #endif // ifdef SPI_INT0_MASK
// 	// #ifdef SPI_INT1_MASK
// 	// case 1: mask = SPI_INT1_MASK; break;
// 	// #endif // ifdef SPI_INT1_MASK
// 	// #ifdef SPI_INT2_MASK
// 	// case 2: mask = SPI_INT2_MASK; break;
// 	// #endif // ifdef SPI_INT2_MASK
// 	// #ifdef SPI_INT3_MASK
// 	// case 3: mask = SPI_INT3_MASK; break;
// 	// #endif // ifdef SPI_INT3_MASK
// 	// #ifdef SPI_INT4_MASK
// 	// case 4: mask = SPI_INT4_MASK; break;
// 	// #endif // ifdef SPI_INT4_MASK
// 	// #ifdef SPI_INT5_MASK
// 	// case 5: mask = SPI_INT5_MASK; break;
// 	// #endif // ifdef SPI_INT5_MASK
// 	// #ifdef SPI_INT6_MASK
// 	// case 6: mask = SPI_INT6_MASK; break;
// 	// #endif // ifdef SPI_INT6_MASK
// 	// #ifdef SPI_INT7_MASK
// 	// case 7: mask = SPI_INT7_MASK; break;
// 	// #endif // ifdef SPI_INT7_MASK
// 	// default:
// 	// 	interruptMode = 2;
// 	// 	break;
// 	// } // switch
// 	// interruptMask |= mask;
// 	// if (!interruptMode)
// 	// 	interruptMode = 1;
// 	// SREG = sreg;
// } // SPIClass::usingInterrupt
//
// /**
//  * @param interruptNumber TODO
//  * @return TODO
//  */
//
// void SPIClass::notUsingInterrupt(uint8_t interruptNumber)
// {
// 	ROS_ERROR(
// 		"ERROR: A DEVICE HAS ATTEMPTED TO USE INTERRUPTS IN THE ARDUINO PORT SPI CLASS, WHICH IS NOT IMPLEMENTED. ERROR FROM %s",
// 		__PRETTY_FUNCTION__);
// 	// Once in mode 2 we can't go back to 0 without a proper reference count
// 	// if (interruptMode == 2)
// 	// 	return;
// 	// uint8_t mask = 0;
// 	// uint8_t sreg = SREG;
// 	// noInterrupts(); // Protect from a scheduler and prevent transactionBegin
// 	// switch (interruptNumber) {
// 	// #ifdef SPI_INT0_MASK
// 	// case 0: mask = SPI_INT0_MASK; break;
// 	// #endif // ifdef SPI_INT0_MASK
// 	// #ifdef SPI_INT1_MASK
// 	// case 1: mask = SPI_INT1_MASK; break;
// 	// #endif // ifdef SPI_INT1_MASK
// 	// #ifdef SPI_INT2_MASK
// 	// case 2: mask = SPI_INT2_MASK; break;
// 	// #endif // ifdef SPI_INT2_MASK
// 	// #ifdef SPI_INT3_MASK
// 	// case 3: mask = SPI_INT3_MASK; break;
// 	// #endif // ifdef SPI_INT3_MASK
// 	// #ifdef SPI_INT4_MASK
// 	// case 4: mask = SPI_INT4_MASK; break;
// 	// #endif // ifdef SPI_INT4_MASK
// 	// #ifdef SPI_INT5_MASK
// 	// case 5: mask = SPI_INT5_MASK; break;
// 	// #endif // ifdef SPI_INT5_MASK
// 	// #ifdef SPI_INT6_MASK
// 	// case 6: mask = SPI_INT6_MASK; break;
// 	// #endif // ifdef SPI_INT6_MASK
// 	// #ifdef SPI_INT7_MASK
// 	// case 7: mask = SPI_INT7_MASK; break;
// 	// #endif // ifdef SPI_INT7_MASK
// 	// default:
// 	// 	break;
// 	// 	// this case can't be reached
// 	// } // switch
// 	// interruptMask &= ~mask;
// 	// if (!interruptMask)
// 	// 	interruptMode = 0;
// 	// SREG = sreg;
// } // SPIClass::notUsingInterrupt
