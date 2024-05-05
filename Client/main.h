#ifndef MAIN_H
#define MAIN_H

#include "types.h"
#include <pthread.h>

// Global client information
extern ClientInfo client_info;

void start_client(const char *server_ip, int server_port, const char *client_name);
void stop_client();

#endif /* MAIN_H */
