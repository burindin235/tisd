#ifndef INPUT_H
#define INPUT_H

#include "types.h"
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int read_line(string_t string);
int is_only_spaces(const char *s);
int input_and_print_students_in_file(const char *filename, size_t students_count);

int is_leap_year(int year);
int validate_date_string(const char *input, date_t *out_date);

int write_student_to_file(const char *filename, const student_t *student);
void flush_stdin(void);

#endif
