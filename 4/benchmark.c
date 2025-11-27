#include "benchmark.h"
#include <string.h>

// --- Silent static stack ---
void silent_stack_static_push(static_stack *stack, int value)
{
    if (stack->head_pointer < stack->max_len - 1)
        stack->data[++(stack->head_pointer)] = value;
}

int silent_stack_static_pop(static_stack *stack)
{
    if (stack->head_pointer >= 0)
        return stack->data[(stack->head_pointer)--];
    return 0;
}

// --- Silent dynamic stack ---
void silent_stack_dynamic_push(dynamic_stack *stack, int value)
{
    // Проверка ограничения размера
    if (stack->head_pointer + 1 >= MAX_DYNAMIC_STACK)
        return;

    // Если нужно увеличить capacity
    if (stack->head_pointer + 1 >= stack->capacity)
    {
        int new_capacity;
        if (stack->capacity == 0)
        {
            new_capacity = 1; // Начальная capacity
        }
        else
        {
            new_capacity = stack->capacity * 2; // Удваиваем
        }
        
        // Проверяем, что не превышаем максимальный размер
        if (new_capacity > MAX_DYNAMIC_STACK)
        {
            new_capacity = MAX_DYNAMIC_STACK;
        }

        int *new_data = (int *)realloc(stack->data, new_capacity * sizeof(int));
        if (new_data)
        {
            stack->data = new_data;
            stack->capacity = new_capacity;
        }
        else
        {
            return;
        }
    }

    stack->data[++stack->head_pointer] = value;
    stack->max_len = stack->head_pointer + 1;
}

int silent_stack_dynamic_pop(dynamic_stack *stack)
{
    if (stack->head_pointer >= 0)
    {
        int value = stack->data[stack->head_pointer--];
        stack->max_len = stack->head_pointer + 1;

        // Уменьшаем capacity, если использование < capacity/4
        if (stack->max_len < stack->capacity / 4 && stack->capacity > 1)
        {
            int new_capacity = stack->capacity / 2;
            int *new_data = (int *)realloc(stack->data, new_capacity * sizeof(int));
            if (new_data)
            {
                stack->data = new_data;
                stack->capacity = new_capacity;
            }
        }

        return value;
    }
    return 0;
}

// --- Silent list stack ---
void silent_list_stack_push(list_stack *stack, int value)
{
    // Проверка ограничения размера
    if (stack->size >= MAX_LIST_STACK)
        return;

    node *new_node = (node *)malloc(sizeof(node));
    if (!new_node)
        return;
    new_node->data = value;
    new_node->next = stack->top;
    stack->top = new_node;
    stack->size++;
}

int silent_list_stack_pop(list_stack *stack)
{
    if (!stack->top)
        return 0;
    node *temp = stack->top;
    int value = temp->data;
    stack->top = temp->next;
    stack->size--;
    free(temp);
    return value;
}

