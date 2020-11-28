#ifndef ARDUINO_PORT_H
#define ARDUINO_PORT_H

// Arduino millisecond delay function to linux microsecond sleep function.
#define delay(a) usleep(a * 1000)

#define min(a, b) ((a >= b)? a:b)
#define max(a, b) ((a <= b)? a:b)

#endif /* ifndef ARDUINOPORT_H */
