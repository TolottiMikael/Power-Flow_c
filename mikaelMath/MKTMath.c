#include "MKTMath.h"

void multiplyMatrices(int row1, int col1,double complex mat1[row1][col1], int row2, int col2,double complex mat2[row2][col2], double complex result[row1][col2])
{
    int x, y, z;
    // printf("matriz 1: \n");
    // showMatrix(row1, col1, mat1);
    // printf("matriz 2: \n");
    // showMatrix(row2, col2, mat2);
    // Performs matrix multiplication
    // printf("o final terá %dX%d\n", row1, col2);
    for(x = 0; x < row1; x++) {
		for(y = 0; y < col2; y++) {
			
			result[x][y] = 0;
			for(z = 0; z < row2; z++) {
                // printf("result[%d][%d] += mat1[%d][%d] * mat2[%d][%d]\n", x, y, x, z, z, y);
				result[x][y]  +=  mat1[x][z] * mat2[z][y];
			}
		}
	}
    // printf("matriz final: \n");
    // showMatrix(row1, col2, result);
}

void showMatrix(int row, int col, double complex mat[row][col])
{
    int x, y;

    printf("Resultant Matrix: (%dx%d)\n", row, col);
    for (x = 0; x < row; x++)
    {
        for (y = 0; y < col; y++)
        {
            printf("%lf + i%lf\t", creal(mat[x][y]), cimag(mat[x][y]));
        }
        printf("\n");
    }
}



void printAdmittanceMatrix(int matrixRange, double complex Y[matrixRange][matrixRange])
{
    int i = 0;
    int j = 0;
    for (i = 0; i < matrixRange; i++)
    {
        for (j = 0; j < matrixRange; j++)
        {
            printf("%.2lf + %.2lfi\t", creal(Y[i][j]), cimag(Y[i][j]));
        }
        printf("\n");
    }
}

void getIdentityMatrix(int length, double complex mat[length][length])
{
    int i = 0;
    int j = 0;

    for (i = 0; i < length; i++)
    {
        for (j = 0; j < length; j++)
        {
            if (i == j)
            {
                mat[i][j] = 1;
            }
            else
            {
                mat[i][j] = 0;
            }
        }
    }
}

void cutMatrix(int row, int col, double complex mat[row][col], int rowCut, int colCut, double complex resultantMatrix[row - 1][col - 1])
{

    int i = 0;
    int j = 0;
    int rowcut_count = 0;
    int colcut_count = 0;

    double complex **result;

    for (i = 0; i < row - 1; i++)
    {
        rowcut_count = 0;
        if (i >= rowCut)
        {
            rowcut_count = 1;
        }
        colcut_count = 0;
        for (j = 0; j < col - 1; j++)
        {
            if (j >= colCut)
            {
                colcut_count = 1;
            }
            resultantMatrix[i][j] = mat[i + rowcut_count][j + colcut_count];
        }
    }
}

void cutColumn(int row, int col, double complex mat[row][col], int colCut, double complex resultantMatrix[row][col - 1])
{
    int i = 0;
    int j = 0;
    int colcut_count = 0;

    double complex **result;

    for (i = 0; i < row; i++)
    {
    
        colcut_count = 0;
        for (j = 0; j < col - 1; j++)
        {
            if (j >= colCut)
            {
                colcut_count = 1;
            }
            resultantMatrix[i][j] = mat[i][j + colcut_count];
        }
    }
}
void cutRow(int row, int col, double complex mat[row][col], int rowCut, double complex resultantMatrix[row - 1][col])
{
    int i = 0;
    int j = 0;
    int rowcut_count = 0;

    double complex **result;

    for (i = 0; i < row - 1; i++)
    {
        rowcut_count = 0;
        if (i >= rowCut)
        {
            rowcut_count = 1;
        }
        for (j = 0; j < col; j++)
        {
            resultantMatrix[i][j] = mat[i + rowcut_count][j];
        }
    }
}

double complex get2x2Determinant(double complex mat[2][2])
{
    double complex result = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
    // printf("returned %lf + i%lf\n", creal(result), cimag(result));
    return result;
}

double complex getDeterminant(int size, double complex mat[size][size])
{
    double complex aux[size - 1][size - 1];
    double complex value = 0;
    if (size > 2)
    {
        // separar e obter o tamanho!
        int l = 0;
        for (l = 0; l < size; l++)
        {
            cutMatrix(size, size, mat, 0, l, aux);
            // printf("(%d:%d) => k=%d l=%d\n", size, size, 0, l);
            double complex det = getDeterminant(size - 1, aux);
            value += pow(-1, 0 + l) * det * mat[0][l];
            // printf("um response  :  %lf + i%lf\n", creal(value), cimag(value));
        }
        return value;
    }
    else
    {
        // showMatrix(2, 2, mat);
        return get2x2Determinant(mat);
    }
}

void getCofactorMatrix(int size, double complex mat[size][size], double complex result[size][size])
{
    double complex aux[size - 1][size - 1];
    int i = 0;
    int j = 0;
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            cutMatrix(size, size, mat, i, j, aux);
            result[i][j] = pow(-1, i + j) * getDeterminant(size - 1, aux);
        }
    }
}
void getTranspose(int size, double complex mat[size][size], double complex result[size][size])
{
    int i = 0;
    int j = 0;
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            result[i][j] = mat[j][i];
        }
    }
}

// devo resolver o bug da inversão
void getInverse_matrix(int size, double complex mat[size][size], double complex inv[size][size])
{
    double complex determinant = getDeterminant(size, mat);
    double complex cofactor[size][size];
    double complex adjoint[size][size];
    getCofactorMatrix(size, mat, cofactor);
    // printf("cofatores       \n");
    // showMatrix(size, size, cofactor);
    getTranspose(size, cofactor, adjoint);
    // printf("matriz adjunta\n");
    // showMatrix(size, size, adjoint);
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            inv[i][j] = adjoint[i][j] / determinant;
        }
    }
}