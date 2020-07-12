cmake_minimum_required(VERSION 2.8.3)
project(arduino_port_lib)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
)

## System dependencies are found with CMake's conventions
# find_package(Boost REQUIRED COMPONENTS system)

###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if your package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(
#  INCLUDE_DIRS include
   # LIBRARIES diagnostic_display #Adafruit_SSD1306
#  CATKIN_DEPENDS roscpp rospy std_msgs
#  DEPENDS system_lib
#CATKIN_DEPENDS message_runtime
)

###########
## Build ##
###########

FIND_LIBRARY(WIRINGPI_LIBRARY wiringPi /usr/include)
#message(AUTHOR_WARNING ${WIRINGPI_LIBRARY})
#message(AUTHOR_WARNING ${WIRINGPI_I2C_LIBRARY})
#FIND_LIBRARY(STDINT stdint /usr/include)


#set(SSD1306_LOC )

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
 include
 src
 ${catkin_INCLUDE_DIRS}
  # /home/pi/diagnostics_ws/src/diagnostic_display/include/Adafruit_SSD1306-master/
#  /usr/include stdint.h
  # ${WIRINGPI_LIBRARY}
  #${WIRINGPI_I2C_LIBRARY}
)

## Declare a C++ library
 #add_library(${PROJECT_NAME}
   #/home/pi/diagnostics_ws/src/diagnostic_display/include/Adafruit_SSD1306-master/arduinoPort.cpp
 #)



## Add cmake target dependencies of the library
## as an example, code may need to be generated before libraries
## either from message generation or dynamic reconfigure
# add_dependencies(${PROJECT_NAME} ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

## Declare a C++ executable
## With catkin_make all packages are built within a single CMake context
## The recommended prefix ensures that target names across packages don't collide
# add_executable(${PROJECT_NAME}_node src/beginner_tutorials_node.cpp)

## Rename C++ executable without prefix
## The above recommended prefix causes long target names, the following renames the
## target back to the shorter version for ease of user use
## e.g. "rosrun someones_pkg node" instead of "rosrun someones_pkg someones_pkg_node"
# set_target_properties(${PROJECT_NAME}_node PROPERTIES OUTPUT_NAME node PREFIX "")

## Add cmake target dependencies of the executable
## same as for the library above
# add_dependencies(${PROJECT_NAME}_node ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

## Specify libraries to link a library or executable target against


#############
## Install ##
#############

# all install targets should use catkin DESTINATION variables
# See http://ros.org/doc/api/catkin/html/adv_user_guide/variables.html

## Mark executable scripts (Python etc.) for installation
## in contrast to setup.py, you can choose the destination
# install(PROGRAMS
#   scripts/my_python_script
#   DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )

## Mark executables for installation
## See http://docs.ros.org/melodic/api/catkin/html/howto/format1/building_executables.html
# install(TARGETS ${PROJECT_NAME}_node
#   RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )

## Mark libraries for installation
## See http://docs.ros.org/melodic/api/catkin/html/howto/format1/building_libraries.html
# install(TARGETS ${PROJECT_NAME}
#   ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#   LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#   RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION}
# )

## Mark cpp header files for installation
# install(DIRECTORY include/${PROJECT_NAME}/
#   DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
#   FILES_MATCHING PATTERN "*.h"
#   PATTERN ".svn" EXCLUDE
# )

## Mark other files for installation (e.g. launch and bag files, etc.)
# install(FILES
#   # myfile1
#   # myfile2
#   DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
# )

# Enable SPI tester node
add_executable(${PROJECT_NAME}_spi_test src/SpiMainTester.cpp src/Ardu_SPI.cpp )
target_link_libraries(${PROJECT_NAME}_spi_test ${catkin_LIBRARIES} ${WIRINGPI_LIBRARY})

# Enable I2C tester node
add_executable(${PROJECT_NAME}_i2c_test src/I2cMainTester.cpp src/Ardu_Wire.cpp )
target_link_libraries(${PROJECT_NAME}_i2c_test ${catkin_LIBRARIES})

# Enable GPIO tester node
add_executable(${PROJECT_NAME}_gpio_test src/GpioMainTester.cpp )
target_link_libraries(${PROJECT_NAME}_gpio_test ${catkin_LIBRARIES} ${WIRINGPI_LIBRARY})

#add_dependencies(listener beginner_tutorials_generate_messages_cpp wiringPi #Adafruit_SSD1306
#stdint
#)
