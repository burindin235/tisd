#include "dynamic_stack.h"

// --- Стек чисел ---
void stack_dynamic_init(dynamic_stack *stack)
{
    stack->data = NULL;
    stack->head_pointer = -1;
    stack->max_len = 0;
    stack->capacity = 0;
}

void stack_dynamic_push(dynamic_stack *stack, int value)
{
    // Проверка ограничения размера
    if (stack->head_pointer + 1 >= MAX_DYNAMIC_STACK)
    {
        printf("Стек переполнен (максимум %d элементов)\n", MAX_DYNAMIC_STACK);
        return;
    }

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
        if (!new_data)
        {
            printf("Ошибка: не удалось выделить память.\n");
            return;
        }
        stack->data = new_data;
        stack->capacity = new_capacity;
    }

    stack->data[++stack->head_pointer] = value;
    stack->max_len = stack->head_pointer + 1;
}

int stack_dynamic_pop(dynamic_stack *stack)
{
    if (stack->head_pointer == -1)
    {
        printf("Стек пустой\n");
        return 0;
    }
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

void stack_dynamic_free(dynamic_stack *stack)
{
    free(stack->data);
    stack->data = NULL;
    stack->head_pointer = -1;
    stack->max_len = 0;
    stack->capacity = 0;
}

void dynamic_stack_print(dynamic_stack *stack)
{
    if (stack->head_pointer == -1)
    {
        printf("Стек пустой\n");
        return;
    }
    printf("Содержимое стека: \n");
    for (int i = 0; i <= stack->head_pointer; i++)
    {
        printf("%d ", stack->data[i]);
    }
    printf("\n");
}

// --- Стек операторов ---
void stack_dynamic_char_init(dynamic_stack_char *stack)
{
    stack->data = NULL;
    stack->head_pointer = -1;
    stack->max_len = 0;
    stack->capacity = 0;
}

void stack_dynamic_char_push(dynamic_stack_char *stack, char value)
{
    // Проверка ограничения размера
    if (stack->head_pointer + 1 >= MAX_DYNAMIC_STACK)
    {
        printf("Стек операторов переполнен (максимум %d элементов)\n", MAX_DYNAMIC_STACK);
        return;
    }

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

        char *new_data = (char *)realloc(stack->data, new_capacity * sizeof(char));
        if (!new_data)
        {
            printf("Ошибка: не удалось выделить память.\n");
            return;
        }
        stack->data = new_data;
        stack->capacity = new_capacity;
    }

    stack->data[++stack->head_pointer] = value;
    stack->max_len = stack->head_pointer + 1;
}

char stack_dynamic_char_pop(dynamic_stack_char *stack)
{
    if (stack->head_pointer == -1)
    {
        printf("Стек операторов пустой\n");
        return '\0';
    }
    char value = stack->data[stack->head_pointer--];
    stack->max_len = stack->head_pointer + 1;

    // Уменьшаем capacity, если использование < capacity/4
    if (stack->max_len < stack->capacity / 4 && stack->capacity > 1)
    {
        int new_capacity = stack->capacity / 2;
        char *new_data = (char *)realloc(stack->data, new_capacity * sizeof(char));
        if (new_data)
        {
            stack->data = new_data;
            stack->capacity = new_capacity;
        }
    }

    return value;
}

char stack_dynamic_char_top(dynamic_stack_char *stack)
{
    if (stack->head_pointer == -1)
        return '\0';
    return stack->data[stack->head_pointer];
}

void stack_dynamic_char_free(dynamic_stack_char *stack)
{
    free(stack->data);
    stack->data = NULL;
    stack->head_pointer = -1;
    stack->max_len = 0;
    stack->capacity = 0;
}

// --- Вспомогательные функции ---
static int precedence(char op)
{
    if (op == '*' || op == '/')
        return 1;
    if (op == '+' || op == '-')
        return 0;
    return 0;
}

static int apply_op(int a, int b, char op)
{
    switch (op)
    {
    case '+':
        return a + b;
    case '-':
        return a - b;
    case '*':
        return a * b;
    case '/':
        return a / b;
    }
    return 0;
}

int dynamic_evaluate_expression(const char *expr)
{
    dynamic_stack values;
    stack_dynamic_init(&values);
    dynamic_stack_char ops;
    stack_dynamic_char_init(&ops);

    int i = 0;
    while (expr[i] != '\0')
    {
        int val = 0;
        while (isdigit(expr[i]))
        {
            val = val * 10 + (expr[i] - '0');
            i++;
        }
        stack_dynamic_push(&values, val);

        if (expr[i] != '\0')
        {
            char cur_op = expr[i++];
            while (stack_dynamic_char_top(&ops) != '\0' &&
                   precedence(stack_dynamic_char_top(&ops)) >= precedence(cur_op))
            {
                int b = stack_dynamic_pop(&values);
                int a = stack_dynamic_pop(&values);
                char op = stack_dynamic_char_pop(&ops);
                stack_dynamic_push(&values, apply_op(a, b, op));
            }
            stack_dynamic_char_push(&ops, cur_op);
        }
    }

    while (stack_dynamic_char_top(&ops) != '\0')
    {
        int b = stack_dynamic_pop(&values);
        int a = stack_dynamic_pop(&values);
        char op = stack_dynamic_char_pop(&ops);
        stack_dynamic_push(&values, apply_op(a, b, op));
    }

    int result = stack_dynamic_pop(&values);
    stack_dynamic_free(&values);
    stack_dynamic_char_free(&ops);

    return result;
}
