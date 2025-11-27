#include "matrix_multiplicate.h"
#include <math.h>
#include <stdlib.h>

matrix_csr *multiply_csr_csc(matrix_csr *A_csr, matrix_csc *B_csc, int rows_a, int cols_a, int cols_b)
{
    int estimated_size = rows_a * cols_b; // предварительная оценка, будет увеличиваться при необходимости
    double *C_A = malloc(estimated_size * sizeof(double));
    int *C_JA = malloc(estimated_size * sizeof(int));
    int *C_IA = malloc((rows_a + 1) * sizeof(int));
    if (!C_A || !C_JA || !C_IA)
        return NULL;

    C_IA[0] = 0;
    int pos = 0;

    for (int i = 0; i < rows_a; i++)
    {
        double *temp_row = calloc(cols_b, sizeof(double));
        int *flag = calloc(cols_b, sizeof(int));

        // перебор ненулевых элементов строки i матрицы A
        for (int a_idx = A_csr->IA[i]; a_idx < A_csr->IA[i + 1]; a_idx++)
        {
            int k = A_csr->JA[a_idx]; // столбец в A, соответствует строке в B
            double value = A_csr->A[a_idx];

            // перебор всех столбцов B
            for (int col = 0; col < cols_b; col++)
            {
                for (int idx = B_csc->IB[col]; idx < B_csc->IB[col + 1]; idx++)
                {
                    if (B_csc->JB[idx] == k) // строка B совпадает с индексом k
                    {
                        temp_row[col] += value * B_csc->B[idx];
                        flag[col] = 1;
                        break; // элемент найден, идем к следующему столбцу
                    }
                }
            }
        }

        // сохраняем ненулевые элементы строки в CSR
        for (int j = 0; j < cols_b; j++)
        {
            if (flag[j])
            {
                if (pos >= estimated_size)
                {
                    estimated_size *= 2;
                    C_A = realloc(C_A, estimated_size * sizeof(double));
                    C_JA = realloc(C_JA, estimated_size * sizeof(int));
                }
                C_A[pos] = temp_row[j];
                C_JA[pos] = j;
                pos++;
            }
        }

        C_IA[i + 1] = pos;
        free(temp_row);
        free(flag);
    }


    if (pos < estimated_size) 
    {
        C_A = realloc(C_A, pos * sizeof(double));
        C_JA = realloc(C_JA, pos * sizeof(int));
    }

    matrix_csr *C_csr = malloc(sizeof(matrix_csr));
    C_csr->A = C_A;
    C_csr->JA = C_JA;
    C_csr->IA = C_IA;
    return C_csr;
}

double **multiply_standard(double **A, double **B, int rows_a, int cols_a, int cols_b)
{
    double **C = matrix_allocate(rows_a, cols_b);
    if (!C)
        return NULL;

    for (int i = 0; i < rows_a; i++)
    {
        for (int j = 0; j < cols_b; j++)
        {
            for (int k = 0; k < cols_a; k++)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    return C;
}


