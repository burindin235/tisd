#include "file_process.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// 1. Парсинг одной строки файла в структуру student_t
// ============================================================================
void parse_student_line(const char *line, student_t *student)
{
    char buffer[LINE_BUF_SIZE + 1];
    strncpy(buffer, line, LINE_BUF_SIZE);
    buffer[LINE_BUF_SIZE] = '\0';

    char *token = strtok(buffer, "|");
    strncpy(student->surname, token, MAX_STRING_LEN);

    token = strtok(NULL, "|");
    strncpy(student->name, token, MAX_STRING_LEN);

    token = strtok(NULL, "|");
    strncpy(student->group_name, token, MAX_STRING_LEN);

    token = strtok(NULL, "|");
    student->gender = *token;

    token = strtok(NULL, "|");
    student->age = atoi(token);

    token = strtok(NULL, "|");
    student->avg_grade = atof(token);

    // Дата
    token = strtok(NULL, ".");
    strncpy(student->entry_date.Day, token, 3);

    token = strtok(NULL, ".");
    strncpy(student->entry_date.Month, token, 3);

    token = strtok(NULL, "|");
    strncpy(student->entry_date.Year, token, 5);

    // Тип адреса
    token = strtok(NULL, "|");
    student->adress_kind = (address_type)atoi(token);

    // Разбор адреса в зависимости от варианта
    switch (student->adress_kind)
    {
    case AddressType_Home:
        token = strtok(NULL, "|");
        strncpy(student->address.home.street, token, MAX_STRING_LEN);

        token = strtok(NULL, "|");
        student->address.home.house_nubmer = atoi(token);

        token = strtok(NULL, "\n");
        student->address.home.apartment_nubmer = atoi(token);
        break;

    case AddressType_Dorm:
        token = strtok(NULL, "|");
        student->address.dorm.dorm_nubmer = atoi(token);

        token = strtok(NULL, "\n");
        student->address.dorm.room_nubmer = atoi(token);
        break;

    case AddressType_Rent:
        token = strtok(NULL, "|");
        strncpy(student->address.rent.street, token, MAX_STRING_LEN);

        token = strtok(NULL, "|");
        student->address.rent.house_nubmer = atoi(token);

        token = strtok(NULL, "|");
        student->address.rent.apartment_nubmer = atoi(token);

        token = strtok(NULL, "\n");
        student->address.rent.rent_cost = atof(token);
        break;
    }
}

// ============================================================================
// 2. Чтение всех студентов из файла в массив
// ============================================================================
student_t *read_students_from_file(const char *filename, size_t *out_count)
{
    FILE *f = fopen(filename, "r");
    if (!f)
        return NULL;

    size_t count = 0;
    char line[LINE_BUF_SIZE + 1];
    while (fgets(line, sizeof(line), f))
        count++;

    rewind(f);

    student_t *students = malloc(count * sizeof(student_t));
    if (!students)
    {
        fclose(f);
        return NULL;
    }

    size_t i = 0;
    while (fgets(line, sizeof(line), f))
    {
        parse_student_line(line, &students[i]);
        i++;
    }

    fclose(f);
    *out_count = count;
    return students;
}

// ============================================================================
// 3. Запись одного студента в файл
// ============================================================================
int write_student_to_file(const char *filename, const student_t *student)
{
    FILE *f = fopen(filename, "a");
    if (f == NULL)
        return -1;

    if (fprintf(f, "%s|%s|%s|%c|%d|%.2lf|%s.%s.%s|%d|", student->surname, student->name, student->group_name,
                student->gender, student->age, student->avg_grade, student->entry_date.Day, student->entry_date.Month,
                student->entry_date.Year, (int)student->adress_kind) < 0)
    {
        fclose(f);
        return -2;
    }

    switch (student->adress_kind)
    {
    case AddressType_Home:
        fprintf(f, "%s|%d|%d\n", student->address.home.street, student->address.home.house_nubmer,
                student->address.home.apartment_nubmer);
        break;

    case AddressType_Dorm:
        fprintf(f, "%d|%d\n", student->address.dorm.dorm_nubmer, student->address.dorm.room_nubmer);
        break;

    case AddressType_Rent:
        fprintf(f, "%s|%d|%d|%.2lf\n", student->address.rent.street, student->address.rent.house_nubmer,
                student->address.rent.apartment_nubmer, student->address.rent.rent_cost);
        break;
    }

    fclose(f);
    return 0;
}

