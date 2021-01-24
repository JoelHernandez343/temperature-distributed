/**
 * Joel Hernández @ 2020
 * Github: https://github.com/JoelHernandez343
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>

#include <syslog.h>


#include "defs.h"
#include "sensor.h"

extern Clock data;
extern pthread_mutex_t lock;
extern pthread_t thread;
extern int exit_flag;

void blocking(int opcode, Clock *c){
    pthread_mutex_lock(&lock);

    if (opcode == GET_DATA){
        copy_clock(&data, c);
    } else {
        copy_clock(c, &data);
    }

    pthread_mutex_unlock(&lock);
}


void *clock_reading(void *args){
    Clock clock;
    int iic_fd = wiringPiI2CSetup(DIR_RTCC);

    while (exit_flag){
        read_clock(iic_fd , &clock);
        // print_clock(&clock);
        blocking(SET_DATA, &clock);
    }

    return NULL;
}

void launch_thread(){
    pthread_create(&thread, NULL, clock_reading, NULL);
}