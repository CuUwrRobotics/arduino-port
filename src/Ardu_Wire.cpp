// Modified by Nicholas Steele to help port Arduino libraries to Raspberry Pi
// Files modifications for port are marked with a # in comments
// Original files gotten from arduino AVR core github 7/5/2020.

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
TwoWire::TwoWire() {
}

// Public Methods //////////////////////////////////////////////////////////////

// # Bitrate is normally set, but that needs to be done through configs on RPI.
void TwoWire::begin(void) {
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
	rxBufferLength = 0;

	txBufferIndex = 0;
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

// # Can't do on RPI since this happens at boot.
void TwoWire::setClock(uint32_t clock)
{
} // TwoWire::setClock

/**
 # Not implemented for RPI port.
 */
void TwoWire::setWireTimeout(uint32_t timeout, bool reset_with_timeout){
} // TwoWire::setWireTimeout

/**
 # Not implemented for RPI port.
 */
bool TwoWire::getWireTimeoutFlag(void){
	return false;
} // TwoWire::getWireTimeoutFlag

/**
 # Not implemented for RPI port.
 */
void TwoWire::clearWireTimeoutFlag(void){
} // TwoWire::clearWireTimeoutFlag

/**
 * Handle data request to given register.
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
	return readBlock(address, quantity);
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
 # No functional changes
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
 */

void TwoWire::beginTransmission(int address)
{
	beginTransmission((uint8_t)address);
} // TwoWire::beginTransmission

/**
 * Opens, sets up, sends data on, then closes the I2C device.
 * @return: 0 .. success
 *          1 .. length to long for buffer
 *          2 .. address send, NACK received (can't do on RPI)
 *          3 .. data send, NACK received (can't do on RPI)
 *          4 .. other twi error (lost bus arbitration, bus error, ..)
 *          5 .. timeout (not doing on RPI)
 */

uint8_t TwoWire::endTransmission(uint8_t sendStop)
{
	// transmit buffer (blocking)
	static int s; // For storing write data size
	transmitting = 0; // This is set to zero regardless of success
	// We have access to the device file, time to connect to a specific address
	s = ioctl(i2cDeviceFile, I2C_SLAVE, txAddress); // Connect to I2C slave on txAddress
	if (s < 0) {
		ROS_ERROR(
			"%s in %s:%d ioctl could not connect to I2C slave. FD %d (for device \"%s\"). Errno: %s",
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
			return 4;
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
		s = ioctl(i2cDeviceFile, I2C_SLAVE, txAddress);
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
			return 0;
		}

		txBufferIndex = 0; // Reset and return success
		return 1; // Success
	}
	return 1;
} // TwoWire::write

/**
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
	} else { // TODO: verify this code works
		// ROS_ERROR(
		// 	"%s in %s:%d Wants to write as a slave device. Not currently supported.",
		// 	__PRETTY_FUNCTION__, __FILE__, __LINE__);
		// in slave send mode
		// reply to master
		static int s; // For storing write data size
		s = ioctl(i2cDeviceFile, I2C_SLAVE, txAddress);
		if (s < 0) {
			ROS_ERROR(
				"%s in %s:%d could not use ioctl on fd %d (for device \"%s\"). Errno: %s",
				__PRETTY_FUNCTION__, __FILE__, __LINE__, i2cDeviceFile,
				I2C_DEVICE_FILE,
				std::strerror(errno));
			return 0; // Generic error
		}
		s = ::write(i2cDeviceFile, data, quantity); // Send the one byte
		if (s < 0) {
			ROS_ERROR(
				"%s in %s:%d could not write to device \"%s\" (FD = %d). Errno: %s",
				__PRETTY_FUNCTION__, __FILE__, __LINE__, I2C_DEVICE_FILE,
				i2cDeviceFile, std::strerror(errno));
			txBufferIndex = 0; // Reset buffers
			return 0;
		}

		txBufferIndex = 0; // Reset and return success
		return 1; // Success
	}
	return quantity;
} // TwoWire::write

/**
 # Added for Raspberry Pi implementation. Reads a block of data into buffer from
 # I2C lines. Meant for internal use only.
 * @return Data in buffer left to read.
 */

int TwoWire::readBlock(uint8_t addr = txAddress, int length = BUFFER_LENGTH) {
	if (length > BUFFER_LENGTH)
		return -1; // Too long
	static int s;
	memset(rxBuffer, 0, BUFFER_LENGTH); // Reset read buffer
	rxBufferLength = 0;
	rxBufferIndex = 0;
	// Start by trying to set address
	// printf("Connecting to I2C device 0x%2x\n", addr);
	s = ioctl(i2cDeviceFile, I2C_SLAVE, addr); // Connect to I2C slave on addr
	// printf("\tResult: %d\n", s);
	if (s < 0) {
		ROS_ERROR(
			"%s in %s:%d ioctl could not connect to I2C slave. FD %d (for device \"%s\"). Errno: %s",
			__PRETTY_FUNCTION__, __FILE__, __LINE__, i2cDeviceFile, I2C_DEVICE_FILE,
			std::strerror(errno));
		return 4; // Generic error
	}
	s = 0; // reuse s
	// Got device and connected to correct address, now read from it.
	s = ::read(i2cDeviceFile, rxBuffer, length);
	printf("Read block running, l=%d, s=%d\n", length, s);
	if (s < 0) {
		ROS_ERROR(
			"%s in %s:%d could not read device \"%s\" on fd %d. Errno: %s",
			__PRETTY_FUNCTION__, __FILE__, __LINE__, I2C_DEVICE_FILE, i2cDeviceFile,
			std::strerror(errno));
		return -1;
	}
	rxBufferLength = s;
	printf("readBlock dump: ");
	for (int i = 0; i < rxBufferLength; i++) {
		printf("0x%2x, ", rxBuffer[i]);
	}
	printf("\n");
	return rxBufferLength;
} // readBlock

/**
 # See TwoWire::read()
 */

int TwoWire::available(void)
{
	// # Check for data first. readBlock will store that data in the buffer.
	if (rxBufferLength == 0) {
		readBlock(); // Read for data if there isn't any left
		// printf("%s read block\n", __PRETTY_FUNCTION__);
	}
	// printf("\tavail=%d\n", rxBufferLength - rxBufferIndex);
	if (rxBufferLength > 0)
		return rxBufferLength - rxBufferIndex;
	return 0;
} // TwoWire::available

/**
 # In the Arduino code, this would not run readBlock() since the buffers were
 # written by a called event any time data was read. This is not possible on
 # the RPI, so instead a data read runs if there is nothing in the read buffer.
 #
 # This is sketchy and needs to be revisited. For now, any time a read is
 # necesary, it will only happen if rxBufferLength == 0 or if readBlock() is
 # explicitly called.
 */

int TwoWire::read(void)
{
	// # Reads data directly, filling the buffer with any data
	if (rxBufferLength == 0)
		readBlock(); // Read for data if there isn't any left

	// No data was on bus.
	if (rxBufferLength == 0)
		return -1;
	// If the index is at the end of the buffer
	// if (rxBufferIndex == rxBufferLength)
	// 	return -1;
	// Index is lower than length and length > 0, so return some data and increment index.
	uint8_t value = rxBuffer[rxBufferIndex++];

	// if (rxBufferLength == rxBufferIndex)
	// rxBufferLength = 0; // reset buffer at end.

	return value;
} // TwoWire::read

/**
 # See TwoWire::read()
 */

int TwoWire::peek(void)
{
	// must be called in:
	// slave rx event callback
	// or after requestFrom(address, numBytes)

	// # Check for data first. readBlock will store that data in the buffer.
	if (rxBufferLength == 0)
		readBlock(); // Read for data if there isn't any left
	if (rxBufferIndex < rxBufferLength) {
		return rxBuffer[rxBufferIndex];
	}
	return -1;
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
