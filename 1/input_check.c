#include "input_check.h"

int read_line(string_t string, size_t max_string_len)
{
    if (!fgets((string), max_string_len, stdin))
    {
        return ERR_CANT_READ_NUM;
    }

    char *newline = strchr(string, '\n');
    if (!newline)
    {
        return ERR_TOO_LONG_STRING;
    }
    *newline = '\0';

    if (string[0] == '\0' || is_only_spaces(string))
    {
        return ERR_EMPTY_STRING;
    }

    return 0;
}

int is_only_spaces(const char *s)
{
    while (*s)
    {
        if (!isspace(*s))
            return 0;
        s++;
    }
    return 1;
}

int validate_first_num(string_t first_num, int *significant_nums_count)
{
    const char *p = first_num;

    if (*p == '+' || *p == '-')
        p++;

    int has_digits_before_point = 0;
    int has_digits_after_point = 0;

    while (isdigit((unsigned char)*p))
    {
        has_digits_before_point = 1;
        p++;
    }

    if (*p == '.')
    {
        p++;
        while (isdigit((unsigned char)*p))
        {
            has_digits_after_point = 1;
            p++;
        }
    }

    if (!has_digits_before_point && !has_digits_after_point)
        return ERR_INCORRECT_FIRST_NUM;

    if (*p == 'e')
    {
        p++;
        if (*p == '+' || *p == '-')
            p++;
        if (!isdigit((unsigned char)*p))
            return ERR_INCORRECT_FIRST_NUM;
        while (isdigit((unsigned char)*p))
            p++;
    }

    if (*p != '\0')
        return ERR_INCORRECT_FIRST_NUM;

    *significant_nums_count = calc_significant_nums(first_num);
    if (*significant_nums_count > MAX_SIGNIFICANTS_NUMS_COUNT)
        return ERR_TOO_MUCH_SIGNIFICANTS_FIRST_NUM;

    return OK;
}

int calc_significant_nums(string_t str_num)
{
    const char *p = str_num;

    if (*p == '+' || *p == '-')
        p++;

    int nums_count = 0;

    while (*p && *p != '.' && *p != 'e')
    {
        if (isdigit((unsigned char)*p))
            nums_count++;
        p++;
    }

    if (*p == '.')
    {
        p++;
        while (*p && *p != 'e')
        {
            if (isdigit((unsigned char)*p))
                nums_count++;
            p++;
        }
    }

    return nums_count;
}

int validate_second_num(string_t second_num, int *significant_nums_count)
{
    const char *p = second_num;

    if (*p == '+' || *p == '-')
        p++;

    if (*p == '\0')
        return ERR_INCORRECT_SECOND_NUM;

    while (*p)
    {
        if (!isdigit((unsigned char)*p))
            return ERR_INCORRECT_SYMBOLS_SECOND_NUM;

        (*significant_nums_count)++;
        p++;
    }

    if (*significant_nums_count > MAX_SIGNIFICANTS_NUMS_COUNT)
        return ERR_TOO_MUCH_SIGNIFICANTS_SECOND_NUM;

    return OK;
}
