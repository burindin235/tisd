#ifndef INPUT_CHECK
#define INPUT_CHECK

#include "types.h"
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define MAX_FIRST_NUM_SIZE 45
#define MAX_SECOND_STRING_LEN 35

#define OK 0

#define ERR_CANT_READ_NUM 1
#define ERR_TOO_LONG_STRING 2
#define ERR_EMPTY_STRING 3

#define ERR_INCORRECT_FIRST_NUM 4
#define ERR_TOO_MUCH_SIGNIFICANTS_FIRST_NUM 5

#define ERR_INCORRECT_SECOND_NUM 6
#define ERR_INCORRECT_SYMBOLS_SECOND_NUM 7
#define ERR_TOO_MUCH_SIGNIFICANTS_SECOND_NUM 8

#define MAX_SIGNIFICANTS_NUMS_COUNT 35

int read_line(string_t string, size_t max_string_len);
int is_only_spaces(const char *s);
int calc_significant_nums(string_t str_num);
int validate_first_num(string_t first_num, int *significant_nums_count);
int validate_second_num(string_t second_num, int *significant_nums_count);

#endif