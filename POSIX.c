#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 1111
#define SERVER_IP "127.0.0.1"
#define USERNAME "Linux Client"

//A simple Linux client for our winsock server.
int main() {
    int sock;
    struct sockaddr_in server_address;
    char buffer[1024];
    int bytes_sent, bytes_received;

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Set up the server address structure
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        close(sock);
        return 1;
    }

    // Send the username to the server
    bytes_sent = send(sock, USERNAME, strlen(USERNAME), 0);
    if (bytes_sent < 0) {
        perror("Failed to send username");
        close(sock);
        return 1;
    }

    // Receive the acknowledgment from the server
    bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received < 0) {
        perror("Failed to receive response");
        close(sock);
        return 1;
    }
    buffer[bytes_received] = '\0';
    printf("Server response: %s\n", buffer);

    // Clean up and close the socket
    close(sock);
    return 0;
}
