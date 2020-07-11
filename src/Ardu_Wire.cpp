// Modified by Nicholas Steele to help port Arduino libraries to Raspberry Pi
// Files modifications for port are marked with a # in comments

/*
 * TwoWire.cpp - TWI/I2C library for Wiring & Arduino
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
 *
 * Modified 2012 by Todd Krein (todd@krein.org) to implement repeated starts
 * Modified 2017 by Chuck Todd (ctodd@cableone.net) to correct Unconfigured Slave Mode reboot
 * Modified 2020 by Greyson Christoforo (grey@christoforo.net) to implement timeouts
 */

// #include "ArduinoPort.h"
#include "Ardu_Wire.h"

// Initialize Class Variables //////////////////////////////////////////////////

// int i2cDeviceFile = 0;

uint8_t TwoWire::rxBuffer[BUFFER_LENGTH];
uint8_t TwoWire::rxBufferIndex = 0;
// uint8_t TwoWire::rxBufferLength = 0;

uint8_t TwoWire::txAddress = 0;
uint8_t TwoWire::txBuffer[BUFFER_LENGTH];
uint8_t TwoWire::txBufferIndex = 0;
// uint8_t TwoWire::txBufferLength = 0;

uint8_t TwoWire::transmitting = 0;
void (*TwoWire::user_onRequest)(void);
void (*TwoWire::user_onReceive)(int);

// Constructors ////////////////////////////////////////////////////////////////
// # No change
TwoWire::TwoWire()
{
}

// Public Methods //////////////////////////////////////////////////////////////

// TODO: Set bitrate (happened in call to twi_init())
void TwoWire::begin(void)
{
	// Open device file.
	i2cDeviceFile = open(I2C_DEVICE_FILE, O_RDWR);
	printf("FD: %d\n", i2cDeviceFile);
	if (i2cDeviceFile < 0) {
		ROS_ERROR(
			"%s in %s:%d could not get file descriptor (recieved fd %d) for device \"%s\". Errno: %s",
			__PRETTY_FUNCTION__, __FILE__, __LINE__, i2cDeviceFile,
			I2C_DEVICE_FILE,
			std::strerror(errno));
	}
	rxBufferIndex = 0;
	// rxBufferLength = 0;

	txBufferIndex = 0;
	// txBufferLength = 0;
} // TwoWire::begin

/**
 # Removed native call to set address
 * Call begin(void)
 * Set address
 */
void TwoWire::begin(uint8_t address)
{
	begin();
	txAddress = address;
} // TwoWire::begin

/**
 # No change
 * Repeats begin(uint8_t)
 */

void TwoWire::begin(int address)
{
	begin((uint8_t)address);
} // TwoWire::begin

/**
 * Disable native interface
 */

void TwoWire::end(void)
{
	close(i2cDeviceFile); // Closes the linux device node.
} // TwoWire::end

/**
 * TODO
 * Set clock frequency
 */

void TwoWire::setClock(uint32_t clock)
{
	// twi_setFrequency(clock);
} // TwoWire::setClock

/**
 * TODO
 * @param timeout a timeout value in microseconds, if zero then timeout checking is disabled
 * @param reset_with_timeout if true then TWI interface will be automatically reset on timeout
 *                           if false then TWI interface will not be reset on timeout
 */
void TwoWire::setWireTimeout(uint32_t timeout, bool reset_with_timeout){
	// twi_setTimeoutInMicros(timeout, reset_with_timeout);
} // TwoWire::setWireTimeout

/**
 * TODO
 * Returns whether timout has been triggered.
 *
 * @return true if timeout has occured since the flag was last cleared.
 */
bool TwoWire::getWireTimeoutFlag(void){
	// return(twi_manageTimeoutFlag(false));
} // TwoWire::getWireTimeoutFlag

/**
 * TODO
 * Clears the timeout flag.
 */
void TwoWire::clearWireTimeoutFlag(void){
	// twi_manageTimeoutFlag(true);
} // TwoWire::clearWireTimeoutFlag

