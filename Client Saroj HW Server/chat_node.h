#ifndef CHAT_NODE_H
#define CHAT_NODE_H

#include <netinet/in.h>
#include "types.h"

typedef struct chat_node {
    char* name;
    struct sockaddr_in address;
} ChatNode;

ChatNode* chat_node_new(const char* name, const char* ip, int port);
void chat_node_free(ChatNode* node);

#endif /* CHAT_NODE_H */
