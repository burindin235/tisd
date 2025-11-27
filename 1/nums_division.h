#ifndef NUMS_DIVISION
#define NUMS_DIVISION

#include "types.h"
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FRACTION_LIMIT 150
#define WORK_MAX 300

void structuring_num(string_t string_num, long_t *long_num, int significant_nums, int *first_num_new_size);
void normalizing_num(string_t num, string_t normalized_num);
int division_big_nums(long_t first_num, int first_num_digits_count, long_t second_num, int second_num_digits_count,
                      long_t *result, int *res_len);
int multiply_array_by_digit(int *a, int len_a, int digit, int *res);
int subtract_arrays(int *a, int len_a, int *b, int len_b);
int compare_arrays(int *a, int len_a, int *b, int len_b);
int is_zero(long_t *num, int len);

#endif
