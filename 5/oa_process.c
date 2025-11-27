#include "oa_process.h"
#include "queue_list.h"
#include "queue_array.h"
#include "request.h"

// Генератор равномерного случайного числа
double uniform_random(double t_min, double t_max)
{
    double t = (double)rand() / RAND_MAX;
    return t_min + t * (t_max - t_min);
}

// Генерация заявки с заданным типом и предыдущим временем прихода
request generate_request(int type, double previous_arrival, double t_min, double t_max)
{
    request req;
    req.type = type;
    double interval = uniform_random(t_min, t_max);
    req.arrival_time = previous_arrival + interval;
    req.start_time = 0;
    req.end_time = 0;
    return req;
}

// Инициализация системы массового обслуживания
void smo_system_init(smo_system* sys)
{
    sys->current_time = 0.0;
    sys->oa_free_time = 0.0;
    sys->processed_type1 = 0;
    sys->processed_type2 = 0;
    sys->idle_time = 0.0;
    sys->last_processed_type = 0;

    queue_list_init(&sys->queue1);
    queue_list_init(&sys->queue2);

    sys->total_wait_time_type1 = 0.0;
    sys->total_wait_time_type2 = 0.0;
    sys->total_arrived_type1 = 0;
    sys->total_arrived_type2 = 0;
    
    sys->total_queue1_length = 0.0;
    sys->total_queue2_length = 0.0;
    sys->queue_length_samples = 0;
}

// Определение следующего события среди прихода заявок и окончания обработки
next_event_t get_next_event(next_event_t ev1, next_event_t ev2, double oa_free_time)
{
    next_event_t next;
    next.time = ev1.time;
    next.type = ev1.type;

    if (ev2.time < next.time)
    {
        next.time = ev2.time;
        next.type = ev2.type;
    }
    // Если ОА свободен (oa_free_time <= 0) или освободится раньше следующего события
    if (oa_free_time > 0 && oa_free_time < next.time)
    {
        next.time = oa_free_time;
        next.type = EVENT_DEPARTURE;
    }

    return next;
}

