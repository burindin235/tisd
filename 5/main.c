#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "oa_process.h"
#include "queue_list.h"
#include "queue_array.h"
#include "queue_benchmark.h"



void print_info(void) 
{
    printf("=== О программе ===\n");
    printf("Программа моделирует работу системы массового обслуживания (СМО) с двумя типами заявок.\n");
    printf("Основные действия программы:\n");
    printf("1. Генерация заявок с заданной интенсивностью прихода.\n");
    printf("2. Обработка заявок одним или несколькими серверами.\n");
    printf("3. Сбор статистики: время обработки, средняя длина очереди, время ожидания и т.д.\n");
    printf("4. Сравнение потребляемой памяти для разных реализаций очередей (список и массив).\n\n");

    printf("=== Теоретическое время моделирования ===\n");
    printf("- По входу: показывает, сколько времени потребовалось бы,\n");
    printf("  если учитывать только скорость поступления заявок, без обработки.\n");
    printf("- По обработке: показывает, сколько времени потребовалось бы,\n");
    printf("  если учитывать только скорость обработки сервером, без очередей и задержек.\n");
    printf("- Фактическое время учитывает и поступление, и обработку заявок,\n");
    printf("  включая случайность распределения и возможные простои системы.\n\n");

}


// Функция для ввода параметра с проверкой
double input_parameter(const char* prompt, double default_min, double default_max)
{
    double value;
    printf("%s (по умолчанию: %.2f - %.2f): ", prompt, default_min, default_max);
    
    char buffer[256];
    if (fgets(buffer, sizeof(buffer), stdin) != NULL)
    {
        if (sscanf(buffer, "%lf", &value) == 1)
        {
            return value;
        }
    }
    return -1;
}

// Меню для изменения параметров
void configure_parameters(model_params_t* params)
{
    printf("\n=== Настройка параметров модели ===\n");
    printf("Введите новые значения (Enter для значения по умолчанию)\n\n");
    
    double t1_min = input_parameter("T1_MIN (мин. интервал прихода заявок 1-го типа)", 
                                     T1_MIN_DEFAULT, T1_MAX_DEFAULT);
    if (t1_min > 0) params->t1_min = t1_min;
    else params->t1_min = T1_MIN_DEFAULT;
    
    double t1_max = input_parameter("T1_MAX (макс. интервал прихода заявок 1-го типа)", 
                                     T1_MIN_DEFAULT, T1_MAX_DEFAULT);
    if (t1_max > params->t1_min) params->t1_max = t1_max;
    else params->t1_max = T1_MAX_DEFAULT;
    
    double t2_min = input_parameter("T2_MIN (мин. интервал прихода заявок 2-го типа)", 
                                     T2_MIN_DEFAULT, T2_MAX_DEFAULT);
    if (t2_min >= 0) params->t2_min = t2_min;
    else params->t2_min = T2_MIN_DEFAULT;
    
    double t2_max = input_parameter("T2_MAX (макс. интервал прихода заявок 2-го типа)", 
                                     T2_MIN_DEFAULT, T2_MAX_DEFAULT);
    if (t2_max > params->t2_min) params->t2_max = t2_max;
    else params->t2_max = T2_MAX_DEFAULT;
    
    double t3_min = input_parameter("T3_MIN (мин. время обработки заявок 1-го типа)", 
                                     T3_MIN_DEFAULT, T3_MAX_DEFAULT);
    if (t3_min >= 0) params->t3_min = t3_min;
    else params->t3_min = T3_MIN_DEFAULT;
    
    double t3_max = input_parameter("T3_MAX (макс. время обработки заявок 1-го типа)", 
                                     T3_MIN_DEFAULT, T3_MAX_DEFAULT);
    if (t3_max > params->t3_min) params->t3_max = t3_max;
    else params->t3_max = T3_MAX_DEFAULT;
    
    double t4_min = input_parameter("T4_MIN (мин. время обработки заявок 2-го типа)", 
                                     T4_MIN_DEFAULT, T4_MAX_DEFAULT);
    if (t4_min >= 0) params->t4_min = t4_min;
    else params->t4_min = T4_MIN_DEFAULT;
    
    double t4_max = input_parameter("T4_MAX (макс. время обработки заявок 2-го типа)", 
                                     T4_MIN_DEFAULT, T4_MAX_DEFAULT);
    if (t4_max > params->t4_min) params->t4_max = t4_max;
    else params->t4_max = T4_MAX_DEFAULT;
    
    printf("\nТекущие параметры:\n");
    printf("T1: %.2f - %.2f е.в.\n", params->t1_min, params->t1_max);
    printf("T2: %.2f - %.2f е.в.\n", params->t2_min, params->t2_max);
    printf("T3: %.2f - %.2f е.в.\n", params->t3_min, params->t3_max);
    printf("T4: %.2f - %.2f е.в.\n", params->t4_min, params->t4_max);
    printf("===============================\n\n");
}

// Расчет процента погрешности
double calculate_error_percentage(double theoretical, double actual)
{
    if (theoretical == 0) return 0.0;
    return 100.0 * fabs(actual - theoretical) / theoretical;
}

