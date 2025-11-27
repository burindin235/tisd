#ifndef LIST_STACK_H
#define LIST_STACK_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>


#define MAX_FREE_LIST 1000
#define MAX_LIST_STACK 10000

// Узлы стека чисел
typedef struct node
{
    int data;
    struct node *next;
} node;

// Узлы стека операторов
typedef struct node_char
{
    char data;
    struct node_char *next;
} node_char;

// Стек чисел
typedef struct
{
    node *top;
    int size; // Текущее количество элементов
} list_stack;

// Стек операторов
typedef struct
{
    node_char *top;
    int size; // Текущее количество элементов
} list_stack_char;

// --- Стек чисел ---
void list_stack_init(list_stack *stack);
void list_stack_push(list_stack *stack, int value);
int list_stack_pop(list_stack *stack, void **free_list, int *free_count);
int list_stack_top(list_stack *stack);
void list_stack_print(list_stack *stack);

// --- Стек операторов ---
void list_stack_char_init(list_stack_char *stack);
void list_stack_char_push(list_stack_char *stack, char value);
char list_stack_char_pop(list_stack_char *stack, void **free_list, int *free_count);
char list_stack_char_top(list_stack_char *stack);
void list_stack_char_print(list_stack_char *stack);

// --- Вычисление выражения ---
int list_evaluate_expression(const char *expr, void **free_list, int *free_count);

// --- Список освобождённых адресов ---
void print_free_list(void **free_list, int free_count);

#endif
