#ifndef QUEUE_ARRAY_H
#define QUEUE_ARRAY_H

#include <stdio.h>
#include <stdlib.h>
#include "request.h"

#define QUEUE_MAX 5000

// Очередь на основе массива (кольцевая)
typedef struct queue_array_t
{
    request* data[QUEUE_MAX]; // массив указателей на заявки
    int head;    // индекс головы (Pout)
    int tail;    // индекс хвоста (Pin)
    int count;   // текущее количество элементов
} queue_array_t;

// Инициализация очереди
void queue_array_init(queue_array_t* queue);

// Проверка, пуста ли очередь
int queue_array_is_empty(queue_array_t* queue);

// Проверка на переполнение очереди
int queue_array_is_full(queue_array_t* queue);

// Добавление заявки в очередь (в хвост) с опциональным выводом адресов
int queue_array_push_request(queue_array_t* queue, request* req, int show_addresses);

// Удаление заявки из очереди (с головы) с опциональным выводом адресов
int queue_array_pop_request(queue_array_t* queue, request** out_req, int show_addresses);

// Печать очереди
void queue_array_print(queue_array_t* queue);

#endif // QUEUE_ARRAY_H

