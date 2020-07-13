#ifndef ARDUINO_PORT_H
#define ARDUINO_PORT_H

// Arduino millisecond delay function to linux microsecond sleep function.
#define delay(a) usleep(a * 1000)

#endif /* ifndef ARDUINOPORT_H */
