#ifndef ARDUINO_PORT_H
#define ARDUINO_PORT_H

// Arduino millisecond delay function to linux microsecond sleep function.
#define delay(a) usleep(a * 1000)

// undefine stdlib's abs if encountered
#ifdef abs
#undef abs
#endif /* ifdef abs */

#define min(a, b) ((a) < (b)?(a):(b))
#define max(a, b) ((a) > (b)?(a):(b))
#define abs(x) ((x) > 0?(x):-(x))
#define constrain(amt, low, high) ((amt) < (low)?(low):((amt) > (high)?(high): \
                                                        (amt)))
#define round(x)     ((x) >= 0?(long)((x) + 0.5):(long)((x) - 0.5))
#define radians(deg) ((deg) * DEG_TO_RAD)
#define degrees(rad) ((rad) * RAD_TO_DEG)
#define sq(x) ((x) * (x))

#define interrupts() sei()
#define noInterrupts() cli()

#define clockCyclesPerMicrosecond() (F_CPU / 1000000L)
#define clockCyclesToMicroseconds(a) ((a) / clockCyclesPerMicrosecond())
#define microsecondsToClockCycles(a) ((a) * clockCyclesPerMicrosecond())

#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitToggle(value, bit) ((value) ^= (1UL << (bit)))
#define bitWrite(value, bit, bitvalue) ((bitvalue) ? bitSet(value, bit) : \
                                        bitClear(value, bit))

#endif /* ifndef ARDUINOPORT_H */
