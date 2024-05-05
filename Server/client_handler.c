#include "client_handler.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void remove_client(int client_socket_fd) {
    pthread_mutex_lock(&server_state.client_list_mutex);
    int i;
    for (i = 0; i < server_state.client_count; i++) {
        if (server_state.clients[i].socket_fd == client_socket_fd) {
            //close(client_socket_fd); // Close the socket
            free(server_state.clients[i].node->name); // Free the chat node name
            free(server_state.clients[i].node); // Free the chat node
            // Shift the rest of the array over
            for (int j = i; j < server_state.client_count - 1; j++) {
                server_state.clients[j] = server_state.clients[j + 1];
            }
            server_state.client_count--;
            break;
        }
    }
    pthread_mutex_unlock(&server_state.client_list_mutex);
}

void shutdown_all_clients(Message *msg, ClientInfo *sender) {

    char shutdown_msg[] = "SHUTDOWN_ALL";
    pthread_mutex_lock(&server_state.client_list_mutex);  // Access the mutex from server_state

    for (int i = 0; i < server_state.client_count; i++) {
        if (server_state.clients[i].socket_fd != sender->socket_fd) {
            send(server_state.clients[i].socket_fd, shutdown_msg, strlen(shutdown_msg), 0);
        }
    }

    int i;
    for (i = 0; i < server_state.client_count; i++) {
        close(server_state.clients[i].socket_fd); // Close the socket
        free(server_state.clients[i].node->name); // Free the chat node name
        free(server_state.clients[i].node); // Free the chat node
    }
    pthread_mutex_unlock(&server_state.client_list_mutex);
    server_state.client_count = 0;

}

void *talk_to_client(void *arg) {
    int client_socket_fd = *((int *)arg); // Extract client socket file descriptor from the argument
    char buffer[1024];  // Buffer to hold incoming data
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Get client's IP address and port
    if (getpeername(client_socket_fd, (struct sockaddr *)&client_addr, &client_addr_len) == -1) {
        perror("getpeername failed");
        close(client_socket_fd);
        return NULL;
    }
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    int client_port = ntohs(client_addr.sin_port);
    ClientInfo *new_client = NULL;

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int read_size = recv(client_socket_fd, buffer, sizeof(buffer), 0);
        if (read_size > 0) {
            Message *received_msg = message_parse(buffer);
            if (received_msg) {
                switch (received_msg->type) {
                    case MSG_JOIN:

                        // Prevent the client from joining again
                        if (new_client != NULL){

                            if (strcmp(received_msg->text, "RE-JOIN") == 0) {
                                printf("Client %s re-joined from %s:%d.\n", received_msg->sender->name, client_ip, client_port);
                                break;        
                            }

                            printf("Already a chat member!\n");
                            break;

                        }

                        printf("Client %s joined from %s:%d.\n", received_msg->sender->name, client_ip, client_port);
                        // Add the client to the list
                        pthread_mutex_lock(&server_state.client_list_mutex);
                        new_client = &server_state.clients[server_state.client_count++];
                        new_client->socket_fd = client_socket_fd;
                        new_client->node = chat_node_new(received_msg->sender->name, client_ip, client_port);
                        pthread_mutex_unlock(&server_state.client_list_mutex);
                        broadcast_message(received_msg, new_client);
                        break;
                    case MSG_LEAVE:
                        if(new_client == NULL)
                            break;
                        
                        if(strcmp(received_msg->text, "SKIP") == 0)
                            break;

                        printf("Client %s left.\n", received_msg->sender->name);
                        broadcast_message(received_msg, new_client);
                        break;
                    case MSG_SHUTDOWN:
                        if(new_client == NULL)
                            break;

                        printf("Client %s left.\n", received_msg->sender->name);
                        broadcast_message(received_msg, new_client);
                        remove_client(new_client->socket_fd);
                        message_free(received_msg);
                        return NULL; // End thread after client leaves
                    case MSG_NOTE:
                        if(new_client == NULL)
                            break;

                        if(strcmp(received_msg->text, "SKIP") == 0)
                            break;

                        printf("Note from %s: %s\n", received_msg->sender->name, received_msg->text);
                        broadcast_message(received_msg, new_client);
                        break;
                    case MSG_SHUTDOWN_ALL:
                        if(new_client == NULL)
                            break;

                        printf("Shutdown all initiated by %s.\n", received_msg->sender->name);
                        shutdown_all_clients(received_msg, new_client);
                        break;
                    default:
                        printf("Unknown message type received.\n");
                }
                message_free(received_msg);
            } else {
                printf("Error parsing message.\n");
            }
        } else if (read_size == 0) {
            printf("Client disconnected.\n");
            break;
        } else {
            break;
        }
    }

    close(client_socket_fd);
    return NULL;
}


void broadcast_message(Message *msg, ClientInfo *sender) {
    pthread_mutex_lock(&server_state.client_list_mutex);  // Access the mutex from server_state
    char formatted_msg[512];

    if (msg->type == MSG_JOIN) {
        snprintf(formatted_msg, sizeof(formatted_msg), "%s %s", msg->sender->name, "joined the chat!");
    } else if (msg->type == MSG_LEAVE) {
        snprintf(formatted_msg, sizeof(formatted_msg), "%s %s", msg->sender->name, "left the chat!");
    } else if (msg->type == MSG_SHUTDOWN) {
        snprintf(formatted_msg, sizeof(formatted_msg), "%s %s", msg->sender->name, "left the chat!");
    } else {
        snprintf(formatted_msg, sizeof(formatted_msg), "%s: %s", msg->sender->name, msg->text);
    }

    for (int i = 0; i < server_state.client_count; i++) {
        if (server_state.clients[i].socket_fd != sender->socket_fd) {
            send(server_state.clients[i].socket_fd, formatted_msg, strlen(formatted_msg), 0);
        }
    }
    pthread_mutex_unlock(&server_state.client_list_mutex);
}


Message* message_parse(const char* buffer) {
    Message *msg = (Message *)malloc(sizeof(Message));
    if (!msg) return NULL;

    char type_str[16]; // Space for the message type string
    char sender_name[64]; // Space for sender name
    char content[1024]; // A separate buffer for message content

    // Parse the message type, sender name, and content
    sscanf(buffer, "%15[^:]:%63[^:]:%1023[^\n]", type_str, sender_name, content);

    // Set message type based on the parsed type string
    if (strcmp(type_str, "JOIN") == 0) {
        msg->type = MSG_JOIN;
    } else if (strcmp(type_str, "LEAVE") == 0) {
        msg->type = MSG_LEAVE;
    } else if (strcmp(type_str, "NOTE") == 0) {
        msg->type = MSG_NOTE;
        } else if (strcmp(type_str, "SHUTDOWN") == 0) {
        msg->type = MSG_SHUTDOWN;
    } else if (strcmp(type_str, "SHUTDOWN_ALL") == 0) {
        msg->type = MSG_SHUTDOWN_ALL;
    } else {
        free(msg);
        return NULL; // Unsupported message type
    }

    // Allocate memory for sender and copy sender name
    msg->sender = (ChatNode*) malloc(sizeof(ChatNode));
    msg->sender->name = strdup(sender_name);

    // Copy the parsed content as the message text
    msg->text = strdup(content);

    return msg;
}

