#ifndef QUEUE_BENCHMARK_H
#define QUEUE_BENCHMARK_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "oa_process.h"

// Структура узла для списка
typedef struct node_bench
{
    int data;
    struct node_bench* next;
} node_bench;

// Очередь на списке
typedef struct
{
    node_bench* head;
    node_bench* tail;
    int count;
} queue_list_bench;

// Очередь на массиве
#define MAX_QUEUE_SIZE 10000

typedef struct
{
    int data[MAX_QUEUE_SIZE];
    int head;
    int tail;
    int count;
} queue_array;

// Функции для работы с очередью на списке
void queue_list_init_bench(queue_list_bench* q);
int queue_list_push(queue_list_bench* q, int value);
int queue_list_pop(queue_list_bench* q, int* value);
void queue_list_free(queue_list_bench* q);

// Функции для работы с очередью на массиве
void queue_array_init_bench(queue_array* q);
int queue_array_push(queue_array* q, int value);
int queue_array_pop(queue_array* q, int* value);

// Бенчмарк функция
void benchmark_queues(void);

#endif