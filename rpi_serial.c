#include <pigpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

time_t begin_time, end_time;

int main(int argc, char *argv[]) {
	int int_main_return = 0;
	if (gpioInitialise() < 0) { int_main_return = 1; }
	else {
		char *serial_id = "/dev/serial0";
		int ser = serOpen(serial_id, 9600, 0);
		char ser_in[256];
		if (ser >= 0) {
			if (argc > 1) {
				if (strcmp(argv[1], "getweight") == 0) {
					char *serial_out = "getweight\n";
					serWrite(ser, serial_out, 10);
				}
				else { int_main_return = 2; }
				bool got_serial = false;
				begin_time = time(0);
				while(got_serial == false) {
					int count = serRead(ser, ser_in, 256);
					if (count > 0) {
						printf(ser_in);
						printf("\n");
						got_serial = true;
					}
					else {
						end_time = time(0);
						if ((end_time - begin_time) > 4) {
							int_main_return = 16;
							break;
						}
			}}}
			else { int_main_return = 4; }
		}
		else { return 8; }
	}
	switch(int_main_return) {
		case(0) : break;
		case(1) : printf("Error 1: Initialize failed!\n");
			break;
		case(2) : printf("Error 2: %s Not found\n", argv[1]);
			break;
		case(4) : printf("Error 4: Improper call\n");
			break;
		case(8) : printf("Error 8: failed to establish connection\n");
			break;
		case(16): printf("Error 16: SerRead operation timed out!\n");
			break;
	}
	return int_main_return;
}