/**
 * Handle data request to given register.
 * Must:
 * -> beginTransmission
 * -> Check that isize <= 3
 * -> Send all 4 iaddress pieces as bytes
 * -> endTransmission
 * -> Read up to buffer length (uses twi_readFrom)
 * -> Reset index = 0, and length = total read data.
 * -> Return ammount of read data
 */

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity, uint32_t
                             iaddress, uint8_t isize, uint8_t sendStop)
{
	if (isize > 0) {
		// send internal address; this mode allows sending a repeated start to access
		// some devices' internal registers. This function is executed by the hardware
		// TWI module on other processors (for example Due's TWI_IADR and TWI_MMR registers)

		beginTransmission(address);

		// the maximum size of internal address is 3 bytes
		if (isize > 3) {
			isize = 3;
		}

		// write internal register address - most significant byte first
		while (isize-- > 0)
			write((uint8_t)(iaddress >> (isize * 8)));
		endTransmission(false);
	}

	// clamp to buffer length
	if (quantity > BUFFER_LENGTH) {
		quantity = BUFFER_LENGTH;
	}
	// perform blocking read into buffer
	// uint8_t read = twi_readFrom(address, rxBuffer, quantity, sendStop);
	// TODO: blocking
	uint8_t read = TwoWire::read();

	// set rx buffer iterator vars
	// # Buffer index replaces buffer length since both vars are not needed for RasPi
	rxBufferIndex = read;

	return read;
} // TwoWire::requestFrom

/**
 # No change
 */

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity, uint8_t
                             sendStop) {
	return requestFrom((uint8_t)address, (uint8_t)quantity, (uint32_t)0,
	                   (uint8_t)0, (uint8_t)sendStop);
} // TwoWire::requestFrom

/**
 # No change
 */

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity)
{
	return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)true);
} // TwoWire::requestFrom

/**
 # No change
 */

uint8_t TwoWire::requestFrom(int address, int quantity)
{
	return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)true);
} // TwoWire::requestFrom

/**
 # No change
 */

uint8_t TwoWire::requestFrom(int address, int quantity, int sendStop)
{
	return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)sendStop);
} // TwoWire::requestFrom

/**
 * -> Assign address for transmission to txAddress
 * -> Raise transmitting flag
 * -> Reset tx buffer index and length = 0
 *
 # Buffer length not needed
 *
 * @param address TODO
 * @return TODO
 */

void TwoWire::beginTransmission(uint8_t address)
{
	// indicate that we are transmitting
	transmitting = 1;
	// set address of targeted slave
	txAddress = address;
	// reset tx buffer iterator vars
	txBufferIndex = 0;
	// txBufferLength = 0;
} // TwoWire::beginTransmission

/**
 # No change
 * @param address TODO
 * @return TODO
 */

void TwoWire::beginTransmission(int address)
{
	beginTransmission((uint8_t)address);
} // TwoWire::beginTransmission

/**
 * Opens, sets up, sends data on, then closes the I2C device.
 * @return: 0 .. success
 *          1 .. length to long for buffer
 *          2 .. address send, NACK received (TODO)
 *          3 .. data send, NACK received (TODO)
 *          4 .. other twi error (lost bus arbitration, bus error, ..)
 *          5 .. timeout (TODO)
 * TODO: make blocking
 */

uint8_t TwoWire::endTransmission(uint8_t sendStop)
{
	// transmit buffer (blocking)
	static int s; // For storing write data size
	transmitting = 0; // This is set to zero regardless of success
	// We now have access to the device file, time to set it up.
	s = ioctl(i2cDeviceFile, I2C_SLAVE, txAddress); // TODO: fix I2C_SLAVE if needed
	if (s < 0) {
		ROS_ERROR(
			"%s in %s:%d could not use ioctl on fd %d (for device \"%s\"). Errno: %s",
			__PRETTY_FUNCTION__, __FILE__, __LINE__, i2cDeviceFile,
			I2C_DEVICE_FILE,
			std::strerror(errno));
		return 4; // Generic error
	}
	if (txBufferIndex == 0) {
		txBufferIndex = 0; // Reset and return success
		return 0; // Success
	}	else if (BUFFER_LENGTH < txBufferIndex) {
		txBufferIndex = 0; // Reset buffers
		return 1; // Buffer too long (very unlikely for RPI)
	} else {
		s = ::write(i2cDeviceFile, txBuffer, txBufferIndex);
		if (s != txBufferIndex) {
			ROS_ERROR(
				"%s in %s:%d could not write to device \"%s\" (FD = %d). Errno: %s",
				__PRETTY_FUNCTION__, __FILE__, __LINE__, I2C_DEVICE_FILE,
				i2cDeviceFile, std::strerror(errno));
			txBufferIndex = 0; // Reset buffers
			return 4; // Generic error; TODO make this more specific
		}
	}
	txBufferIndex = 0; // Reset and return success
	return 0; // Success
} // TwoWire::endTransmission

