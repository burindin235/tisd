#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "dynamic_stack.h"
#include "list_stack.h"
#include "stack_static.h"

#define PUSH_POP_REPEAT 100000
#define EVAL_REPEAT 100


// неправильное время замера с динамикой увеличивать размер в 2 раза. огранчить для проверки переполнения
void silent_stack_static_push(static_stack *stack, int value);
int silent_stack_static_pop(static_stack *stack);

void silent_stack_dynamic_push(dynamic_stack *stack, int value);
int silent_stack_dynamic_pop(dynamic_stack *stack);

void silent_list_stack_push(list_stack *stack, int value);
int silent_list_stack_pop(list_stack *stack);

double get_time_s(void);


void bench_static_push(int value);
void bench_static_pop(int value);
void bench_static_eval(const char *expr);

void bench_dynamic_push(int value);
void bench_dynamic_pop(int value);
void bench_dynamic_eval(const char *expr);

void bench_list_push(int value);
void bench_list_pop(int value);
void bench_list_eval(const char *expr);

// Комплексные замеры (старый стиль)
void comprehensive_benchmark(void);

// Новый комплексный бенчмарк с расчётом памяти
void comparative_benchmark(void);

#endif
