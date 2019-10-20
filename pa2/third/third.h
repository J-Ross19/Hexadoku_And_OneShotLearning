#ifndef ONE_SHOT_LEARNING_H
#define ONE_SHOT_LEARNING_H

double** allocateMatrix(int, int);
void setMatrixVector(FILE*, double**, double**, int, int);
double** matrixTranspose(double**, int, int);
double** multiplyMatrix(double**, double**, int, int, int, int);
double** calculateInverse(double**, int);
void freeMatrix(double**, int);

#endif
