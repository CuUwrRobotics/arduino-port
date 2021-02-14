/**
 * @Author: Nick Steele
 * @Date:   19:49 Aug 10 2020
 * @Last modified by:   Nick Steele
 * @Last modified time: 22:15 Feb 13 2021
 */

// Modified by Nicholas Steele to help port Arduino libraries to Raspberry Pi
// Files modifications for port are marked with a # in comments
// Original files gotten from arduino AVR core github 7/5/2020.

#ifndef _SPI_H_INCLUDED
#define _SPI_H_INCLUDED

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h> // Enums and ioctl() data handler
#include <stdio.h> // printf()
#include <errno.h> // errno()
// #include <stderr.h>
#include <string.h> // strerr(), memset()

#include "arduino_port_lib/BitReverse.h" // For software bit reversal. Just contains a table.

// #include <Arduino.h>

// SPI_HAS_TRANSACTION means SPI has beginTransaction(), endTransaction(),
// usingInterrupt(), and SPISetting(clock, bitOrder, dataMode)
#define SPI_HAS_TRANSACTION 1 // TODO

// SPI_HAS_NOTUSINGINTERRUPT means that SPI has notUsingInterrupt() method
#define SPI_HAS_NOTUSINGINTERRUPT 1 // TODO

// SPI_ATOMIC_VERSION means that SPI has atomicity fixes and what version.
// This way when there is a bug fix you can check this define to alert users
// of your code if it uses better version of this library.
// This also implies everything that SPI_HAS_TRANSACTION as documented above is
// available too.
#define SPI_ATOMIC_VERSION 1 // TODO

#ifndef LSBFIRST
#define LSBFIRST 0
#endif /* ifndef LSBFIRST */
#ifndef MSBFIRST
#define MSBFIRST 1
#endif /* ifndef MSBFIRST */

// Macros for porting to RPi
#define W_PI_HIGHEST_PIN 31
#define PIN_VALUE_DEFAULT_CS 0xFF
#define DEFAULT_SPI_DELAY 0 // Placeholder in case of future development
#define DEFAULT_SPI_DEV_FILE "/dev/spidev0.0" // TODO: make this selectable
// #define SPI_VERBOSE 1

// #define SPI_CLOCK_DIV4 0x00
// #define SPI_CLOCK_DIV16 0x01
// #define SPI_CLOCK_DIV64 0x02
// #define SPI_CLOCK_DIV128 0x03
// #define SPI_CLOCK_DIV2 0x04
// #define SPI_CLOCK_DIV8 0x05
// #define SPI_CLOCK_DIV32 0x06

#define SPI_MODE0 0x00
#define SPI_MODE1 0x04
#define SPI_MODE2 0x08
#define SPI_MODE3 0x0C

// #define SPI_MODE_MASK 0x0C // CPOL = bit 3, CPHA = bit 2 on SPCR
// #define SPI_CLOCK_MASK 0x03 // SPR1 = bit 1, SPR0 = bit 0 on SPCR
// #define SPI_2XCLOCK_MASK 0x01 // SPI2X = bit 0 on SPSR

// define SPI_AVR_EIMSK for AVR boards with external interrupt pins
#if defined(EIMSK)
  #define SPI_AVR_EIMSK  EIMSK
#elif defined(GICR)
  #define SPI_AVR_EIMSK  GICR
#elif defined(GIMSK)
  #define SPI_AVR_EIMSK  GIMSK
#endif /* if defined(EIMSK) */

class SPISettings {
public:

