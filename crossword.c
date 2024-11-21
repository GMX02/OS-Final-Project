//Test of crossword implentation, has been added to client.c for easier use.
#include <stdio.h>
#include <stdlib.h>  // Include stdlib.h for malloc and free

// Function to create the crossword grid dynamically
char **createGrid(int size) {
    // Allocate memory for the array of row pointers
    char **grid = malloc(size * sizeof(char *));
    if (grid == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    // Allocate memory for each row and initialize cells to spaces
    for (int i = 0; i < size; i++) {
        grid[i] = malloc(size * sizeof(char));
        if (grid[i] == NULL) {
            printf("Memory allocation failed.\n");
            exit(1);
        }
        // Initialize the row with spaces
        for (int j = 0; j < size; j++) {
            grid[i][j] = ' ';
        }
    }
    return grid;
}

// Function to display the crossword grid with borders
void displayGrid(char **grid, int size) {
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

int main() {
    int gridSize = 0;
    char inputLine[100];
    while (gridSize != 3 && gridSize != 5) {
        printf("Enter 1 for a 3x3 grid or 2 for a 5x5 grid: ");
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
            return 1;
        }
    }

    // Create the grid dynamically
    char **grid = createGrid(gridSize);

    // Variable to control the loop
    int keepEditing = 1;

    while (keepEditing) {
        // Display the grid
        displayGrid(grid, gridSize);

        // Prompt the user for input
        printf("Enter the row (1-%d), column (1-%d), and character to place (or 'q' to quit): ", gridSize, gridSize);
        
        // Read input
        int row, col;
        char ch;

        if (fgets(inputLine, sizeof(inputLine), stdin) != NULL) {
            // Check if the user wants to quit
            if (inputLine[0] == 'q' || inputLine[0] == 'Q') {
                keepEditing = 0;
                break;
            }

            // Parse the input
            if (sscanf(inputLine, "%d %d %c", &row, &col, &ch) == 3) {
                // Adjust for 0-based indexing
                row -= 1;
                col -= 1;

                // Validate the input
                if (row >= 0 && row < gridSize && col >= 0 && col < gridSize) {
                    grid[row][col] = ch;
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

    // Display the final grid
    printf("Final grid:\n");
    displayGrid(grid, gridSize);

    // Free the allocated memory
    freeGrid(grid, gridSize);

    printf("Goodbye!\n");
    return 0;
}
