// Slide
// slide.c
//
// This program was written by Evan Williams (z5368211)
// on DATE
//
// Version 1.0.0 (2021-03-08): Initial Assignment Release

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SIZE 15
#define EMPTY 0
#define STONE 1

#define LASER_MAX 4

void print_map(int map[SIZE][SIZE], int laser_y);

// TODO: Add any extra function prototypes here.

int main (void) {
    // This line creates our 2D array called "map" and sets all
    // of the blocks in the map to EMPTY.
    int map[SIZE][SIZE] = {EMPTY};

    // This line creates our laser_y variable. The laser starts in the
    // middle of the map, at position 7.
    int laser_y = SIZE / 2;


    int blocks;
    printf("How many blocks? ");
    scanf("%d", &blocks);


    printf("Enter blocks:\n");
    // This is a loop that allows each block to be scanned.
    for (int i = 0; i < blocks; i++) {
        int row, col, digit;

        scanf("%d %d %d", &row, &col, &digit);

        // Only change values if the row and column numbers are valid.
        if (0 <= row && row < SIZE && 0 <= col && col < SIZE) {
            map[row][col] = digit;
        }
    }


    print_map(map, laser_y);

    // Read commands until EOF
    int instruction;
    int game_over = 0;
    while ( !game_over && (scanf("%d", &instruction) != EOF) ) {

        // Move laser command
        if (instruction == 1) {
            int shift;
            scanf("%d", &shift); 
            
            // Check that shift is valid and the laser would not move off map
            if (shift == 1 && laser_y < SIZE - 1) {
                laser_y += shift; 
            } else if (shift == -1 && laser_y > 0) {
                laser_y += shift;
            }
        // Fire Laser command
        } else if (instruction == 2) {
            int col = 0; 
            int destroyed_stones = 0;

            // Loop over the columns in the row where the laser is
            while (col < SIZE && destroyed_stones < LASER_MAX) {
                // Change stones to empty and increment destroyed stone counter
                if (map[laser_y][col] == STONE) {
                    map[laser_y][col] = EMPTY;
                    destroyed_stones++;
                }
                col++;
            }

            //Check if the game is won
            int row = 0;
            int is_empty = 1;
            while (row < SIZE && is_empty) {
                col = 0;
                while (col < SIZE && is_empty) {
                    // If index of map is not empty, the game is not won.
                    if (map[row][col] != EMPTY) {
                        is_empty = !is_empty;
                    } 
                    col++;
                }
                row++;
            }
            if (is_empty) {
                print_map(map, laser_y);
                printf("Game Won!\n");
                game_over = 1;
            }

        // Shift Everything Left
        } else if (instruction == 3) {
            // Check if there are any stones in the leftmost column
            int row = 0;
            while (row < SIZE && !game_over) {
                if (map[row][0] == STONE) {
                    game_over = 1;
                }
                row++;
            }

            // If game is over, print Game Lost!. 
            // Otherwise, move everything left.
            if (game_over) {
                print_map(map, laser_y);
                printf("Game Lost!\n");    
            } else {
                for (row = 0; row < SIZE; row++) {
                    for (int col = 1; col < SIZE; col++) {
                        if (map[row][col] != EMPTY) {
                            map[row][col-1] = map[row][col];
                            map[row][col] = EMPTY;
                        }
                    }
                }
            }
            
        // Rotate the map
        } else if (instruction == 4) {
            // Scan the next number (the direction) into a variable
            int direction;
            scanf("%d", &direction);

            // Only continue if direction is 1 or 2
            if (direction == 1 || direction == 2) {
                int new_map[SIZE][SIZE] = {EMPTY};    
                
                // Loop over the rows of the map starting from the bottom
                // and then change the new_map value based on the map value
                for (int curr_row = 0; curr_row < SIZE; curr_row++) {
                    for (int curr_col = 0; curr_col < SIZE; curr_col++) {
                        // If non-empty, change the value in new_map
                        if (map[curr_row][curr_col] != EMPTY) {
                            // Convert the curr_row to the col value of new_map.
                            // If the direction is 1, this means getting the
                            // absolute value of the row - (SIZE - 1).
                            // Otherwise, this is just the row value.
                            int new_col;
                            if (direction == 1) {
                                new_col = -(curr_row - (SIZE - 1));
                            } else {
                                new_col = curr_row;
                            }

                            // Convert the curr_col to the row value of new_map
                            int new_row = curr_col

                            new_map[new_row][new_col] = map[curr_row][curr_col];
                        }
                    }
                }

            } 
        }

        if (!game_over) {
            print_map(map, laser_y);
        }
    }


    return 0;
}

// Print out the contents of the map array. 
// Also print out a > symbol to denote the current laser position.
void print_map(int map[SIZE][SIZE], int laser_y) {
    int i = 0;
    while (i < SIZE) {
        if (i == laser_y) {
            printf("> ");
        } else {
            printf("  ");
        }
        int j = 0;
        while (j < SIZE) {
            printf("%d ", map[i][j]);
            j++;
        }
        printf("\n");
        i++;
    }
}
