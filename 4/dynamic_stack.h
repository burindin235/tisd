#ifndef DYNAMIC_STACK_H
#define DYNAMIC_STACK_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_DYNAMIC_STACK 10000

// --- Стек чисел ---
typedef struct
{
    int *data;
    int head_pointer;
    int max_len;  // Текущее количество элементов
    int capacity; // Выделенная память (в элементах)
} dynamic_stack;

typedef struct
{
    char *data;
    int head_pointer;
    int max_len;  // Текущее количество элементов
    int capacity; // Выделенная память (в элементах)
} dynamic_stack_char;

// --- Стек чисел ---
void stack_dynamic_init(dynamic_stack *stack);
void stack_dynamic_push(dynamic_stack *stack, int value);
int stack_dynamic_pop(dynamic_stack *stack);
void stack_dynamic_free(dynamic_stack *stack);
void dynamic_stack_print(dynamic_stack *stack);

// --- Стек операторов ---
void stack_dynamic_char_init(dynamic_stack_char *stack);
void stack_dynamic_char_push(dynamic_stack_char *stack, char value);
char stack_dynamic_char_pop(dynamic_stack_char *stack);
char stack_dynamic_char_top(dynamic_stack_char *stack);
void stack_dynamic_char_free(dynamic_stack_char *stack);

// --- Вспомогательные функции ---
int dynamic_evaluate_expression(const char *expr);

#endif
