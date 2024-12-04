#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>      // For character functions
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib") // Link against Winsock library

#define PORT 4206
#define BUFFER_SIZE 1024
#define USERNAME_SIZE 50

char **createGrid(int size) {
    char **grid = malloc(size * sizeof(char *));
    if (grid == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    for (int i = 0; i < size; i++) {
        grid[i] = malloc(size * sizeof(char));
        if (grid[i] == NULL) {
            printf("Memory allocation failed.\n");
            exit(1);
        }
        // Initialize the row with spaces
        for (int j = 0; j < size; j++) {
            grid[i][j] = ' '; // Initialize all cells to spaces
        }
    }
    return grid;
}
// Function to display the crossword grid with borders
void displayGrid(char **grid, int size) {
    printf("\nCurrent Grid:\n");
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

// Function to free the dynamically allocated grid memory
void freeGrid(char **grid, int size) {
    // Free each row
    for (int i = 0; i < size; i++) {
        free(grid[i]);
    }
    // Free the array of row pointers
    free(grid);
}

// Function to send the grid to the server
void sendGridToServer(SOCKET sock, char **grid, int size, const char *username) {
    // Serialize the grid into a string
    char buffer[BUFFER_SIZE];
    int offset = 0;

    // Include the username and grid size at the beginning
    offset += snprintf(buffer + offset, BUFFER_SIZE - offset, "%s;%d;", username, size);

    // Serialize the grid data
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (offset < BUFFER_SIZE - 2) { // Reserve space for '\0'
                buffer[offset++] = grid[i][j];
            } else {
                printf("Buffer overflow while sending grid.\n");
                return;
            }
        }
    }
    buffer[offset] = '\0'; // Null-terminate the string

    // Send the grid data to the server
    if (send(sock, buffer, strlen(buffer), 0) == SOCKET_ERROR) {
        printf("Failed to send data. Error Code: %d\n", WSAGetLastError());
    }
}

// Function to display the answer grid
void displayAnswers(int gridSize) {
    printf("\nAnswer Grid:\n");

    if (gridSize == 3) {
        // Display the 3x3 answer grid
        printf(" --- --- ---\n");
        printf("| D | A | D |\n");
        printf(" --- --- ---\n");
        printf("| O | W | E |\n");
        printf(" --- --- ---\n");
        printf("| T | E | N |\n");
        printf(" --- --- ---\n");

    } else if (gridSize == 5) {
        // Display the 5x5 answer grid
        printf(" --- --- --- --- ---\n");
        printf("| S | H | A | N | T |\n");
        printf(" --- --- --- --- ---\n");
        printf("| C | E | D | A | R |\n");
        printf(" --- --- --- --- ---\n");
        printf("| O | L | I | V | E |\n");
        printf(" --- --- --- --- ---\n");
        printf("| F | L | E | E | S |\n");
        printf(" --- --- --- --- ---\n");
        printf("| F | O | U | L | S |\n");
        printf(" --- --- --- --- ---\n");
    } else {
        printf("Invalid grid size. No answers available.\n");
    }
}

// Function to display clues
void displayClues(int gridSize) {
    if (gridSize == 3) {
        printf("\nClues:\n");
        printf("Across:\n");
        printf("1. Familiar term for father,\n");
        printf("2. To be in debt for something.\n");
        printf("3. Between nine and eleven.\n");
        printf("Down:\n");
        printf("1. A very small mark.\n");
        printf("2. Fear/Shock.\n");
        printf("3. A place where a wild animal lives.\n");
    } else if (gridSize == 5) {
        printf("\nClues:\n");
        printf("Across:\n");
        printf("1. Opposite of shall in poetry.\n");
        printf("2. Fragrant cabinet wood.\n");
        printf("3. Kind of green.\n");
        printf("4. Makes a run for it.\n");
        printf("5. Infractions that may lead to yellow cards.\n");
        printf("Down:\n");
        printf("1. Mocks verbally.\n");
        printf("2. Greetings!\n");
        printf("3. Farewell, in Paris.\n");
        printf("4. Kind of orange.\n");
        printf("5. Long lock of hair.\n");
    }
}

// Function to create the answer grid
char **createAnswerGrid(int gridSize) {
    char **answerGrid = createGrid(gridSize);

    if (gridSize == 3) {
        // Place the answers for 3x3 crossword
        answerGrid[0][0] = 'D';
        answerGrid[0][1] = 'A';
        answerGrid[0][2] = 'D';
        answerGrid[1][0] = 'O';
        answerGrid[1][1] = 'W';
        answerGrid[1][2] = 'E';
        answerGrid[2][0] = 'T';
        answerGrid[2][1] = 'E';
        answerGrid[2][2] = 'N';
    } else if (gridSize == 5) {
        // Place the answers for 5x5 crossword
        answerGrid[0][0] = 'S'; answerGrid[0][1] = 'H'; answerGrid[0][2] = 'A'; answerGrid[0][3] = 'N'; answerGrid[0][4] = 'T';
        answerGrid[1][0] = 'C'; answerGrid[1][1] = 'E'; answerGrid[1][2] = 'D'; answerGrid[1][3] = 'A'; answerGrid[1][4] = 'R';
        answerGrid[2][0] = 'O'; answerGrid[2][1] = 'L'; answerGrid[2][2] = 'I'; answerGrid[2][3] = 'V'; answerGrid[2][4] = 'E';
        answerGrid[3][0] = 'F'; answerGrid[3][1] = 'L'; answerGrid[3][2] = 'E'; answerGrid[3][3] = 'E'; answerGrid[3][4] = 'S';
        answerGrid[4][0] = 'F'; answerGrid[4][1] = 'O'; answerGrid[4][2] = 'U'; answerGrid[4][3] = 'L'; answerGrid[4][4] = 'S';
    }

    return answerGrid;
}

