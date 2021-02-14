/**
 * @Author: Nick Steele
 * @Date:   19:49 Aug 10 2020
 * @Last modified by:   Nick Steele
 * @Last modified time: 19:20 Feb 13 2021
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

#include <wiringPi.h> // For CS handling

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
  static void begin();

  // If SPI is used from within an interrupt, this function registers
  // that interrupt with the SPI library, so beginTransaction() can
  // prevent conflicts.  The input interruptNumber is the number used
  // with attachInterrupt.  If SPI is used from a different interrupt
  // (eg, a timer), interruptNumber should be 255.
  static void usingInterrupt(uint8_t interruptNumber);

  // And this does the opposite.
  static void notUsingInterrupt(uint8_t interruptNumber);
  // Note: the usingInterrupt and notUsingInterrupt functions should
  // not to be called from ISR context or inside a transaction.
  // For details see:
  // https://github.com/arduino/Arduino/pull/2381
  // https://github.com/arduino/Arduino/pull/2449

  inline static void updateDeviceSettings(){
    int retW;
    int retR;
    retW = ioctl(spiDeviceFile, SPI_IOC_WR_MODE, &mode);
    retR = ioctl(spiDeviceFile, SPI_IOC_RD_MODE, &mode);
    if (retR != 0 || retW != 0) {
      printf(
        "%s%s in %s:%d ioctl could not set modes for %s on device \"%s\" (fd=%d). Error: %s%s",
        RED, __PRETTY_FUNCTION__, __FILE__, __LINE__,
        ((retR != 0) ? "read" : ((retW != 0) ? "read/write" : "write")),
        DEFAULT_SPI_DEV_FILE, spiDeviceFile, strerror(errno), NO_COLOR);
    }

    retW = ioctl(spiDeviceFile, SPI_IOC_WR_BITS_PER_WORD, &bits);
    retR = ioctl(spiDeviceFile, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (retR != 0 || retW != 0) {
      printf(
        "%s%s in %s:%d ioctl could not set databits for %s on device \"%s\" (fd=%d). Error: %s%s",
        RED, __PRETTY_FUNCTION__, __FILE__, __LINE__,
        ((retR != 0) ? "read" : ((retW != 0) ? "read/write" : "write")),
        DEFAULT_SPI_DEV_FILE, spiDeviceFile, strerror(errno), NO_COLOR);
    }

    retW = ioctl(spiDeviceFile, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    retR = ioctl(spiDeviceFile, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (retR != 0 || retW != 0) {
      printf(
        "%s%s in %s:%d ioctl could not set databits for %s on device \"%s\" (fd=%d). Error: %s%s",
        RED, __PRETTY_FUNCTION__, __FILE__, __LINE__,
        ((retR != 0) ? "read" : ((retW != 0) ? "read/write" : "write")),
        DEFAULT_SPI_DEV_FILE, spiDeviceFile, strerror(errno), NO_COLOR);
    }

    #ifdef SPI_VERBOSE
    // mode = 0; // TODO
    printf("SPI Settings: \n");
    printf("\tMode: 0x%2x\n", mode);
    printf("\tBits: %d\n", bits);
    printf("\tSpeed: %d Hz (%d kHz)\n", speed, speed / 1000);
    #endif /* ifdef SPI_VERBOSE */
  } /* updateDeviceSettings */

