#include "error_description.h"

void error_input_description(int rc)
{
    switch (rc)
    {
    case ERR_CANT_READ_NUM:
        printf("Ошибка при считывании числа\n");
        break;

    case ERR_TOO_LONG_STRING:
        printf("Ошибка, слишком много символов при вводе числа\n");
        break;

    case ERR_EMPTY_STRING:
        printf("Ошибка, число пустое\n");
        break;

    default:
        printf("Неизвестная ошибка при вводе числа\n");
        break;
    }
}

void error_validate_first_num_description(int rc)
{
    switch (rc)
    {
    case ERR_INCORRECT_FIRST_NUM:
        printf("Ошибка, некорректыне символы в первом числе\n");
        break;
    case ERR_TOO_MUCH_SIGNIFICANTS_FIRST_NUM:
        printf("Ошибка, слишком много значащих цифр в первом числе\n");
        break;
    default:
        printf("Неизвестная ошибка при обработке первого числа\n");
        break;
    }
}

void error_validate_second_num_description(int rc)
{
    switch (rc)
    {
    case ERR_INCORRECT_SECOND_NUM:
        printf("Ошибка, второе число введено некорректно\n");
        break;
    case ERR_INCORRECT_SYMBOLS_SECOND_NUM:
        printf("Ошибка, некорректные символы во втором числе\n");
        break;
    case ERR_TOO_MUCH_SIGNIFICANTS_SECOND_NUM:
        printf("Слишком много значащих цифр во втором числе\n");
        break;
    default:
        printf("Неизвестная ошибка при обработке второго числа\n");
        break;
    }
}
