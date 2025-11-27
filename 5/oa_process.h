#ifndef SMO_SYSTEM_H
#define SMO_SYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "queue_list.h"
#include "request.h"

// Параметры модели (по умолчанию)
#define T1_MIN_DEFAULT 1.0
#define T1_MAX_DEFAULT 5.0
#define T2_MIN_DEFAULT 0.0
#define T2_MAX_DEFAULT 3.0
#define T3_MIN_DEFAULT 0.0
#define T3_MAX_DEFAULT 4.0
#define T4_MIN_DEFAULT 0.0
#define T4_MAX_DEFAULT 1.0
#define MAX_TYPE1_REQUESTS 1000

// Структура системы массового обслуживания
typedef struct smo_system
{
    double current_time;     // текущее модельное время
    double oa_free_time;     // время, когда ОА станет свободен
    int processed_type1;      // количество обработанных заявок 1-го типа
    int processed_type2;      // количество обработанных заявок 2-го типа
    double idle_time;        // общее время простоя ОА
    int last_processed_type;  // тип последней обработанной заявки (0 = нет, 1 или 2)

    // Очереди заявок
    queue_list queue1;       // очередь заявок 1-го типа
    queue_list queue2;       // очередь заявок 2-го типа

    // статистика для средних значений
    double total_wait_time_type1;
    double total_wait_time_type2;
    int total_arrived_type1;
    int total_arrived_type2;
    
    // Статистика для средних длин очередей
    double total_queue1_length;
    double total_queue2_length;
    int queue_length_samples; // количество измерений длины очереди
} smo_system;

// Типы событий
typedef enum
{
    EVENT_ARRIVAL_TYPE1,   // Приход заявки 1-го типа
    EVENT_ARRIVAL_TYPE2,   // Приход заявки 2-го типа
    EVENT_DEPARTURE        // Окончание обработки на аппарате
} event_type;

// Структура следующего события
typedef struct
{
    event_type type;   // Тип события
    double time;       // Время наступления события
} next_event_t;

// Параметры модели
typedef struct
{
    double t1_min, t1_max;  // интервал прихода заявок 1-го типа
    double t2_min, t2_max;  // интервал прихода заявок 2-го типа
    double t3_min, t3_max;  // время обработки заявок 1-го типа
    double t4_min, t4_max;  // время обработки заявок 2-го типа
} model_params_t;

// Генератор равномерного случайного числа
double uniform_random(double t_min, double t_max);

// Генерация заявки с заданным типом и предыдущим временем прихода
request generate_request(int type, double previous_arrival, double t_min, double t_max);

// Инициализация системы массового обслуживания
void smo_system_init(smo_system* sys);

// Определение следующего события среди прихода заявок и окончания обработки
next_event_t get_next_event(next_event_t ev1, next_event_t ev2, double oa_free_time);

// Функция измерения текущего реального времени в секундах
double get_time_s_bench(void);

// Функция моделирования системы массового обслуживания
void simulate_smo(smo_system* sys, model_params_t* params, int show_addresses);

// Вывод статистики каждые 100 заявок
void print_periodic_stats(smo_system* sys, int processed_count);

// Вывод финальной статистики
void print_final_stats(smo_system* sys, double simulation_time, double real_time);

// Функция моделирования системы массового обслуживания (массив)
void simulate_smo_array(model_params_t* params, int show_addresses, 
                        double* out_model_time, int* out_proc_type1, int* out_arr1, 
                        int* out_proc_type2, int* out_arr2, double* out_idle_time, 
                        double* out_avglen1, double* out_avglen2, 
                        double* out_avgwait1, double* out_avgwait2);

// Вывод финальной статистики для массива
void print_final_stats_array(double simulation_time, double idle_time, 
                             int arrived1, int arrived2, int processed1, int processed2,
                             double real_time);

// Вывод периодической статистики для массива
void print_periodic_stats_array(int q1_len, int q2_len, double avg_q1, double avg_q2,
                                double avg_wait1, double avg_wait2, int processed_count);

#endif // SMO_SYSTEM_H

