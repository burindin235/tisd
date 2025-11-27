#ifndef QUEUE_LIST_H
#define QUEUE_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include "request.h"

#define QUEUE_LIST_MAX 1000 // Максимальный размер очереди

// Узел списка
typedef struct node
{
    request* data;      // указатель на заявку
    struct node* next;
} node;

// Очередь на основе списка
typedef struct queue_list
{
    node* head;   // голова (Pout)
    node* tail;   // хвост (Pin)
    int count;
} queue_list;

// Инициализация очереди
void queue_list_init(queue_list* q);

// Проверка пустоты
int queue_list_is_empty(queue_list* q);

// Проверка переполнения
int queue_list_is_full(queue_list* q);

// Добавление заявки в хвост (с опциональным выводом адресов)
int queue_list_push_request(queue_list* q, request* req, int show_addresses);

// Удаление заявки с головы (с опциональным выводом адресов)
int queue_list_pop_request(queue_list* q, request** out_req, int show_addresses);

// Печать очереди с адресами
void queue_list_print(queue_list* q);

#endif // QUEUE_LIST_H

