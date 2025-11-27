#ifndef KEY_TABLE_H
#define KEY_TABLE_H

#include "file_process.h"
#include "types.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

students_keys_t *build_students_keys_table_from_file(const char *filename, size_t *out_count);
students_keys_t *build_students_keys_table_from_students(student_t *students, size_t count);
void sort_students_keys_table(students_keys_t *keys, size_t count);
void print_students_keys_table(const students_keys_t *keys, size_t count);
void sort_students_by_avg_grade(student_t *students, size_t count);
void print_students_array_by_index(student_t *students, size_t count, students_keys_t *keys);
void quicksort_students_by_avg_grade(student_t *arr, int low, int high);
void quicksort_keys(students_keys_t *arr, int low, int high);
student_t *copy_students_array(student_t *arr, size_t n);
students_keys_t *copy_keys_array(students_keys_t *arr, size_t n);
void bubble_sort_students_by_key(student_t *students, size_t count);
void bubble_sort_keys(students_keys_t *keys, size_t count);
student_t* create_students_array_of_size(student_t *base_students, size_t base_count, int target_size);
int copy_student(student_t *dest, const student_t *src);
student_t* duplicate_students_array(student_t *src, size_t count);
students_keys_t* duplicate_keys_array(students_keys_t *src, size_t count);
student_t* create_students_array_of_size(student_t *base_students, size_t base_count, int target_size);
int copy_student(student_t *dest, const student_t *src);

#endif
