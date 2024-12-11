
// This iteration of client.c has crossword implementation with both 3x3 and 5x5 grids. Haven't been able to test with multiple users.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>      // For character functions
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib") // Link against Winsock library

#define PORT 1111
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

    if (gridSize == 1) {
        // Display the 3x3 answer grid
        printf(" ---\n");
        printf("| Y |\n");
        printf(" ---\n");

    }

    if (gridSize == 2) {
        // Display the 3x3 answer grid
        printf(" --- ---\n");
        printf("| Y | E |\n");
        printf(" --- ---\n");
        printf("| O | W |\n");
        printf(" --- ---\n");
    }

    if (gridSize == 3) {
        // Display the 3x3 answer grid
        printf(" --- --- ---\n");
        printf("| D | A | D |\n");
        printf(" --- --- ---\n");
        printf("| O | W | E |\n");
        printf(" --- --- ---\n");
        printf("| T | E | N |\n");
        printf(" --- --- ---\n");

    } 
    if (gridSize == 4) {
        // Replace this section with the correct display logic for a 5x5 answer grid
        // Example grid for demonstration:
        printf(" --- --- --- ---\n");
        printf("| B | E | E | R |\n");
        printf(" --- --- --- ---\n");
        printf("| R | A | V | E |\n");
        printf(" --- --- --- ---\n");
        printf("| A | S | I | A |\n");
        printf(" --- --- --- ---\n");
        printf("| Y | E | L | L |\n");
        printf(" --- --- --- ---\n");
}       else if (gridSize == 5) {
        // Replace this section with the correct display logic for a 5x5 answer grid
        // Example grid for demonstration:
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

}
    else if (gridSize == 6) {
        // Replace this section with the correct display logic for a 5x5 answer grid
        // Example grid for demonstration:
        printf(" --- --- --- --- --- ---\n");
        printf("| A | L | A | S | K | A |\n");
        printf(" --- --- --- --- --- ---\n");
        printf("| S | I | P | P | E | D |\n");
        printf(" --- --- --- --- --- ---\n");
        printf("| S | N | O | R | E | D |\n");
        printf(" --- --- --- --- --- ---\n");
        printf("| E | N | G | I | N | E |\n");
        printf(" --- --- --- --- --- ---\n");
        printf("| T | E | E | T | E | R |\n");
        printf(" --- --- --- --- --- ---\n");
        printf("| S | T | E | E | R | S |\n");
        printf(" --- --- --- --- --- ---\n");


    } else {
        printf("Invalid grid size. No answers available.\n");
    }

}
// Function to display clues
void displayClues(int gridSize) {
    if (gridSize == 1){
        printf("Clue: You have a 1 in 26 chance. Good luck!.\n");
    }if(gridSize == 2){
        printf("\nClues:\n");
        printf("1. Across: Kanye          | Down: 'I' in Spanish.\n");
        printf("2. Across: __, that hurt! | Down: That's nasty!\n");
    }
    if (gridSize == 3) {
        printf("\nClues:\n");
        printf("1. Across: Familiar term for father,   |  Down: A very small mark.\n");
        printf("2. Across: To be in debt for something.|  Down: Fear Shock.\n");
        printf("3. Across: Between nine and eleven     |  Down: A place where a wild animal lives.\n");
    }
    if (gridSize == 4) {
        printf("\nClues:\n");
        printf("1. Across: ____ Keg               | Down: Loud cry of mule or donkey.\n");
        printf("2. Across: Special kind of music.| Down: Greetings!\n");
        printf("3. Across: Where is Russia?       | Down: Maleficent.\n");
        printf("4. Across: To scream.             | Down: Really not fake.\n");
    }
    if (gridSize == 5) {
        printf("\nClues:\n");
        printf("1. Across: Opposite of shall in poetry.              | Down: Mocks verbally.\n");
        printf("2. Across: Fragrant cabinet wood.                    | Down: Greetings!\n");
        printf("3. Across: Kind of green.                            | Down: Farewell, in Paris.\n");
        printf("4. Across: Makes a run for it.                       | Down: Kind of orange.\n");
        printf("5. Across: Infractions that may lead to yellow cards.| Down: Long lock of hair.\n");
    }
    if (gridSize == 6) {
        printf("\nClues:\n");
        printf("1. Across: Largest state.     | Down: Valuable Properties.\n");
        printf("2. Across: Drank gingerly.    | Down: Small finch\n");
        printf("3. Across: Slept noisily.     | Down: Furthest orbit.\n");
        printf("4. Across: Movement machine.  | Down: Fairy, elf.\n");
        printf("5. Across: Balance unsteadily.| Down: More enthusiastic.\n");
        printf("6. Across: Navigatess vehicle. | Down: Mathematical snakes.\n");

    }
}