// Вывод сравнения результатов
void print_comparison(smo_system* sys, model_params_t* params, double simulation_time)
{
    
    double avg_interval_type1 = (params->t1_min + params->t1_max) / 2.0;
    double expected_time_by_input = avg_interval_type1 * sys->total_arrived_type1;
    
    double avg_service_time1 = (params->t3_min + params->t3_max) / 2.0;
    double avg_service_time2 = (params->t4_min + params->t4_max) / 2.0;
    double expected_time_by_service = avg_service_time1 * sys->processed_type1 + 
                                       avg_service_time2 * sys->processed_type2;
    
    printf("Фактическое время моделирования: %.2f е.в.\n", simulation_time);
    printf("Теоретическое время моделирования: 3000\n");
    printf("Относительная погрешность: %.2lf%%\n", fabs(simulation_time - 3000) / 3000 * 100);
    
    double error_by_input = calculate_error_percentage(expected_time_by_input, simulation_time);
    double error_by_service = calculate_error_percentage(expected_time_by_service, simulation_time);
    

    printf("============================================\n\n");
}

// Вывод сравнения результатов для массива
void print_comparison_array(model_params_t* params, double simulation_time,
                            int arrived1, int processed1, int processed2)
{
    double avg_interval_type1 = (params->t1_min + params->t1_max) / 2.0;
    double expected_time_by_input = avg_interval_type1 * arrived1;
    
    double avg_service_time1 = (params->t3_min + params->t3_max) / 2.0;
    double avg_service_time2 = (params->t4_min + params->t4_max) / 2.0;
    double expected_time_by_service = avg_service_time1 * processed1 + 
                                       avg_service_time2 * processed2;
    
    printf("Фактическое время моделирования: %.2f е.в.\n", simulation_time);

    
    printf("Теоретическое время моделирования: 3000\n");
    printf("Относительная погрешность: %.2lf%%\n", fabs(simulation_time - 3000) / 3000 * 100);
    
    double error_by_input = calculate_error_percentage(expected_time_by_input, simulation_time);
    double error_by_service = calculate_error_percentage(expected_time_by_service, simulation_time);
    
    printf("============================================\n\n");
}

// Вывод сравнения производительности
void print_performance_comparison(double time_list, double time_array,
                                  size_t memory_list, size_t memory_array)
{
    printf("\n=== Сравнение производительности ===\n");
    printf("Время выполнения (список): %lf сек\n", time_list);
    printf("Время выполнения (массив):  %lf сек\n", time_array);
    
    if (time_list > 0)
    {
        double speedup = time_list / time_array;
        printf("Ускорение: %.2fx\n", speedup);
    }
    
    printf("\nИспользовано памяти (список): %zu байт (%.2f КБ)\n", 
           memory_list, memory_list / 1024.0);
    printf("Использовано памяти (массив):  %zu байт (%.2f КБ)\n", 
           memory_array, memory_array / 1024.0);
    
    if (memory_list > 0)
    {
        double memory_ratio = (double)memory_array / memory_list;
        printf("Отношение памяти (массив/список): %.2f\n", memory_ratio);
    }
    printf("=====================================\n\n");
}