/**
 */

  inline static void beginTransaction(SPISettings settings) {
    // Configures SPI setups and modes
    // Reset mode first
    mode = 0;

    // Never uses loopback
    // mode &= ~SPI_LOOP; // Loopback

    // Clock frequency
    speed = settings.clockHzSet; // Clock speed in Hz

    // Clock edge settings
    switch (settings.dataModeSet) {
    case SPI_MODE0: // CPOL = 0, CPHA = 0
      mode &= ~SPI_CPOL;
      mode &= ~SPI_CPHA;
      break;
    case SPI_MODE1: // CPOL = 0, CPHA = 1
      mode &= ~SPI_CPOL;
      mode |= SPI_CPHA;
      break;
    case SPI_MODE2: // CPOL = 1, CPHA = 0
      mode |= SPI_CPOL;
      mode &= ~SPI_CPHA;
      break;
    case SPI_MODE3: // CPOL = 1, CPHA = 1
      mode |= SPI_CPOL;
      mode |= SPI_CPHA;
      break;
    } /* switch */

    // Bit order. Setting LSB first on linux SPI interface breaks it, so this is
    // done in software.
    if (settings.bitOrderSet == LSBFIRST)
      lsbFirst = true;
    else lsbFirst = false;

    // Set up whether the CS pin is active high
    csHigh = settings.csHighSet;
    if (settings.csHighSet)
      mode |= SPI_CS_HIGH; // Chip Enable = HIGH
    else mode &= ~SPI_CS_HIGH; // Chip Enable = LOW

    // CS Pin number setup
    csPin = settings.csPinSet; // Save the pin to use
    if (csPin != PIN_VALUE_DEFAULT_CS) { // Don't use default CS pin
      if (csPin > W_PI_HIGHEST_PIN) {
        // Invalid pin (TODO: add other bad pins)
        printf(
          "%s%s in %s:%d Got invalid CS pin: %d. Using built in default SPI CS.%s",
          RED, __PRETTY_FUNCTION__, __FILE__, __LINE__, csPin, NO_COLOR);
        // mode &= ~SPI_NO_CS;
        // csPin is used to check if WiringPi is needed later, so it must be set!
        csPin = PIN_VALUE_DEFAULT_CS;
      } else {
        // Get wiringpi going.
        // csPin = settings.csPinSet; // Save the pin to use
        mode |= SPI_NO_CS; // Detaches normal chip select pin
        wiringPiSetup();
        pinMode(csPin, OUTPUT);
        digitalWrite(csPin, csInactiveLevel()); // Disable CS
        // printf("\nusing wpi %d\n", csPin);
      }
    }	else mode &= ~SPI_NO_CS; // Use default pin

    // Databits
    bits = settings.bitsSet; // untested

    // Don't know what this does, but it must be important sometimes
    if (settings.spiReadySet)
      mode |= SPI_READY; // untested
    else mode &= ~SPI_READY;

    // 3 Wire mode
    if (settings.threeWireSet)
      mode |= SPI_3WIRE; // untested
    else mode &= ~SPI_3WIRE;

    // Finally, open the 'file' that represents the device
    spiDeviceFile = open(DEFAULT_SPI_DEV_FILE, O_RDWR);
    if (spiDeviceFile < 0) {
      printf(
        "%s%s in %s:%d could not get file descriptor (recieved fd %d) for device \"%s\". Error: %s%s",
        RED, __PRETTY_FUNCTION__, __FILE__, __LINE__, spiDeviceFile,
        DEFAULT_SPI_DEV_FILE, strerror(errno), NO_COLOR);
      return;
    }

    updateDeviceSettings();
  } /* beginTransaction */

