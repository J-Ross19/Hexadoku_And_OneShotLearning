#include <stdio.h>
#include <stdlib.h>
#include "third.h"

int main(int argc, char** argv) {
	// Invalid args
	if (argc != 3) {
		return 0;
	}
	
	FILE* fp = fopen(argv[1], "r");
	if (fp == NULL) {
		// File doesn't exist or cannot be accessed
		return 0;
	}
	
	int numAttributes = 0;
	int numExamples = 0;
	if (fscanf(fp, "%d\n", &numAttributes) == EOF || fscanf(fp, "%d\n", &numExamples) == EOF) {
		// Invalid file format
		return 0;
	}
	
	int rowsX = numExamples;
	int colsX = numAttributes - 1;
	
	// Create matrix X with N rows and K columns
	int** X = allocateMatrix(rowsX, colsX);
	
	// Create column vector Y
	int* Y = malloc(numExamples * sizeof(int));
	
	// Read in value for X and Y
	setMatrixVector(fp, X, Y, numExamples, numAttributes);
	
	// Calculate W = (X^T*X)^-18X^T*Y
	int** transposeX = matrixTranspose(X, rowsX, colsX);
	int** productXT_X = multiplyMatrix(transposeX, X, colsX, rowsX, rowsX, colsX);
	int** inverseXT_X = calculateInverse(productXT_X, colsX);
	freeMatrix(productXT_X, colsX); // Done w X^T*X
	int** pseudoInverse = multiplyMatrix(inverseXT_X, transposeX, colsX, colsX, colsX, rowsX);
	freeMatrix(transposeX, colsX);
	freeMatrix(inverseXT_X, colsX);
	
	
	freeMatrix(X, numExamples);
	return 0;
}
 
/*
** Function to allocate memory for a nxm matrix
** Inputs: Number of rows, Number of Columns
** Outputs: Allocated matrix
*/
int** allocateMatrix(int rows, int cols) {
	int** matrix = malloc(rows * sizeof(int*));
	for (int i = 0; i < rows; i++) {
		matrix[i] = malloc(cols * sizeof(int));
	}
	return matrix;
}
 
/*
** Function to set matrix X and vector Y using the training data
** Inputs: Pointer to a file, row x col matrix, column vector with size of rows, 
** number of rows, number of cols
*/
void setMatrixVector(FILE* fp, int** matrix, int* vector, int rows, int cols) {
	for (int i = 0; i < rows; i++) {
		int  num = 0;
		for (int j = 0; j < cols - 1; j++) {
			if (fscanf(fp, "%d\t", &num) == EOF) {
				exit(0);
			}
			matrix[i][j] = num;
		}
		if (fscanf(fp, "%d\n", &num) == EOF) {
			exit(0);
		}
		vector[i] = num;
	}
}

/*
** Function to  get the transpose of a matrix
** Inputs: Matrix, number of rows, number of cols
** Outputs: The transposed matrix of the input
*/
int** matrixTranspose(int** matrix, int rows, int cols) {
	int** transpose = allocateMatrix(cols, rows);
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			transpose[j][i] = matrix[i][j];
		}
	}
	return transpose;
}

/*
** Function to multiply two matrices together
*/
int** multiplyMatrix(int** m1, int** m2, int row1, int col1, int row2, int col2) {
	
	if (col1 != row2) {
		// Should never happen in this case
		return NULL;
	}
	
	int** product = allocateMatrix(row1, col2);
	
	for (int i = 0; i < row1; i++) {
		for (int j = 0; j < col2; j++) {
			int sum = 0;
			for (int k = 0; k < col1; k++) {
				sum += m1[i][k] * m2[k][j];
			}
			product[i][j] = sum;
		}
	}
	
	return product;
}

/*
** Function to calculate inverse of a matrix
*/
int** calculateInverse(int** matrix, int n) {
	int** inverse = allocateMatrix(n, n);
	// Make inverse the identity matrix
	for (int i= 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			inverse[i][j] = (i == j) ? 1 : 0;
		}
	}
	
	// Perform Gauss-Jordan Elimination (Without row swaps???)
	for (int i = 0; i < n; i++) {
		// Make the diagonal entry 1
		if (matrix[i][i] == 0) {
			printf("I WAS TOLD THIS WOULDNT HAPPEN :("); 
			exit(0);
			// It would've been easy to code row swaps here (just saying)
		} else if (matrix[i][i] != 1) {
			// matrix[i] / matrix[i][i]
			for (int k = 0; k < n; k++) {
				matrix[i][k] /= matrix[i][i];
				inverse[i][k] /= matrix[i][i];
			}
		}
		for (int j = 0; j < n; j++) {
			// Make all entries in current column 0 (except the diagonal)
			if (j == i || j == 0) {
				continue;
			} else {
				// Welcome to O(n^3)
				//row[j] - row[j][i] * row[i]
				for (int k = 0; k < n; k++) {
					matrix[j][k] -= matrix[j][i]*matrix[i][k];
					inverse[j][k] -= matrix[j][i]*matrix[i][k];
				}
			}
		}
	}
	return inverse;
}

/*
** Function to free memory of a matrix
*/
void freeMatrix(int** matrix, int rows) {
	for (int i = 0; i < rows; i++) {
		free(matrix[i]);
	}
	free(matrix);
}
