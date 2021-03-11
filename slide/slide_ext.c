// Slide
// slide.c
//
// This program was written by Evan Williams (z5368211)
// on 11/03/2021
//
// Stage 3

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
void fire_laser(int map[SIZE][SIZE], int laser_y, bool *game_over, 
                bool *inverted, bool *day_win, bool *night_win);
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

int main (void) {
    // This line creates our 2D array called "map" and sets all
    // of the blocks in the map to EMPTY. The night mode map is also created.
    int map[SIZE][SIZE] = {EMPTY};
    int night_map[SIZE][SIZE] = {EMPTY};

    // We will use a pointer which will reference either the night_map or
    // the day map, starting with the day map.
    int (*curr_map)[SIZE][SIZE] = &map;

    // This line creates our laser_y variable. The laser starts in the
    // middle of the map, at position 7.
    int laser_y = SIZE / 2;

    int blocks;
    printf("How many blocks? ");
    scanf("%d", &blocks);
    
    // Create variables to track the state of the game.
    bool is_rotated = false;
    bool game_over = true;
    bool inverted = false;
    bool night_win = false;
    bool day_win = false; 

    printf("Enter blocks:\n");
    // This is a loop that allows each block to be scanned.
    for (int i = 0; i < blocks; i++) {
        int row, col, digit;

        scanf("%d %d %d", &row, &col, &digit);

        // Only change values if the row and column numbers are valid.
        if (indices_are_valid(row, col)) {
            
            // If the digit is negative, add its absolute value to the
            // night mode map.
            if (digit <= 0) {
                night_map[row][col] = abs(digit);                
            
            // Otherwise, add it to the normal map and note that there
            // is a normal block so the game can begin.
            } else {
                map[row][col] = digit;
                game_over = false;
            }
        }
    }


    print_map(*curr_map, laser_y);

    // Read commands until EOF or the game is over
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
            fire_laser(*curr_map, laser_y, &game_over, &inverted,
                       &day_win, &night_win);

        // Shift Everything Left
        } else if (instruction == SHIFT_EVERYTHING_LEFT) {
            // Flip the inverted boolean
            inverted = !inverted;
            
            // Reference the correct map with curr_map
            if (inverted) {
                curr_map = &night_map;
            } else {
                curr_map = &map;
            }

            // Now, shift the current map
            shift_everything_left(*curr_map, laser_y, &game_over);
            

        // Rotate the map
        } else if (instruction == ROTATE_MAP) {
            rotate_map(*curr_map, &game_over, &is_rotated);
        }

        // If the game is not finished, we want to print the map.
        // If the game is finished, the map is printed in the instruction's 
        // function, so it should not be printed again.
        if (!game_over) {
            print_map(*curr_map, laser_y);
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

// This function checks whether a row and column index are inside the map

bool indices_are_valid(int row, int col) {
    return (0 <= row && row < SIZE && 0 <= col && col < SIZE);
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

void fire_laser(int map[SIZE][SIZE], int laser_y, bool *game_over, 
                bool *inverted, bool *day_win, bool *night_win) {
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

    //Check if the game is won by looping over all blocks
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
    
    // Now we need to check if the current map is empty (and the other map)
    // If both are empty, game is over. Otherwise, set the respective
    // variables and move on.
    if (is_empty) {
        if (inverted) {
            *night_win = true;
            if (*day_win) {
                *game_over = true;
            }
        } else {
            *day_win = true;
            if (*night_win) {
                *game_over = true;
            }
        }
    }

    if (*game_over) {
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
    if (!(*is_rotated)) {
        // Note that the rotate has been used (even if it is not valid)
        *is_rotated = true;

        // Scan the next number (the direction) into a variable
        int direction;
        scanf("%d", &direction);

        // Only continue if direction is clockwise or counterclockwise
        if (direction == CLOCKWISE || direction == COUNTER_CLOCKWISE) {
            // Call a function to complete the logic to rotate the map
            rotate_in_direction(map, direction); 
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

void shift_left(int map[SIZE][SIZE]) {
    for (int row = 0; row < SIZE; row++) {
        for (int col = 1; col < SIZE; col++) {
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