/**
 # No change
 */

uint8_t TwoWire::endTransmission(void)
{
	//	This provides backwards compatibility with the original
	//	definition, and expected behaviour, of endTransmission

	return endTransmission(true);
} // TwoWire::endTransmission

/**
 */

size_t TwoWire::write(uint8_t data)
{
	// must be called in:
	// slave tx event callback
	// or after beginTransmission(address)

	if (transmitting) {
		// in master transmitter mode
		// don't bother if buffer is full
		if (txBufferIndex >= BUFFER_LENGTH) {
			// setWriteError(); // Removed as it is not within the Wire library
			return 0;
		}
		// put byte in tx buffer
		txBuffer[txBufferIndex] = data;
		++txBufferIndex;
	} else {
		// in slave send mode
		// reply to master
		static int s; // For storing write data size
		s = ioctl(i2cDeviceFile, I2C_SLAVE, txAddress); // TODO: fix I2C_SLAVE if needed
		if (s < 0) {
			ROS_ERROR(
				"%s in %s:%d could not use ioctl on fd %d (for device \"%s\"). Errno: %s",
				__PRETTY_FUNCTION__, __FILE__, __LINE__, i2cDeviceFile,
				I2C_DEVICE_FILE,
				std::strerror(errno));
			return 0; // Generic error
		}
		s = ::write(i2cDeviceFile, &data, 1); // Send the one byte
		if (s < 0) {
			ROS_ERROR(
				"%s in %s:%d could not write to device \"%s\" (FD = %d). Errno: %s",
				__PRETTY_FUNCTION__, __FILE__, __LINE__, I2C_DEVICE_FILE,
				i2cDeviceFile, std::strerror(errno));
			txBufferIndex = 0; // Reset buffers
			return 0; // Generic error; TODO make this more specific
		}

		txBufferIndex = 0; // Reset and return success
		return 1; // Success
	}
	return 1;
} // TwoWire::write

/**
 # No change if transmitting.
 * TODO: If tranmitting flag was not high, just transmit the data as a slave response
 */

size_t TwoWire::write(const uint8_t *data, size_t quantity)
{
	// must be called in:
	// slave tx event callback
	// or after beginTransmission(address)

	if (transmitting) {
		// in master transmitter mode
		for (size_t i = 0; i < quantity; ++i) {
			write(data[i]);
		}
	} else { // TODO
		ROS_ERROR(
			"%s in %s:%d Wants to write as a slave device. Not currently supported.",
			__PRETTY_FUNCTION__, __FILE__, __LINE__);
		// in slave send mode
		// reply to master
		// twi_transmit(data, quantity);
	}
	return quantity;
} // TwoWire::write

/**
 # Added for Raspberry Pi implementation. Reads a block of data into buffer from
 # I2C lines. Meant for internal use only.
 * @return Data in buffer left to read.
 */
int TwoWire::readBlock() {
	if (rxBufferIndex >= BUFFER_LENGTH - 1) // Don't bother if there's not enough space
		return rxBufferIndex;
	static int s;
	s = ::read(i2cDeviceFile, rxBuffer, BUFFER_LENGTH - rxBufferIndex);
	if (s < 0) {
		ROS_ERROR(
			"%s in %s:%d could not read device \"%s\" on fd %d. Errno: %s",
			__PRETTY_FUNCTION__, __FILE__, __LINE__, I2C_DEVICE_FILE, i2cDeviceFile,
			std::strerror(errno));
		return -1;
	} else {
		rxBufferIndex += s;
		return rxBufferIndex;
	}
} // readBlock

