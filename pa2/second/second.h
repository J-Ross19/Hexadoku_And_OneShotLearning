#ifndef HEXADOKU2_H
#define HEXADOKU2_H

int createGrid(int[16][16], FILE*);
int checkInitialGrid(int[16][16], int[16][16], int[16][16], int[16][16]);
void initZero(int[16][16]);
int solveGrid(int[16][16], int[16][16], int[16][16], int[16][16], int, int, int);
int findEmpty(int[16][16], int*, int*);
void printGrid(int[16][16]);

#endif
