// types.h
#ifndef TYPES_H
#define TYPES_H

#include <pthread.h>  // For pthread_t

struct chat_node;

typedef struct client_info {
    int socket_fd;
    struct chat_node *node; 
    pthread_t thread_id;
} ClientInfo;

#endif