// Function to create the answer grid
char **createAnswerGrid(int gridSize) {
    char **answerGrid = createGrid(gridSize);
    if (gridSize == 1) {
        // Initialize all cells to spaces
        for (int i = 0; i < gridSize; i++) {
            for (int j = 0; j < gridSize; j++) {
                answerGrid[i][j] = ' ';
            }
        }
        answerGrid[0][0] = 'Y';
    }
    if (gridSize == 2) {
        // Initialize all cells to spaces
        for (int i = 0; i < gridSize; i++) {
            for (int j = 0; j < gridSize; j++) {
                answerGrid[i][j] = ' ';
            }
        }
        answerGrid[0][0] = 'Y';
        answerGrid[0][1] = 'E';
        answerGrid[1][0] = 'O';
        answerGrid[1][1] = 'W';
        

    }
    if (gridSize == 3) {
        // Initialize all cells to spaces
        for (int i = 0; i < gridSize; i++) {
            for (int j = 0; j < gridSize; j++) {
                answerGrid[i][j] = ' ';
            }
        }

        answerGrid[0][0] = 'D';
        answerGrid[0][1] = 'A';
        answerGrid[0][2] = 'D';
        answerGrid[1][0] = 'O';
        answerGrid[1][1] = 'W';
        answerGrid[1][2] = 'E';
        answerGrid[2][0] = 'T';
        answerGrid[2][1] = 'E';
        answerGrid[2][2] = 'N';

    } 
    
    if (gridSize == 4) {
        // Initialize all cells to spaces
        for (int i = 0; i < gridSize; i++) {
            for (int j = 0; j < gridSize; j++) {
                answerGrid[i][j] = ' ';
            }
        }
        answerGrid[0][0] = 'B';
        answerGrid[0][1] = 'E';
        answerGrid[0][2] = 'E';
        answerGrid[0][3] = 'R';
        answerGrid[1][0] = 'R';
        answerGrid[1][1] = 'A';
        answerGrid[1][2] = 'V';
        answerGrid[1][3] = 'E';
        answerGrid[2][0] = 'A';
        answerGrid[2][1] = 'S';
        answerGrid[2][2] = 'I';
        answerGrid[2][3] = 'A';
        answerGrid[3][0] = 'Y';
        answerGrid[3][1] = 'E';
        answerGrid[3][2] = 'L';
        answerGrid[3][3] = 'L';
    }

    if (gridSize == 5) {
        // Answer grid for 5x5 crossword
        // Across 1: "SHANT"
        // A2: "CEDAR"
        // A3: "OLIVE"
        // A4: "FLEES"
        // A5: "FOULS"
        // Down 1: "SCOFF"
        // D2: "HELLO"
        // D3: "ADIEU"
        // D4: "NAVEL"
        // D5: "TRESS"

        // Initialize all cells to spaces
        for (int i = 0; i < gridSize; i++) {
            for (int j = 0; j < gridSize; j++) {
                answerGrid[i][j] = ' ';
            }
        }

        // Place the answers
        // Across 1
        answerGrid[0][0] = 'S';
        answerGrid[0][1] = 'H';
        answerGrid[0][2] = 'A';
        answerGrid[0][3] = 'N';
        answerGrid[0][4] = 'T';
        answerGrid[1][0] = 'C';
        answerGrid[1][1] = 'E';
        answerGrid[1][2] = 'D';
        answerGrid[1][3] = 'A';
        answerGrid[1][4] = 'R';
        answerGrid[2][0] = 'O';
        answerGrid[2][1] = 'L';
        answerGrid[2][2] = 'I';
        answerGrid[2][3] = 'V';
        answerGrid[2][4] = 'E';
        answerGrid[3][0] = 'F';
        answerGrid[3][1] = 'L';
        answerGrid[3][2] = 'E';
        answerGrid[3][3] = 'E';
        answerGrid[3][4] = 'S';
        answerGrid[4][0] = 'F';
        answerGrid[4][1] = 'O';
        answerGrid[4][2] = 'U';
        answerGrid[4][3] = 'L';
        answerGrid[4][4] = 'S';

    }

    if (gridSize == 6) {
        // Initialize all cells to spaces
        for (int i = 0; i < gridSize; i++) {
            for (int j = 0; j < gridSize; j++) {
                answerGrid[i][j] = ' ';
            }
        }

        answerGrid[0][0] = 'A';
        answerGrid[0][1] = 'L';
        answerGrid[0][2] = 'A';
        answerGrid[0][3] = 'S';
        answerGrid[0][4] = 'K';
        answerGrid[0][5] = 'A';

        answerGrid[1][0] = 'S';
        answerGrid[1][1] = 'I';
        answerGrid[1][2] = 'P';
        answerGrid[1][3] = 'P';
        answerGrid[1][4] = 'E';
        answerGrid[1][5] = 'D';
        
        answerGrid[2][0] = 'S';
        answerGrid[2][1] = 'N';
        answerGrid[2][2] = 'O';
        answerGrid[2][3] = 'R';
        answerGrid[2][4] = 'E';
        answerGrid[2][5] = 'D';
        
        answerGrid[3][0] = 'E';
        answerGrid[3][1] = 'N';
        answerGrid[3][2] = 'G';
        answerGrid[3][3] = 'I';
        answerGrid[3][4] = 'N';
        answerGrid[3][5] = 'E';
        
        answerGrid[4][0] = 'T';
        answerGrid[4][1] = 'E';
        answerGrid[4][2] = 'E';
        answerGrid[4][3] = 'T';
        answerGrid[4][4] = 'E';
        answerGrid[4][5] = 'R';

        answerGrid[5][0] = 'S';
        answerGrid[5][1] = 'T';
        answerGrid[5][2] = 'E';
        answerGrid[5][3] = 'E';
        answerGrid[5][4] = 'R';
        answerGrid[5][5] = 'S';
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
    server_address.sin_addr.s_addr = inet_addr("10.2.146.250"); // Replace with your server's IPv4 address

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

    int playAgain = 1; // Variable to control the "play again" loop
    while (playAgain) {
        int gridSize = 0;
        char inputLine[100];

        // Prompt user for grid size
        while (gridSize != 1 && gridSize != 2 && gridSize != 3 && gridSize != 4 && gridSize != 5 && gridSize != 6) {
            printf("Enter 1 for a 1x1 grid, 2 for a 2x2 grid, 3 for a 3x3 grid, 4 for a 4x4 grid, 5 for a 5x5 grid, 6 for a 6x6 grid: ");
            if (fgets(inputLine, sizeof(inputLine), stdin) != NULL) {
                int input = 0;
                if (sscanf(inputLine, "%d", &input) == 1) {
                    if (input >= 1 && input <= 6) {
                        gridSize = input;
                    } else {
                        printf("Invalid input. Please input a number between 1 and 6.\n");
                    }
                } else {
                    printf("Invalid input. Please input a number between 1 and 6.\n");
                }
            } else {
                printf("Error reading input.\n");
                closesocket(sock);
                WSACleanup();
                return 1;
            }
        }

        // Create grids dynamically
        char **grid = createGrid(gridSize);
        char **answerGrid = createAnswerGrid(gridSize);

        

        // Main game loop
        int keepEditing = 1;
        while (keepEditing) {
            // Display grid and clues
            displayClues(gridSize);
            displayGrid(grid, gridSize);

            // Send the grid to the server
            sendGridToServer(sock, grid, gridSize, username);

            // Receive and print response from server
            bytes_read = recv(sock, buffer, BUFFER_SIZE - 1, 0);
            if (bytes_read <= 0) {
                printf("Connection closed by server.\n");
                break;
            }
            buffer[bytes_read] = '\0';
            printf("Server: %s\n", buffer);

            printf("Enter the row (1-%d), column (1-%d), and character to place,\n'or c' to check your answers, or 'x' to quit: ", gridSize, gridSize);

            int row, col;
            char ch;
            if (fgets(inputLine, sizeof(inputLine), stdin) != NULL) {
                if (inputLine[0] == 'x' || inputLine[0] == 'X') {
                    keepEditing = 0;
                    break;
                }
                if (inputLine[0] == '~') {
                    displayAnswers(gridSize);
                    continue;
                }
                if (inputLine[0] == 'c' || inputLine[0] == 'C') {
                    if (compareGrids(grid, answerGrid, gridSize)) {
                        displayGrid(grid, gridSize);
                        printf("Congratulations, you win!\n");
                        keepEditing = 0;
                        break;
                    } else {
                        printf("\nTry again!\n");
                        continue;
                    }
                }
                if (sscanf(inputLine, "%d %d %c", &row, &col, &ch) == 3) {
                    row -= 1;
                    col -= 1;
                    if (row >= 0 && row < gridSize && col >= 0 && col < gridSize) {
                        if (answerGrid[row][col] != ' ') {
                            if (isalpha(ch)) {
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

        // Send final grid to the server
        sendGridToServer(sock, grid, gridSize, username);

        // Free allocated memory
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

    // Close socket and clean up
    closesocket(sock);
    WSACleanup();

    printf("Goodbye %s!\n", username);
    return 0;
}
