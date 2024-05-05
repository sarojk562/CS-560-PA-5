#include "message.h"
#include "receiver_handler.h"
#include "main.h"
#include "message.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// This function runs in a separate thread to handle incoming messages
void *receiver_handler(void *arg) {
    char buffer[1024];  // Buffer to store incoming data
    int read_size;

    while ((read_size = recv(client_info.socket_fd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[read_size] = '\0'; // Null-terminate the string
        if (strcmp(buffer, "SHUTDOWN_ALL") == 0) {
            printf("[Server] Shutdown command received.\n");
            exit(EXIT_SUCCESS);  // Exit the client as the server is shutting down
        }
        
        printf("%s\n", buffer);
        printf("Enter message ('JOIN', 'LEAVE', 'SHUTDOWN', 'SHUTDOWN_ALL' or any message): \n");

    }

    if (read_size == 0) {
        printf("Server has closed the connection.\n");
        exit(EXIT_SUCCESS);
    } else if (read_size == -1) {
        exit(EXIT_FAILURE);
    }

    return NULL;
}
