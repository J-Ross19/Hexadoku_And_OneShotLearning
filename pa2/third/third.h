#ifndef ONE_SHOT_LEARNING_H
#define ONE_SHOT_LEARNING_H

int** allocateMatrix(int, int);
void setMatrixVector(FILE*, int**, int*, int, int);
int** matrixTranspose(int**, int, int);
int** multiplyMatrix(int**, int**, int, int, int, int);
int** calculateInverse(int**, int);
void freeMatrix(int**, int);

#endif
