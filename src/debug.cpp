#include "debug.h"

#include <stdarg.h>
#include <stdio.h>
#include <psp2/kernel/clib.h>

static FILE *f;

int debug_init() {
    debugNetInit(ip_server, port_server, DEBUG);
    f = fopen("ux0:/data/shiro.log", "w");
}

void log_info(const char *format, ...) {
    char msgbuf[0x800];
    va_list args;
    va_start(args, format);
    sceClibVsnprintf(msgbuf, 0x800, format, args);
    msgbuf[0x7FF] = 0;
    va_end(args);
    fprintf(f, msgbuf);
    fclose(f);
    f = fopen("ux0:/data/shiro.log", "a");
    debugNetPrintf(INFO, msgbuf);
}

void log_debug(const char *format, ...) {
    char msgbuf[0x800];
    va_list args;
    va_start(args, format);
    sceClibVsnprintf(msgbuf, 0x800, format, args);
    msgbuf[0x7FF] = 0;
    va_end(args);
    fprintf(f, msgbuf);
    fclose(f);
    f = fopen("ux0:/data/shiro.log", "a");
    debugNetPrintf(DEBUG, msgbuf);
}

void log_err(const char *format, ...) {
    char msgbuf[0x800];
    va_list args;
    va_start(args, format);
    sceClibVsnprintf(msgbuf, 0x800, format, args);
    msgbuf[0x7FF] = 0;
    va_end(args);
    fprintf(f, msgbuf);
    fclose(f);
    f = fopen("ux0:/data/shiro.log", "a");
    debugNetPrintf(ERROR, msgbuf);
}
