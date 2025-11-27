#include "error_description.h"
#include "input_check.h"
#include "nums_division.h"
#include "types.h"
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#define ERROR_INPUT_FIRST_NUM -1
#define ERROR_INCORRECT_FIRST_NUM -2
#define ERROR_INPUT_SECOND_NUM -3
#define ERROR_INCORRECT_SECOND_NUM -4
#define ERROR_ZERO_DIVISON -10
#define ERROR_MACHINE_INF -11
#define ERRROR_MACHINE_ZERO -12

void print_long_number(long_t *num, int first_num_significant_nums);
void print_description(void);
void print_division_result(long_t *num, int len);
int remove_zeros(long_t *num, int len);

int main(void)
{
    SetConsoleOutputCP(CP_UTF8);
    int rc;

    print_description();
    printf("-----------------------------\n\n");

    string_t first_num;
    printf("Введите первое число (действительное):\n");
    printf("\n>+---------1---------2---------3----|----4---------5\n\n>");
    rc = read_line(first_num, sizeof(first_num));
    if (rc != 0)
    {
        error_input_description(rc);
        return ERROR_INPUT_FIRST_NUM;
    }

    int first_num_significant_nums = 0;
    rc = validate_first_num(first_num, &first_num_significant_nums);
    if (rc != 0)
    {
        error_validate_first_num_description(rc);
        return ERROR_INCORRECT_FIRST_NUM;
    }

    string_t second_num;
    printf("Введите второе число (целое):\n");
    printf("\n>+---------1---------2---------3----|----4---------5\n\n>");
    rc = read_line(second_num, sizeof(second_num));
    if (rc != 0)
    {
        error_input_description(rc);
        return ERROR_INPUT_SECOND_NUM;
    }

    int second_num_significant_nums = 0;
    rc = validate_second_num(second_num, &second_num_significant_nums);
    if (rc != 0)
    {
        error_validate_second_num_description(rc);
        return ERROR_INCORRECT_SECOND_NUM;
    }

    long_t structured_first_num;
    int first_num_new_size = 0;
    structuring_num(first_num, &structured_first_num, first_num_significant_nums, &first_num_new_size);

    long_t structured_second_num;
    int second_num_new_size = 0;
    structuring_num(second_num, &structured_second_num, second_num_significant_nums, &second_num_new_size);

    if (is_zero(&structured_second_num, second_num_new_size))
    {
        printf("Ошибка: деление на ноль");
        return ERROR_ZERO_DIVISON;
    }

    printf("\n-----------------------------\n");

    long_t division_result_struct;
    int rc_div = 0;
    int res_len = division_big_nums(structured_first_num, first_num_new_size, structured_second_num,
                                    second_num_new_size, &division_result_struct, &rc_div);

    if (res_len < 0)
    {
        if (rc_div == -10)
        {
            printf("Ошибка, машинная бесконечность");
            return -1;
        }
        else if (rc_div == -11)
        {
            printf("Ошибка, машинный ноль");
            return -1;
        }
    }

    res_len = remove_zeros(&division_result_struct, res_len);

    printf("\nРезультат деления:\n");
    printf(">+---------1---------2---------3----|----4---------5\n\n");
    print_division_result(&division_result_struct, res_len);

    return 0;
}

void print_long_number(long_t *num, int first_num_significant_nums)
{
    if (num->sign == negative)
        printf("-");
    for (int i = 0; i < first_num_significant_nums; i++)
        printf("%d", num->integer[i]);
    printf("e%d", num->exp);
}

void print_description(void)
{
    printf("====================================================\n");
    printf("Программа деления действительного числа (±m.n e ±K) на целое\n");
    printf("поддерживается олько e с маленькой буквы\n");
    printf("Мантисса действительного числа до 35 значащих цифр, порядок до 5 цифр\n");
    printf("Целое число до 35 цифр\n");
    printf("Результат выводится в форме ±0.m1 e ±K1, где m1 до 35 значащих цифр\n");
    printf("====================================================\n\n");
}

int remove_zeros(long_t *num, int len)
{
    int new_len = len;
    while (new_len > 0 && num->integer[new_len - 1] == 0)
        new_len--;

    if (new_len == 0)
    {
        num->integer[0] = 0;
        new_len = 1;
        num->exp = 0;
        num->sign = positive;
    }
    return new_len;
}

void print_division_result(long_t *num, int len)
{
    if (num->sign == negative)
        printf("-");

    printf("0.");
    for (int i = 0; i < len; i++)
        printf("%d", num->integer[i]);

    printf("e%d\n", num->exp);
}
