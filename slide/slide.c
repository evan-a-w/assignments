// Slide
// slide.c
//
// This program was written by Evan Williams (z5368211)
// from 11/03/2021 - 25/03/2021
//
// Stage 3

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define SIZE 15
#define EMPTY 0
#define STONE 1

#define SUCCESS 1

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

// These functions are used for the fundamental operations related to editing
// and printing the map as well as running the game loop.
void print_map(int map[SIZE][SIZE], int laser_y);
void read_blocks(int map[SIZE][SIZE]);
void instruction_loop(int map[SIZE][SIZE], int laser_y, 
                      bool *game_over, bool *is_rotated);

// These functions correspond to different commands. Move laser is not included
// because it is very short.
void fire_laser(int map[SIZE][SIZE], int laser_y, bool *game_over);
void shift_everything_left(int map[SIZE][SIZE], int laser_y, bool *game_over);
void rotate_map(int map[SIZE][SIZE], bool *game_over, bool *is_rotated);

// These functions are all helper functions to split up code.
double distance(int x1, int y1, int x2, int y2);
void explode_blocks(int map[SIZE][SIZE], int laser_y, int col,
                    int explosion_radius);
void rotate_in_direction(int map[SIZE][SIZE], int direction);
bool indices_are_valid(int row, int col);
void shift_left(int map[SIZE][SIZE]);
int rotated_row(int map[SIZE][SIZE], int direction, int curr_col);
int rotated_column(int map[SIZE][SIZE], int direction, int curr_row);
bool block_is_valid(int block);
bool map_is_empty(int map[SIZE][SIZE]);

int main (void) {
    // This line creates our 2D array called "map" and sets all
    // of the blocks in the map to EMPTY.
    int map[SIZE][SIZE] = {EMPTY};

    // This line creates our laser_y variable. The laser starts in the
    // middle of the map, at position 7.
    int laser_y = SIZE / 2;
    
    // Create variables to track the state of the game.
    bool is_rotated = false;
    bool game_over = false;

    // Read and print the map
    read_blocks(map);
    print_map(map, laser_y);

    // The instruction loop reads in user input until EOF and calls instruction
    // commands.
    instruction_loop(map, laser_y, &game_over, &is_rotated); 

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

// This function gets user input and sets the map values for each given block.
// This also modifies the game state bools.
void read_blocks(int map[SIZE][SIZE]) {
    int blocks;
    printf("How many blocks? ");
    scanf("%d", &blocks);

    printf("Enter blocks:\n");

    // This is a loop that allows each block to be scanned.
    for (int i = 0; i < blocks; i++) {
        int row, col, digit;

        scanf("%d %d %d", &row, &col, &digit);

        // Only change values if the row and column numbers are valid and the
        // block is valid.
        if (indices_are_valid(row, col) && block_is_valid(digit)) {
            map[row][col] = digit;
        }
    }
}

// This function reads in instructions and calls their respective functions
// before printing the map. This modifies the map array and the game state bools
void instruction_loop(int map[SIZE][SIZE], int laser_y, 
                      bool *game_over, bool *is_rotated) {
    // Read commands until an error or the game is over
    int instruction;

    while (*game_over == false && (scanf("%d", &instruction) == SUCCESS)) {
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
            fire_laser(map, laser_y, game_over);

        // Shift Everything Left
        } else if (instruction == SHIFT_EVERYTHING_LEFT) {
            shift_everything_left(map, laser_y, game_over);
            
        // Rotate the map
        } else if (instruction == ROTATE_MAP) {
            rotate_map(map, game_over, is_rotated);
        }

        // If the game is not finished, we want to print the map.
        // If the game is finished, the map is printed in the instruction's 
        // function, so it should not be printed again.
        if (*game_over == false) {
            print_map(map, laser_y);
        }
    }

}

// This function checks whether a row and column index are inside the map
bool indices_are_valid(int row, int col) {
    return (0 <= row && row < SIZE && 0 <= col && col < SIZE);
}

// This function returns whether a given block is valid.
bool block_is_valid(int block) {
    return (block == STONE || (TNT_MIN <= block && block <= TNT_MAX));
}

// This function returns the distance between two points.
double distance(int x1, int y1, int x2, int y2) {
    return sqrt(1.0 * ((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)));
}

