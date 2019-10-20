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
	
	// Part 1: Training
	int numAttributes = 0;
	int numExamples = 0;
	if (fscanf(fp, "%d\n", &numAttributes) == EOF || fscanf(fp, "%d\n", &numExamples) == EOF) {
		// Invalid file format
		return 0;
	}
	
	int rowsX = numExamples;
	int colsX = numAttributes - 1;
	
	// Create matrix X with N rows and K columns
	double** X = allocateMatrix(rowsX, colsX);
	
	// Create column vector Y
	double** Y = allocateMatrix(rowsX, 1);
	
	// Read in value for X and Y
	setMatrixVector(fp, X, Y, numExamples, numAttributes);
	
	// Calculate weights : W = (X^T*X)^-1*X^T*Y
	double** transposeX = matrixTranspose(X, rowsX, colsX);
	double** productXT_X = multiplyMatrix(transposeX, X, colsX, rowsX, rowsX, colsX);
	double** inverseXT_X = calculateInverse(productXT_X, colsX);
	freeMatrix(productXT_X, colsX); // Done w X^T*X
	double** pseudoInverse = multiplyMatrix(inverseXT_X, transposeX, colsX, colsX, colsX, rowsX);
	freeMatrix(transposeX, colsX); // Done w X^T
	freeMatrix(inverseXT_X, colsX); // Done w (X^T*X)^-1
	double** W = multiplyMatrix(pseudoInverse, Y, colsX, rowsX, rowsX, 1);
	freeMatrix(pseudoInverse, colsX); // Done w (X^T*X)^-1*X^T
	freeMatrix(Y, rowsX); // Done w Y
	freeMatrix(X, rowsX); // Done w X
	
	fclose(fp); // Close first file
	
	// Part 2: Prediction
	//MIGHT NOTT WORK
	fp = fopen(argv[2], "r");
	if (fp == NULL) {
		return 0; // No file, or file cannot be accessed
	}
	
	int numDataPoints = 0;
	if (fscanf(fp, "%d\n", &numDataPoints) == EOF) {
		// Invalid file format
		return 0;
	}
	
	X = allocateMatrix(numDataPoints, colsX);
	setMatrixVector(fp, X, NULL, numDataPoints, colsX);
	double** predictionCosts = multiplyMatrix(X, W, numDataPoints, colsX, colsX, 1);
	
	//Print Prediction Costs
	for (int i = 0; i < numDataPoints; i++) {
		printf("%0.0lf\n", predictionCosts[i][0]);
	}
	
	freeMatrix(X, numDataPoints);
	freeMatrix(W, colsX);
	fclose(fp);
	return 0;
}
 
/*
** Function to allocate memory for a nxm matrix
** Inputs: Number of rows, Number of Columns
** Outputs: Allocated matrix
*/
double** allocateMatrix(int rows, int cols) {
	double** matrix = malloc(rows * sizeof(double*));
	for (int i = 0; i < rows; i++) {
		matrix[i] = malloc(cols * sizeof(double));
	}
	return matrix;
}
 
/*
** Function to set matrix X and vector Y using the training data
** Inputs: Pointer to a file, row x col matrix, column vector with size of rows, 
** number of rows, number of cols
*/
void setMatrixVector(FILE* fp, double** matrix, double** vector, int rows, int cols) {
	for (int i = 0; i < rows; i++) {
		double  num = 0;
		for (int j = 0; j < cols - 1; j++) {
			if (fscanf(fp, "%lf\t", &num) == EOF) {
				exit(0);
			}
			matrix[i][j] = num;
		}
		if (fscanf(fp, "%lf\n", &num) == EOF) {
			exit(0); 
		}
		if (vector != NULL) {
			vector[i][0] = num;
		} else {
			matrix[i][cols - 1] = num;
		}
	}
}

/*
** Function to  get the transpose of a matrix
** Inputs: Matrix, number of rows, number of cols
** Outputs: The transposed matrix of the input
*/
double** matrixTranspose(double** matrix, int rows, int cols) {
	double** transpose = allocateMatrix(cols, rows);
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
double** multiplyMatrix(double** m1, double** m2, int row1, int col1, int row2, int col2) {
	
	if (col1 != row2) {
		// Should never happen in this case
		return NULL;
	}
	
	double** product = allocateMatrix(row1, col2);
	
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
double** calculateInverse(double** matrix, int n) {
	double** inverse = allocateMatrix(n, n);
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
void freeMatrix(double** matrix, int rows) {
	for (int i = 0; i < rows; i++) {
		free(matrix[i]);
	}
	free(matrix);
}
