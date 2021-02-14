/**
 * @Author: Nick Steele
 * @Date:   19:49 Aug 10 2020
 * @Last modified by:   Nick Steele
 * @Last modified time: 22:02 Feb 13 2021
 */

// Modified by Nicholas Steele to help port Arduino libraries to Raspberry Pi
// Files modifications for port are marked with a # in comments
// Original files gotten from arduino AVR core github 7/5/2020.

#include "arduino_port_lib/Ardu_SPI.h"
#include <wiringPi.h> // For CS handling

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

void SPIClass::updateDeviceSettings(){
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

void SPIClass::beginTransaction(SPISettings settings) {
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
  #ifdef SPI_VERBOSE
  printf("Device File: %d\n", spiDeviceFile);
  #endif /* ifdef SPI_DEBUG */
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

uint8_t SPIClass::transfer(uint8_t data, bool disable_cs_after_xfer) {
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
  if (csPin != PIN_VALUE_DEFAULT_CS && disable_cs_after_xfer)
    digitalWrite(csPin, csInactiveLevel()); // Disable CS
  if (ret < 1) {
    printf(
      "%s%s in %s:%d ioctl could not perform a data transfer on device \"%s\" (fd=%d). Error: %s%s",
      RED, __PRETTY_FUNCTION__, __FILE__, __LINE__, DEFAULT_SPI_DEV_FILE,
      spiDeviceFile, strerror(errno), NO_COLOR);
  }

  // printf("[byte=%d]", spiDeviceFile);
  return rx;
} /* transfer */

uint16_t SPIClass::transfer16(uint16_t data, bool
                              disable_cs_after_xfer) {
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
  if (csPin != PIN_VALUE_DEFAULT_CS && disable_cs_after_xfer)
    digitalWrite(csPin, csInactiveLevel()); // Disable CS
  if (ret < 1) {
    printf(
      "%s%s in %s:%d ioctl could not perform a data transfer on device \"%s\" (fd=%d). Error: %s%s",
      RED, __PRETTY_FUNCTION__, __FILE__, __LINE__, DEFAULT_SPI_DEV_FILE,
      spiDeviceFile, strerror(errno), NO_COLOR);
  }
  return rx;
} /* transfer16 */

/**
 */

void SPIClass::transfer(void *buf, size_t count, bool
                        disable_cs_after_xfer) {
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
  if (csPin != PIN_VALUE_DEFAULT_CS && disable_cs_after_xfer)
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
void SPIClass::endTransaction(void) {
  if (csPin != PIN_VALUE_DEFAULT_CS)
    digitalWrite(csPin, csInactiveLevel()); // Disable CS
  close(spiDeviceFile);
  #ifdef SPI_VERBOSE
  printf("Closing SPI\n");
  #endif /* ifdef SPI_VERBOSE */
} /* endTransaction */

// This function is deprecated.  New applications should use
// beginTransaction() to configure SPI settings.
void SPIClass::setBitOrder(uint8_t bitOrder) {
  if (bitOrder == LSBFIRST)
    mode |= SPI_LSB_FIRST;
  else mode &= ~SPI_LSB_FIRST; // TODO: verify this
  updateDeviceSettings();
} /* setBitOrder */

// This function is deprecated.  New applications should use
// beginTransaction() to configure SPI settings.
void SPIClass::setDataMode(uint8_t dataMode) {
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
void SPIClass::setClockDivider(uint8_t clockDiv) {
  printf(
    "%s%s in %s:%d Set clock div called, but this is not implemented for the arduino/RPi port. No changes were made.%s",
    RED, __PRETTY_FUNCTION__, __FILE__, __LINE__, NO_COLOR);
} /* setClockDivider */

void SPIClass::attachInterrupt() {
  printf("%s%s in %s:%d This function is not implmented for the RPi port.%s",
         RED, __PRETTY_FUNCTION__, __FILE__, __LINE__, NO_COLOR);
} /* attachInterrupt */

/**
 *
 */

void SPIClass::detachInterrupt() {
  printf("%s%s in %s:%d This function is not implmented for the RPi port.%s",
         RED, __PRETTY_FUNCTION__, __FILE__, __LINE__, NO_COLOR);
} /* detachInterrupt */

// Returns the inactive (disabled) level for current SPI conenction.
uint8_t SPIClass::csInactiveLevel(){
  return (csHigh ? LOW : HIGH);
} /* csInactiveLevel */

// Returns the active (enabled) level for current SPI conenction.
uint8_t SPIClass::csActiveLevel(){
  return (csHigh ? HIGH : LOW);
} /* csActiveLevel */