// This function loops over the map and deletes any blocks that are within a
// given explosion radius (calculated by the distance function)
void explode_blocks(int map[SIZE][SIZE], int laser_y, int col,
                    int explosion_radius) {
    for (int curr_row = 0; curr_row < SIZE; curr_row++) {
        for (int curr_col = 0; curr_col < SIZE; curr_col++) {
            if (distance(laser_y, col, curr_row, curr_col) < explosion_radius) {
                map[curr_row][curr_col] = EMPTY;
            }
        }
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
        int curr_block = map[laser_y][col];
        // Change stones to empty and increment destroyed blocks counter
        if (curr_block == STONE) {
            map[laser_y][col] = EMPTY;
            destroyed_blocks++;
        
        // TNT Blocks
        } else if (TNT_MIN <= curr_block && curr_block <= TNT_MAX) {
            int explosion_radius = curr_block;
            
            explode_blocks(map, laser_y, col, explosion_radius);

            // Laser is used up by TNT so we set destroyed_blocks to LASER_MAX
            destroyed_blocks = LASER_MAX;
        }
        col++;
    }

    bool is_empty = map_is_empty(map);

    // If map is empty, game is won so we print the map and "Game Won!".
    if (is_empty) {
        print_map(map, laser_y);
        printf("Game Won!\n");
        // Note that the game is over.
        *game_over = true;
    }
}

// This function checks whether the map is empty and returns a bool.
bool map_is_empty(int map[SIZE][SIZE]) {
    //Check if the game is won by looping over all blocks
    int row = 0;
    bool is_empty = true;
    while (row < SIZE && is_empty) {
        int col = 0;
        while (col < SIZE && is_empty) {
            // If index of map is not empty, the game is not won.
            if (map[row][col] != EMPTY) {
                is_empty = false;
            } 
            col++;
        }
        row++;
    }
    return is_empty;
}

// This function shifts all objects in the map left or prints
// game over if objects are in the leftmost column.
// This can change the map array and the game_over boolean.
void shift_everything_left(int map[SIZE][SIZE], int laser_y, bool *game_over) {
    // Check if there are any stones in the leftmost column
    int row = 0;
    while (row < SIZE && *game_over == false) {
        // If there is a block in column 0, note that the game is over.
        if (map[row][0] != EMPTY) {
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
        shift_left(map);
    }
}

// Rotates the map clockwise, counter clockwise, or not at all depending
// on the argument and whether a rotate has been used.
// This can change the map array, game_over boolean and is_rotated boolean.
void rotate_map(int map[SIZE][SIZE], bool *game_over, bool *is_rotated) {
    // Scan the next number (the direction) into a variable
    int direction;
    scanf("%d", &direction);

    if (*is_rotated == false) {
        // Note that the rotate has been used (even if it is not valid)
        *is_rotated = true;

        // Only continue if direction is clockwise or counterclockwise
        if (direction == CLOCKWISE || direction == COUNTER_CLOCKWISE) {
            // Call a function to complete the logic to rotate the map
            rotate_in_direction(map, direction); 
        } 
    }
}

// This function implements the logic to rotate the map either clockwise or
// counterclockwise. This changes the map array.
void rotate_in_direction(int map[SIZE][SIZE], int direction) {

    int new_map[SIZE][SIZE] = {EMPTY};    

    // Loop over the rows of the map starting from the bottom
    // and then change the new_map value based on the map value
    for (int curr_row = 0; curr_row < SIZE; curr_row++) {
        for (int curr_col = 0; curr_col < SIZE; curr_col++) {
            // If non-empty, change the value in new_map
            if (map[curr_row][curr_col] != EMPTY) {
                // Convert the current values to the new values.
                int new_col = rotated_column(map, direction, curr_row);
                int new_row = rotated_row(map, direction, curr_col);
                
                // Now we replace the value on the new map at the new indices
                // with the value on the old map.
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

// This function moves every column one place left in the map array, discarding
// the first column.
void shift_left(int map[SIZE][SIZE]) {
    for (int row = 0; row < SIZE; row++) {
        for (int col = 1; col < SIZE; col++) {
            // If a position is empty, move its value to the column to its left.
            if (map[row][col] != EMPTY) {
                map[row][col-1] = map[row][col];
                map[row][col] = EMPTY;
            }
        }
    }
}

// This function returns the column value after rotation
int rotated_column(int map[SIZE][SIZE], int direction, int curr_row) {
    // To do this counterclockwise, we simply take the current row value.
    // Otherwise, we must subtract the SIZE by the current row plus one.
    if (direction == CLOCKWISE) {
        return SIZE - curr_row - 1;
    } else {
        return curr_row;
    }
}

// This function returns the row value after rotation
int rotated_row(int map[SIZE][SIZE], int direction, int curr_col) {
    // This functions similarly to the rotated_column function, but we switch
    // columns for rows and flip the methods used for 
    // clockwise/counterclockwise.
    if (direction == CLOCKWISE) {
        return curr_col;
    } else {
        return SIZE - curr_col - 1;
    }
}
