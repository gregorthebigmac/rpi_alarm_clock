#include <pigpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char *argv[]) {
	if (gpioInitialise() < 0) {
		printf("Error 1: Initialise failed!\n");
		return 1;
	}
	else {
		char *serial_id = "/dev/serial0";
		int ser = serOpen(serial_id, 9600, 0);
		char ser_in[256];
		if (ser >= 0) {
			while (true) {
				char *serial_out = "getweight\n";
				serWrite(ser, serial_out, 10);
				bool got_serial = false;
				while(got_serial == false) {
					int count = serRead(ser, ser_in, 256);
					if (count > 0) {
						printf(ser_in);
						printf("\n");
						got_serial = true;
					}
				}
			}
		}
		else {
			printf("Error 8: failed to establish connection\n");
			return 8;
		}
	}
	return 0;
}
