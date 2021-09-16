#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

#include "worker_thread.h"

// Adds "delay" nanoseconds to timespecs and sleeps until that time
static void sleep_until(struct timespec *ts, int delay)
{
	
	ts->tv_nsec += delay;
	if(ts->tv_nsec >= 1000*1000*1000){
		ts->tv_nsec -= 1000*1000*1000;
		ts->tv_sec++;
	}
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, ts,  NULL);
}

// General purpose error message
// A real system would probably have a better error handling method...
static void panic(char *message)
{
	fprintf(stderr,"Fatal error: %s\n", message);
	exit(1);
}


// Initialize a GPIO pin in Linux using the sysfs interface
FILE *init_gpio(int gpioport)
{
	// Export the pin to the GPIO directory
	FILE *fp = fopen("/sys/class/gpio/export","w");
	fprintf(fp,"%d",gpioport);
	fclose(fp);

	// Set the pin as an output
	char filename[256];
	sprintf(filename,"/sys/class/gpio/gpio%d/direction",gpioport);
	fp = fopen(filename,"w");
	if(!fp){
		panic("Could not open gpio file");
	}
	fprintf(fp,"out");
	fclose(fp);

	// Open the value file and return a pointer to it.
	sprintf(filename,"/sys/class/gpio/gpio%d/value",gpioport);
	fp = fopen(filename,"w");
	if(!fp){
		panic("Could not open gpio file");
	}
	return fp;
}

// Given a FP in the stepper struct, set the I/O pin
// to the specified value. Uses the sysfs GPIO interface.
void setiopin(FILE *fp, int val)
{
	fprintf(fp,"%d\n",val);
	fflush(fp);
}

void step(int step_size) {

	static int init;
	static int state;

	static FILE *pin0 = NULL;
	static FILE *pin1 = NULL;
	static FILE *pin2 = NULL;
	static FILE *pin3 = NULL;

	if (pin0 == NULL) {
		pin0 = init_gpio(17);
		pin1 = init_gpio(18);
		pin2 = init_gpio(23);
		pin3 = init_gpio(24);
		//init = 1;
	}

	if (step_size == 0) {
		setiopin(pin0, 0);
		setiopin(pin1, 0);
		setiopin(pin2, 0);
		setiopin(pin3, 0);
		return NULL;
	}
	
	
	switch (state%8) {
		//half-step sequence
		case 0: setiopin(pin0, 1); break;
		case 1: setiopin(pin3, 0); break;
		case 2: setiopin(pin1, 1); break;
		case 3: setiopin(pin0, 0); break;
		case 4: setiopin(pin2, 1); break;
		case 5: setiopin(pin1, 0); break;
		case 6: setiopin(pin3, 1); break;
		case 7: setiopin(pin2, 0); break;
	}
	

	/*
	switch (state%4) {
		//full-step sequence
		case 0: setiopin(pin3, 0); 
			 	setiopin(pin0, 1); 
				break;
		case 1: setiopin(pin0, 0); 
				setiopin(pin1, 1);
				break;
		case 2: setiopin(pin1, 0); 
				setiopin(pin2, 1);
				break;
		case 3: setiopin(pin2, 0); 
				setiopin(pin3, 1);
				break;
	}
	*/

	state += 1;
	//printf("state = %d\n", state);

	return NULL;
}


void *thread_func(void *data)
{
    unsigned int delay_ns;
    unsigned int freq_hz;
    int cnt=0;
	int pin25 = 25;

    struct timespec ts;

    printf("rt thread is running\n");

    clock_gettime(CLOCK_MONOTONIC, &ts);


    freq_hz = 520/1;
    //freq_hz = 10;
    delay_ns = 1e9/2/freq_hz;


    /* Do RT specific stuff here */
    while (1) {
        //printf("rt thread is running\n");
        //setiopin(pin25, cnt++%2);
		//gpioWrite(pin25, cnt++%2);
		//usleep(100);
		step(1);
		//step(0);
        sleep_until(&ts, delay_ns);
        //sleep_until(&ts, /*ns*/ 100*1000);
    }
    return NULL;
}

