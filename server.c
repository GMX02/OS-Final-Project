#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <process.h> // For _beginthread
#include <ctype.h>   // For character functions

#pragma comment(lib, "ws2_32.lib") // Link against Winsock library

#define PORT 4206
#define BUFFER_SIZE 1024
#define USERNAME_SIZE 50

// Function to create the crossword grid dynamically
char **createGrid(int size) {
    // Allocate memory for the array of row pointers
    char **grid = malloc(size * sizeof(char *));
    if (grid == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    // Allocate memory for each row
    for (int i = 0; i < size; i++) {
        grid[i] = malloc(size * sizeof(char));
        if (grid[i] == NULL) {
            printf("Memory allocation failed.\n");
            exit(1);
        }
    }
    return grid;
}

// Function to free the dynamically allocated grid memory
void freeGrid(char **grid, int size) {
    // Free each row
    for (int i = 0; i < size; i++) {
        free(grid[i]);
    }
    // Free the array of row pointers
    free(grid);
}

// Function to display the crossword grid with borders
void displayGrid(char **grid, int size, const char *username) {
    printf("Grid from user '%s' (%dx%d):\n", username, size, size);
    for (int i = 0; i < size; i++) {
        // Print horizontal borders
        for (int j = 0; j < size; j++) {
            printf(" ---");
        }
        printf("\n");

        // Print vertical borders and cell content
        for (int j = 0; j < size; j++) {
            printf("| %c ", grid[i][j]);
        }
        printf("|\n");
    }

    // Print the bottom horizontal borders
    for (int j = 0; j < size; j++) {
        printf(" ---");
    }
    printf("\n");
}

// Function to deserialize the grid data
char **deserializeGrid(const char *data, int size) {
    char **grid = createGrid(size);
    int index = 0;
    for (int i = 0; i < size && data[index] != '\0'; i++) {
        for (int j = 0; j < size && data[index] != '\0'; j++) {
            grid[i][j] = data[index++];
        }
    }
    return grid;
}

void handle_client(void *client_socket_ptr) {
    SOCKET client_socket = *(SOCKET *)client_socket_ptr;
    free(client_socket_ptr);

    char buffer[BUFFER_SIZE];
    char username[USERNAME_SIZE];

    // Receive the username from the client
    int bytes_read = recv(client_socket, username, USERNAME_SIZE - 1, 0);
    if (bytes_read <= 0) {
        printf("Failed to receive username from client.\n");
        closesocket(client_socket);
        _endthread();
        return;
    }
    username[bytes_read] = '\0'; // Null-terminate the username
    printf("Client connected with username: %s\n", username);

    // Send acknowledgment to the client
    const char *ack_message = "Username received by server.";
    send(client_socket, ack_message, strlen(ack_message), 0);

    while (1) {
        bytes_read = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_read <= 0) {
            break; // Connection closed or error
        }
        buffer[bytes_read] = '\0'; // Null-terminate the received string

        // Parse the received data to extract the username, grid size, and grid data
        char received_username[USERNAME_SIZE];
        int gridSize = 0;
        const char *gridData;
        char *token = strtok(buffer, ";");
        if (token != NULL) {
            strncpy(received_username, token, USERNAME_SIZE - 1);
            received_username[USERNAME_SIZE - 1] = '\0';
            token = strtok(NULL, ";");
            if (token != NULL) {
                gridSize = atoi(token);
                gridData = strtok(NULL, ";");
            } else {
                printf("Invalid data format from client %s.\n", username);
                continue;
            }
        } else {
            printf("Invalid data format from client %s.\n", username);
            continue;
        }

        // Validate the grid size
        if (gridSize != 3 && gridSize != 5) {
            printf("Received invalid grid size %d from client %s.\n", gridSize, username);
            const char *response = "Invalid grid size.";
            send(client_socket, response, strlen(response), 0);
            continue;
        }

        // Check if the username matches
        if (strcmp(username, received_username) != 0) {
            printf("Username mismatch for client %s.\n", username);
            const char *response = "Username mismatch.";
            send(client_socket, response, strlen(response), 0);
            continue;
        }

        // Deserialize and display the grid
        char **grid = deserializeGrid(gridData, gridSize);
        displayGrid(grid, gridSize, username);
        freeGrid(grid, gridSize);

        // Send a response to the client
        const char *response = "Grid received and displayed on server.";
        send(client_socket, response, strlen(response), 0);
    }

    printf("Client %s disconnected.\n", username);
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

        // Create a new thread to handle the client
        SOCKET *client_socket_ptr = malloc(sizeof(SOCKET));
        *client_socket_ptr = client_socket;
        _beginthread(handle_client, 0, client_socket_ptr);
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}

