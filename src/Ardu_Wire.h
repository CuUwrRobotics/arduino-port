// Modified by Nicholas Steele to help port Arduino libraries to Raspberry Pi
// Files modifications for port are marked with a # in comments

/*
 * TwoWire.h - TWI/I2C library for Arduino & Wiring
 * Copyright (c) 2006 Nicholas Zambetti.  All right reserved.
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * Modified 2012 by Todd Krein (todd@krein.org) to implement repeated starts
 * Modified 2020 by Greyson Christoforo (grey@christoforo.net) to implement timeouts
 */

#ifndef TWO_WIRE_H
#define TWO_WIRE_H

// TODO: FIX THESE!
#include </usr/include/linux/types.h>
// #include </usr/include/linux/stat.h>
#include </usr/include/string.h>
#include </usr/include/stdlib.h>
#include </usr/include/errno.h>
#include </usr/include/fcntl.h>
#include </usr/include/unistd.h>
// #include </usr/include/linux/ioctl.h>
#include <sys/ioctl.h>
#include </usr/include/linux/i2c.h>
#include </usr/include/linux/i2c-dev.h>

#include <stdint.h>
#include <ros/ros.h>
// #include </usr/include/stdio.h>
#include <stdio.h>
#include <stddef.h>

#define RASPI_I2C_MAX_BUFFER 8192 // TODO: check this number.
#define I2C_DEVICE_FILE "/dev/i2c-1" // The I2C device node file
// # From deprecated WiringPi
#define I2C_SMBUS_BLOCK_MAX	32 /* As specified in SMBus standard */

#define BUFFER_LENGTH 32 // TODO: possibly set to RASPI_I2C_MAX_BUFFER

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
static int i2cDeviceFile;

class TwoWire {
private:

	static uint8_t rxBuffer[];
	static uint8_t rxBufferIndex;
	// static uint8_t rxBufferLength; // # Removed for port

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

	virtual int readBlock(); // # Added for RPI port

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
