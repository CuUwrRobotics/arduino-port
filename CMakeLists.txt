cmake_minimum_required(VERSION 2.8.3)
project(arduino_port_lib)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

################################################################################
# LIBRARY BUILD/INSTALL CONFIGS

find_package(catkin REQUIRED COMPONENTS
)

FIND_LIBRARY(WIRINGPI_LIBRARY wiringPi /usr/include )

include_directories(
 src
 include
 ${catkin_INCLUDE_DIRS}
)

catkin_package(INCLUDE_DIRS include
               LIBRARIES arduino_port_spi arduino_port_wire
               DEPENDS $(WIRINGPI_LIBRARY)
               )

# Wire (I2C) Library
add_library(arduino_port_wire src/Ardu_Wire.cpp)
target_link_libraries(arduino_port_wire ${catkin_LIBRARIES})

# SPI Library
add_library(arduino_port_spi src/Ardu_SPI.cpp)
target_link_libraries(arduino_port_spi ${WIRINGPI_LIBRARY} ${catkin_LIBRARIES})

# Install both libraries
install(TARGETS arduino_port_spi arduino_port_wire
        ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION})
install(DIRECTORY include/${PROJECT_NAME}/
        DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})



################################################################################
# TESTING NODES BUILD CONFIGS

# find_package(catkin REQUIRED COMPONENTS
# )
#
# FIND_LIBRARY(WIRINGPI_LIBRARY wiringPi /usr/include)
#
# include_directories(
#  src
#  include/arduino_port_lib
#  ${catkin_INCLUDE_DIRS}
# )
#
# # Enable SPI tester node
# add_executable(${PROJECT_NAME}_spi_test src/SpiMainTester.cpp src/Ardu_SPI.cpp )
# target_link_libraries(${PROJECT_NAME}_spi_test ${catkin_LIBRARIES} ${WIRINGPI_LIBRARY})
#
# # Enable I2C tester node
# add_executable(${PROJECT_NAME}_i2c_test src/I2cMainTester.cpp src/Ardu_Wire.cpp )
# target_link_libraries(${PROJECT_NAME}_i2c_test ${catkin_LIBRARIES})
#
# # Enable GPIO tester node
# add_executable(${PROJECT_NAME}_gpio_test src/GpioMainTester.cpp )
# target_link_libraries(${PROJECT_NAME}_gpio_test ${catkin_LIBRARIES} ${WIRINGPI_LIBRARY})
