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
    while (scanf("%d", &instruction) != EOF) {

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

        }

        print_map(map, laser_y);
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
