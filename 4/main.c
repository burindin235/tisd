#include "dynamic_stack.h"
#include "list_stack.h"
#include "menu_utils.h"
#include "stack_static.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// написать что делает программа. подсчёт врыаения на основе стека такого то такого то 
#define EXPR_SIZE 256


int main()
{
    int choice, op_choice;
    char expr[EXPR_SIZE];

    void *free_list[MAX_FREE_LIST];
    int free_count = 0;

    show_info();
    while (1)
    {
        show_menu();
        scanf("%d", &choice);
        getchar(); // считываем перевод строки

        if (choice == 0)
            break; // выход из программы

        // ================== Статический стек ==================
        if (choice == 1)
        {
            static_stack s;
            stack_static_init(&s);
            s.max_len = MAX_STATIC_STACK;

            while (1)
            {
                show_operations(0);
                scanf("%d", &op_choice);
                getchar();

                if (op_choice == 0)
                {
                    s.head_pointer = -1;
                    printf("Стек очищен.\n");
                    break; // выход в главное меню
                }


                if (op_choice == 1)
                { // push
                    int val;
                    printf("Введите число: ");
                    scanf("%d", &val);
                    getchar();
                    stack_static_push(&s, val);
                    bench_static_push(val); // отдельный стек для замера
                }
                else if (op_choice == 2)
                { // pop
                    if (s.head_pointer >= 0)
                    {
                        int top_val = s.data[s.head_pointer];
                        bench_static_pop(top_val);
                        (void)stack_static_pop(&s);
                    }
                    else
                    {
                        printf("Стек пустой\n");
                    }
                }
                else if (op_choice == 3)
                { // вычисление выражения
                    read_expression(expr, EXPR_SIZE);
                    if (!validate_expression(expr))
                    {
                        printf("Ошибка: некорректное выражение.\n");
                    }
                    else
                    {
                        int res = static_evaluate_expression(expr);
                        printf("Результат: %d\n", res);
                        bench_static_eval(expr);
                    }
                }
                else if (op_choice == 4)
                    static_stack_print(&s);
                else if (op_choice == 5)
                {
                    size_t used = (size_t)(s.head_pointer + 1) * sizeof(int) + 2 * sizeof(int);
                    size_t allocated = sizeof(static_stack);
                    printf("Статический стек:\n");
                    printf("Используемая память: %zu байт\n", used);
                    printf("Выделено памяти: %zu байт\n", allocated);
                }
                else if (op_choice == 6)
                {
                    s.head_pointer = -1;
                    printf("Стек очищен.\n");
                }
            }
        }

        // ================== Динамический стек ==================
        else if (choice == 2)
        {
            dynamic_stack s;
            stack_dynamic_init(&s);
            dynamic_stack_char ops;
            stack_dynamic_char_init(&ops);

            while (1)
            {
                show_operations(0);
                scanf("%d", &op_choice);
                getchar();

                if (op_choice == 0)
                {
                    stack_dynamic_free(&s);
                    stack_dynamic_init(&s);
                    printf("Стек очищен.\n");
                    break;
                }

                if (op_choice == 1)
                { // push
                    int val;
                    printf("Введите число: ");
                    scanf("%d", &val);
                    getchar();
                    stack_dynamic_push(&s, val);
                    bench_dynamic_push(val); // отдельный стек для замера
                }
                else if (op_choice == 2)
                { // pop
                    if (s.head_pointer >= 0)
                    {
                        int top_val = s.data[s.head_pointer];
                        bench_dynamic_pop(top_val);
                        (void)stack_dynamic_pop(&s);
                    }
                    else
                    {
                        printf("Стек пустой\n");
                    }
                }
                else if (op_choice == 3)
                { // вычисление выражения
                    read_expression(expr, EXPR_SIZE);
                    if (!validate_expression(expr))
                    {
                        printf("Ошибка: некорректное выражение.\n");
                    }
                    else
                    {
                        int res = dynamic_evaluate_expression(expr);
                        printf("Результат: %d\n", res);
                        bench_dynamic_eval(expr);
                    }
                }
                else if (op_choice == 4)
                    dynamic_stack_print(&s);
                else if (op_choice == 5)
                {
                    size_t used = (size_t)(s.head_pointer + 1) * sizeof(int) + 2 * sizeof(int) + sizeof(int *);
                    size_t allocated = (size_t)s.max_len * sizeof(int) + 2 * sizeof(int) + sizeof(int *);
                    printf("Динамический стек:\n");
                    printf("Используемая память: %zu байт\n", used);
                    printf("Выделено памяти: %zu байт\n", allocated);
                }
                else if (op_choice == 6)
                {
                    stack_dynamic_free(&s);
                    stack_dynamic_init(&s);
                    printf("Стек очищен.\n");
                }
            }
            stack_dynamic_free(&s);
            stack_dynamic_char_free(&ops);
        }

        // ================== Стек списка ==================
        else if (choice == 3)
        {
            list_stack s;
            list_stack_init(&s);
            free_count = 0;

            while (1)
            {
                show_operations(1);
                scanf("%d", &op_choice);
                getchar();

                if (op_choice == 0)
                {
                    while (s.top)
                    {
                        node *tmp = s.top;
                        s.top = s.top->next;
                        free(tmp);
                    }
                    free_count = 0;
                    printf("Стек очищен.\n");

                    break;
                }

                if (op_choice == 1)
                { // push
                    int val;
                    printf("Введите число: ");
                    scanf("%d", &val);
                    getchar();
                    list_stack_push(&s, val);
                    bench_list_push(val);
                }
                else if (op_choice == 2)
                { // pop
                    if (s.top)
                    {
                        int top_val = s.top->data;
                        bench_list_pop(top_val);
                        (void)list_stack_pop(&s, free_list, &free_count);
                    }
                    else
                    {
                        printf("Стек чисел пуст.\n");
                    }
                }
                else if (op_choice == 3)
                { // вычисление выражения
                    read_expression(expr, EXPR_SIZE);
                    if (!validate_expression(expr))
                    {
                        printf("Ошибка: некорректное выражение.\n");
                    }
                    else
                    {
                        int res = list_evaluate_expression(expr, free_list, &free_count);
                        printf("Результат: %d\n", res);
                        bench_list_eval(expr);
                    }
                }
                else if (op_choice == 4)
                    list_stack_print(&s);
                else if (op_choice == 5)
                { // память
                    size_t cnt = 0;
                    for (node *cur = s.top; cur; cur = cur->next)
                        cnt++;
                    size_t used = cnt * sizeof(node);                 // узлы в стеке
                    size_t freed = (size_t)free_count * sizeof(node); // освобождённые узлы
                    size_t total = used + freed;
                    printf("Стек списка:\n");
                    printf("Используемая память (узлы в стеке): %zu байт\n", used);
                    printf("Память освобождённых узлов: %zu байт\n", freed);
                    printf("Всего памяти на стек и свободные узлы: %zu байт\n", total);
                }
                else if (op_choice == 6)
                { // очистка стека
                    while (s.top)
                    {
                        node *tmp = s.top;
                        s.top = s.top->next;
                        free(tmp);
                    }
                    free_count = 0;
                    printf("Стек очищен.\n");
                }
                else if (op_choice == 7)
                { // показать список освобождённых
                    print_free_list(free_list, free_count);
                }
            }
        }

        // ================== Замеры времени ==================
        else if (choice == 4)
        {
            comprehensive_benchmark();
        }

        // ================== Сравнение эффективности ==================
        else if (choice == 5)
        {
            comparative_benchmark();
        }
    }

    printf("Выход из программы.\n");
    return 0;
}


