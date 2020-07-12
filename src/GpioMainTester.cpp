#include <stdio.h>
#include <signal.h>
#include <wiringPi.h>
#include <unistd.h>

void sigintHandle(int sig) {
} // sigintHandle

int main() {
	signal(SIGINT, sigintHandle);
	const int pin = 0; // 4th up from right

	printf("Starting on pin %d\n", pin);

	wiringPiSetup();

	pinMode(pin, OUTPUT);

	for (int i = 0; i < 10; i++) {
		digitalWrite(pin, HIGH);
		printf("1\n");
		usleep(50000);
		digitalWrite(pin, LOW);
		printf("0\n");
		usleep(50000);
	}

	return 0;
} // main
