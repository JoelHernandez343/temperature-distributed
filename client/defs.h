#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define PORT 4000
#define VPORT 19535
#define SERVER_DOMAIN_NAME "2.tcp.ngrok.io"

#define ERR_LOG "\033[40;31mERR!\033[0m "
#define DONE_LOG "\033[40;32mDONE\033[0m "

#define GET_DATA 1
#define SET_DATA 2

#define DIR_RTCC 0x68

typedef struct clock {
    char seconds;
    char minute;
    char hour;
    float temp;
} Clock;


#endif