#ifndef MESSAGE_H
#define MESSAGE_H

#include "chat_node.h"

typedef enum {
    MSG_JOIN,
    MSG_LEAVE,
    MSG_NOTE,
    MSG_SHUTDOWN,
    MSG_SHUTDOWN_ALL
} MessageType;

typedef struct message {
    MessageType type;
    ChatNode* sender;
    char* text; // For MSG_NOTE type
} Message;

Message* message_new(MessageType type, ChatNode* sender, const char* text);
void message_free(Message* message);

#endif /* MESSAGE_H */