// ============================================================================
// 4. Перезапись всех студентов в файл
// ============================================================================
int rewrite_students_to_file(const char *filename, student_t *students, size_t count)
{
    FILE *f = fopen(filename, "w");
    if (!f)
        return -1;

    for (size_t i = 0; i < count; i++)
    {
        fprintf(f, "%s|%s|%s|%c|%d|%.2lf|%s.%s.%s|%d|", students[i].surname, students[i].name, students[i].group_name,
                students[i].gender, students[i].age, students[i].avg_grade, students[i].entry_date.Day,
                students[i].entry_date.Month, students[i].entry_date.Year, (int)students[i].adress_kind);

        switch (students[i].adress_kind)
        {
        case AddressType_Home:
            fprintf(f, "%s|%d|%d\n", students[i].address.home.street, students[i].address.home.house_nubmer,
                    students[i].address.home.apartment_nubmer);
            break;

        case AddressType_Dorm:
            fprintf(f, "%d|%d\n", students[i].address.dorm.dorm_nubmer, students[i].address.dorm.room_nubmer);
            break;

        case AddressType_Rent:
            fprintf(f, "%s|%d|%d|%.2lf\n", students[i].address.rent.street, students[i].address.rent.house_nubmer,
                    students[i].address.rent.apartment_nubmer, students[i].address.rent.rent_cost);
            break;
        }
    }

    fclose(f);
    return 0;
}

// ============================================================================
// 5. вывод студентов в виде таблицы
// ============================================================================
int print_students_from_file_formatted(const char *filename)
{
    size_t count;
    student_t *students = read_students_from_file(filename, &count);
    if (!students)
        return -1;

    printf("____| %-10s | %-10s | %-10s | %-6s | %-4s | %-6s | %-10s | %-10s | "
           "%-15s | %-5s | %-10s | %-10s |\n",
           "Surname", "Name", "Group", "Gender", "Age", "score", "date", "h-s type", "street/dorm", "home", "flat/room",
           "rent");

    for (size_t i = 0; i < count; i++)
    {
        printf(" %2zu | %-10s | %-10s | %-10s | %-6c | %4d | %6.2f | "
               "%02s.%02s.%04s | %10d ",
               i + 1, students[i].surname, students[i].name, students[i].group_name, students[i].gender, students[i].age,
               students[i].avg_grade, students[i].entry_date.Day, students[i].entry_date.Month,
               students[i].entry_date.Year, (int)students[i].adress_kind);

        switch (students[i].adress_kind)
        {
        case AddressType_Home:
            printf("| %-15s | %5d | %10d | %-10s", students[i].address.home.street,
                   students[i].address.home.house_nubmer, students[i].address.home.apartment_nubmer, "-");
            break;

        case AddressType_Dorm:
            printf("| %-15s | %5d | %10d | %-10s", "dorm", students[i].address.dorm.dorm_nubmer,
                   students[i].address.dorm.room_nubmer, "-");
            break;

        case AddressType_Rent:
            printf("| %-15s | %5d | %10d | %10.2f", students[i].address.rent.street,
                   students[i].address.rent.house_nubmer, students[i].address.rent.apartment_nubmer,
                   students[i].address.rent.rent_cost);
            break;
        }

        printf(" |\n");
    }

    free(students);
    return 0;
}

