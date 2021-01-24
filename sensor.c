#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <unistd.h>

#include <syslog.h>

#include "defs.h"

void read_clock(int iic_fd, Clock *clock){
    int tempMSB, tempLSB;
    float temp;

    clock->seconds = wiringPiI2CReadReg8(iic_fd,0);
    usleep(2000);

    clock->minute = wiringPiI2CReadReg8(iic_fd,1);
    usleep(2000);

    clock->hour = wiringPiI2CReadReg8(iic_fd,2);
    usleep(2000);

    tempMSB = wiringPiI2CReadReg8(iic_fd, 0x11);
    usleep(2000);
    tempLSB = wiringPiI2CReadReg8(iic_fd,0x12);
    usleep(2000);
    tempLSB = tempLSB >> 6;
    temp = tempLSB * 25 / 100.0;
    temp += tempMSB;
    clock->temp = temp;
}

void print_clock(Clock *clock){
    syslog(LOG_INFO, "Seconds: %x", clock->seconds);
    syslog(LOG_INFO, "Minutes: %x", clock->minute);
    syslog(LOG_INFO, "Hours: %x", clock->hour);
    syslog(LOG_INFO, "Temp: %f", clock->temp);
}

void copy_clock(Clock *origin, Clock *dest){
    dest->seconds = origin->seconds;
    dest->minute = origin->minute;
    dest->hour = origin->hour;
    dest->temp = origin->temp;
}
