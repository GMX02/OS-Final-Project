#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <process.h> // For _beginthread
#include <ctype.h>   // For character functions

#pragma comment(lib, "ws2_32.lib") // Link against Winsock library

#define PORT 1111
#define BUFFER_SIZE 1024
#define USERNAME_SIZE 50
#define MAX_BANNED_WORDS 1000
#define MAX_WORD_LENGTH 50

// Global variables to store banned words
char *banned_words[MAX_BANNED_WORDS];
int banned_word_count = 0;

// Function to trim newline and carriage return characters
void trim_newline(char *str) {
    size_t len = strlen(str);
    while(len > 0 && (str[len-1] == '\n' || str[len-1] == '\r')) {
        str[len-1] = '\0';
        len--;
    }
}

// Function to load banned words from Profanity.txt
void load_profanity(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Failed to open %s. No profanity filtering will be applied.\n", filename);
        return;
    }

    char line[MAX_WORD_LENGTH];
    while (fgets(line, sizeof(line), file) && banned_word_count < MAX_BANNED_WORDS) {
        trim_newline(line);
        // Allocate memory for the word and store it
        banned_words[banned_word_count] = malloc(strlen(line) + 1);
        if (banned_words[banned_word_count] == NULL) {
            printf("Memory allocation failed while loading profanity words.\n");
            fclose(file);
            exit(1);
        }
        strcpy(banned_words[banned_word_count], line);
        banned_word_count++;
    }

    fclose(file);
}

// Function to convert a string to lowercase (for case-insensitive comparison)
void to_lowercase(char *str) {
    for (; *str; ++str) {
        *str = (char)tolower(*str);
    }
}

// Function to check if a string contains any banned words (case-insensitive)
int contains_profanity(const char *str) {
    if (banned_word_count == 0) {
        return 0; // No banned words loaded
    }

    // Create a lowercase copy of the input string for case-insensitive comparison
    char *lower_str = malloc(strlen(str) + 1);
    if (!lower_str) {
        printf("Memory allocation failed during profanity check.\n");
        return 0; // On failure, assume no profanity
    }
    strcpy(lower_str, str);
    to_lowercase(lower_str);

    for (int i = 0; i < banned_word_count; i++) {
        // Create a lowercase copy of the banned word
        char *lower_banned = malloc(strlen(banned_words[i]) + 1);
        if (!lower_banned) {
            printf("Memory allocation failed during profanity check.\n");
            free(lower_str);
            return 0;
        }
        strcpy(lower_banned, banned_words[i]);
        to_lowercase(lower_banned);

        if (strstr(lower_str, lower_banned) != NULL) {
            free(lower_banned);
            free(lower_str);
            return 1; // Profanity found
        }
        free(lower_banned);
    }

    free(lower_str);
    return 0; // No profanity found
}

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

    // Check if the username contains any banned words
    if (contains_profanity(username)) {
        const char *profanity_msg = "Username contains prohibited language. Connection terminated.";
        send(client_socket, profanity_msg, strlen(profanity_msg), 0);
        printf("Client disconnected");
        closesocket(client_socket);
        _endthread();
        return;
    }

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
        char *buffer_copy = malloc(bytes_read + 1);
        if (!buffer_copy) {
            printf("Memory allocation failed while handling client %s.\n", username);
            continue;
        }
        strcpy(buffer_copy, buffer); // strtok modifies the string, so use a copy

        char *token = strtok(buffer_copy, ";");
        if (token != NULL) {
            strncpy(received_username, token, USERNAME_SIZE - 1);
            received_username[USERNAME_SIZE - 1] = '\0';
            token = strtok(NULL, ";");
            if (token != NULL) {
                gridSize = atoi(token);
                gridData = strtok(NULL, ";");
            } else {
                printf("Invalid data format from client %s.\n", username);
                free(buffer_copy);
                continue;
            }
        } else {
            printf("Invalid data format from client %s.\n", username);
            free(buffer_copy);
            continue;
        }

        free(buffer_copy);

        // Validate the grid size
        if (gridSize < 1 || gridSize > 6) {
            printf("Received invalid grid size %d from client %s.\n", gridSize, username);
            const char *response = "Invalid grid size.";
            send(client_socket, response, strlen(response), 0);
            continue;
        } else 

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

    // Load banned words from Profanity.txt
    load_profanity("Profanity.txt");

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
        if (!client_socket_ptr) {
            printf("Memory allocation failed while accepting a new client.\n");
            closesocket(client_socket);
            continue;
        }
        *client_socket_ptr = client_socket;
        _beginthread(handle_client, 0, client_socket_ptr);
    }

    // Cleanup (unreachable in current server loop)
    closesocket(server_socket);
    WSACleanup();

    // Free banned words
    for (int i = 0; i < banned_word_count; i++) {
        free(banned_words[i]);
    }

    return 0;
}
