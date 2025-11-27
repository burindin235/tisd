#include "list_stack.h"

// ----------------- Стек чисел -----------------
void list_stack_init(list_stack *stack)
{
    stack->top = NULL;
    stack->size = 0;
}

void list_stack_push(list_stack *stack, int value)
{
    // Проверка ограничения размера
    if (stack->size >= MAX_LIST_STACK)
    {
        printf("Стек переполнен (максимум %d элементов)\n", MAX_LIST_STACK);
        return;
    }

    node *new_node = (node *)malloc(sizeof(node));
    if (!new_node)
    {
        printf("Ошибка: не удалось выделить память.\n");
        return;
    }
    new_node->data = value;
    new_node->next = stack->top;
    stack->top = new_node;
    stack->size++;
}

int list_stack_pop(list_stack *stack, void **free_list, int *free_count)
{
    if (!stack->top)
    {
        printf("Стек чисел пуст.\n");
        return 0;
    }
    node *temp = stack->top;
    int value = temp->data;
    stack->top = temp->next;
    stack->size--;

    if (*free_count < MAX_FREE_LIST)
        free_list[(*free_count)++] = (void *)temp;
    free(temp);

    return value;
}

int list_stack_top(list_stack *stack)
{
    if (!stack->top)
        return 0;
    return stack->top->data;
}

void list_stack_print(list_stack *stack)
{
    node *cur = stack->top;
    printf("Стек чисел:\n");
    while (cur)
    {
        printf("%d [адрес: %p]\n", cur->data, (void *)cur);
        cur = cur->next;
    }
}

// ----------------- Стек операторов -----------------
void list_stack_char_init(list_stack_char *stack)
{
    stack->top = NULL;
    stack->size = 0;
}

void list_stack_char_push(list_stack_char *stack, char value)
{
    // Проверка ограничения размера
    if (stack->size >= MAX_LIST_STACK)
    {
        printf("Стек операторов переполнен (максимум %d элементов)\n", MAX_LIST_STACK);
        return;
    }

    node_char *new_node = (node_char *)malloc(sizeof(node_char));
    if (!new_node)
    {
        printf("Ошибка: не удалось выделить память.\n");
        return;
    }
    new_node->data = value;
    new_node->next = stack->top;
    stack->top = new_node;
    stack->size++;
}

char list_stack_char_pop(list_stack_char *stack, void **free_list, int *free_count)
{
    if (!stack->top)
    {
        printf("Стек операторов пуст.\n");
        return '\0';
    }
    node_char *temp = stack->top;
    char value = temp->data;
    stack->top = temp->next;
    stack->size--;

    if (*free_count < MAX_FREE_LIST)
        free_list[(*free_count)++] = (void *)temp;
    free(temp);

    return value;
}

char list_stack_char_top(list_stack_char *stack)
{
    if (!stack->top)
        return '\0';
    return stack->top->data;
}

void list_stack_char_print(list_stack_char *stack)
{
    node_char *cur = stack->top;
    printf("Стек операторов:\n");
    while (cur)
    {
        printf("%c [адрес: %p]\n", cur->data, (void *)cur);
        cur = cur->next;
    }
}

// ----------------- Вспомогательные функции -----------------
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

// ----------------- Вычисление выражения -----------------
int list_evaluate_expression(const char *expr, void **free_list, int *free_count)
{
    list_stack values;
    list_stack_init(&values);
    list_stack_char ops;
    list_stack_char_init(&ops);

    int i = 0;
    while (expr[i] != '\0')
    {
        // считываем число
        int val = 0;
        while (isdigit(expr[i]))
        {
            val = val * 10 + (expr[i] - '0');
            i++;
        }
        list_stack_push(&values, val);

        // считываем оператор
        if (expr[i] != '\0')
        {
            char cur_op = expr[i++];
            while (list_stack_char_top(&ops) != '\0' && precedence(list_stack_char_top(&ops)) >= precedence(cur_op))
            {
                int b = list_stack_pop(&values, free_list, free_count);
                int a = list_stack_pop(&values, free_list, free_count);
                char op = list_stack_char_pop(&ops, free_list, free_count);
                list_stack_push(&values, apply_op(a, b, op));
            }
            list_stack_char_push(&ops, cur_op);
        }
    }

    while (list_stack_char_top(&ops) != '\0')
    {
        int b = list_stack_pop(&values, free_list, free_count);
        int a = list_stack_pop(&values, free_list, free_count);
        char op = list_stack_char_pop(&ops, free_list, free_count);
        list_stack_push(&values, apply_op(a, b, op));
    }

    return list_stack_pop(&values, free_list, free_count);
}

// ----------------- Список освобождённых адресов -----------------
void print_free_list(void **free_list, int free_count)
{
    printf("\nСписок освобождённых адресов:\n");
    if (free_count == 0)
    {
        printf("Нет освобождённых областей.\n");
        return;
    }
    for (int i = 0; i < free_count; i++)
        printf("%p\n", free_list[i]);
}