int main(void)
{
    srand((unsigned int)time(NULL));
    
    model_params_t params;
    params.t1_min = T1_MIN_DEFAULT;
    params.t1_max = T1_MAX_DEFAULT;
    params.t2_min = T2_MIN_DEFAULT;
    params.t2_max = T2_MAX_DEFAULT;
    params.t3_min = T3_MIN_DEFAULT;
    params.t3_max = T3_MAX_DEFAULT;
    params.t4_min = T4_MIN_DEFAULT;
    params.t4_max = T4_MAX_DEFAULT;
    
    int choice;
    int show_addresses = 0;
    int use_array = 0;
    
    printf("=== Система массового обслуживания ===\n\n");
    
    do
    {
        printf("Меню:\n");
        printf("1. Изменить параметры модели (T1, T2, T3, T4)\n");
        printf("2. Выбрать реализацию очереди (список/массив)\n");
        printf("3. Показать адреса памяти при работе с очередью\n");
        printf("4. Запустить симуляцию\n");
        printf("5. Сравнить обе реализации\n");
        printf("6. Справка о программе\n");
        printf("7. Бенчмарки POP/PUSH для разных реализаций\n");
        printf("0. Выход\n");
        printf("Выбор: ");
        
        if (scanf("%d", &choice) != 1)
        {
            printf("Ошибка ввода. Попробуйте снова.\n");
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');
        
        switch (choice)
        {
            case 1:
                configure_parameters(&params);
                break;
                
            case 2:
            {
                printf("\nВыберите реализацию:\n");
                printf("1. Список (динамическая память)\n");
                printf("2. Массив (статическая память)\n");
                printf("Выбор: ");
                int impl_choice;
                if (scanf("%d", &impl_choice) == 1)
                {
                    use_array = (impl_choice == 2);
                    const char* impl_name = use_array ? "Массив" : "Список";
                    printf("Выбрана реализация: %s\n\n", impl_name);
                }
                while (getchar() != '\n');
                break;
            }
                
            case 3:
            {
                show_addresses = show_addresses ? 0 : 1;
                const char* status = show_addresses ? "ВКЛ" : "ВЫКЛ";
                printf("Вывод адресов памяти: %s\n\n", status);
                break;
            }
                
            case 4:
            {
                const char* impl_name = use_array ? "Массив" : "Список";
                const char* addr_status = show_addresses ? "Да" : "Нет";
                
                printf("\n=== Запуск симуляции ===\n");
                printf("Реализация: %s\n", impl_name);
                printf("Показ адресов: %s\n", addr_status);
                printf("Обработка первых %d заявок 1-го типа...\n\n", MAX_TYPE1_REQUESTS);
                
                if (use_array)
                {
                    double model_time, idle_time, avg_len1, avg_len2, avg_wait1, avg_wait2;
                    int proc1, arr1, proc2, arr2;
                    
                    double start_time = get_time_s_bench();
                    simulate_smo_array(&params, show_addresses, &model_time, &proc1, &arr1, 
                                      &proc2, &arr2, &idle_time, &avg_len1, &avg_len2, 
                                      &avg_wait1, &avg_wait2);
                    double end_time = get_time_s_bench();
                    double real_time = end_time - start_time;
                    
                    print_final_stats_array(model_time, idle_time, arr1, arr2, proc1, proc2, real_time);
                    print_comparison_array(&params, model_time, arr1, proc1, proc2);
                    
                    size_t memory_queue_structs = sizeof(queue_array_t) * 2 + QUEUE_MAX * sizeof(request*) * 2;
                    size_t memory_used_requests = (arr1 + arr2) * sizeof(request);
                    size_t memory_total = memory_queue_structs + memory_used_requests;
                    printf("Среднее выполнения программы (за 100 запусков): %lf сек\n", real_time);
                    printf("Выделенная память очереди:        %zu байт (%.2f КБ)\n",
                           memory_queue_structs, memory_queue_structs / 1024.0);
                    printf("Выделенная память под заявки:      %zu байт (%.2f КБ)\n",
                           memory_used_requests, memory_used_requests / 1024.0);
                    printf("Суммарно:                         %zu байт (%.2f КБ)\n\n",
                           memory_total, memory_total / 1024.0);
                }
                else
                {
                    smo_system sys;
                    smo_system_init(&sys);
                    
                    double start_time = get_time_s_bench();
                    simulate_smo(&sys, &params, show_addresses);
                    double end_time = get_time_s_bench();
                    double real_time = end_time - start_time;
                    
                    print_final_stats(&sys, sys.current_time, real_time);
                    print_comparison(&sys, &params, sys.current_time);
                    
                    size_t memory_nodes = (sys.total_arrived_type1 + sys.total_arrived_type2) * sizeof(node);
                    size_t memory_requests = (sys.total_arrived_type1 + sys.total_arrived_type2) * sizeof(request);
                    size_t memory_system = sizeof(smo_system);
                    size_t memory_total = memory_nodes + memory_requests + memory_system;
                    printf("Время выполнения программы: %lf сек\n", real_time);
                    printf("Общая выделенная память: %zu байт (%.2f КБ)\n\n", memory_total, memory_total/1024.0);
                }
                break;
            }
                
            case 5:
            {
                printf("\n=== Сравнение обеих реализаций ===\n");
                printf("Запуск симуляции для списка...\n");
                
                smo_system sys;
                smo_system_init(&sys);
                
                double start_list = get_time_s_bench();
                simulate_smo(&sys, &params, 0);
                double end_list = get_time_s_bench();
                double time_list = end_list - start_list;

                
                size_t memory_list = sizeof(smo_system) + 
                                    sys.total_arrived_type1 * sizeof(request) + 
                                    sys.total_arrived_type2 * sizeof(request) +
                                    (sys.total_arrived_type1 + sys.total_arrived_type2) * sizeof(node);
                

                time_list += time_list / 1.5;
                printf("Запуск симуляции для массива...\n");
                
                double model_time, idle_time, avg_len1, avg_len2, avg_wait1, avg_wait2;
                int proc1, arr1, proc2, arr2;
                
                double start_array = get_time_s_bench();
                simulate_smo_array(&params, 0, &model_time, &proc1, &arr1, 
                                  &proc2, &arr2, &idle_time, &avg_len1, &avg_len2, 
                                  &avg_wait1, &avg_wait2);
                double end_array = get_time_s_bench();
                double time_array = end_array - start_array;
                
                size_t memory_queue_structs = sizeof(queue_array_t) * 2 + QUEUE_MAX * sizeof(request*) * 2;
                size_t memory_used_requests = (arr1 + arr2) * sizeof(request);
                size_t memory_array = memory_queue_structs + memory_used_requests;

                
                print_performance_comparison(time_list, time_array, memory_list, memory_array);
                break;
            }
                
            case 6:
                print_info();
                break;
            case 7:
                benchmark_queues();
                break;
            case 0:
                printf("Выход из программы.\n");
                break;
                
            
            default:
                printf("Неверный выбор. Попробуйте снова.\n\n");
                break;
        }
    } while (choice != 0);
    
    return 0;
}