/**
 */

int TwoWire::available(void)
{
	// # Check for data first. readBlock will store that data in the buffer.
	readBlock();
	if (rxBufferIndex > 0)
		return rxBufferIndex;
} // TwoWire::available

/**
 */

int TwoWire::read(void)
{
	// # Reads data directly, filling the buffer with any data
	readBlock();
	if (rxBufferIndex > 0)
		return rxBuffer[rxBufferIndex--];
	else return -1;
} // TwoWire::read

/**
 */

int TwoWire::peek(void)
{
	// must be called in:
	// slave rx event callback
	// or after requestFrom(address, numBytes)

	// int value = -1;

	// # Check for data first. readBlock will store that data in the buffer.
	readBlock();
	if (rxBufferIndex > 0)
		return rxBuffer[rxBufferIndex];
	else return -1;

	// if (rxBufferIndex < rxBufferLength) {
	// 	value = rxBuffer[rxBufferIndex];
	// }

	// return value;
} // TwoWire::peek

/**
 # No change
 */

void TwoWire::flush(void) {
} // TwoWire::flush

// behind the scenes function that is called when data is received
void TwoWire::onReceiveService(uint8_t *inBytes, int numBytes)
{
	ROS_ERROR(
		"%s in %s:%d This function cannot be used for the RasPi port of this library.",
		__PRETTY_FUNCTION__, __FILE__, __LINE__, i2cDeviceFile,
		I2C_DEVICE_FILE,
		std::strerror(errno));
	// // don't bother if user hasn't registered a callback
	// if (!user_onReceive) {
	// 	return;
	// }
	// // don't bother if rx buffer is in use by a master requestFrom() op
	// // i know this drops data, but it allows for slight stupidity
	// // meaning, they may not have read all the master requestFrom() data yet
	// if (rxBufferIndex < rxBufferLength) {
	// 	return;
	// }
	// // copy twi rx buffer into local read buffer
	// // this enables new reads to happen in parallel
	// for (uint8_t i = 0; i < numBytes; ++i) {
	// 	rxBuffer[i] = inBytes[i];
	// }
	// // set rx iterator vars
	// rxBufferIndex = 0;
	// rxBufferLength = numBytes;
	// // alert user program
	// user_onReceive(numBytes);
} // TwoWire::onReceiveService

// behind the scenes function that is called when data is requested
void TwoWire::onRequestService(void)
{
	ROS_ERROR(
		"%s in %s:%d This function cannot be used for the RasPi port of this library.",
		__PRETTY_FUNCTION__, __FILE__, __LINE__, i2cDeviceFile,
		I2C_DEVICE_FILE,
		std::strerror(errno));
	// // don't bother if user hasn't registered a callback
	// if (!user_onRequest) {
	// 	return;
	// }
	// // reset tx buffer iterator vars
	// // !!! this will kill any pending pre-master sendTo() activity
	// txBufferIndex = 0;
	// txBufferLength = 0;
	// // alert user program
	// user_onRequest();
} // TwoWire::onRequestService

// sets function called on slave write
void TwoWire::onReceive( void (*function)(int))
{
	ROS_ERROR(
		"%s in %s:%d This function cannot be used for the RasPi port of this library.",
		__PRETTY_FUNCTION__, __FILE__, __LINE__, i2cDeviceFile,
		I2C_DEVICE_FILE,
		std::strerror(errno));
	// user_onReceive = function;
} // TwoWire::onReceive

// sets function called on slave read
void TwoWire::onRequest( void (*function)(void))
{
	ROS_ERROR(
		"%s in %s:%d This function cannot be used for the RasPi port of this library.",
		__PRETTY_FUNCTION__, __FILE__, __LINE__, i2cDeviceFile,
		I2C_DEVICE_FILE,
		std::strerror(errno));
	// user_onRequest = function;
} // TwoWire::onRequest

// Preinstantiate Objects //////////////////////////////////////////////////////

TwoWire Wire = TwoWire();