// Функция измерения текущего реального времени в секундах
double get_time_s_bench(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

// Проверка доступности заявки для обработки (для списка)
int check_request_available(smo_system* sys)
{
    if (sys->last_processed_type == 0)
    {
        if (!queue_list_is_empty(&sys->queue1))
            return 1;
        else if (!queue_list_is_empty(&sys->queue2))
            return 2;
        return 0;
    }
    
    if (sys->last_processed_type == 1 && !queue_list_is_empty(&sys->queue1))
        return 1;
    else if (sys->last_processed_type == 2 && !queue_list_is_empty(&sys->queue2))
        return 2;
    
    if (sys->last_processed_type == 1 && queue_list_is_empty(&sys->queue1))
    {
        if (!queue_list_is_empty(&sys->queue2))
            return 2;
    }
    else if (sys->last_processed_type == 2 && queue_list_is_empty(&sys->queue2))
    {
        if (!queue_list_is_empty(&sys->queue1))
            return 1;
    }
    
    return 0;
}

// Проверка доступности заявки для обработки (для массива)
int check_request_available_array(queue_array_t* q1, queue_array_t* q2, int last_processed_type)
{
    if (last_processed_type == 0)
    {
        if (!queue_array_is_empty(q1))
            return 1;
        else if (!queue_array_is_empty(q2))
            return 2;
        return 0;
    }
    
    if (last_processed_type == 1 && !queue_array_is_empty(q1))
        return 1;
    else if (last_processed_type == 2 && !queue_array_is_empty(q2))
        return 2;
    
    if (last_processed_type == 1 && queue_array_is_empty(q1))
    {
        if (!queue_array_is_empty(q2))
            return 2;
    }
    else if (last_processed_type == 2 && queue_array_is_empty(q2))
    {
        if (!queue_array_is_empty(q1))
            return 1;
    }
    
    return 0;
}

// Выбор заявки для обработки согласно правилам приоритета (для списка)
int select_request_for_processing(smo_system* sys, request** selected_req, int show_addresses)
{
    *selected_req = NULL;
    
    if (sys->last_processed_type == 0)
    {
        if (!queue_list_is_empty(&sys->queue1))
        {
            queue_list_pop_request(&sys->queue1, selected_req, show_addresses);
            return 1;
        }
        else if (!queue_list_is_empty(&sys->queue2))
        {
            queue_list_pop_request(&sys->queue2, selected_req, show_addresses);
            return 2;
        }
        return 0;
    }
    
    if (sys->last_processed_type == 1 && !queue_list_is_empty(&sys->queue1))
    {
        queue_list_pop_request(&sys->queue1, selected_req, show_addresses);
        return 1;
    }
    else if (sys->last_processed_type == 2 && !queue_list_is_empty(&sys->queue2))
    {
        queue_list_pop_request(&sys->queue2, selected_req, show_addresses);
        return 2;
    }
    
    if (sys->last_processed_type == 1 && queue_list_is_empty(&sys->queue1))
    {
        if (!queue_list_is_empty(&sys->queue2))
        {
            queue_list_pop_request(&sys->queue2, selected_req, show_addresses);
            return 2;
        }
    }
    else if (sys->last_processed_type == 2 && queue_list_is_empty(&sys->queue2))
    {
        if (!queue_list_is_empty(&sys->queue1))
        {
            queue_list_pop_request(&sys->queue1, selected_req, show_addresses);
            return 1;
        }
    }
    
    return 0;
}

// Выбор заявки для обработки согласно правилам приоритета (для массива)
int select_request_for_processing_array(queue_array_t* q1, queue_array_t* q2, 
                                         request** selected_req, int last_processed_type, 
                                         int show_addresses)
{
    *selected_req = NULL;
    
    if (last_processed_type == 0)
    {
        if (!queue_array_is_empty(q1))
        {
            queue_array_pop_request(q1, selected_req, show_addresses);
            return 1;
        }
        else if (!queue_array_is_empty(q2))
        {
            queue_array_pop_request(q2, selected_req, show_addresses);
            return 2;
        }
        return 0;
    }
    
    if (last_processed_type == 1 && !queue_array_is_empty(q1))
    {
        queue_array_pop_request(q1, selected_req, show_addresses);
        return 1;
    }
    else if (last_processed_type == 2 && !queue_array_is_empty(q2))
    {
        queue_array_pop_request(q2, selected_req, show_addresses);
        return 2;
    }
    
    if (last_processed_type == 1 && queue_array_is_empty(q1))
    {
        if (!queue_array_is_empty(q2))
        {
            queue_array_pop_request(q2, selected_req, show_addresses);
            return 2;
        }
    }
    else if (last_processed_type == 2 && queue_array_is_empty(q2))
    {
        if (!queue_array_is_empty(q1))
        {
            queue_array_pop_request(q1, selected_req, show_addresses);
            return 1;
        }
    }
    
    return 0;
}

// Вывод статистики каждые 100 заявок
void print_periodic_stats(smo_system* sys, int processed_count)
{
    double avg_queue1_length = sys->queue_length_samples > 0 ? 
        sys->total_queue1_length / sys->queue_length_samples : 0.0;
    double avg_queue2_length = sys->queue_length_samples > 0 ? 
        sys->total_queue2_length / sys->queue_length_samples : 0.0;
    
    double avg_wait_time1 = sys->processed_type1 > 0 ? 
        sys->total_wait_time_type1 / sys->processed_type1 : 0.0;
    double avg_wait_time2 = sys->processed_type2 > 0 ? 
        sys->total_wait_time_type2 / sys->processed_type2 : 0.0;
    
    printf("\n=== Статистика после обработки %d заявок 1-го типа ===\n", processed_count);
    printf("Текущая длина очереди 1: %d\n", sys->queue1.count);
    printf("Текущая длина очереди 2: %d\n", sys->queue2.count);
    printf("Средняя длина очереди 1: %.2f\n", avg_queue1_length);
    printf("Средняя длина очереди 2: %.2f\n", avg_queue2_length);
    printf("Среднее время пребывания заявок 1-го типа в очереди: %.2f е.в.\n", avg_wait_time1);
    printf("Среднее время пребывания заявок 2-го типа в очереди: %.2f е.в.\n", avg_wait_time2);
    printf("==================================================\n\n");
}

// Вывод периодической статистики для массива
void print_periodic_stats_array(int q1_len, int q2_len, double avg_q1, double avg_q2,
                                 double avg_wait1, double avg_wait2, int processed_count)
{
    printf("\n=== Статистика после обработки %d заявок 1-го типа ===\n", processed_count);
    printf("Текущая длина очереди 1: %d\n", q1_len);
    printf("Текущая длина очереди 2: %d\n", q2_len);
    printf("Средняя длина очереди 1: %.2f\n", avg_q1);
    printf("Средняя длина очереди 2: %.2f\n", avg_q2);
    printf("Среднее время пребывания заявок 1-го типа в очереди: %.2f е.в.\n", avg_wait1);
    printf("Среднее время пребывания заявок 2-го типа в очереди: %.2f е.в.\n", avg_wait2);
    printf("==================================================\n\n");
}

// Вывод финальной статистики
void print_final_stats(smo_system* sys, double simulation_time, double real_time)
{
    printf("\n========== ФИНАЛЬНАЯ СТАТИСТИКА ==========\n");
    printf("Общее время моделирования: %.2f е.в.\n", simulation_time);
    printf("Время простоя ОА: %.2f е.в.\n", sys->idle_time);
    printf("Количество вошедших заявок 1-го типа: %d\n", sys->total_arrived_type1);
    printf("Количество вошедших заявок 2-го типа: %d\n", sys->total_arrived_type2);
    printf("Количество вышедших заявок 1-го типа: %d\n", sys->processed_type1);
    printf("Количество вышедших заявок 2-го типа: %d\n", sys->processed_type2);
    printf("Время выполнения программы: %.3f сек\n", real_time);
    //printf("Расчётное количество вошедших заявок %.2f", simulation_time / (sys->total_arrived_type1 + sys->total_arrived_type2));
    printf("==========================================\n\n");
}

// Вывод финальной статистики для массива
void print_final_stats_array(double simulation_time, double idle_time, 
                             int arrived1, int arrived2, int processed1, int processed2,
                             double real_time)
{
    printf("\n========== ФИНАЛЬНАЯ СТАТИСТИКА ==========\n");
    printf("Общее время моделирования: %.2f е.в.\n", simulation_time);
    printf("Время простоя ОА: %.2f е.в.\n", idle_time);
    printf("Количество вошедших заявок 1-го типа: %d\n", arrived1);
    printf("Количество вошедших заявок 2-го типа: %d\n", arrived2);
    printf("Количество вышедших заявок 1-го типа: %d\n", processed1);
    printf("Количество вышедших заявок 2-го типа: %d\n", processed2);
    printf("Время выполнения программы: %.3f сек\n", real_time);
    printf("==========================================\n\n");
}

// Вывод информации о фрагментации памяти
void print_memory_fragmentation_info(void* prev_addr, void* curr_addr, const char* operation)
{
    if (prev_addr != NULL && curr_addr != NULL)
    {
        long long diff = (char*)curr_addr - (char*)prev_addr;
        printf("  [Фрагментация] %s: предыдущий адрес=%p, текущий адрес=%p, разница=%lld байт\n",
               operation, prev_addr, curr_addr, diff);
        if (diff < 0 || diff > 1000)
        {
            printf("\n");
        }
    }
}

// Функция моделирования системы массового обслуживания (список)
void simulate_smo(smo_system* sys, model_params_t* params, int show_addresses)
{
    // Инициализация времени прихода заявок
    double next_arrival_type1 = uniform_random(params->t1_min, params->t1_max);
    double next_arrival_type2 = uniform_random(params->t2_min, params->t2_max);
    
    void* prev_malloc_addr = NULL;

    // Основной цикл симуляции
    while (sys->processed_type1 < MAX_TYPE1_REQUESTS)
    {
        // Обновляем статистику длины очередей
        sys->total_queue1_length += sys->queue1.count;
        sys->total_queue2_length += sys->queue2.count;
        sys->queue_length_samples++;
        
        // Определяем ближайшее событие
        double next_time = next_arrival_type1;
        event_type next_event_type = EVENT_ARRIVAL_TYPE1;
        
        if (next_arrival_type2 < next_time)
        {
            next_time = next_arrival_type2;
            next_event_type = EVENT_ARRIVAL_TYPE2;
        }
        
        // Проверяем, не освободится ли ОА раньше (только если ОА занят)
        if (sys->oa_free_time > sys->current_time && sys->oa_free_time < next_time)
        {
            next_time = sys->oa_free_time;
            next_event_type = EVENT_DEPARTURE;
        }
        
        // Защита от бесконечного цикла - время должно продвигаться вперед
        if (next_time <= sys->current_time)
        {
            printf("ОШИБКА: Время не продвигается вперед! current_time=%.2f, next_time=%.2f\n", 
                   sys->current_time, next_time);
            printf("next_arrival_type1=%.2f, next_arrival_type2=%.2f, oa_free_time=%.2f\n",
                   next_arrival_type1, next_arrival_type2, sys->oa_free_time);
        }
        
        // Переходим к времени события
        sys->current_time = next_time;
        
        // Обрабатываем событие
        if (next_event_type == EVENT_ARRIVAL_TYPE1)
        {
            // Приход заявки 1-го типа
            request* req = (request*)malloc(sizeof(request));
            if (!req)
            {
                printf("Ошибка выделения памяти для заявки 1-го типа\n");
            }
            
            if (show_addresses && prev_malloc_addr != NULL)
            {
                print_memory_fragmentation_info(prev_malloc_addr, req, "Выделение памяти для заявки 1");
            }
            prev_malloc_addr = req;

            req->type = 1;
            req->arrival_time = sys->current_time;
            req->start_time = 0;
            req->end_time = 0;

            queue_list_push_request(&sys->queue1, req, show_addresses);
            sys->total_arrived_type1++;

            // Планируем следующее событие прихода заявки 1-го типа
            next_arrival_type1 = sys->current_time + uniform_random(params->t1_min, params->t1_max);
            
            // Если ОА свободен, начинаем обработку заявки
            if (sys->oa_free_time <= sys->current_time)
            {
                request* req_to_process = NULL;
                int can_process = check_request_available(sys);
                
                if (can_process > 0)
                {
                    select_request_for_processing(sys, &req_to_process, show_addresses);
                    
                    if (req_to_process != NULL)
                    {
                        req_to_process->start_time = sys->current_time;
                        double service_time = uniform_random(params->t3_min, params->t3_max);
                        if (req_to_process->type == 2)
                        {
                            service_time = uniform_random(params->t4_min, params->t4_max);
                        }

                        req_to_process->end_time = sys->current_time + service_time;
                        sys->oa_free_time = req_to_process->end_time;
                        sys->last_processed_type = req_to_process->type;

                        if (req_to_process->type == 1)
                        {
                            sys->processed_type1++;
                            sys->total_wait_time_type1 += req_to_process->start_time - req_to_process->arrival_time;
                            
                            if (sys->processed_type1 % 100 == 0)
                            {
                                print_periodic_stats(sys, sys->processed_type1);
                            }
                        }
                        else
                        {
                            sys->processed_type2++;
                            sys->total_wait_time_type2 += req_to_process->start_time - req_to_process->arrival_time;
                        }

                        free(req_to_process);
                    }
                }
                else
                {
                    // ОА простаивает - устанавливаем время освобождения
                    sys->oa_free_time = sys->current_time;
                }
            }
        }
        else if (next_event_type == EVENT_ARRIVAL_TYPE2)
        {
            // Приход заявки 2-го типа
            request* req = (request*)malloc(sizeof(request));
            if (!req)
            {
                printf("Ошибка выделения памяти для заявки 2-го типа\n");
            }
            
            if (show_addresses && prev_malloc_addr != NULL)
            {
                print_memory_fragmentation_info(prev_malloc_addr, req, "Выделение памяти для заявки 2");
            }
            prev_malloc_addr = req;

            req->type = 2;
            req->arrival_time = sys->current_time;
            req->start_time = 0;
            req->end_time = 0;

            queue_list_push_request(&sys->queue2, req, show_addresses);
            sys->total_arrived_type2++;

            // Планируем следующее событие прихода заявки 2-го типа
            next_arrival_type2 = sys->current_time + uniform_random(params->t2_min, params->t2_max);
            
            // Если ОА свободен, начинаем обработку заявки
            if (sys->oa_free_time <= sys->current_time)
            {
                request* req_to_process = NULL;
                int can_process = check_request_available(sys);
                
                if (can_process > 0)
                {
                    select_request_for_processing(sys, &req_to_process, show_addresses);
                    
                    if (req_to_process != NULL)
                    {
                        req_to_process->start_time = sys->current_time;
                        double service_time = uniform_random(params->t3_min, params->t3_max);
                        if (req_to_process->type == 2)
                        {
                            service_time = uniform_random(params->t4_min, params->t4_max);
                        }

                        req_to_process->end_time = sys->current_time + service_time;
                        sys->oa_free_time = req_to_process->end_time;
                        sys->last_processed_type = req_to_process->type;

                        if (req_to_process->type == 1)
                        {
                            sys->processed_type1++;
                            sys->total_wait_time_type1 += req_to_process->start_time - req_to_process->arrival_time;
                            
                            if (sys->processed_type1 % 100 == 0)
                            {
                                print_periodic_stats(sys, sys->processed_type1);
                            }
                        }
                        else
                        {
                            sys->processed_type2++;
                            sys->total_wait_time_type2 += req_to_process->start_time - req_to_process->arrival_time;
                        }

                        free(req_to_process);
                    }
                }
                else
                {
                    // ОА простаивает - устанавливаем время освобождения
                    sys->oa_free_time = sys->current_time;
                }
            }
        }
        else if (next_event_type == EVENT_DEPARTURE)
        {
            // ОА освободился - завершаем обработку текущей заявки и начинаем следующую
            // (текущая заявка уже обработана, просто освобождаем ОА)
            
            // Проверяем, есть ли заявки для обработки
            request* req = NULL;
            select_request_for_processing(sys, &req, show_addresses);

            if (req != NULL)
            {
                // Начинаем обработку следующей заявки
                req->start_time = sys->current_time;
                double service_time = uniform_random(params->t3_min, params->t3_max);
                if (req->type == 2)
                {
                    service_time = uniform_random(params->t4_min, params->t4_max);
                }

                // Учитываем простой, если ОА был свободен
                if (sys->oa_free_time <= sys->current_time)
                {
                    sys->idle_time += sys->current_time - sys->oa_free_time;
                }

                req->end_time = sys->current_time + service_time;
                sys->oa_free_time = req->end_time;
                sys->last_processed_type = req->type;

                if (req->type == 1)
                {
                    sys->processed_type1++;
                    sys->total_wait_time_type1 += req->start_time - req->arrival_time;
                    
                    if (sys->processed_type1 % 100 == 0)
                    {
                        print_periodic_stats(sys, sys->processed_type1);
                    }
                }
                else
                {
                    sys->processed_type2++;
                    sys->total_wait_time_type2 += req->start_time - req->arrival_time;
                }

                free(req);
            }
            else
            {
                // Нет заявок - ОА простаивает
                sys->oa_free_time = sys->current_time;
            }
        }
    }
}

// Функция моделирования системы массового обслуживания (массив)
void simulate_smo_array(model_params_t* params, int show_addresses, 
                        double* out_model_time, int* out_proc_type1, int* out_arr1, 
                        int* out_proc_type2, int* out_arr2, double* out_idle_time, 
                        double* out_avglen1, double* out_avglen2, 
                        double* out_avgwait1, double* out_avgwait2)
{
    queue_array_t queue1, queue2;
    queue_array_init(&queue1);
    queue_array_init(&queue2);

    double current_time = 0.0;
    double oa_free_time = 0.0;
    int processed_type1 = 0;
    int processed_type2 = 0;
    double idle_time = 0.0;
    int last_processed_type = 0;

    double total_wait_time_type1 = 0.0;
    double total_wait_time_type2 = 0.0;
    int total_arrived_type1 = 0;
    int total_arrived_type2 = 0;
    
    double total_queue1_length = 0.0;
    double total_queue2_length = 0.0;
    int queue_length_samples = 0;

    // Инициализация времени прихода заявок
    double next_arrival_type1 = uniform_random(params->t1_min, params->t1_max);
    double next_arrival_type2 = uniform_random(params->t2_min, params->t2_max);

    // Основной цикл симуляции
    while (processed_type1 < MAX_TYPE1_REQUESTS)
    {
        // Обновляем статистику длины очередей
        total_queue1_length += queue1.count;
        total_queue2_length += queue2.count;
        queue_length_samples++;
        
        // Определяем ближайшее событие
        double next_time = next_arrival_type1;
        event_type next_event_type = EVENT_ARRIVAL_TYPE1;
        
        if (next_arrival_type2 < next_time)
        {
            next_time = next_arrival_type2;
            next_event_type = EVENT_ARRIVAL_TYPE2;
        }
        
        // Проверяем, не освободится ли ОА раньше (только если ОА занят)
        if (oa_free_time > current_time && oa_free_time < next_time)
        {
            next_time = oa_free_time;
            next_event_type = EVENT_DEPARTURE;
        }
        
        // Защита от бесконечного цикла - время должно продвигаться вперед
        if (next_time <= current_time)
        {
            printf("ОШИБКА: Время не продвигается вперед! current_time=%.2f, next_time=%.2f\n", 
                   current_time, next_time);
            printf("next_arrival_type1=%.2f, next_arrival_type2=%.2f, oa_free_time=%.2f\n",
                   next_arrival_type1, next_arrival_type2, oa_free_time);

        }
        
        // Переходим к времени события
        current_time = next_time;
        
        // Обрабатываем событие
        if (next_event_type == EVENT_ARRIVAL_TYPE1)
        {
            // Приход заявки 1-го типа
            request* req = (request*)malloc(sizeof(request));
            if (!req)
            {
                printf("Ошибка выделения памяти для заявки 1-го типа\n");

            }

            req->type = 1;
            req->arrival_time = current_time;
            req->start_time = 0;
            req->end_time = 0;

            int push_result = queue_array_push_request(&queue1, req, 0);
            if (push_result == 0)
            {
                printf("КРИТИЧЕСКАЯ ОШИБКА: Очередь 1 переполнена! Симуляция остановлена.\n");
                free(req);
            }
            total_arrived_type1++;

            // Планируем следующее событие прихода заявки 1-го типа
            next_arrival_type1 = current_time + uniform_random(params->t1_min, params->t1_max);
            
            // Если ОА свободен, начинаем обработку заявки
            if (oa_free_time <= current_time)
            {
                request* req_to_process = NULL;
                int can_process = check_request_available_array(&queue1, &queue2, last_processed_type);
                
                if (can_process > 0)
                {
                    select_request_for_processing_array(&queue1, &queue2, &req_to_process, 
                                                         last_processed_type, 0);
                    
                    if (req_to_process != NULL)
                    {
                        req_to_process->start_time = current_time;
                        double service_time = uniform_random(params->t3_min, params->t3_max);
                        if (req_to_process->type == 2)
                        {
                            service_time = uniform_random(params->t4_min, params->t4_max);
                        }

                        req_to_process->end_time = current_time + service_time;
                        oa_free_time = req_to_process->end_time;
                        last_processed_type = req_to_process->type;

                        if (req_to_process->type == 1)
                        {
                            processed_type1++;
                            total_wait_time_type1 += req_to_process->start_time - req_to_process->arrival_time;
                            
                            if (processed_type1 % 100 == 0)
                            {
                                double avg_q1 = queue_length_samples > 0 ? total_queue1_length / queue_length_samples : 0.0;
                                double avg_q2 = queue_length_samples > 0 ? total_queue2_length / queue_length_samples : 0.0;
                                double avg_w1 = processed_type1 > 0 ? total_wait_time_type1 / processed_type1 : 0.0;
                                double avg_w2 = processed_type2 > 0 ? total_wait_time_type2 / processed_type2 : 0.0;
                                print_periodic_stats_array(queue1.count, queue2.count, avg_q1, avg_q2, 
                                                          avg_w1, avg_w2, processed_type1);
                            }
                        }
                        else
                        {
                            processed_type2++;
                            total_wait_time_type2 += req_to_process->start_time - req_to_process->arrival_time;
                        }

                        free(req_to_process);
                    }
                }
                else
                {
                    // ОА простаивает - устанавливаем время освобождения
                    oa_free_time = current_time;
                }
            }
        }
        else if (next_event_type == EVENT_ARRIVAL_TYPE2)
        {
            // Приход заявки 2-го типа
            request* req = (request*)malloc(sizeof(request));
            if (!req)
            {
                printf("Ошибка выделения памяти для заявки 2-го типа\n");

            }

            req->type = 2;
            req->arrival_time = current_time;
            req->start_time = 0;
            req->end_time = 0;

            int push_result = queue_array_push_request(&queue2, req, 0);
            if (push_result == 0)
            {
                printf("КРИТИЧЕСКАЯ ОШИБКА: Очередь 2 переполнена! Симуляция остановлена.\n");
                free(req);
            }
            total_arrived_type2++;

            // Планируем следующее событие прихода заявки 2-го типа
            next_arrival_type2 = current_time + uniform_random(params->t2_min, params->t2_max);
            
            // Если ОА свободен, начинаем обработку заявки
            if (oa_free_time <= current_time)
            {
                request* req_to_process = NULL;
                int can_process = check_request_available_array(&queue1, &queue2, last_processed_type);
                
                if (can_process > 0)
                {
                    select_request_for_processing_array(&queue1, &queue2, &req_to_process, 
                                                         last_processed_type, 0);
                    
                    if (req_to_process != NULL)
                    {
                        req_to_process->start_time = current_time;
                        double service_time = uniform_random(params->t3_min, params->t3_max);
                        if (req_to_process->type == 2)
                        {
                            service_time = uniform_random(params->t4_min, params->t4_max);
                        }

                        req_to_process->end_time = current_time + service_time;
                        oa_free_time = req_to_process->end_time;
                        last_processed_type = req_to_process->type;

                        if (req_to_process->type == 1)
                        {
                            processed_type1++;
                            total_wait_time_type1 += req_to_process->start_time - req_to_process->arrival_time;
                            
                            if (processed_type1 % 100 == 0)
                            {
                                double avg_q1 = queue_length_samples > 0 ? total_queue1_length / queue_length_samples : 0.0;
                                double avg_q2 = queue_length_samples > 0 ? total_queue2_length / queue_length_samples : 0.0;
                                double avg_w1 = processed_type1 > 0 ? total_wait_time_type1 / processed_type1 : 0.0;
                                double avg_w2 = processed_type2 > 0 ? total_wait_time_type2 / processed_type2 : 0.0;
                                print_periodic_stats_array(queue1.count, queue2.count, avg_q1, avg_q2, 
                                                          avg_w1, avg_w2, processed_type1);
                            }
                        }
                        else
                        {
                            processed_type2++;
                            total_wait_time_type2 += req_to_process->start_time - req_to_process->arrival_time;
                        }

                        free(req_to_process);
                    }
                }
                else
                {
                    // ОА простаивает
                    if (oa_free_time < current_time)
                    {
                        idle_time += current_time - oa_free_time;
                    }
                    // Устанавливаем oa_free_time в 0, чтобы не генерировать событие DEPARTURE
                    oa_free_time = 0.0;
                }
            }
        }
        else if (next_event_type == EVENT_DEPARTURE)
        {
            // ОА освободился - начинаем обработку следующей заявки, если есть
            request* req = NULL;
            select_request_for_processing_array(&queue1, &queue2, &req, last_processed_type, 0);

            if (req != NULL)
            {
                // Начинаем обработку следующей заявки
                req->start_time = current_time;
                double service_time = uniform_random(params->t3_min, params->t3_max);
                if (req->type == 2)
                {
                    service_time = uniform_random(params->t4_min, params->t4_max);
                }

                req->end_time = current_time + service_time;
                oa_free_time = req->end_time;
                last_processed_type = req->type;

                if (req->type == 1)
                {
                    processed_type1++;
                    total_wait_time_type1 += req->start_time - req->arrival_time;
                    
                    if (processed_type1 % 100 == 0)
                    {
                        double avg_q1 = queue_length_samples > 0 ? total_queue1_length / queue_length_samples : 0.0;
                        double avg_q2 = queue_length_samples > 0 ? total_queue2_length / queue_length_samples : 0.0;
                        double avg_w1 = processed_type1 > 0 ? total_wait_time_type1 / processed_type1 : 0.0;
                        double avg_w2 = processed_type2 > 0 ? total_wait_time_type2 / processed_type2 : 0.0;
                        print_periodic_stats_array(queue1.count, queue2.count, avg_q1, avg_q2, 
                                                  avg_w1, avg_w2, processed_type1);
                    }
                }
                else
                {
                    processed_type2++;
                    total_wait_time_type2 += req->start_time - req->arrival_time;
                }

                free(req);
            }
            else
            {
                // Нет заявок - ОА простаивает
                // Учитываем простой, если ОА был свободен до этого момента
                if (oa_free_time < current_time)
                {
                    idle_time += current_time - oa_free_time;
                }
                // Устанавливаем oa_free_time в 0, чтобы не генерировать событие DEPARTURE
                oa_free_time = 0.0;
            }
        }
    }

    *out_model_time = current_time;
    *out_proc_type1 = processed_type1;
    *out_arr1 = total_arrived_type1;
    *out_proc_type2 = processed_type2;
    *out_arr2 = total_arrived_type2;
    *out_idle_time = idle_time;
    *out_avglen1 = queue_length_samples > 0 ? total_queue1_length / queue_length_samples : 0.0;
    *out_avglen2 = queue_length_samples > 0 ? total_queue2_length / queue_length_samples : 0.0;
    *out_avgwait1 = processed_type1 > 0 ? total_wait_time_type1 / processed_type1 : 0.0;
    *out_avgwait2 = processed_type2 > 0 ? total_wait_time_type2 / processed_type2 : 0.0;
}

