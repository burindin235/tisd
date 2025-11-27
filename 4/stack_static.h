#ifndef STACK_STATIC_H
#define STACK_STATIC_H

#define MAX_STATIC_STACK 10000

#define MAX_STRING_LEN 100

#include <ctype.h>
#include <stdio.h>

typedef struct
{
    int data[MAX_STATIC_STACK];
    int max_len;
    int head_pointer;
} static_stack;

typedef struct
{
    char data[MAX_STATIC_STACK];
    int head_pointer;
    int max_len;
} static_stack_char;

typedef char string_t[MAX_STRING_LEN];

// Стек чисел
void stack_static_push(static_stack *stack, int value);
int stack_static_pop(static_stack *stack);
void stack_static_init(static_stack *stack);
void static_stack_print(static_stack *stack);

// Стек операторов
void stack_static_char_init(static_stack_char *stack);
void stack_static_char_push(static_stack_char *stack, char value);
char stack_static_char_pop(static_stack_char *stack);
char stack_static_char_top(static_stack_char *stack);

// Вспомогательные функции
int static_evaluate_expression(const char *expr);

#endif