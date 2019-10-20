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
	
	// Create 3 matrices to keep track of occurrences of each value in each row (Ex. rowCheck[0][3] represents # of 3s in thee first row)
	int rowCheck[16][16];
	int colCheck[16][16];
	int blockCheck[16][16];

	initZero(rowCheck);
	initZero(colCheck);
	initZero(blockCheck);
	
	// Read in input grid and add relevant values
	if (!createGrid(grid, fp, rowCheck, colCheck, blockCheck) || 
	!solveGrid(grid, rowCheck, colCheck, blockCheck)) { 
		// Grid did not contain proper inputs or could not be solved
		printf("no-solution");
		return 0;
	}
	
	printGrid(grid);
	fclose(fp);
	return 0;
}

/* 
** Function to read in input data and convert hexadecimal values 0-F to integer valuees 0-15
** for easier arithmetic 
** INPUTS: 16x16 hexadoku grid, Pointer to file
** OUTPUT: Success value (1) or failure value (0)
*/
int createGrid(int grid[16][16], FILE* fp, int rowCheck[16][16], int colCheck[16][16], int blockCheck[16][16]) {
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			char temp;
			// Scan an integer followeed by a tab
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
				return 0;
			}
			
			grid[i][j] = val;
			
			// Check validity of initial grid
			if (val > -1 && val < 16) {
				if (++rowCheck[i][val] > 1 || ++colCheck[j][val] > 1 || 
				++blockCheck[(i / 4) * 4 + (j / 4)][val] > 1) {
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
	int eCol = -1;
	
	// oRow and oCol represent the row and column of the first empty element (after filling in)
	int oRow = 0;
	int oCol = -1;
	int passed = 0;
	
	while (findEmpty(grid, &eRow, &eCol)) {
		
		int num = 0;
		int count = 0;
		for (int i  = 0; i < 16; i++) {
			// Check validity
			if (rowCheck[eRow][i] + 1 > 1 || colCheck[eCol][i] + 1 > 1 || 
			blockCheck[(eRow / 4) * 4 + (eCol / 4)][i] + 1 > 1) {
				// If it fails any test, check next value
				continue;
			} else {
				// Otherwise, increment count
				count++;
				num = i;
			}
		}

		if (count > 1) {
			// Decide if we have checked all empty values without success
			if (passed && eRow == oRow && eCol == oCol) {
				return 0;
			}
			passed = 1;
		} else if (count < 1) {
			// If less than one possibility, no solvable grid
			return 0;
		} else {
			// Only possibility: Insert number into grid
			grid[eRow][eCol] = num;
			
			//Increment occurrence in respecting checks
			rowCheck[eRow][num]++;
			colCheck[eCol][num]++;
			blockCheck[(eRow / 4) * 4 + (eCol / 4)][num]++;
			
			// Create new original check and set passed to 0, so we can tell if there has been a full loop
			findEmpty(grid, &oRow, &oCol);
			passed = 0;
		}
	}
	
	return 1;
}

/*
** Function to find the first empty row/column of a grid
** Inputs: 16x16 hexadoku grid, pointers to two integer vaiables
** Output: 1 - if value is found, 0 - if no more empty spots are left
*/
int findEmpty(int grid[16][16], int* eRow, int* eCol) {
	// Starting at last empty row and column, find the next empty number
	for (int i = *eRow; i < 16; i++) {
		int j = (i == *eRow) ? *eCol + 1 : 0;
		while (j < 16) {
			if (grid[i][j] == -1) {
				*eRow = i;
				*eCol = j;
				return 1;
			}
			j++;
		}
	}
	
	// Gone through rest of matrix. Start over
	for (int i = 0; i < *eRow + 1; i++) {
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

/*
** Function to print entire grid
** Input: 16x16 hexadoku grid
*/
void printGrid(int grid[16][16]) {
	// Starting at last empty row, find the next empty number
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			int num = grid[i][j];
			char converted = 'G';
			// Convert int to hexadecimal char
			if (num < 10) {
				// Case 1: Number 0-9
				converted = num + '0';
			} else {
				//Case 2: Number 10-15
				converted = num - 10 + 'A';
			}
			printf("%c", converted);
			if (j != 15) {
				printf("\t");
			}
		}
		printf("\n");
	}
}


