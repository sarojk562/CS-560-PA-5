// main.h
#ifndef MAIN_H
#define MAIN_H

#include "types.h" 

#define MAX_CLIENTS 100

typedef struct server_state {
    ClientInfo clients[MAX_CLIENTS];
    int client_count;
    pthread_mutex_t client_list_mutex;
} ServerState;

extern ServerState server_state;

void start_server(const char *ip, int port);
void stop_server();

#endif /* MAIN_H */
