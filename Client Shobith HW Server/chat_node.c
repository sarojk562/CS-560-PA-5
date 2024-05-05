#include "chat_node.h"
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>  

ChatNode* chat_node_new(const char* name, const char* ip, int port) {
    ChatNode* node = (ChatNode*)malloc(sizeof(ChatNode));
    if (!node) return NULL;

    node->name = strdup(name);
    node->address.sin_family = AF_INET;
    node->address.sin_port = htons(port);
    inet_pton(AF_INET, ip, &node->address.sin_addr);

    return node;
}

void chat_node_free(ChatNode* node) {
    if (node) {
        free(node->name);
        free(node);
    }
}
