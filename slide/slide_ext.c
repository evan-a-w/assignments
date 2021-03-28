// Slide extensions
// slide_ext.c
//
// This program was written by Evan Williams (z5368211)
// From 12/03/2021 - 26/03/2021
//
// This is the slide game but with night mode and mirrors.

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

#define MIRROR 2

void print_map(int map[SIZE][SIZE], int laser_y);
void read_blocks(int map[SIZE][SIZE], int night_map[SIZE][SIZE], 
                 bool *game_over, bool *day_win, bool *night_win);

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
void reflect_laser(int map[SIZE][SIZE], int laser_y, int laser_x,
                   int direction_x, int direction_y, int laser_charge);
void delete_in_direction(int map[SIZE][SIZE], int curr_row, int curr_col,
                         int direction_x, int direction_y, int laser_charge);
bool block_is_valid(int block);
bool check_empty(int curr_map[SIZE][SIZE]); 

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

    // Create variables to track the state of the game.
    bool is_rotated = false;
    bool game_over = true;
    bool inverted = false;
    bool night_win = true;
    bool day_win = true; 
    
    read_blocks(map, night_map, &game_over, &day_win, &night_win);

    // Check whether the night map is now empty because all values that were
    // added were later set to EMPTY
    night_win = check_empty(night_map);
    
    // If map is not empty (indicated by the game_over bool), print the map. 
    // Otherwise, print that the map was empty.
    if (!game_over) {
        print_map(*curr_map, laser_y);
    } else {
        printf("The map was initially completely empty.\n");
        printf("This case will never be tested.");
        printf(" You do not need to handle this case.\n");
    }

    // Read commands until an error in input or the game is over
    int instruction;
    while (!game_over && (scanf("%d", &instruction) == SUCCESS)) {

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
            
            // Reference the correct map with curr_map.
            // Don't switch if we are on day and the night map is already empty.
            // If the day map is empty and we are on the night map,
            // we still switch (reference does so).
            if (inverted && !night_win) {
                curr_map = &night_map;
            } else if (!inverted){
                curr_map = &map;

            // Otherwise, we aren't switching, so reflip the inverted boolean.
            } else {
                inverted = !inverted;
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

// This function gets user input and sets the map values for each given block.
// This modifies both map arrays as well as the game state bools.
void read_blocks(int map[SIZE][SIZE], int night_map[SIZE][SIZE], 
                 bool *game_over, bool *day_win, bool *night_win) {
    
    // Read the number of blocks into blocks
    int blocks;
    printf("How many blocks? ");
    scanf("%d", &blocks);

    printf("Enter blocks:\n");
    // This is a loop that allows each block to be scanned.
    for (int i = 0; i < blocks; i++) {
        int row, col, digit;

        scanf("%d %d %d", &row, &col, &digit);

        // Only change values if the row and column numbers are valid.
        if (indices_are_valid(row, col) && block_is_valid(digit)) {
            
            // If the digit is negative or 0, add its absolute value to the
            // night mode map.
            if (digit <= 0) {
                night_map[row][col] = abs(digit);                
                *night_win = false;
            // Otherwise, add it to the normal map and note that there
            // is a normal block so the game can begin.
            } else {
                map[row][col] = digit;
                *game_over = false;
                *day_win = false;
            }
        }
    }
}

// This function checks whether a row and column index are inside the map
bool indices_are_valid(int row, int col) {
    return (0 <= row && row < SIZE && 0 <= col && col < SIZE);
}

// This function returns whether a given block is valid.
bool block_is_valid(int block) {
    block = abs(block);
    return (block == STONE || (TNT_MIN <= block && block <= TNT_MAX)
            || block == MIRROR || block == EMPTY);
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

// This function checks whether the current map is empty and returns a boolean.
bool check_empty(int curr_map[SIZE][SIZE]) {
    int row = 0;
    bool is_empty = true;
    while (row < SIZE && is_empty) {
        int col = 0;
        while (col < SIZE && is_empty) {
            // If index of map is not empty, the game is not won.
            if (curr_map[row][col] != EMPTY) {
                is_empty = false;
            } 
            col++;
        }
        row++;
    }
    
    return is_empty;
}

// This function fires the laser and removes any objects in its path, and prints
// Game Won! if the map is empty.
// This can change the map array and the game_over boolean.
void fire_laser(int map[SIZE][SIZE], int laser_y, bool *game_over, 
                bool *inverted, bool *day_win, bool *night_win) {
    int col = 0; 
    int laser_charge = LASER_MAX;

    // Loop over the columns in the row where the laser is
    while (col < SIZE && laser_charge > 0) {
        int curr_block = map[laser_y][col];
        // Change stones to empty and increment destroyed blocks counter
        if (curr_block == STONE) {
            map[laser_y][col] = EMPTY;
            laser_charge--;
        
        // TNT Blocks
        } else if (TNT_MIN <= curr_block && curr_block <= TNT_MAX) {
            int explosion_radius = curr_block;
            
            explode_blocks(map, laser_y, col, explosion_radius);

            // Laser is used up by TNT so we set laser_charge to 0
            laser_charge = 0;
            
        // Mirror blocks
        } else if (curr_block == MIRROR) {
            // Create variables to indicate the positive direction of the
            // rotated lasers. This begins to point vertically because we are 
            // currently horizontal and we want to change direction.
            int direction_x = 0;
            int direction_y = 1;

            int laser_x = col;
            
            // Send to the mirror function
            reflect_laser(map, laser_y, laser_x, direction_x, 
                          direction_y, laser_charge);

            // The current laser is used up and the mirror is destroyed.
            laser_charge = 0;
            map[laser_y][col] = EMPTY;
        }
        col++;
    }

    //Check if the game is won by looping over all blocks
    bool is_empty = check_empty(map);
    
    // Now we need to check if the current map is empty (and the other map)
    // If both are empty, game is over. Otherwise, set the respective
    // variables and move on.
    if (is_empty) {
        if (*inverted) {
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
    // Scan the next number (the direction) into a variable
    int direction;
    scanf("%d", &direction);

    if (!(*is_rotated)) {
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

// This function implements the laser mirror functionality, creating a laser
// that destroyed blocks in two directions. This changes the map array.
void reflect_laser(int map[SIZE][SIZE], int laser_y, int laser_x,
                   int direction_x, int direction_y, int laser_charge) {

    // Divide the destroyed blocks by two since it has been reflected.
    laser_charge /= 2;    

    // First, deal with the positive direction
    
    delete_in_direction(map, laser_y + direction_y, laser_x + direction_x, 
                        direction_x, direction_y, laser_charge);

    // Then, the negative direction
    
    delete_in_direction(map, laser_y - direction_y, laser_x - direction_x, 
                        -direction_x, -direction_y, laser_charge);
}

// This function deletes blocks in a direction, reflecting the laser when it 
// strikes a mirror. This changes the map array.
void delete_in_direction(int map[SIZE][SIZE], int curr_row, int curr_col,
                         int direction_x, int direction_y, int laser_charge) {

    // We want to delete until the laser_charge is zero or we are out of bounds

    while (laser_charge > 0
           && indices_are_valid(curr_row, curr_col)) {
        if (map[curr_row][curr_col] == MIRROR) {
            // Call reflect laser again, flipping the directions
            reflect_laser(map, curr_row, curr_col, !direction_x, !direction_y,
                          laser_charge);

            // Set charge to 0 and remove the mirror block
            laser_charge = 0;
            map[curr_row][curr_col] = EMPTY;
        // If there is a TNT block, we want to delete it and set laser_charge
        // to 0.
        } else if (TNT_MIN <= map[curr_row][curr_col] && map[curr_row][curr_col]
                   <= TNT_MAX) {
            map[curr_row][curr_col] = EMPTY;
            laser_charge = 0;
        // Otherwise, if there is a block present, delete it
        } else if (map[curr_row][curr_col] != EMPTY) {
            map[curr_row][curr_col] = EMPTY;
            laser_charge--;
        }
        
        curr_row += direction_y;
        curr_col += direction_x;
    }

}