/**
 */

  inline static uint8_t transfer(uint8_t data) {
    // Write to the SPI bus (MOSI pin) and also receive (MISO pin)
    int ret;
    uint8_t tx; // Byte to store data. If data needs inverting, this is used.
    uint8_t rx; // Single byte var to store recieved data

    if (lsbFirst) tx = reversedBitTable[data];
    else tx = data;

    // Set up data to send, lengths, and general settings
    struct spi_ioc_transfer iocSettings = {
      // .tx_buf = (unsigned long)&data,
      .tx_buf = (unsigned long)&tx,
      .rx_buf = (unsigned long)&rx,
      .len = 1,
      .speed_hz = speed, // TODO
      .delay_usecs = DEFAULT_SPI_DELAY,
      .bits_per_word = bits,
    };

#ifdef SPI_VERBOSE
    if (iocSettings.speed_hz != speed)
      printf("SPI requested clock of %d, but the value will be set to %d.",
             speed, iocSettings.speed_hz);

    printf("\n\tlen: %d\n", iocSettings.len);
    printf("\tspeed: %d\n", iocSettings.speed_hz);
    printf("\tdelay: %d\n", iocSettings.delay_usecs);
    printf("\tbits: %d\n", iocSettings.bits_per_word);
#endif /* ifdef SPI_VERBOSE */

    if (csPin != PIN_VALUE_DEFAULT_CS)
      digitalWrite(csPin, csActiveLevel()); // Enable CS
    // Transfer data
    ret = ioctl(spiDeviceFile, SPI_IOC_MESSAGE(1), &iocSettings);
    if (csPin != PIN_VALUE_DEFAULT_CS)
      digitalWrite(csPin, csInactiveLevel()); // Disable CS
    if (ret < 1) {
      printf(
        "%s%s in %s:%d ioctl could not perform a data transfer on device \"%s\" (fd=%d). Error: %s%s",
        RED, __PRETTY_FUNCTION__, __FILE__, __LINE__, DEFAULT_SPI_DEV_FILE,
        spiDeviceFile, strerror(errno), NO_COLOR);
    }

    // printf("[byte=%d]", spiDeviceFile);
    return ret;
  } /* transfer */

  inline static uint16_t transfer16(uint16_t data) {
    // Write to the SPI bus (MOSI pin) and also receive (MISO pin)
    int ret;
    uint8_t tx[2] = {0};
    uint16_t rx; // Two byte var to store recieved data

    if (lsbFirst) {
      // Reversal accounts for full 16-bit integer, so reversed binary values are
      // computed and bytes are swapped.
      tx[1] = (reversedBitTable[(data & 0xFF00) >> 8]);
      tx[0] = (reversedBitTable[data & 0x00FF]);
    } else {
      // No reversal, just assign values
      tx[0] = ((data & 0xFF00) >> 8);
      tx[1] = (data & 0x00FF);
    }

    // Set up data to send, lengths, and general settings
    struct spi_ioc_transfer iocSettings = {
      .tx_buf = (unsigned long)&tx,
      .rx_buf = (unsigned long)&rx,
      .len = 2,
      .speed_hz = speed, // TODO
      .delay_usecs = DEFAULT_SPI_DELAY,
      .bits_per_word = bits,
    };

#ifdef SPI_VERBOSE
    if (iocSettings.speed_hz != speed)
      printf("SPI requested clock of %d, but the value will be set to %d.",
             speed, iocSettings.speed_hz);

    printf("\n\tlen: %d\n", iocSettings.len);
    printf("\tspeed: %d\n", iocSettings.speed_hz);
    printf("\tdelay: %d\n", iocSettings.delay_usecs);
    printf("\tbits: %d\n", iocSettings.bits_per_word);
#endif /* ifdef SPI_VERBOSE */

    if (csPin != PIN_VALUE_DEFAULT_CS)
      digitalWrite(csPin, csActiveLevel()); // Enable CS
    // Transfer data
    ret = ioctl(spiDeviceFile, SPI_IOC_MESSAGE(1), &iocSettings);
    if (csPin != PIN_VALUE_DEFAULT_CS)
      digitalWrite(csPin, csInactiveLevel()); // Disable CS
    if (ret < 1) {
      printf(
        "%s%s in %s:%d ioctl could not perform a data transfer on device \"%s\" (fd=%d). Error: %s%s",
        RED, __PRETTY_FUNCTION__, __FILE__, __LINE__, DEFAULT_SPI_DEV_FILE,
        spiDeviceFile, strerror(errno), NO_COLOR);
    }
    return ret;
  } /* transfer16 */

/**
 */

  inline static void transfer(void *buf, size_t count) {
    // Write to the SPI bus (MOSI pin) and also receive (MISO pin)
    int ret;
    uint8_t *b = (uint8_t *) buf;
    // Buffers for data storage (can't pass &buf or &b to ioctl, it will mangle the data)
    uint8_t tx[count] = {0};
    uint8_t rx[count] = {0};

    if (lsbFirst) {
      // printf("variable bit, reversal: \n");
      for (size_t i = 0; i < count; i++) {
        tx[i] = reversedBitTable[b[i]];
        // printf("\t0x%2x --> ", b[i]);
        // printf("0x%2x, \n", tx[i]);
      }
    } else {
      // printf("variable bit, NO reversal: \n");
      for (size_t i = 0; i < count; i++) {
        tx[i] = b[i];
        // printf("\t0x%2x\n", tx[i]);
      }
    }

    // Set up data to send, lengths, and general settings
    struct spi_ioc_transfer iocSettings = {
      .tx_buf = (unsigned long)&tx,
      .rx_buf = (unsigned long)&rx,
      .len = count,
      .speed_hz = speed, // TODO
      .delay_usecs = DEFAULT_SPI_DELAY,
      .bits_per_word = bits,
    };

#ifdef SPI_VERBOSE
    if (iocSettings.speed_hz != speed)
      printf("SPI requested clock of %d, but the value will be set to %d.",
             speed, iocSettings.speed_hz);
    printf("\n\tlen: %d\n", iocSettings.len);
    printf("\tspeed: %d\n", iocSettings.speed_hz);
    printf("\tdelay: %d\n", iocSettings.delay_usecs);
    printf("\tbits: %d\n", iocSettings.bits_per_word);
#endif /* ifdef SPI_VERBOSE */

    if (csPin != PIN_VALUE_DEFAULT_CS)
      digitalWrite(csPin, csActiveLevel()); // Enable CS
    // Transfer data
    ret = ioctl(spiDeviceFile, SPI_IOC_MESSAGE(1), &iocSettings);
    if (csPin != PIN_VALUE_DEFAULT_CS)
      digitalWrite(csPin, csInactiveLevel()); // Disable CS
    if (ret < 1) {
      printf(
        "%s%s in %s:%d ioctl could not perform a data transfer on device \"%s\" (fd=%d). Errno: %s%s",
        RED, __PRETTY_FUNCTION__, __FILE__, __LINE__, DEFAULT_SPI_DEV_FILE,
        spiDeviceFile, strerror(errno), NO_COLOR);
    }
    for (size_t i = 0; i < count; i++)
      b[i] = rx[i];
    return;
  } /* transfer */