// ============================================================================
// 6. Удаление студентов по группе в массиве
// ============================================================================
int delete_students_by_group(student_t *students, size_t *count, const char *target_group)
{
    int deleted = 0;
    size_t write_index = 0;
    for (size_t i = 0; i < *count; i++)
    {
        if (strcmp(students[i].group_name, target_group) != 0)
        {

            students[write_index++] = students[i];
        }
        else
            deleted++;
    }
    *count = write_index;

    return deleted;
}

// ============================================================================
// 7. Удаление студентов по группе в файле
// ============================================================================
int delete_students_by_group_in_file(const char *filename, const char *target_group)
{

    size_t count;
    student_t *students = read_students_from_file(filename, &count);
    if (!students)
        return -1;

    int deleted_elements = delete_students_by_group(students, &count, target_group);

    int res = rewrite_students_to_file(filename, students, count);
    free(students);

    if (deleted_elements == 0)
        res = -1;

    return res;
}

// ============================================================================
// 8. Печать массива студентов
// ============================================================================
void print_students_array(student_t *students, size_t count)
{
    if (!students || count == 0)
    {
        printf("Нет данных для вывода.\n");
        return;
    }

    printf("____| %-10s | %-10s | %-10s | %-6s | %-4s | %-6s | %-10s | %-10s | "
           "%-15s | %-5s | %-10s | %-10s |\n",
           "Surname", "Name", "Group", "Gender", "Age", "score", "date", "h-s type", "street/dorm", "home", "flat/room",
           "rent");

    for (size_t i = 0; i < count; i++)
    {
        printf(" %2zu | %-10s | %-10s | %-10s | %-6c | %4d | %6.2f | "
               "%02s.%02s.%04s | %10d ",
               i + 1, students[i].surname, students[i].name, students[i].group_name, students[i].gender, students[i].age,
               students[i].avg_grade, students[i].entry_date.Day, students[i].entry_date.Month,
               students[i].entry_date.Year, (int)students[i].adress_kind);

        switch (students[i].adress_kind)
        {
        case AddressType_Home:
            printf("| %-15s | %5d | %10d | %-10s", students[i].address.home.street,
                   students[i].address.home.house_nubmer, students[i].address.home.apartment_nubmer, "-");
            break;

        case AddressType_Dorm:
            printf("| %-15s | %5d | %10d | %-10s", "dorm", students[i].address.dorm.dorm_nubmer,
                   students[i].address.dorm.room_nubmer, "-");
            break;

        case AddressType_Rent:
            printf("| %-15s | %5d | %10d | %10.2f", students[i].address.rent.street,
                   students[i].address.rent.house_nubmer, students[i].address.rent.apartment_nubmer,
                   students[i].address.rent.rent_cost);
            break;
        }

        printf(" |\n");
    }
}

// ============================================================================
// 9. Вывод студентов арендаторов с годом = X и стоимостью < Y
// ============================================================================
void print_students_rent_cheaper_than(const char *filename, const char *year, double max_rent)
{
    size_t count;
    student_t *students = read_students_from_file(filename, &count);
    if (!students)
    {
        printf("Ошибка чтения файла\n");
        return;
    }

    student_t *filtered = malloc(count * sizeof(student_t));
    if (!filtered)
    {
        free(students);
        return;
    }

    size_t filtered_count = 0;
    for (size_t i = 0; i < count; i++)
    {
        if (students[i].adress_kind == AddressType_Rent && strcmp(students[i].entry_date.Year, year) == 0 &&
            students[i].address.rent.rent_cost < max_rent)
        {
            filtered[filtered_count++] = students[i];
        }
    }

    if (filtered_count == 0)
    {
        printf("Нет студентов %s года в съёмном жилье дешевле %.2lf\n", year, max_rent);
    }
    else
    {
        print_students_array(filtered, filtered_count);
    }

    free(students);
    free(filtered);
}

