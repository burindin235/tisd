#include "queue_benchmark.h"


// Реализация очереди на списке
void queue_list_init_bench(queue_list_bench* q)
{
    q->head = NULL;
    q->tail = NULL;
    q->count = 0;
}

int queue_list_push(queue_list_bench* q, int value)
{
    node_bench* new_node = (node_bench*)malloc(sizeof(node_bench));
    if (new_node == NULL)
    {
        return -1;
    }
    
    new_node->data = value;
    new_node->next = NULL;
    
    if (q->tail == NULL)
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
    return 0;
}

int queue_list_pop(queue_list_bench* q, int* value)
{
    if (q->head == NULL)
    {
        return -1;
    }
    
    node_bench* temp = q->head;
    *value = temp->data;
    q->head = q->head->next;
    
    if (q->head == NULL)
    {
        q->tail = NULL;
    }
    
    free(temp);
    q->count--;
    return 0;
}

void queue_list_free(queue_list_bench* q)
{
    int value;
    while (queue_list_pop(q, &value) == 0)
    {
        // Освобождаем все элементы
    }
}

// Реализация очереди на массиве
void queue_array_init_bench(queue_array* q)
{
    q->head = 0;
    q->tail = -1;
    q->count = 0;
}

int queue_array_push(queue_array* q, int value)
{
    if (q->count >= MAX_QUEUE_SIZE)
    {
        return -1;
    }
    
    q->tail = (q->tail + 1) % MAX_QUEUE_SIZE;
    q->data[q->tail] = value;
    q->count++;
    return 0;
}

int queue_array_pop(queue_array* q, int* value)
{
    if (q->count <= 0)
    {
        return -1;
    }
    
    *value = q->data[q->head];
    q->head = (q->head + 1) % MAX_QUEUE_SIZE;
    q->count--;
    return 0;
}

// Функция бенчмарка
void benchmark_queues(void)
{
    const int operations_count = 1000;
    double start_time, end_time;
    int i, value;
    
    printf("=== БЕНЧМАРК ОЧЕРЕДЕЙ ===\n");
    printf("Количество операций: %d\n\n", operations_count);
    
    // Тестирование очереди на списке
    queue_list_bench list_queue;
    queue_list_init_bench(&list_queue);
    
    // Замер времени push для списка
    start_time = get_time_s_bench();
    for (i = 0; i < operations_count; i++)
    {
        queue_list_push(&list_queue, i);
    }
    end_time = get_time_s_bench();
    double list_push_time = (end_time - start_time) / operations_count * 1e6; // мкс на операцию
    
    // Замер времени pop для списка
    start_time = get_time_s_bench();
    for (i = 0; i < operations_count; i++)
    {
        queue_list_pop(&list_queue, &value);
    }
    end_time = get_time_s_bench();
    double list_pop_time = (end_time - start_time) / operations_count * 1e6; // мкс на операцию
    
    queue_list_free(&list_queue);
    
    // Тестирование очереди на массиве
    queue_array array_queue;
    queue_array_init_bench(&array_queue);
    
    // Замер времени push для массива
    start_time = get_time_s_bench();
    for (i = 0; i < operations_count; i++)
    {
        queue_array_push(&array_queue, i);
    }
    end_time = get_time_s_bench();
    double array_push_time = (end_time - start_time) / operations_count * 1e6; // мкс на операцию
    
    // Замер времени pop для массива
    start_time = get_time_s_bench();
    for (i = 0; i < operations_count; i++)
    {
        queue_array_pop(&array_queue, &value);
    }
    end_time = get_time_s_bench();
    double array_pop_time = (end_time - start_time) / operations_count * 1e6; // мкс на операцию
    
    // Вывод результатов
    printf("РЕЗУЛЬТАТЫ:\n");
    printf("----------------------------------------\n");
    printf("ОПЕРАЦИЯ        | СПИСОК     | МАССИВ\n");
    printf("----------------------------------------\n");
    printf("Push (мкс/оп)   | %8.3f  | %8.3f\n", list_push_time, array_push_time);
    printf("Pop (мкс/оп)    | %8.3f  | %8.3f\n", list_pop_time, array_pop_time);
    printf("----------------------------------------\n\n");
    
    // Анализ результатов
    printf("АНАЛИЗ:\n");
    if (array_push_time < list_push_time)
    {
        printf("• Push на массиве быстрее на %.2f%%\n", 
               (list_push_time - array_push_time) / list_push_time * 100);
    }
    else
    {
        printf("• Push на списке быстрее на %.2f%%\n", 
               (array_push_time - list_push_time) / array_push_time * 100);
    }
    
    if (array_pop_time < list_pop_time)
    {
        printf("• Pop на массиве быстрее на %.2f%%\n", 
               (list_pop_time - array_pop_time) / list_pop_time * 100);
    }
    else
    {
        printf("• Pop на списке быстрее на %.2f%%\n", 
               (array_pop_time - list_pop_time) / array_pop_time * 100);
    }
    
    printf("\nВремя указано в микросекундах на одну операцию\n");
}