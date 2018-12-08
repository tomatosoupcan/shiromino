#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <debugnet.h>

#define ip_server "192.168.43.66"
#define port_server 18194

int debug_init();
void log_info(const char *format, ...);
void log_debug(const char *format, ...);
void log_err(const char *format, ...);
#define check(A, M, ...) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

#endif // _DEBUG_H_