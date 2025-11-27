#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "matrix_create.h"
#include "matrix_multiplicate.h"
#include <stddef.h>
#include <stdio.h>
#include <time.h>

#define MAX_SIZES_TO_BENCHMARK 24
#define MAX_FILL_COUNT 100

double get_time_s(void);

size_t calc_dense_memory(int rows, int cols);
size_t calc_sparse_memory(matrix_csr *mat, int rows);

void run_benchmarks(int is_fast);

void array_init_by_num(int *arr, int arr_len, int num);

#endif
