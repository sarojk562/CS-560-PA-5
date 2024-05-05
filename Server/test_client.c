#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SERVER_PORT 8605
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

void *receive_messages(void *sock_fd);

int main(int argc, char **argv) {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    pthread_t recv_thread;

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    printf("Connected to the server at %s:%d\n", SERVER_IP, SERVER_PORT);

    // Create a thread to listen for messages from the server
    pthread_create(&recv_thread, NULL, receive_messages, (void *)&sock);

    // Send commands to the server
    while (1) {
        printf("Enter command: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        // Send message to server
        if (send(sock, buffer, strlen(buffer), 0) < 0) {
            perror("Send failed");
            break;
        }

        if (strncmp(buffer, "SHUTDOWN", 8) == 0) {
            printf("Shutdown command sent, closing client.\n");
            break;
        }
    }

    // Cleanup
    close(sock);
    pthread_join(recv_thread, NULL);
    return 0;
}

void *receive_messages(void *sock_fd) {
    int sock = *(int *)sock_fd;
    char buffer[BUFFER_SIZE];

    while (1) {
        int len = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (len > 0) {
            buffer[len] = '\0';
            printf("Received: %s\n", buffer);
        } else if (len == 0) {
            printf("Server closed the connection\n");
            break;
        } else {
            perror("recv failed");
            break;
        }
    }

    return NULL;
}
