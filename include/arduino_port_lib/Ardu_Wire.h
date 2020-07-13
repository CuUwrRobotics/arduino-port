// Modified by Nicholas Steele to help port Arduino libraries to Raspberry Pi
// Files modifications for port are marked with a # in comments
// Original files gotten from arduino AVR core github 7/5/2020.

#ifndef TWO_WIRE_H
#define TWO_WIRE_H

#include <errno.h> // errno stuff
#include <fcntl.h> // open(), close()
#include <unistd.h> // read(), write()
#include <sys/ioctl.h> // ioctl()
#include <stdint.h> // Explicit bitwidth integers like uint8_t
#include <stdio.h> // printf()
#include <errno.h> // errno()
// #include <stderr.h>
#include <string.h> // strerr(), memset()

#define I2C_DEVICE_FILE "/dev/i2c-1" // The I2C device node file
// # From deprecated WiringPi
#define I2C_SMBUS_BLOCK_MAX	32 /* As specified in SMBus standard */

// TODO: rename this macro to be less generic
#define BUFFER_LENGTH 32 // Limited by I2C_SMBUS_BLOCK_MAX

// SMBus transaction sizes for use with i2c_smbus_data
#define I2C_SMBUS_QUICK       0
#define I2C_SMBUS_BYTE        1
#define I2C_SMBUS_BYTE_DATA     2
#define I2C_SMBUS_WORD_DATA     3
#define I2C_SMBUS_PROC_CALL     4
#define I2C_SMBUS_BLOCK_DATA      5
#define I2C_SMBUS_I2C_BLOCK_BROKEN  6
#define I2C_SMBUS_BLOCK_PROC_CALL   7 /* SMBus 2.0 */
#define I2C_SMBUS_I2C_BLOCK_DATA    8
// SMBUS access types
#define I2C_SLAVE	0x0703
#define I2C_SMBUS	0x0720 /* SMBus-level access */
#define I2C_SMBUS_READ	1
#define I2C_SMBUS_WRITE	0

// WIRE_HAS_END means Wire has end()
#define WIRE_HAS_END 1

// # Holds file descriptor for device node

class TwoWire {
private:

	// For error printing in red
	static char RED[10];
	static char NO_COLOR[7];

	static int i2cDeviceFile;

	static uint8_t rxBufferLength;
	static uint8_t rxBuffer[];
	static uint8_t rxBufferIndex;
	// static uint8_t rxBufferLength;

	static uint8_t txAddress;
	static uint8_t txBuffer[];
	static uint8_t txBufferIndex;
	// static uint8_t txBufferLength; // # Removed for port

	static uint8_t transmitting;
	static void (*user_onRequest)(void);
	static void (*user_onReceive)(int);

	static void onRequestService(void);

	static void onReceiveService(uint8_t *, int);
public:
	TwoWire();

	void begin();

	void begin(uint8_t);

	void begin(int);

	void end();

	void setClock(uint32_t);

	void setWireTimeout(uint32_t timeout = 25000, bool reset_with_timeout =
												false);

	bool getWireTimeoutFlag(void);

	void clearWireTimeoutFlag(void);

	void beginTransmission(uint8_t);

	void beginTransmission(int);

	uint8_t endTransmission(void);

	uint8_t endTransmission(uint8_t);

	uint8_t requestFrom(uint8_t, uint8_t);

	uint8_t requestFrom(uint8_t, uint8_t, uint8_t);

	uint8_t requestFrom(uint8_t, uint8_t, uint32_t, uint8_t, uint8_t);

	uint8_t requestFrom(int, int);

	uint8_t requestFrom(int, int, int);

	virtual size_t write(uint8_t);

	virtual size_t write(const uint8_t *, size_t);

	virtual int readBlock(uint8_t addr, int length); // # Added for RPI port

	virtual int available(void);

	virtual int read(void);

	virtual int peek(void);

	virtual void flush(void);

	void onReceive( void (*)(int));

	void onRequest( void (*)(void));

	/**
	 * @param n TODO
	 * @return TODO
	 */

	inline size_t write(unsigned long n) {
		return write((uint8_t)n);
	} /* write */

	/**
	 * @param n TODO
	 * @return TODO
	 */

	inline size_t write(long n) {
		return write((uint8_t)n);
	} /* write */

	/**
	 * @param n TODO
	 * @return TODO
	 */

	inline size_t write(unsigned int n) {
		return write((uint8_t)n);
	} /* write */

	/**
	 * @param n TODO
	 * @return TODO
	 */

	inline size_t write(int n) {
		return write((uint8_t)n);
	} /* write */
	  // using Print::write;
};

extern TwoWire Wire;

// #include "Ardu_Wire.cpp"

#endif /* ifndef TwoWire_h */
