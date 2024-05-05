#include "main.h"
#include "properties.h"
#include "chat_node.h"
#include "receiver_handler.h"
#include "sender_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>

ClientInfo client_info; // GLOBAL client information
pthread_mutex_t connection_mutex = PTHREAD_MUTEX_INITIALIZER;
bool shouldTerminate = false;

void start_client(const char *server_ip, int server_port, const char *client_name) {
    struct sockaddr_in server_addr;

    // Create socket
    client_info.socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_info.socket_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    // Connect to the server
    if (connect(client_info.socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to server failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server at %s:%d\n", server_ip, server_port);

    // Initialize client_info
    client_info.node = malloc(sizeof(ChatNode));
    client_info.node->name = strdup(client_name);

    // Start thread for sending messages
    pthread_t send_thread;
    pthread_create(&send_thread, NULL, sender_handler, NULL);

    // Start thread for receiving messages
    receiver_handler(NULL);

    // When receiving thread exits, close the client
    stop_client();
}

void stop_client() {
    // Clean up and close the connection
    close(client_info.socket_fd);
    free(client_info.node->name);
    free(client_info.node);
    printf("Disconnected from server.\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Properties File>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char* properties_file = argv[1];
    Properties* properties = property_read_properties(properties_file);

    char* server_ip = property_get_property(properties, "ServerIP");
    if (server_ip == NULL) {
        fprintf(stderr, "Server IP not found in properties file.\n");
        exit(EXIT_FAILURE);
    }

    char* server_port_str = property_get_property(properties, "ServerPort");
    if (server_port_str == NULL) {
        fprintf(stderr, "Server Port not found in properties file.\n");
        exit(EXIT_FAILURE);
    }
    int server_port = atoi(server_port_str);

    char* client_name = property_get_property(properties, "ClientName");
    if (client_name == NULL) {
        fprintf(stderr, "Client Name not found in properties file.\n");
        exit(EXIT_FAILURE);
    }

    start_client(server_ip, server_port, client_name);
    return 0;
}