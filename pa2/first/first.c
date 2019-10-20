#include <stdio.h>
#include <stdlib.h>
#include "first.h"

int main(int argc, char** argv) {
	// Invalid arguments
	if (argc != 2) {
		return 0;
	}
	
	FILE* fp = fopen(argv[1], "r");
	
	// File does not exist or can't be reached
	if (fp == NULL) {
		return 0;
	}
	
	// Create 16x16 hexadoku grid
	int grid[16][16];
	
	// Read in input grid and add relevant values
	if (!createGrid(grid, fp)) { 
		// Grid did not contain proper inputs
		return 0;
	}
	
	// Create 3 matrices to keep track of occurrences of each value in each row (Ex. rowCheck[0][3] represents # of 3s in thee first row)
	int rowCheck[16][16];
	int colCheck[16][16];
	int blockCheck[16][16];

	initZero(rowCheck);
	initZero(colCheck);
	initZero(blockCheck);
	
	// Check initial grid and fill in check arrays and solve grid
	if (!checkInitialGrid(grid, rowCheck, colCheck, blockCheck) || 
	!solveGrid(grid, rowCheck, colCheck, blockCheck)) {
		// Invalid grid
		printf("no-solution");
		return 0;
	}
	
	
	
	return 0;
}

/* 
** Function to read in input data and convert hexadecimal values 0-F to integer valuees 0-15
** for easier arithmetic 
** INPUTS: 16x16 hexadoku grid, Pointer to file
** OUTPUT: Success value (1) or failure value (0)
*/
int createGrid(int grid[16][16], FILE* fp) {
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			char temp;
			// Scan an integer followeed by either a tab or newline
			if (fscanf(fp, "%c\t", &temp) == EOF) {
				return 0;
			}
			
			// Convert current character to int
			int val;
			if (temp >= '0' && temp <= '9') {
				// Case 1: Range from ['0','9']
				val = temp - '0';
			} else if (temp >= 'A' && temp <= 'F') {
				// Case 2: Range from ['A','F']
				val = temp - 'A' + 10;
			} else if (temp == '-') {
				// Case 3: '-' (Value to be filled in)
				val = -1;
			} else {
				// Case 4: Invalid input
				return 1;
			}
			
			grid[i][j] = val;
		}
	}
	return 1;		
}

/*
** Function to check whether the board is valid
** INPUT: 16x16 hexadoku grid, Three 16x16 2d arrays initialized to 0
** OUTPUT: Valid (1) or invalid (0)
*/
int checkInitialGrid(int grid[16][16], int rowCheck[16][16], int colChec[16][16], int blockCheck[16][16]) {
	// Initialize array to 0 (will represent each number 0-15)

	
	// Check if rows are valid 
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			if (grid[i][j] == -1) {
				// Grid number should be filled in later
				continue;
			}
			
			// Increment occurrences of current number for row i by 1
			rowCheck[i][grid[i][j]]++;
			if (rowCheck[i][grid[i][j]] > 1) {
				return 0;
			}
		}
	}
	
	// Check if columns are valid
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			if (grid[j][i] == -1) {
				// Grid number should be filled in later
				continue;
			}
			
			// Increment occurrences of current number for column i by 1
			rowCheck[i][grid[j][i]]++;
			if (rowCheck[i][grid[j][i]] > 1) {
				return 0;
			}
		}
	}
	
	// Check if the 4x4 blocks are valid
	for (int i = 0; i < 16; i++) {
		// i represents block number
		// Rows: Blocks 0-3 should start at 0, 4-7 at 4, 8-11 at 8, and 12-15 at 12.
		// Cols: Blocks should start at 0, 4, 8, or 12 (starting with zero and moving to the next each time)
		int startingRow = (i / 4) * 4;
		int startingCol = (i % 4) * 4;
		
		
		for (int x = startingRow; x < startingRow + 4; x++) {
			for (int y = startingCol; y < startingCol + 4; y++) {
				if (grid[x][y] == -1) {
					// Grid number should be filled in later
					continue;
				}
			
				// Increment occurrences of current number for row i by 1
				blockCheck[i][grid[x][y]]++;
				if (blockCheck[i][grid[x][y]] > 1) {
					return 0;
				}	
				
			}
		}
	
	}
	
	return 1;
}

/*
** Function to initilize an array to all 0s
** INPUT: 16 x 16 2-d array
*/
void initZero(int grid[16][16]) {
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			grid[i][j] = 0;
		}
	}
}

/*
** Function to fill in remainder of grid
** Input: 16x16 hexadoku grid
** Output: Valid (1) or invalid (0)
*/
int solveGrid(int grid[16][16], int rowCheck[16][16], int colCheck[16][16], int blockCheck[16][16]) {
	
	int eRow = 0;
	int eCol = 0;
	while (findEmpty(grid, &eRow, &eCol)) {
		int num = 0;
		int count = 0;
		for (int i  = 0; i < 16; i++) {
			// Check validity
			if (rowCheck[eRow][i]++ > 1) {
				rowCheck[eRow][i]--;
				continue;
			} else if (colCheck[eCol][i]++ > 1) {
				rowCheck[eRow][i]--;
				colCheck[eCol][i]--;
				continue;
			} else if (blockCheck[(eRow / 4) * 4 + (eCol / 4)][i]++ > 1) {
				rowCheck[eRow][i]--;
				colCheck[eCol][i]--;
				blockCheck[(eRow / 4) * 4 + (eCol / 4)][i]--;
				continue;
			} else {
				count++;
				num = i;
			}
		}
		
		// If zero or more than one possibility, no solvable grid
		if (count != 1) {
			return 0;
		}
		
		grid[eRow][eCol] = num;
	}
	
	return 1;
}

/*
** Function to find the first empty row/column of a grid
** Inputs: 16x16 hexadoku grid, pointers to two integer vaiables
** Output: 1 - if value is found, 0 - if no more empty spots are left
*/
int findEmpty(int grid[16][16], int* eRow, int* eCol) {
	// Starting at last empty row, find the next empty number
	for (int i = *eRow; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			if (grid[i][j] == -1) {
				*eRow = i;
				*eCol = j;
				return 1;
			}
		}
	}
	return 0;
}


