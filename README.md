# arduino-port-lib

A library that can be used to port Arduino-based code to run on the Raspberry Pi.

This port is designed to be as compatible as possible with Arduino code interfacing with it. For more info on the limitations, see below.

If you find something that is broken and not noted there, please let me know/raise an issue! If a function is intentionally removed for the port, a message like this will show up:

> [function] in [file]&#x3A;[line #] This function cannot be used for the RasPi port of this library.

## SPI Library

The SPI pin control is handled within the library now. To set an SPI pin, change lines like this:

```c++
// Settings object, this may be integrated into another line of code
SPISettings settingsMode0(500000, LSBFIRST, SPI_MODE0);
```

To this:

```c++
// Where pin is a valid WiringPi pin number on the Raspberry Pi
SPISettings settingsMode0(500000, MSBFIRST, SPI_MODE0, pin);
```

You'll also need to remove calls which try to drive the pin, as they won't work.

To change later arguments without changing the pin, use `PIN_VALUE_DEFAULT_CS`, which will just keep using the Raspeberry Pi's hardware pin.

If the pin is active high, add another argument `true`. Active low (`false`) is default:

```c++
SPISettings settingsMode0(500000, MSBFIRST, SPI_MODE0, pin, true);
```

Bit order is done in software, where `MSBFIRST` takes no extra processing.

## Wire Library (I2C)

When a call to `read()`, `available()`, or `peek()` is called, it may not work. If there are any issues with this, let us know and raise an issue, as this may need a very different solution from the Arduino Wire implementation.

## Adding This Library to Your Package

To add one of the libraries from this package, change the given files with these snippets as guides.

**CMakeLists.txt**

```yaml
# Find the package
find_package(catkin REQUIRED COMPONENTS
  arduino_port_lib
)
# Include the found directory
include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)
# Set up the package
catkin_package(
  INCLUDE_DIRS include ${catkin_INCLUDE_DIRS}
  CATKIN_DEPENDS arduino_port_lib
)

# After the add_executable line, link the library
# Choose arduino_port_wire and/or arduino_port_spi
target_link_libraries([executable_node_name] arduino_port_wire ${catkin_LIBRARIES})
```

**package.xml**

```xml
<!-- Put this in after the catkin depend -->
<depend>arduino_port_lib</depend>
```

**C++ Files**

```c++
// Choose any or all of these to include:
#include "arduino_port_lib/ArduinoPort.h.h" // Generic functions like delay()
#include "arduino_port_lib/Ardu_Wire.h"     // Arduino Wire (I2C)
#include "arduino_port_lib/Ardu_SPI.h"      // Arduino SPI
```