// ============================================================================
// 10. Вспомогательная печать одного студента
// ============================================================================
void print_student(const student_t *s)
{
    printf("%s %s, группа %s, возраст %d, ср.балл %.2f, дата %s.%s.%s\n", s->surname, s->name, s->group_name, s->age,
           s->avg_grade, s->entry_date.Day, s->entry_date.Month, s->entry_date.Year);

    switch (s->adress_kind)
    {
    case AddressType_Home:
        printf("Дом: %s, %d-%d\n", s->address.home.street, s->address.home.house_nubmer,
               s->address.home.apartment_nubmer);
        break;
    case AddressType_Dorm:
        printf("Общежитие №%d, комната %d\n", s->address.dorm.dorm_nubmer, s->address.dorm.room_nubmer);
        break;
    case AddressType_Rent:
        printf("Съём: %s, %d-%d, цена %.2f\n", s->address.rent.street, s->address.rent.house_nubmer,
               s->address.rent.apartment_nubmer, s->address.rent.rent_cost);
        break;
    }
    printf("\n");
}

// ============================================================================
// 11. Поиск студентов по адресу: Дом
// ============================================================================
void find_home_students(const student_t *students, size_t count)
{
    int choice;
    printf("Искать по:\n1 - улице\n2 - номеру дома\n3 - номеру квартиры\nВаш "
           "выбор: ");
    scanf("%d", &choice);

    int found = 0;
    if (choice == 1)
    {
        char street[MAX_STRING_LEN];
        printf("Введите улицу: ");
        scanf("%s", street);
        for (size_t i = 0; i < count; i++)
        {
            if (students[i].adress_kind == AddressType_Home && strcmp(students[i].address.home.street, street) == 0)
            {
                print_student(&students[i]);
                found = 1;
            }
        }
    }
    else if (choice == 2)
    {
        int house;
        printf("Введите номер дома: ");
        scanf("%d", &house);
        for (size_t i = 0; i < count; i++)
        {
            if (students[i].adress_kind == AddressType_Home && students[i].address.home.house_nubmer == house)
            {
                print_student(&students[i]);
                found = 1;
            }
        }
    }
    else if (choice == 3)
    {
        int apt;
        printf("Введите номер квартиры: ");
        scanf("%d", &apt);
        for (size_t i = 0; i < count; i++)
        {
            if (students[i].adress_kind == AddressType_Home && students[i].address.home.apartment_nubmer == apt)
            {
                print_student(&students[i]);
                found = 1;
            }
        }
    }

    if (!found)
        printf("Ничего не найдено.\n");
}

// ============================================================================
// 12. Поиск студентов по адресу: Общежитие
// ============================================================================
void find_dorm_students(const student_t *students, size_t count)
{
    int choice;
    printf("Искать по:\n1 - номеру общежития\n2 - номеру комнаты\nВаш выбор: ");
    scanf("%d", &choice);

    int found = 0;
    if (choice == 1)
    {
        int dorm;
        printf("Введите номер общежития: ");
        scanf("%d", &dorm);
        for (size_t i = 0; i < count; i++)
        {
            if (students[i].adress_kind == AddressType_Dorm && students[i].address.dorm.dorm_nubmer == dorm)
            {
                print_student(&students[i]);
                found = 1;
            }
        }
    }
    else if (choice == 2)
    {
        int room;
        printf("Введите номер комнаты: ");
        scanf("%d", &room);
        for (size_t i = 0; i < count; i++)
        {
            if (students[i].adress_kind == AddressType_Dorm && students[i].address.dorm.room_nubmer == room)
            {
                print_student(&students[i]);
                found = 1;
            }
        }
    }

    if (!found)
        printf("Ничего не найдено.\n");
}

