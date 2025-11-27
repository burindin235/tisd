#ifndef MENU_UTILS_H
#define MENU_UTILS_H

#include "matrix_create.h"
#include "matrix_multiplicate.h"
#include "matrix_types.h"

void print_menu(void);
void print_matrix_input_menu(void);

void print_output_sparse_matrix_menu(int is_huge);
void print_output_dense_matrix_menu(int is_huge);
void print_dense_result_matrix_menu(int is_huge, int rows, int cols);
void print_sparse_result_matrix_menu(int is_huge, int rows, int cols);

double **init_matrix_with_choice(int *rows, int *cols, int num);
double **input_matrix_coordinate(int *rows, int *cols);

void print_dense_matrix(double **matrix, int rows, int cols);
void print_sparse_matrices(double **matrix1, int rows1, int cols1, double **matrix2, int rows2, int cols2,
                           matrix_csr *A_csr, matrix_csc *B_csc);
void print_result_sparse_matrix(matrix_csr *result_csr, int rows, int cols);

void print_dense_matrices(double **matrix1, double **matrix2, int rows1, int cols1, int rows2, int cols2);
void print_result_dense_matrix(double **result, int rows, int cols);
int is_dense_matrix_zero(double **matrix, int rows, int cols);

int read_menu_choice(void);
void flush_stdin(void);

void print_csc_lists(matrix_csc *mat, int cols);
void print_csr_lists(matrix_csr *mat, int rows);
void matrix_print(double **matrix, int rows, int cols);

#endif