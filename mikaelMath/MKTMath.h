#ifndef matematica_h
#define matematica_h

#include <complex.h>
#include <math.h>

void multiplyMatrices(int row1, int col1,double complex mat1[row1][col1], int row2, int col2,double complex mat2[row2][col2], double complex result[row1][col2]);
void showMatrix(int row, int col, double complex mat[row][col]);
void printAdmittanceMatrix(int matrixRange, double complex Y[matrixRange][matrixRange]);
void getIdentityMatrix(int length, double complex mat[length][length]);
double complex get2x2Determinant(double complex mat[2][2]);
void cutMatrix(int row, int col, double complex mat[row][col], int rowCut, int colCut, double complex resultantMatrix[row - 1][col - 1]);
void cutColumn(int row, int col, double complex mat[row][col], int colCut, double complex resultantMatrix[row][col - 1]);
void cutRow(int row, int col, double complex mat[row][col], int rowCut, double complex resultantMatrix[row - 1][col]);
double complex getDeterminant(int size, double complex mat[size][size]);
void getCofactorMatrix(int size, double complex mat[size][size], double complex result[size][size]);
void getTranspose(int size, double complex mat[size][size], double complex result[size][size]);
void getInverse_matrix(int size, double complex mat[size][size], double complex inv[size][size]);

#include "MKTMath.c"
#endif