// ============================================================================
// 13. Поиск студентов по адресу: Съёмное жильё
// ============================================================================
void find_rent_students(const student_t *students, size_t count)
{
    int choice;
    printf("Искать по:\n1 - улице\n2 - номеру дома\n3 - номеру квартиры\n4 - "
           "стоимости <= X\nВаш выбор: ");
    scanf("%d", &choice);

    int found = 0;
    if (choice == 1)
    {
        char street[MAX_STRING_LEN];
        printf("Введите улицу: ");
        scanf("%s", street);
        for (size_t i = 0; i < count; i++)
        {
            if (students[i].adress_kind == AddressType_Rent && strcmp(students[i].address.rent.street, street) == 0)
            {
                print_student(&students[i]);
                found = 1;
            }
        }
    }
    else if (choice == 2)
    {
        int house;
        printf("Введите номер дома: ");
        scanf("%d", &house);
        for (size_t i = 0; i < count; i++)
        {
            if (students[i].adress_kind == AddressType_Rent && students[i].address.rent.house_nubmer == house)
            {
                print_student(&students[i]);
                found = 1;
            }
        }
    }
    else if (choice == 3)
    {
        int apt;
        printf("Введите номер квартиры: ");
        scanf("%d", &apt);
        for (size_t i = 0; i < count; i++)
        {
            if (students[i].adress_kind == AddressType_Rent && students[i].address.rent.apartment_nubmer == apt)
            {
                print_student(&students[i]);
                found = 1;
            }
        }
    }
    else if (choice == 4)
    {
        double max_cost;
        printf("Введите максимальную стоимость: ");
        scanf("%lf", &max_cost);
        for (size_t i = 0; i < count; i++)
        {
            if (students[i].adress_kind == AddressType_Rent && students[i].address.rent.rent_cost <= max_cost)
            {
                print_student(&students[i]);
                found = 1;
            }
        }
    }

    if (!found)
        printf("Ничего не найдено.\n");
}



static void find_home(const student_t *students, size_t count)
{
    char street[MAX_STRING_LEN];
    int house;
    printf("Введите улицу: ");
    scanf("%s", street);
    printf("Введите номер дома: ");
    scanf("%d", &house);

    int found = 0;
    for (size_t i = 0; i < count; i++)
    {
        if (students[i].adress_kind == AddressType_Home && strcmp(students[i].address.home.street, street) == 0 &&
            students[i].address.home.house_nubmer == house)
        {
            print_students_array((student_t *)&students[i], 1);
            found = 1;
        }
    }
    if (!found)
        printf("Студенты не найдены.\n");
}

static void find_dorm(const student_t *students, size_t count)
{
    int dorm, room;
    printf("Введите номер общежития: ");
    scanf("%d", &dorm);
    printf("Введите номер комнаты: ");
    scanf("%d", &room);

    int found = 0;
    for (size_t i = 0; i < count; i++)
    {
        if (students[i].adress_kind == AddressType_Dorm && students[i].address.dorm.dorm_nubmer == dorm &&
            students[i].address.dorm.room_nubmer == room)
        {
            print_students_array((student_t *)&students[i], 1);
            found = 1;
        }
    }
    if (!found)
        printf("Студенты не найдены.\n");
}

static void find_rent(const student_t *students, size_t count)
{
    char street[MAX_STRING_LEN];
    double max_price;
    printf("Введите улицу: ");
    scanf("%s", street);
    printf("Введите максимальную цену аренды: ");
    scanf("%lf", &max_price);

    int found = 0;
    for (size_t i = 0; i < count; i++)
    {
        if (students[i].adress_kind == AddressType_Rent && strcmp(students[i].address.rent.street, street) == 0 &&
            students[i].address.rent.rent_cost <= max_price)
        {
            print_students_array((student_t *)&students[i], 1);
            found = 1;
        }
    }
    if (!found)
        printf("Студенты не найдены.\n");
}

/* основная функция */
void find_students_by_variant(const student_t *students, size_t count)
{
    int choice;
    printf("Выберите тип адреса для поиска:\n");
    printf("0 - Дом\n1 - Общежитие\n2 - Съём\nВаш выбор: ");
    scanf("%d", &choice);

    switch (choice)
    {
    case AddressType_Home:
        find_home(students, count);
        break;
    case AddressType_Dorm:
        find_dorm(students, count);
        break;
    case AddressType_Rent:
        find_rent(students, count);
        break;
    default:
        printf("Неверный выбор!\n");
        break;
    }
}
