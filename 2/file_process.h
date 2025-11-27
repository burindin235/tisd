#ifndef FILE_PROCESS_H
#define FILE_PROCESS_H

#include "types.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_BUF_SIZE 256

void parse_student_line(const char *line, student_t *student);
student_t *read_students_from_file(const char *filename, size_t *out_count);
int write_student_to_file(const char *filename, const student_t *student);
int rewrite_students_to_file(const char *filename, student_t *students, size_t count);
int print_students_from_file_formatted(const char *filename);
void print_students_array(student_t *students, size_t count);
int delete_students_by_group(student_t *students, size_t *count, const char *target_group);
int delete_students_by_group_in_file(const char *filename, const char *target_group);
void print_students_rent_cheaper_than(const char *filename, const char *year, double max_rent);
void find_students_by_variant(const student_t *students, size_t count);

#endif