/**
 */
  inline static void endTransaction(void) {
    close(spiDeviceFile);
    #ifdef SPI_VERBOSE
    printf("Closing SPI\n");
    #endif /* ifdef SPI_VERBOSE */
  } /* endTransaction */

  // Disable the SPI bus
  static void end();

  // This function is deprecated.  New applications should use
  // beginTransaction() to configure SPI settings.
  inline static void setBitOrder(uint8_t bitOrder) {
    if (bitOrder == LSBFIRST)
      mode |= SPI_LSB_FIRST;
    else mode &= ~SPI_LSB_FIRST; // TODO: verify this
    updateDeviceSettings();
  } /* setBitOrder */

  // This function is deprecated.  New applications should use
  // beginTransaction() to configure SPI settings.
  inline static void setDataMode(uint8_t dataMode) {
    switch (dataMode) {
    case SPI_MODE0: // CPOL = 0, CPHA = 0
      mode &= ~SPI_CPOL;
      mode &= ~SPI_CPHA;
      break;
    case SPI_MODE1: // CPOL = 0, CPHA = 1
      mode &= ~SPI_CPOL;
      mode |= SPI_CPHA;
      break;
    case SPI_MODE2: // CPOL = 1, CPHA = 0
      mode |= SPI_CPOL;
      mode &= ~SPI_CPHA;
      break;
    case SPI_MODE3: // CPOL = 1, CPHA = 1
      mode |= SPI_CPOL;
      mode |= SPI_CPHA;
      break;
    } /* switch */
    updateDeviceSettings();
  } /* setDataMode */

  // This function is deprecated.  New applications should use
  // beginTransaction() to configure SPI settings.
  inline static void setClockDivider(uint8_t clockDiv) {
    printf(
      "%s%s in %s:%d Set clock div called, but this is not implemented for the arduino/RPi port. No changes were made.%s",
      RED, __PRETTY_FUNCTION__, __FILE__, __LINE__, NO_COLOR);
  } /* setClockDivider */

  inline static void attachInterrupt() {
    printf("%s%s in %s:%d This function is not implmented for the RPi port.%s",
           RED, __PRETTY_FUNCTION__, __FILE__, __LINE__, NO_COLOR);
  } /* attachInterrupt */

  /**
   *
   */

  inline static void detachInterrupt() {
    printf("%s%s in %s:%d This function is not implmented for the RPi port.%s",
           RED, __PRETTY_FUNCTION__, __FILE__, __LINE__, NO_COLOR);
  } /* detachInterrupt */

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
  // static const char *device;

  static uint8_t initialized;

  // Interrupts not used for port
  static uint8_t interruptMode; // 0=none, 1=mask, 2=global
  static uint8_t interruptMask; // which interrupts to mask
  static uint8_t interruptSave; // temp storage, to restore state

  // Returns the inactive (disabled) level for current SPI conenction.
  inline static uint8_t csInactiveLevel(){
    return (csHigh ? LOW : HIGH);
  } /* csInactiveLevel */

  // Returns the active (enabled) level for current SPI conenction.
  inline static uint8_t csActiveLevel(){
    return (csHigh ? HIGH : LOW);
  } /* csActiveLevel */
};

extern SPIClass SPI;

#endif /* ifndef _SPI_H_INCLUDED */
