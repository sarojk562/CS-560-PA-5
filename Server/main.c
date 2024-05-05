#include "properties.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "main.h"
#include "client_handler.h"  
ServerState server_state;  

void start_server(const char *ip, int port) {
    // Initialize mutex within the server state
    pthread_mutex_init(&server_state.client_list_mutex, NULL);
    server_state.client_count = 0;
    
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);

    // Setting up the server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attach socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip);
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server started on %s:%d\n", ip, port);

    // Main loop to accept clients
    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (new_socket < 0) {
            perror("accept");
            continue;
        }

        // Create a thread to handle the new client
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, talk_to_client, (void *)&new_socket) != 0) {
            perror("pthread_create failed");
            close(new_socket);
        } else {
            printf("Thread created for new client.\n");
        }
    }
}

void stop_server() {
    // Clean up resources
    pthread_mutex_destroy(&server_state.client_list_mutex);
    exit(EXIT_SUCCESS);
}

int main(int argc, char const *argv[]) {


    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Properties File>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char* properties_file = malloc(100 * sizeof(char));
    strcpy(properties_file, argv[1]);
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

    start_server(server_ip, server_port);
    return 0;
}
