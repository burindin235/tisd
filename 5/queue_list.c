#include "request.h"
#include <stdio.h>
#include <stdlib.h>
#include "queue_list.h"

// Инициализация очереди
void queue_list_init(queue_list* q)
{
    q->head = NULL;
    q->tail = NULL;
    q->count = 0;
}

// Проверка пустоты
int queue_list_is_empty(queue_list* q)
{
    return q->count == 0;
}

// Проверка переполнения
int queue_list_is_full(queue_list* q)
{
    return q->count > QUEUE_LIST_MAX;
}

// Добавление заявки в хвост
int queue_list_push_request(queue_list* q, request* req, int show_addresses)
{
    
    if (queue_list_is_full(q))
    {
        printf("Ошибка: очередь переполнена\n");
        return 1;
    }

    node* new_node = (node*)malloc(sizeof(node));
    if (!new_node)
    {
        printf("Ошибка: выделение памяти не удалось\n");
        return 2;
    }

    new_node->data = req;
    new_node->next = NULL;

    if (queue_list_is_empty(q))
    {
        q->head = new_node;
        q->tail = new_node;
    }
    else
    {
        q->tail->next = new_node;
        q->tail = new_node;
    }

    q->count++;
    if (show_addresses)
    {
        printf("Добавлена заявка type=%d, arrival=%.2f, node_addr=%p, data_addr=%p\n",
               req->type, req->arrival_time, (void*)new_node, (void*)req);
    }
    return 0;
}

// Удаление заявки с головы
int queue_list_pop_request(queue_list* q, request** out_req, int show_addresses)
{
    if (queue_list_is_empty(q))
        return 1;

    node* temp = q->head;
    *out_req = temp->data;

    q->head = q->head->next;
    if (q->head == NULL)
        q->tail = NULL;

    if (show_addresses)
    {
        printf("Удалена заявка type=%d, arrival=%.2f, node_addr=%p, data_addr=%p\n",
               (*out_req)->type, (*out_req)->arrival_time, (void*)temp, (void*)(*out_req));
    }
    free(temp);
    q->count--;
    return 0;
}

// Печать очереди с адресами
void queue_list_print(queue_list* q)
{
    if (queue_list_is_empty(q))
    {
        printf("Очередь пустая\n");
        return;
    }

    printf("Очередь (count=%d):\n", q->count);
    node* current = q->head;

    while (current)
    {
        printf("Type=%d, Arrival=%.2f, Start=%.2f, NodePtr=%p, Next=%p\n",
               current->data->type,
               current->data->arrival_time,
               current->data->start_time,
               (void*)current,
               (void*)current->next);
        current = current->next;
    }

    printf("\n");
}
