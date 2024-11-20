#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib") // Link against Winsock library

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Winsock initialization failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Configure server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr("10.2.147.178"); // Replace with your server's IPv4 adress

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        printf("Connection failed. Error Code: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    printf("Connected to the server.\n");

    // Communicate with the server
    while (1) {
        printf("Enter message: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        // Send message to the server
        send(sock, buffer, strlen(buffer), 0);

        // Receive and print the response from the server
        int bytes_read = recv(sock, buffer, BUFFER_SIZE, 0);
        if (bytes_read <= 0) {
            printf("Connection closed by server.\n");
            break;
        }
        buffer[bytes_read] = '\0'; // Null-terminate the received data
        printf("Server: %s\n", buffer);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
