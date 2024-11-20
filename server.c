#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <process.h> // For _beginthread

#pragma comment(lib, "ws2_32.lib") // Link against Winsock library

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_client(void *client_socket_ptr) {
    SOCKET client_socket = *(SOCKET *)client_socket_ptr;
    free(client_socket_ptr);

    char buffer[BUFFER_SIZE];

    // Send a basic welcome message
    const char *welcome_message = "Connected to the server!\n";
    send(client_socket, welcome_message, strlen(welcome_message), 0);

    // Echo messages back to the client (basic functionality)
    while (1) {
        int bytes_read = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_read <= 0) {
            break; // Connection closed or error
        }
        buffer[bytes_read] = '\0'; // Null-terminate the received string
        printf("Client: %s\n", buffer);
        send(client_socket, buffer, strlen(buffer), 0); // Echo back
    }

    printf("Client disconnected.\n");
    closesocket(client_socket);
    _endthread(); // End the thread
}

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    int addrlen = sizeof(client_address);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Winsock initialization failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Bind socket to the specified port
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY; // Bind to any available interface
    server_address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        printf("Bind failed. Error Code: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_socket, 3) == SOCKET_ERROR) {
        printf("Listen failed. Error Code: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }
    printf("Server is running on port %d\n", PORT);

    // Accept and handle incoming connections
    while (1) {
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, &addrlen)) == INVALID_SOCKET) {
            printf("Accept failed. Error Code: %d\n", WSAGetLastError());
            continue;
        }
        printf("New client connected.\n");

        // Create a new thread to handle the client
        SOCKET *client_socket_ptr = malloc(sizeof(SOCKET));
        *client_socket_ptr = client_socket;
        _beginthread(handle_client, 0, client_socket_ptr);
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}
