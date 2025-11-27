#ifndef MATRIX_MULTIPLICATE_H
#define MATRIX_MULTIPLICATE_H

#include "matrix_create.h"
#include "matrix_types.h"
#include <stddef.h>

matrix_csr *multiply_csr_csc(matrix_csr *A_csr, matrix_csc *B_csc, int rows_a, int cols_a, int cols_b);
double **multiply_standard(double **A, double **B, int rows_a, int cols_a, int cols_b);

#endif
