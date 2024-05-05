// client_handler.h
#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include "types.h"  // Includes ClientInfo and necessary system includes
#include "message.h"

void *talk_to_client(void *arg);
void broadcast_message(Message *msg, ClientInfo *sender);
Message* message_parse(const char* buffer);

#endif /* CLIENT_HANDLER_H */
