#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "TorqueRod.h"

static int keepRunning = 1;

void intHandler(int value) {
    keepRunning = 0;
}



int main(int argc, char **argv)
{
	signal(SIGINT, intHandler);

	const char *chipname = "gpiochip2"; // AXI GPIO
	unsigned int line_num_p = 0; // GPIO Pin #10
	unsigned int line_num_n = 1; // GPIO Pin #11
	MtrDevice trodX;
	MtrDevice trodY;
	MtrState state = MTR_OFF;
	
	mtr_constructor("MTR_X", &trodX);
	mtr_constructor("MTR_Y", &trodY);
	mtr_init(chipname, line_num_p, chipname, line_num_n, &trodX);
	mtr_init(chipname, line_num_p+2, chipname, line_num_n+2, &trodY);

	for (int i =0; i<5; i++) {

		if (!keepRunning) break;

		if (trodX.reset) {
			mtr_reset(&trodX);
		}
		if (trodY.reset) {
			mtr_reset(&trodY);
		}
		if (trodX.active && trodY.active) {
			printf("Set MTR_X and MTR_Y State to %u\n", state);
			mtr_set_state(&trodX, state);
			mtr_set_state(&trodY, state);
			sleep(2);
		} else {
			break;
		}
		state++;
		if (state > MTR_TORQUE_N) state = MTR_OFF;
	}

	mtr_close(&trodX);
	mtr_close(&trodY);
}