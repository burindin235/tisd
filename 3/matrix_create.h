#ifndef MATRIX_CREATE_H
#define MATRIX_CREATE_H

#include "menu_utils.h"
#include "matrix_types.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MIN_GENERATED_MATRIX_ELEMENT -10000.0
#define MAX_GENERATED_MATRIX_ELEMENT 10000.0
#define EPS 1e-9

int matrix_input_size(int *rows, int *cols);
double **matrix_input(int *rows, int *cols);
double **matrix_generate(int rows, int cols, int fill_ratio);

matrix_csr *matrix_to_csr(double **matrix, int rows, int cols);
matrix_csc *matrix_to_csc(double **matrix, int rows, int cols);
double **csr_to_matrix(matrix_csr *csr_matrix, int rows, int cols);
double **csc_to_matrix(matrix_csc *csc_matrix, int rows, int cols);

int matrix_calc_non_zero(double **matrix, int rows, int cols);
matrix_csr *matrix_csr_allocate(int elems, int rows);
matrix_csc *matrix_csc_allocate(int elems, int cols);

double random_double(double min, double max);
void matrix_generate_indexes(int rows, int cols, int *rows_indexes, int *cols_indexes, int elems);
double **matrix_allocate(int rows, int cols);

double **input_matrix_coordinate(int *rows, int *cols);

void matrix_free(double **matrix, int rows);
void matrix_csc_free(matrix_csc *matrix);
void matrix_csr_free(matrix_csr *matrix);

#endif
