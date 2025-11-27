#include "request.h"
#include <stdio.h>
#include <stdlib.h>
#include "queue_array.h"

// Инициализация очереди
void queue_array_init(queue_array_t* queue)
{
    queue->head = 0;
    queue->tail = 0;
    queue->count = 0;
}

// Проверка, пуста ли очередь
int queue_array_is_empty(queue_array_t* queue)
{
    return queue->count == 0;
}

// Проверка на переполнение очереди
int queue_array_is_full(queue_array_t* queue)
{
    return queue->count > QUEUE_MAX;
}

// Добавление заявки в очередь (в хвост)
    int queue_array_push_request(queue_array_t* queue, request* req, int show_addresses)
    {
        if (queue_array_is_full(queue))
        {
            printf("Ошибка: очередь переполнена (максимум %d элементов)\n", QUEUE_MAX);
            return 0; // переполнение
        }

        queue->data[queue->tail] = req;
        
        if (show_addresses)
        {
            printf("Добавлена заявка type=%d, arrival=%.2f, array_index=%d, data_addr=%p\n",
                req->type, req->arrival_time, queue->tail, (void*)req);
        }
        
        queue->tail++;
        if (queue->tail == QUEUE_MAX)
            queue->tail = 0;

        queue->count++;
        return 1; // успешно
    }

    // Удаление заявки из очереди (с головы)
    int queue_array_pop_request(queue_array_t* queue, request** out_req, int show_addresses)
    {
        if (queue_array_is_empty(queue))
            return 0; // очередь пустая

        *out_req = queue->data[queue->head];
        
        if (show_addresses)
        {
            printf("Удалена заявка type=%d, arrival=%.2f, array_index=%d, data_addr=%p\n",
                (*out_req)->type, (*out_req)->arrival_time, queue->head, (void*)(*out_req));
        }
        
        queue->head++;
        if (queue->head == QUEUE_MAX)
            queue->head = 0;

        queue->count--;
        return 1; // успешно
    }

// Печать очереди
void queue_array_print(queue_array_t* queue)
{
    if (queue_array_is_empty(queue))
    {
        printf("Очередь пустая\n");
        return;
    }

    printf("Очередь (count=%d):\n", queue->count);

    int index = queue->head;
    int printed = 0;

    while (printed < queue->count)
    {
        request* req = queue->data[index];
        printf("Type=%d, Arrival=%.2f, Start=%.2f, Ptr=%p\n",
               req->type, req->arrival_time, req->start_time, (void*)req);

        index++;
        if (index == QUEUE_MAX)
            index = 0;

        printed++;
    }
    printf("\n");
}
