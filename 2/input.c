// input.c
#include "input.h"
#include "types.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int validate_date_string(const char *input, date_t *out_date);

int is_only_spaces(const char *s)
{
    while (*s)
    {
        if (!isspace((unsigned char)*s))
            return 0;
        s++;
    }
    return 1;
}

// очистка стдина до конца строки, для работы со
void flush_stdin(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF){}
}

int read_line(string_t string)
{
    if (!fgets(string, MAX_STRING_LEN + 1, stdin))
    {
        return -1; 
    }

    char *newline = strchr(string, '\n');
    if (newline)
    {
        *newline = '\0';
    }
    else
    {
        flush_stdin();
    }

    if (string[0] == '\0' || is_only_spaces(string))
        return -2;

    return 0;
}


int input_and_print_students_in_file(const char *filename, size_t students_count)
{
    student_t student;

    printf("=== Ввод студента ===\n");

    /* фамилия */
    printf("Введите фамилию: ");
    if (read_line(student.surname) != 0)
    {
        printf("Ошибка ввода: неверная фамилия.\n");
        return -11;
    }

    /* имя */
    printf("Введите имя: ");
    if (read_line(student.name) != 0)
    {
        printf("Ошибка ввода: неверное имя.\n");
        return -13;
    }

    /* группа */
    printf("Введите название группы: ");
    if (read_line(student.group_name) != 0)
    {
        printf("Ошибка ввода: неверное название группы.\n");
        return -15;
    }

    /* пол */
    printf("Введите пол (F/M): ");
    if (scanf(" %c", &student.gender) != 1)
    {
        printf("Ошибка ввода: не удалось прочитать пол.\n");
        flush_stdin();
        return -17;
    }
    if (student.gender != 'F' && student.gender != 'M')
    {
        printf("Ошибка: пол должен быть 'F' или 'M'.\n");
        flush_stdin();
        return -18;
    }

    /* возраст */
    printf("Введите возраст: ");
    if (scanf("%d", &student.age) != 1)
    {
        printf("Ошибка ввода: неверный возраст.\n");
        flush_stdin();
        return -19;
    }

    /* средний балл */
    printf("Введите средний балл: ");
    if (scanf("%lf", &student.avg_grade) != 1)
    {
        printf("Ошибка ввода: неверный средний балл.\n");
        flush_stdin();
        return -20;
    }

    /* перед тем, как читать дату через read_line, нужно очистить остаток ввода */
    flush_stdin();

    /* дата поступления в формате dd.mm.yyyy */
    printf("Введите дату поступления (dd.mm.yyyy): ");
    string_t date_buf;
    if (read_line(date_buf) != 0)
    {
        printf("Ошибка ввода: неверная строка даты.\n");
        return -21;
    }
    if (validate_date_string(date_buf, &student.entry_date) != 0)
    {
        printf("Ошибка: дата введена некорректно. Ожидается формат dd.mm.yyyy.\n");
        return -22;
    }

    /* выбор типа адреса */
    printf("Выберите тип адреса (1 - квартира, 2 - общага, 3 - съемное жильё): ");
    int address_type_input;
    if (scanf("%d", &address_type_input) != 1)
    {
        printf("Ошибка ввода: неверный тип адреса.\n");
        flush_stdin();
        return -24;
    }
    if (address_type_input < 1 || address_type_input > 3)
    {
        printf("Ошибка: тип адреса должен быть 1, 2 или 3.\n");
        flush_stdin();
        return -25;
    }

    student.adress_kind = (address_type)(address_type_input - 1);

    /* перед следующими вызовами read_line очистим остаток после scanf */
    flush_stdin();

    switch (student.adress_kind)
    {
    case AddressType_Home:
        printf("Введите улицу: ");
        if (read_line(student.address.home.street) != 0)
        {
            printf("Ошибка ввода: неверная улица.\n");
            return -26;
        }

        printf("Введите номер дома: ");
        if (scanf("%d", &student.address.home.house_nubmer) != 1)
        {
            printf("Ошибка ввода: неверный номер дома.\n");
            flush_stdin();
            return -27;
        }

        printf("Введите номер квартиры: ");
        if (scanf("%d", &student.address.home.apartment_nubmer) != 1)
        {
            printf("Ошибка ввода: неверный номер квартиры.\n");
            flush_stdin();
            return -28;
        }
        break;

    case AddressType_Dorm:
        printf("Введите номер общежития: ");
        if (scanf("%d", &student.address.dorm.dorm_nubmer) != 1)
        {
            printf("Ошибка ввода: неверный номер общежития.\n");
            flush_stdin();
            return -29;
        }

        printf("Введите номер комнаты: ");
        if (scanf("%d", &student.address.dorm.room_nubmer) != 1)
        {
            printf("Ошибка ввода: неверный номер комнаты.\n");
            flush_stdin();
            return -30;
        }
        break;

    case AddressType_Rent:
        /* после предыдущих scanf очищаем stdin перед read_line */

        printf("Введите улицу: ");
        if (read_line(student.address.rent.street) != 0)
        {
            printf("Ошибка ввода: неверная улица.\n");
            return -31;
        }

        printf("Введите номер дома: ");
        if (scanf("%d", &student.address.rent.house_nubmer) != 1)
        {
            printf("Ошибка ввода: неверный номер дома.\n");
            flush_stdin();
            return -32;
        }

        printf("Введите номер квартиры: ");
        if (scanf("%d", &student.address.rent.apartment_nubmer) != 1)
        {
            printf("Ошибка ввода: неверный номер квартиры.\n");
            flush_stdin();
            return -33;
        }

        printf("Введите стоимость аренды: ");
        if (scanf("%lf", &student.address.rent.rent_cost) != 1)
        {
            printf("Ошибка ввода: неверная стоимость аренды.\n");
            flush_stdin();
            return -34;
        }
        break;
    }

    flush_stdin();
    int rc;
    /* Записываем студента в файл */
    if (rc = write_student_to_file(filename, &student) != 0)
    {
        printf("Ошибка записи студента в файл.\n");
        return -rc-100;
    }

    printf("Студент успешно добавлен в файл.\n");
    return 0;
}


/* проверка високосного года */
int is_leap_year(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

/* validate_date_string: проверяет строку "dd.mm.yyyy" и заполняет out_date */
int validate_date_string(const char *input, date_t *out_date)
{
    if (!input || strlen(input) != 10)
        return -1;

    if (input[2] != '.' || input[5] != '.')
        return -2;

    char day_str[3] = {0};
    char month_str[3] = {0};
    char year_str[5] = {0};

    strncpy(day_str, input, 2);
    strncpy(month_str, input + 3, 2);
    strncpy(year_str, input + 6, 4);

    for (int i = 0; i < 2; ++i)
    {
        if (!isdigit((unsigned char)day_str[i]) || !isdigit((unsigned char)month_str[i]))
            return -3;
    }
    for (int i = 0; i < 4; ++i)
    {
        if (!isdigit((unsigned char)year_str[i]))
            return -4;
    }

    int day = atoi(day_str);
    int month = atoi(month_str);
    int year = atoi(year_str);

    if (month < 1 || month > 12)
        return -5;

    int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (month == 2 && is_leap_year(year))
        days_in_month[1] = 29;

    if (day < 1 || day > days_in_month[month - 1])
        return -6;

    strncpy(out_date->Day, day_str, 3);
    strncpy(out_date->Month, month_str, 3);
    strncpy(out_date->Year, year_str, 5);

    return 0;
}
