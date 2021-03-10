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
#include <stdbool.h>

#define SIZE 15
#define EMPTY 0
#define STONE 1

#define LASER_MAX 4

#define MOVE_LASER 1
#define FIRE_LASER 2
#define SHIFT_EVERYTHING_LEFT 3
#define ROTATE_MAP 4

#define DOWNWARDS 1
#define UPWARDS -1

#define CLOCKWISE 1
#define COUNTER_CLOCKWISE 2

#define TNT_MIN 4
#define TNT_MAX 9

void print_map(int map[SIZE][SIZE], int laser_y);

// These functions correspond to different commands. Move laser is not included
// because it is very short.
void fire_laser(int map[SIZE][SIZE], int laser_y, bool *game_over);
void shift_everything_left(int map[SIZE][SIZE], int laser_y, bool *game_over);
void rotate_map(int map[SIZE][SIZE], bool *game_over, bool *is_rotated);

double distance(int x1, int y1, int x2, int y2);
void delete_in_line(int map[SIZE][SIZE], int laser_y, int col,
                    int direction_x, int direction_y, int explosion_radius);

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
    
    // Create variables to track the state of the game.
    bool is_rotated = false;
    bool game_over = false;

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
    while ( !game_over && (scanf("%d", &instruction) != EOF) ) {

        // Move laser command
        if (instruction == MOVE_LASER) {
            int shift;
            scanf("%d", &shift); 
            
            // Check that shift is valid and the laser would not move off map
            if (shift == DOWNWARDS && laser_y < SIZE - 1) {
                laser_y += shift; 
            } else if (shift == UPWARDS && laser_y > 0) {
                laser_y += shift;
            }

        // Fire Laser command
        } else if (instruction == FIRE_LASER) {
            fire_laser(map, laser_y, &game_over);

        // Shift Everything Left
        } else if (instruction == SHIFT_EVERYTHING_LEFT) {
            shift_everything_left(map, laser_y, &game_over);
            
        // Rotate the map
        } else if (instruction == ROTATE_MAP) {
            rotate_map(map, &game_over, &is_rotated);
        }

        // If the game is not finished, we want to print the map.
        // If the game is finished, the map is printed in the instruction's 
        // function, so it should not be printed again.
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

// This function returns the distance between two points.

double distance(int x1, int y1, int x2, int y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

// This function moves across the map from the position of the laser by a
// given vector, and deletes all values inside a given radius from the starting
// position. This is intended to be used to implement TNT explosions.

void delete_in_line(int map[SIZE][SIZE], int laser_y, int col,
                    int direction_x, int direction_y, int explosion_radius) {
    int curr_row = laser_y;
    int curr_col = col;

    // Move in the given direction until outside of the explosion radius
    // or outside of the bounds of the map.
    while (distance(col, laser_y, curr_col, curr_row) < explosion_radius
           && curr_col < SIZE && curr_row < SIZE) {
        map[curr_row][curr_col] = EMPTY;
        curr_col += direction_x;
        curr_row += direction_y;
    }
}

// This function fires the laser and removes any objects in its path, and prints
// Game Won! if the map is empty.
// This can change the map array and the game_over boolean.

void fire_laser(int map[SIZE][SIZE], int laser_y, bool *game_over) {
    int col = 0; 
    int destroyed_blocks = 0;

    // Loop over the columns in the row where the laser is
    while (col < SIZE && destroyed_blocks < LASER_MAX) {
        // Change stones to empty and increment destroyed blocks counter
        if (map[laser_y][col] == STONE) {
            map[laser_y][col] = EMPTY;
            destroyed_blocks++;
        
        // TNT Blocks
        } else if (TNT_MIN <= map[laser_y][col] && map[laser_y][col] <= TNT_MAX) {
            int explosion_radius = map[laser_y][col];

            // First delete blocks in the positive x direction
            // Negative x is not needed because the laser came from there.
            delete_in_line(map, laser_y, col, 1, 0, explosion_radius);
            // Then in the positive y direction
            delete_in_line(map, laser_y, col, 0, 1, explosion_radius);
            // Then in the negative y direction
            delete_in_line(map, laser_y, col, 0, -1, explosion_radius);

            destroyed_blocks++;
        }
        col++;
    }

    //Check if the game is won
    int row = 0;
    bool is_empty = true;
    while (row < SIZE && is_empty) {
        col = 0;
        while (col < SIZE && is_empty) {
            // If index of map is not empty, the game is not won.
            if (map[row][col] != EMPTY) {
                is_empty = false;
            } 
            col++;
        }
        row++;
    }
    if (is_empty) {
        print_map(map, laser_y);
        printf("Game Won!\n");
        // Note that the game is over.
        *game_over = true;
    }
}

// This function shifts all objects in the map left or prints
// game over if objects are in the leftmost column.
// This can change the map array and the game_over boolean.

void shift_everything_left(int map[SIZE][SIZE], int laser_y, bool *game_over) {
    // Check if there are any stones in the leftmost column
    int row = 0;
    while (row < SIZE && !(*game_over)) {
        // If there is a stone in column 0, note that the game is over.
        if (map[row][0] == STONE) {
            *game_over = true;
        }
        row++;
    }

    // If game is over, print Game Lost!. 
    // Otherwise, move everything left.
    if (*game_over) {
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
}

// Rotates the map clockwise, counter clockwise, or not at all depending
// on the argument and whether a rotate has been used.
// This can change the map array, game_over boolean and is_rotated boolean.

void rotate_map(int map[SIZE][SIZE], bool *game_over, bool *is_rotated) {
    if (!(*is_rotated)) {
        // Note that the rotate has been used (even if it is not valid)
        *is_rotated = true;

        // Scan the next number (the direction) into a variable
        int direction;
        scanf("%d", &direction);

        // Only continue if direction is 1 or 2
        if (direction == CLOCKWISE || direction == COUNTER_CLOCKWISE) {
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
                        if (direction == CLOCKWISE) {
                            new_col = -(curr_row - (SIZE - 1));
                        } else {
                            new_col = curr_row;
                        }

                        // Convert the curr_col to the row value of new_map
                        int new_row = curr_col;

                        new_map[new_row][new_col] = map[curr_row][curr_col];
                    }
                }
            }
            
            // Change the map to be the new_map
            for (int row = 0; row < SIZE; row++) {
                for (int col = 0; col < SIZE; col++) {
                    map[row][col] = new_map[row][col];
                }
            }
        } 
        
    // Need to clear the scanf buffer if the rotate command is called but
    // rotate has been used (otherwise causes problems in the next loop).
    } else {
        int c = 0;
        // Keep getting integers until a newline. This clears the buffer.
        // TODO check style guide about this.
        while (c != '\n') {
            c = getchar();
        }
    }
}