// Function to compare user's grid with the answer grid
int compareGrids(char **userGrid, char **answerGrid, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (answerGrid[i][j] != ' ') { // Only compare cells that should have letters
                if (userGrid[i][j] != answerGrid[i][j]) {
                    return 0; // Grids do not match
                }
            }
        }
    }
    return 1; // Grids match
}

// Main function
int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE];
    char username[USERNAME_SIZE];

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
    server_address.sin_addr.s_addr = inet_addr("192.168.1.170"); // Replace with your server's IPv4 address

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        printf("Connection failed. Error Code: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    printf("Connected to the server.\n");

    // Prompt for username
    printf("Enter your username (max %d characters): ", USERNAME_SIZE - 1);
    fgets(username, USERNAME_SIZE, stdin);
    // Remove newline character if present
    size_t len = strlen(username);
    if (username[len - 1] == '\n') {
        username[len - 1] = '\0';
    }

    // Send the username to the server
    if (send(sock, username, strlen(username), 0) == SOCKET_ERROR) {
        printf("Failed to send username. Error Code: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Receive server's acknowledgment
    int bytes_read = recv(sock, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Server: %s\n", buffer);
    } else {
        printf("Failed to receive acknowledgment from server.\n");
    }

    int playAgain = 1;
    while (playAgain) {
        int gridSize = 0;
        char inputLine[100];

        while (gridSize != 3 && gridSize != 5) {
            printf("Enter 1 for a 3x3 grid, 2 for a 5x5 grid: ");
            if (fgets(inputLine, sizeof(inputLine), stdin) != NULL) {
                int input = 0;
                if (sscanf(inputLine, "%d", &input) == 1) {
                    if (input == 1) {
                        gridSize = 3;
                    } else if (input == 2) {
                        gridSize = 5;
                    } else {
                        printf("Invalid input. Please input 1 or 2.\n");
                    }
                } else {
                    printf("Invalid input. Please input 1 or 2.\n");
                }
            } else {
                printf("Error reading input.\n");
                closesocket(sock);
                WSACleanup();
                return 1;
            }
        }

        // Create the grid dynamically
        char **grid = createGrid(gridSize);
        char **answerGrid = createAnswerGrid(gridSize);

        // Display the clues
        displayClues(gridSize);

        // Variable to control the loop
        int keepEditing = 1;

        while (keepEditing) {
            // Display the grid
            displayClues(gridSize);
            displayGrid(grid, gridSize);

            // Send the grid to the server
            sendGridToServer(sock, grid, gridSize, username);

            // Receive and print the response from the server
            bytes_read = recv(sock, buffer, BUFFER_SIZE - 1, 0);
            if (bytes_read <= 0) {
                printf("Connection closed by server.\n");
                break;
            }
            buffer[bytes_read] = '\0'; // Null-terminate the received data
            printf("Server: %s\n", buffer);

            printf("Enter the row (1-%d), column (1-%d), and character to place,\n'or c' to check your answers, or 'x' to quit: ", gridSize, gridSize);

            // Read input
            int row, col;
            char ch;

            if (fgets(inputLine, sizeof(inputLine), stdin) != NULL) {
                // Check if the user wants to quit
                if (inputLine[0] == 'x' || inputLine[0] == 'X') {
                    keepEditing = 0;
                    break;
                }

                // backdoor answers
                if (inputLine[0] == '~') {
                    displayAnswers(gridSize);
                    continue;
                }

                // Check if the user wants to check the answers
                if (inputLine[0] == 'c' || inputLine[0] == 'C') {
                    if (compareGrids(grid, answerGrid, gridSize)) {
                        printf("Congratulations, you win!\n");
                        keepEditing = 0;
                        break;
                    } else {
                        printf("Try again!\n");
                        continue;
                    }
                }

                // Parse the input
                if (sscanf(inputLine, "%d %d %c", &row, &col, &ch) == 3) {
                    // Adjust for 0-based indexing
                    row -= 1;
                    col -= 1;

                    // Validate the input
                    if (row >= 0 && row < gridSize && col >= 0 && col < gridSize) {
                        // Check if the cell should have a letter
                        if (answerGrid[row][col] != ' ') {
                            // Check if the character is a letter
                            if (isalpha(ch)) {
                                // Convert to uppercase
                                ch = toupper(ch);
                                grid[row][col] = ch;
                            } else {
                                printf("Invalid character. Please enter a letter.\n");
                            }
                        } else {
                            printf("This cell is not part of any clue.\n");
                        }
                    } else {
                        printf("Invalid row or column. Please try again.\n");
                    }
                } else {
                    printf("Invalid input format. Please enter row, column, and character.\n");
                }
            } else {
                printf("Error reading input.\n");
                break;
            }
        }

        // Send the final grid to the server
        sendGridToServer(sock, grid, gridSize, username);

        // Free the allocated memory
        freeGrid(grid, gridSize);
        freeGrid(answerGrid, gridSize);

        // Ask if the user wants to play again
        printf("Would you like to play again? (y/n): ");
        char playAgainInput[10];
        fgets(playAgainInput, sizeof(playAgainInput), stdin);
        if (playAgainInput[0] != 'y' && playAgainInput[0] != 'Y') {
            playAgain = 0;
        }
    }

    // Close the socket and clean up
    closesocket(sock);
    WSACleanup();

    printf("Goodbye %s!\n", username);

    return 0;
}