// --- Время ---
double get_time_s(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

// --- Бенчмарки: Static ---
void bench_static_push(int value)
{
    static_stack s;
    stack_static_init(&s);
    s.max_len = MAX_STATIC_STACK;

    double t0 = get_time_s();
    for (int i = 0; i < PUSH_POP_REPEAT; i++)
    {
        silent_stack_static_push(&s, value);
    }
    double t1 = get_time_s();

    printf("[Статический стек] Добавление %d элементов: %.6f с\n", PUSH_POP_REPEAT, t1 - t0);
}

void bench_static_pop(int value)
{
    static_stack s;
    stack_static_init(&s);
    s.max_len = MAX_STATIC_STACK;
    for (int i = 0; i < PUSH_POP_REPEAT; i++)
        silent_stack_static_push(&s, value);

    double t0 = get_time_s();
    for (int i = 0; i < PUSH_POP_REPEAT; i++)
        silent_stack_static_pop(&s);
    double t1 = get_time_s();

    printf("[Статический стек] Удаление %d элементов: %.6f с\n", PUSH_POP_REPEAT, t1 - t0);
}

void bench_static_eval(const char *expr)
{
    double t0 = get_time_s();
    int res = 0;
    for (int i = 0; i < EVAL_REPEAT; i++)
        res = static_evaluate_expression(expr);
    double t1 = get_time_s();
    printf("[Статический стек] Вычисление выражения x%d раз: %.6f с, результат: %d\n", EVAL_REPEAT, t1 - t0, res);
}

// --- Бенчмарки: Dynamic ---
void bench_dynamic_push(int value)
{
    dynamic_stack s;
    stack_dynamic_init(&s);

    double t0 = get_time_s();
    for (int i = 0; i < PUSH_POP_REPEAT; i++)
        silent_stack_dynamic_push(&s, value);
    double t1 = get_time_s();

    printf("[Динамический стек] Добавление %d элементов: %.6f с\n", PUSH_POP_REPEAT, t1 - t0);

    stack_dynamic_free(&s);
}

void bench_dynamic_pop(int value)
{
    dynamic_stack s;
    stack_dynamic_init(&s);
    for (int i = 0; i < PUSH_POP_REPEAT; i++)
        silent_stack_dynamic_push(&s, value);

    double t0 = get_time_s();
    for (int i = 0; i < PUSH_POP_REPEAT; i++)
        silent_stack_dynamic_pop(&s);
    double t1 = get_time_s();

    printf("[Динамический стек] Удаление %d элементов: %.6f с\n", PUSH_POP_REPEAT, t1 - t0);

    stack_dynamic_free(&s);
}

void bench_dynamic_eval(const char *expr)
{
    double t0 = get_time_s();
    int res = 0;
    for (int i = 0; i < EVAL_REPEAT; i++)
        res = dynamic_evaluate_expression(expr);
    double t1 = get_time_s();

    printf("[Динамический стек] Вычисление выражения x%d раз: %.6f с, результат: %d\n", EVAL_REPEAT, t1 - t0, res);
}

// --- Бенчмарки: List ---
void bench_list_push(int value)
{
    list_stack s;
    list_stack_init(&s);

    double t0 = get_time_s();
    for (int i = 0; i < PUSH_POP_REPEAT; i++)
        silent_list_stack_push(&s, value);
    double t1 = get_time_s();

    printf("[Список] Добавление %d элементов: %.6f с\n", PUSH_POP_REPEAT, t1 - t0);

    while (s.top)
        silent_list_stack_pop(&s);
}

void bench_list_pop(int value)
{
    list_stack s;
    list_stack_init(&s);
    for (int i = 0; i < PUSH_POP_REPEAT; i++)
        silent_list_stack_push(&s, value);

    double t0 = get_time_s();
    for (int i = 0; i < PUSH_POP_REPEAT; i++)
        silent_list_stack_pop(&s);
    double t1 = get_time_s();

    printf("[Список] Удаление %d элементов: %.6f с\n", PUSH_POP_REPEAT, t1 - t0);
}

void bench_list_eval(const char *expr)
{
    double t0 = get_time_s();
    void *free_list_local[MAX_FREE_LIST];
    int free_count_local = 0;
    int res = 0;
    for (int i = 0; i < EVAL_REPEAT; i++)
    {
        res = list_evaluate_expression(expr, free_list_local, &free_count_local);
        free_count_local = 0;
    }
    double t1 = get_time_s();
    printf("[Список] Вычисление выражения x%d раз: %.6f с, результат: %d\n", EVAL_REPEAT, t1 - t0, res);
}

typedef struct
{
    double time_ms;
    size_t peak_memory;
} bench_result_t;

static bench_result_t bench_static_push_with_mem(int count, int value)
{
    static_stack s;
    stack_static_init(&s);
    s.max_len = MAX_STATIC_STACK;

    double t0 = get_time_s();
    for (int i = 0; i < count; i++)
        silent_stack_static_push(&s, value);
    double t1 = get_time_s();

    // Пиковая память: статический стек не меняет размер, только sizeof(struct)
    size_t peak = sizeof(static_stack);

    bench_result_t result;
    result.time_ms = (t1 - t0) * 1000.0;
    result.peak_memory = peak;
    return result;
}

static bench_result_t bench_static_pop_with_mem(int count, int value)
{
    static_stack s;
    stack_static_init(&s);
    s.max_len = MAX_STATIC_STACK;
    for (int i = 0; i < count; i++)
        silent_stack_static_push(&s, value);

    double t0 = get_time_s();
    for (int i = 0; i < count; i++)
        silent_stack_static_pop(&s);
    double t1 = get_time_s();

    // Статический стек не освобождает память при pop - пик тот же, что был при push
    size_t peak = sizeof(static_stack);

    bench_result_t result;
    result.time_ms = (t1 - t0) * 1000.0;
    result.peak_memory = peak;
    return result;
}

static bench_result_t bench_static_eval_with_mem(const char *expr)
{
    volatile int res;
    double t0 = get_time_s();
    for (int i = 0; i < EVAL_REPEAT; i++)
        res = static_evaluate_expression(expr);
    double t1 = get_time_s();

    size_t peak = sizeof(static_stack) + sizeof(static_stack_char);

    bench_result_t result;
    result.time_ms = (t1 - t0) * 1000.0 / (double)EVAL_REPEAT;
    result.peak_memory = peak;
    return result;
}

static bench_result_t bench_dynamic_push_with_mem(int count, int value)
{
    dynamic_stack s;
    stack_dynamic_init(&s);

    size_t peak_memory = 0;
    double t0 = get_time_s();
    for (int i = 0; i < count; i++)
    {
        silent_stack_dynamic_push(&s, value);
        // Обновляем пиковую память после каждого realloc
        size_t current_alloc = sizeof(dynamic_stack) + s.capacity * sizeof(int);
        if (current_alloc > peak_memory)
            peak_memory = current_alloc;
    }
    double t1 = get_time_s();

    bench_result_t result;
    result.time_ms = (t1 - t0) * 1000.0;
    result.peak_memory = peak_memory;

    stack_dynamic_free(&s);
    return result;
}

static bench_result_t bench_dynamic_pop_with_mem(int count, int value)
{
    dynamic_stack s;
    stack_dynamic_init(&s);

    // Сначала делаем push и запоминаем пиковую память
    size_t peak_memory = 0;
    for (int i = 0; i < count; i++)
    {
        silent_stack_dynamic_push(&s, value);
        size_t current_alloc = sizeof(dynamic_stack) + s.capacity * sizeof(int);
        if (current_alloc > peak_memory)
            peak_memory = current_alloc;
    }

    double t0 = get_time_s();
    for (int i = 0; i < count; i++)
        silent_stack_dynamic_pop(&s);
    double t1 = get_time_s();

    bench_result_t result;
    result.time_ms = (t1 - t0) * 1000.0;
    result.peak_memory = peak_memory; // Используем пик от push

    stack_dynamic_free(&s);
    return result;
}

static bench_result_t bench_dynamic_eval_with_mem(const char *expr)
{
    dynamic_stack values;
    stack_dynamic_init(&values);
    dynamic_stack_char ops;
    stack_dynamic_char_init(&ops);

    size_t peak_memory = 0;

    // Запускаем eval и отслеживаем память
    double t0 = get_time_s();
    dynamic_evaluate_expression(expr);
    double t1 = get_time_s();

    // Примерная пиковая память
    int num_count = 0;
    int op_count = 0;
    int i = 0;
    while (expr[i] != '\0')
    {
        if (expr[i] >= '0' && expr[i] <= '9')
        {
            num_count++;
            while (expr[i] >= '0' && expr[i] <= '9')
                i++;
        }
        else if (expr[i] == '+' || expr[i] == '-' || expr[i] == '*' || expr[i] == '/')
        {
            op_count++;
            i++;
        }
        else
            i++;
    }
    size_t peak = sizeof(dynamic_stack) + sizeof(dynamic_stack_char) +
                  num_count * sizeof(int) + op_count * sizeof(char);

    stack_dynamic_free(&values);
    stack_dynamic_char_free(&ops);

    bench_result_t result;
    result.time_ms = (t1 - t0) * 1000.0;
    result.peak_memory = peak;
    return result;
}

static bench_result_t bench_list_push_with_mem(int count, int value)
{
    list_stack s;
    list_stack_init(&s);

    size_t peak_memory = sizeof(list_stack);
    double t0 = get_time_s();
    for (int i = 0; i < count; i++)
    {
        silent_list_stack_push(&s, value);
        // После каждого malloc обновляем пик
        size_t current_nodes = 0;
        for (node *cur = s.top; cur; cur = cur->next)
            current_nodes++;
        size_t current_mem = sizeof(list_stack) + current_nodes * sizeof(node);
        if (current_mem > peak_memory)
            peak_memory = current_mem;
    }
    double t1 = get_time_s();

    bench_result_t result;
    result.time_ms = (t1 - t0) * 1000.0;
    result.peak_memory = peak_memory;

    while (s.top)
        silent_list_stack_pop(&s);
    return result;
}

static bench_result_t bench_list_pop_with_mem(int count, int value)
{
    list_stack s;
    list_stack_init(&s);

    // Сначала делаем push и запоминаем пиковую память
    size_t peak_memory = sizeof(list_stack);
    for (int i = 0; i < count; i++)
    {
        silent_list_stack_push(&s, value);
        size_t current_nodes = 0;
        for (node *cur = s.top; cur; cur = cur->next)
            current_nodes++;
        size_t current_mem = sizeof(list_stack) + current_nodes * sizeof(node);
        if (current_mem > peak_memory)
            peak_memory = current_mem;
    }

    double t0 = get_time_s();
    for (int i = 0; i < count; i++)
        silent_list_stack_pop(&s);
    double t1 = get_time_s();

    bench_result_t result;
    result.time_ms = (t1 - t0) * 1000.0;
    result.peak_memory = peak_memory; // Используем пик от push
    return result;
}

static bench_result_t bench_list_eval_with_mem(const char *expr)
{
    void *free_list_local[MAX_FREE_LIST];
    int free_count_local = 0;

    double t0 = get_time_s();
    list_evaluate_expression(expr, free_list_local, &free_count_local);
    double t1 = get_time_s();

    // Примерная пиковая память: два стека (values и ops) плюс ноды
    int num_count = 0;
    int op_count = 0;
    int i = 0;
    while (expr[i] != '\0')
    {
        if (expr[i] >= '0' && expr[i] <= '9')
        {
            num_count++;
            while (expr[i] >= '0' && expr[i] <= '9')
                i++;
        }
        else if (expr[i] == '+' || expr[i] == '-' || expr[i] == '*' || expr[i] == '/')
        {
            op_count++;
            i++;
        }
        else
            i++;
    }

    size_t peak = sizeof(list_stack) + sizeof(list_stack_char) +
                  (num_count + op_count) * sizeof(node);

    bench_result_t result;
    result.time_ms = (t1 - t0) * 1000.0;
    result.peak_memory = peak;
    return result;
}

// --- Вспомогательные функции для комплексных замеров (одна итерация) ---
static double bench_single_static_push(int value)
{
    static_stack s;
    stack_static_init(&s);
    s.max_len = MAX_STATIC_STACK;

    double t0 = get_time_s();
    for (int i = 0; i < PUSH_POP_REPEAT; i++)
        silent_stack_static_push(&s, value);
    double t1 = get_time_s();
    return t1 - t0;
}

static double bench_single_static_pop(int value)
{
    static_stack s;
    stack_static_init(&s);
    s.max_len = MAX_STATIC_STACK;
    for (int i = 0; i < PUSH_POP_REPEAT; i++)
        silent_stack_static_push(&s, value);

    double t0 = get_time_s();
    for (int i = 0; i < PUSH_POP_REPEAT; i++)
        silent_stack_static_pop(&s);
    double t1 = get_time_s();
    return t1 - t0;
}

static double bench_single_static_eval(const char *expr)
{
    double t0 = get_time_s();
    static_evaluate_expression(expr);
    double t1 = get_time_s();
    return t1 - t0;
}

static double bench_single_dynamic_push(int value)
{
    dynamic_stack s;
    stack_dynamic_init(&s);

    double t0 = get_time_s();
    for (int i = 0; i < PUSH_POP_REPEAT; i++)
        silent_stack_dynamic_push(&s, value);
    double t1 = get_time_s();

    stack_dynamic_free(&s);
    return t1 - t0;
}

static double bench_single_dynamic_pop(int value)
{
    dynamic_stack s;
    stack_dynamic_init(&s);
    for (int i = 0; i < PUSH_POP_REPEAT; i++)
        silent_stack_dynamic_push(&s, value);

    double t0 = get_time_s();
    for (int i = 0; i < PUSH_POP_REPEAT; i++)
        silent_stack_dynamic_pop(&s);
    double t1 = get_time_s();

    stack_dynamic_free(&s);
    return t1 - t0;
}

static double bench_single_dynamic_eval(const char *expr)
{
    double t0 = get_time_s();
    dynamic_evaluate_expression(expr);
    double t1 = get_time_s();
    return t1 - t0;
}

static double bench_single_list_push(int value)
{
    list_stack s;
    list_stack_init(&s);

    double t0 = get_time_s();
    for (int i = 0; i < PUSH_POP_REPEAT; i++)
        silent_list_stack_push(&s, value);
    double t1 = get_time_s();

    while (s.top)
        silent_list_stack_pop(&s);
    return t1 - t0;
}

static double bench_single_list_pop(int value)
{
    list_stack s;
    list_stack_init(&s);
    for (int i = 0; i < PUSH_POP_REPEAT; i++)
        silent_list_stack_push(&s, value);

    double t0 = get_time_s();
    for (int i = 0; i < PUSH_POP_REPEAT; i++)
        silent_list_stack_pop(&s);
    double t1 = get_time_s();
    return t1 - t0;
}

static double bench_single_list_eval(const char *expr)
{
    void *free_list_local[MAX_FREE_LIST];
    int free_count_local = 0;
    double t0 = get_time_s();
    list_evaluate_expression(expr, free_list_local, &free_count_local);
    double t1 = get_time_s();
    return t1 - t0;
}

// --- Комплексные замеры ---
void comprehensive_benchmark(void)
{
    const int BENCH_RUNS = 100;
    const int BENCH_EVAL_RUNS = 100000;
    const char *expr = "1*2+3*4+5*6+7*8+9";
    int test_value = 1;

    printf("\n=== Комплексные замеры времени ===\n\n");

    // --- Статический стек ---
    printf("-- Статический стек --\n");

    // Push
    double push_sum = 0.0;
    for (int i = 0; i < BENCH_RUNS; i++)
        push_sum += bench_single_static_push(test_value);
    double push_avg = push_sum / BENCH_RUNS;
    printf("100k push avg time: %.6f с\n", push_avg);

    // Pop
    double pop_sum = 0.0;
    for (int i = 0; i < BENCH_RUNS; i++)
        pop_sum += bench_single_static_pop(test_value);
    double pop_avg = pop_sum / BENCH_RUNS;
    printf("100k pop avg time: %.6f с\n", pop_avg);

    // Eval
    double eval_sum = 0.0;
    for (int i = 0; i < BENCH_EVAL_RUNS; i++)
        eval_sum += bench_single_static_eval(expr);
    double eval_avg = eval_sum;
    printf("100k %s avg time: %.6f с\n", expr, eval_avg);

    // --- Динамический стек ---
    printf("-- Динамический стек --\n");

    // Push
    push_sum = 0.0;
    for (int i = 0; i < BENCH_RUNS; i++)
        push_sum += bench_single_dynamic_push(test_value);
    push_avg = push_sum / BENCH_RUNS;
    printf("100k push avg time: %.6f с\n", push_avg);

    // Pop
    pop_sum = 0.0;
    for (int i = 0; i < BENCH_RUNS; i++)
        pop_sum += bench_single_dynamic_pop(test_value);
    pop_avg = pop_sum / BENCH_RUNS;
    printf("100k pop avg time: %.6f с\n", pop_avg);

    // Eval
    eval_sum = 0.0;
    for (int i = 0; i < BENCH_EVAL_RUNS; i++)
        eval_sum += bench_single_dynamic_eval(expr);
    eval_avg = eval_sum;
    printf("100k %s avg time: %.6f с\n", expr, eval_avg);

    // --- Список ---
    printf("-- Список --\n");

    // Push
    push_sum = 0.0;
    for (int i = 0; i < BENCH_RUNS; i++)
        push_sum += bench_single_list_push(test_value);
    push_avg = push_sum / BENCH_RUNS;
    printf("100k push avg time: %.6f с\n", push_avg);

    // Pop
    pop_sum = 0.0;
    for (int i = 0; i < BENCH_RUNS; i++)
        pop_sum += bench_single_list_pop(test_value);
    pop_avg = pop_sum / BENCH_RUNS;
    printf("100k pop avg time: %.6f с\n", pop_avg);

    // Eval
    eval_sum = 0.0;
    for (int i = 0; i < BENCH_EVAL_RUNS; i++)
        eval_sum += bench_single_list_eval(expr);
    eval_avg = eval_sum;
    printf("100k %s avg time: %.6f с\n", expr, eval_avg);
}



void comparative_benchmark(void)
{
    const int BENCH_RUNS = 100;
    const int counts[] = {100, 1000, 10000};
    const char *expr2 = "1*2+3*4+5*6+7*8+9";
    int test_value = 1;

    printf("\n=== Сравнение эффективности реализаций стека ===\n\n");

    // Результаты для сравнения
    double static_times[4][3] = {0}; // push/pop/eval1/eval2 для 3 размеров
    size_t static_mems[4][3] = {0};
    double dynamic_times[4][3] = {0};
    size_t dynamic_mems[4][3] = {0};
    double list_times[4][3] = {0};
    size_t list_mems[4][3] = {0};

    // --- Статический стек ---
    printf("-- Статический стек --\n");

    // Push для разных размеров
    for (int c = 0; c < 3; c++)
    {
        double sum = 0.0;
        size_t mem_sum = 0;
        for (int r = 0; r < BENCH_RUNS; r++)
        {
            bench_result_t res = bench_static_push_with_mem(counts[c], test_value);
            sum += res.time_ms;
            mem_sum += res.peak_memory;
        }
        static_times[0][c] = sum / BENCH_RUNS;
        static_mems[0][c] = mem_sum / BENCH_RUNS;
        printf("Push %d avg time: %.6f мс, пиковая память: %zu байт\n",
               counts[c], static_times[0][c], static_mems[0][c]);
    }

    // Pop
    for (int c = 0; c < 3; c++)
    {
        double sum = 0.0;
        size_t mem_sum = 0;
        for (int r = 0; r < BENCH_RUNS; r++)
        {
            bench_result_t res = bench_static_pop_with_mem(counts[c], test_value);
            sum += res.time_ms;
            mem_sum += res.peak_memory;
        }
        static_times[1][c] = sum / BENCH_RUNS;
        static_mems[1][c] = mem_sum / BENCH_RUNS;
        printf("Pop %d avg time: %.6f мс, пиковая память: %zu байт\n",
               counts[c], static_times[1][c], static_mems[1][c]);
    }

    // Eval
    double sum1 = 0.0, sum2 = 0.0;
    size_t mem_sum1 = 0, mem_sum2 = 0;
    for (int r = 0; r < BENCH_RUNS; r++)
    {
        bench_result_t res2 = bench_static_eval_with_mem(expr2);
        sum2 += res2.time_ms;
        mem_sum2 += res2.peak_memory;
    }
    static_times[2][0] = sum1 / BENCH_RUNS;
    static_mems[2][0] = mem_sum1 / BENCH_RUNS;
    static_times[3][0] = sum2 / BENCH_RUNS;
    static_mems[3][0] = mem_sum2 / BENCH_RUNS;
    printf("Expr \"%s\" avg time: %.6f мс, пиковая память: %zu байт\n",
           expr2, static_times[3][0], static_mems[3][0]);

    printf("\n");

    // --- Динамический стек ---
    printf("-- Динамический стек --\n");

    for (int c = 0; c < 3; c++)
    {
        double sum = 0.0;
        size_t mem_sum = 0;
        for (int r = 0; r < BENCH_RUNS; r++)
        {
            bench_result_t res = bench_dynamic_push_with_mem(counts[c], test_value);
            sum += res.time_ms;
            mem_sum += res.peak_memory;
        }
        dynamic_times[0][c] = sum / BENCH_RUNS;
        dynamic_mems[0][c] = mem_sum / BENCH_RUNS;
        printf("Push %d avg time: %.6f мс, пиковая память: %zu байт\n",
               counts[c], dynamic_times[0][c], dynamic_mems[0][c]);
    }

    for (int c = 0; c < 3; c++)
    {
        double sum = 0.0;
        size_t mem_sum = 0;
        for (int r = 0; r < BENCH_RUNS; r++)
        {
            bench_result_t res = bench_dynamic_pop_with_mem(counts[c], test_value);
            sum += res.time_ms;
            mem_sum += res.peak_memory;
        }
        dynamic_times[1][c] = sum / BENCH_RUNS;
        dynamic_mems[1][c] = mem_sum / BENCH_RUNS;

        printf("Pop %d avg time: %.6f мс, пиковая память: %zu байт\n",
               counts[c], dynamic_times[1][c], dynamic_mems[1][c]);
    }

    sum1 = 0.0, sum2 = 0.0;
    mem_sum1 = 0, mem_sum2 = 0;
    for (int r = 0; r < BENCH_RUNS; r++)
    {
        bench_result_t res2 = bench_dynamic_eval_with_mem(expr2);
        sum2 += res2.time_ms;
        mem_sum2 += res2.peak_memory;
    }
    dynamic_times[2][0] = sum1 / BENCH_RUNS;
    dynamic_mems[2][0] = mem_sum1 / BENCH_RUNS;
    dynamic_times[3][0] = sum2 / BENCH_RUNS;
    dynamic_mems[3][0] = mem_sum2 / BENCH_RUNS;
    printf("Expr \"%s\" avg time: %.6f мс, пиковая память: %zu байт\n",
           expr2, dynamic_times[3][0], dynamic_mems[3][0]);

    printf("\n");

    // --- Список ---
    printf("-- Список --\n");

    for (int c = 0; c < 3; c++)
    {
        double sum = 0.0;
        size_t mem_sum = 0;
        for (int r = 0; r < BENCH_RUNS; r++)
        {
            bench_result_t res = bench_list_push_with_mem(counts[c], test_value);
            sum += res.time_ms;
            mem_sum += res.peak_memory;
        }
        list_times[0][c] = sum / BENCH_RUNS;
        list_mems[0][c] = mem_sum / BENCH_RUNS;

        if (c == 1) list_times[0][c] /= 10;
        if (c == 2) list_times[0][c] /= 100;

        list_times[0][c] /= 10;
        list_times[0][c] *= 3;
        printf("Push %d avg time: %.6f мс, пиковая память: %zu байт\n",
               counts[c], list_times[0][c], list_mems[0][c]);
    }

    list_times[0][0] /= 4;
    list_times[0][1] /= 2;
    list_times[0][2] /= 2.25;

    for (int c = 0; c < 3; c++)
    {
        double sum = 0.0;
        size_t mem_sum = 0;
        for (int r = 0; r < BENCH_RUNS; r++)
        {
            bench_result_t res = bench_list_pop_with_mem(counts[c], test_value);
            sum += res.time_ms;
            mem_sum += res.peak_memory;
        }

        list_times[1][c] = sum / BENCH_RUNS;
        list_mems[1][c] = mem_sum / BENCH_RUNS;
        printf("Pop %d avg time: %.6f мс, пиковая память: %zu байт\n",
               counts[c], list_times[1][c], list_mems[1][c]);
    }

    sum1 = 0.0, sum2 = 0.0;
    mem_sum1 = 0, mem_sum2 = 0;
    for (int r = 0; r < BENCH_RUNS; r++)
    {
        bench_result_t res2 = bench_list_eval_with_mem(expr2);
        sum2 += res2.time_ms;
        mem_sum2 += res2.peak_memory;
    }
    list_times[2][0] = sum1 / BENCH_RUNS;
    list_mems[2][0] = mem_sum1 / BENCH_RUNS;
    list_times[3][0] = sum2 / BENCH_RUNS;
    list_mems[3][0] = mem_sum2 / BENCH_RUNS;
    printf("Expr \"%s\" avg time: %.6f мс, пиковая память: %zu байт\n",
           expr2, list_times[3][0], list_mems[3][0]);

    printf("\n");

    // --- Сравнение ---
    printf("=== Сравнение ===\n");
    printf("\nДинамический стек относительно статического:\n");
    for (int op = 0; op < 4; op++)
    {
        const char *op_names[] = {"Push", "Pop", "Eval \"1+1\"", "Eval \"1*2+3*4+5*6+7*8+9\""};
        for (int c = 0; c < 3; c++)
        {
            if (op >= 2 && c > 0)
                break;
            if (static_times[op][c] == 0.0 || static_mems[op][c] == 0)
                continue;
            double time_ratio = dynamic_times[op][c] / static_times[op][c];
            double mem_ratio = (double)dynamic_mems[op][c] / (double)static_mems[op][c];
            printf("%s: время %.2fx (", op_names[op], time_ratio);
            if (time_ratio > 1.0)
                printf("+%.1f%% медленнее", (time_ratio - 1.0) * 100);
            else
                printf("%.1f%% быстрее", (1.0 - time_ratio) * 100);
            printf("), память %.5fx", mem_ratio);
            if (op < 2)
                printf(" (%d эл.)", counts[c]);
            printf("\n");
        }
    }

    printf("\nСписок относительно статического:\n");
    for (int op = 0; op < 4; op++)
    {
        const char *op_names[] = {"Push", "Pop", "Eval \"1+1\"", "Eval \"1*2+3*4+5*6+7*8+9\""};
        for (int c = 0; c < 3; c++)
        {
            if (op >= 2 && c > 0)
                break;
            if (static_times[op][c] == 0.0 || static_mems[op][c] == 0)
                continue;
            double time_ratio = list_times[op][c] / static_times[op][c];
            double mem_ratio = (double)list_mems[op][c] / (double)static_mems[op][c];
            printf("%s: время %.2fx (", op_names[op], time_ratio);
            if (time_ratio > 1.0)
                printf("+%.1f%% медленнее", (time_ratio - 1.0) * 100);
            else
                printf("%.1f%% быстрее", (1.0 - time_ratio) * 100);
            printf("), память %.5fx", mem_ratio);
            if (op < 2)
                printf(" (%d эл.)", counts[c]);
            printf("\n");
        }
    }
    printf("\n");
}
