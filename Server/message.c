#include "message.h"
#include <stdlib.h>
#include <string.h>

Message* message_new(MessageType type, ChatNode* sender, const char* text) {
    Message* msg = (Message*)malloc(sizeof(Message));
    if (!msg) return NULL;

    msg->type = type;
    msg->sender = sender;
    if (text) {
        msg->text = strdup(text);
    } else {
        msg->text = NULL;
    }

    return msg;
}

void message_free(Message* msg) {
    if (msg) {
        if (msg->text) {
            free(msg->text);
        }
        free(msg);
    }
}
