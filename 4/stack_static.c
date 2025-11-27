#include "stack_static.h"

void stack_static_push(static_stack *stack, int value)
{
    if (stack->head_pointer == stack->max_len - 1)
    {
        printf("Cтек переполнен\n");
        return;
    }

    stack->data[++(stack->head_pointer)] = value;
}

int stack_static_pop(static_stack *stack)
{
    if (stack->head_pointer == -1)
    {
        printf("Cтек пустой\n");
    }

    return stack->data[(stack->head_pointer)--];
}

void stack_static_init(static_stack *stack)
{
    stack->head_pointer = -1;
}

void static_stack_print(static_stack *stack)
{
    if (stack->head_pointer == -1)
    {
        printf("Cтек пустой\n");
        return;
    }

    printf("Содержимое стека: \n");
    int *cur = stack->data;
    int *end = stack->data + stack->head_pointer;
    while (cur <= end)
    {
        printf("%d ", *cur);
        cur++;
    }
    printf("\n");
}

void stack_static_char_init(static_stack_char *stack)
{
    stack->head_pointer = -1;
    stack->max_len = 100;
}

void stack_static_char_push(static_stack_char *stack, char value)
{
    if (stack->head_pointer == stack->max_len - 1)
    {
        printf("Стек операторов переполнен\n");
        return;
    }
    stack->data[++(stack->head_pointer)] = value;
}

char stack_static_char_pop(static_stack_char *stack)
{
    if (stack->head_pointer == -1)
    {
        printf("Стек операторов пустой\n");
        return '\0';
    }
    return stack->data[(stack->head_pointer)--];
}

char stack_static_char_top(static_stack_char *stack)
{
    if (stack->head_pointer == -1)
        return '\0';
    return stack->data[stack->head_pointer];
}

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

int static_evaluate_expression(const char *expr)
{
    static_stack values;
    stack_static_init(&values);
    static_stack_char ops;
    stack_static_char_init(&ops);

    int i = 0;
    while (expr[i] != '\0')
    {
        // считывается число до оператора
        int val = 0;
        while (isdigit(expr[i]))
        {
            val = val * 10 + (expr[i] - '0');
            i++;
        }
        stack_static_push(&values, val);

        // считывается оператор
        if (expr[i] != '\0')
        {
            char cur_op = expr[i];
            i++;
            // пока в стеке операторов есть оператор и его приоретет >= два числа из стека извлекаются
            // и считается значение
            while (stack_static_char_top(&ops) != '\0' && precedence(stack_static_char_top(&ops)) >= precedence(cur_op))
            {
                int b = stack_static_pop(&values);
                int a = stack_static_pop(&values);
                char op = stack_static_char_pop(&ops);
                stack_static_push(&values, apply_op(a, b, op));
            }
            stack_static_char_push(&ops, cur_op);
        }
    }

    // применяются оставшиеся операторы
    while (stack_static_char_top(&ops) != '\0')
    {
        int b = stack_static_pop(&values);
        int a = stack_static_pop(&values);
        char op = stack_static_char_pop(&ops);
        stack_static_push(&values, apply_op(a, b, op));
    }

    return stack_static_pop(&values);
}