  /**
   * New arguments for the arduino/RPi port:
   * csPin: WiringPi pin for CS. 0xFF == default CS pin for the RPi SPI bus.
   *        Note: This is important to change if there are multiple chips per bus!
   * csHigh: Active level of the chip select pin. HIGH == true
   * bits: Databits for transactions (untested)
   * spiReady: Not sure what this is. Maybe for 3 wire? (untested)
   * threeWire: Whether to use 3-wire SPI. (untested)
   */
  SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode,
              bool csHigh = false, uint8_t csPin = PIN_VALUE_DEFAULT_CS,
              uint8_t bits = 8, bool spiReady = false, bool threeWire = false) {
    // Normally, this function will go through clock value setups for internal
    // registers.
    clockHzSet = clock;
    bitOrderSet = bitOrder;
    dataModeSet = dataMode;
    csPinSet = csPin;
    csHighSet = csHigh;
    bitsSet = bits;
    spiReadySet = spiReady;
    threeWireSet = threeWire;
  }
  SPISettings() {
    clockHzSet = 500000;
    bitOrderSet = MSBFIRST;
    dataModeSet = SPI_MODE0;
    csPinSet = PIN_VALUE_DEFAULT_CS;
    csHighSet = false;
    bitsSet = 8;
    spiReadySet = false;
    threeWireSet = false;
  }
private:
  uint32_t clockHzSet;
  uint8_t bitOrderSet;
  uint8_t dataModeSet;
  uint8_t csPinSet;
  bool csHighSet;
  uint8_t bitsSet;
  bool spiReadySet;
  bool threeWireSet;

  friend class SPIClass;
};

class SPIClass {
public:

  // Initialize the SPI library
  void begin();

  // If SPI is used from within an interrupt, this function registers
  // that interrupt with the SPI library, so beginTransaction() can
  // prevent conflicts.  The input interruptNumber is the number used
  // with attachInterrupt.  If SPI is used from a different interrupt
  // (eg, a timer), interruptNumber should be 255.
  void usingInterrupt(uint8_t interruptNumber);

  // And this does the opposite.
  void notUsingInterrupt(uint8_t interruptNumber);
  // Note: the usingInterrupt and notUsingInterrupt functions should
  // not to be called from ISR context or inside a transaction.
  // For details see:
  // https://github.com/arduino/Arduino/pull/2381
  // https://github.com/arduino/Arduino/pull/2449

  void updateDeviceSettings();

/**
 */

  void beginTransaction(SPISettings settings);

/**
 */

  uint8_t transfer(uint8_t data, bool disable_cs_after_xfer);

  uint16_t transfer16(uint16_t data, bool
                      disable_cs_after_xfer);

/**
 */

  void transfer(void *buf, size_t count, bool
                disable_cs_after_xfer);

/**
 */
  void endTransaction(void);

  // Disable the SPI bus
  void end();

  // This function is deprecated.  New applications should use
  // beginTransaction() to configure SPI settings.
  void setBitOrder(uint8_t bitOrder);

  // This function is deprecated.  New applications should use
  // beginTransaction() to configure SPI settings.
  void setDataMode(uint8_t dataMode);

  // This function is deprecated.  New applications should use
  // beginTransaction() to configure SPI settings.
  void setClockDivider(uint8_t clockDiv);

  void attachInterrupt();

  /**
   *
   */

  void detachInterrupt();

private:
  // For error printing in red
  static char RED[10];
  static char NO_COLOR[7];

  static uint8_t mode;
  static uint8_t bits;
  static uint32_t speed;
  static bool lsbFirst;
  static uint8_t csPin; // WiringPi CS pin
  static bool csHigh;
  static uint8_t spiDeviceFile;
// const char *device;

  static uint8_t initialized;

// Interrupts not used for port
  static uint8_t interruptMode; // 0=none, 1=mask, 2=global
  static uint8_t interruptMask; // which interrupts to mask
  static uint8_t interruptSave; // temp storage, to restore state

// Returns the inactive (disabled) level for current SPI conenction.
  static uint8_t csInactiveLevel();

// Returns the active (enabled) level for current SPI conenction.
  static uint8_t csActiveLevel();
};

extern SPIClass SPI;

#endif /* ifndef _SPI_H_INCLUDED */
