#include "key_table.h"

students_keys_t *build_students_keys_table_from_file(const char *filename, size_t *out_count)
{
    *out_count = 0;
    size_t student_count = 0;
    student_t *students = read_students_from_file(filename, &student_count);
    if (!students || student_count == 0)
        return NULL;

    students_keys_t *keys = malloc(student_count * sizeof(students_keys_t));
    if (!keys)
    {
        free(students);
        return NULL;
    }

    for (size_t i = 0; i < student_count; i++)
    {
        keys[i].key = students[i].avg_grade;
        keys[i].index = i;
    }

    *out_count = student_count;
    free(students);
    return keys;
}

students_keys_t *build_students_keys_table_from_students(student_t *students, size_t count)
{
    if (!students || count == 0)
        return NULL;

    students_keys_t *keys = malloc(count * sizeof(students_keys_t));
    if (!keys)
        return NULL;

    for (size_t i = 0; i < count; i++)
    {
        keys[i].key = students[i].avg_grade;
        keys[i].index = i;
    }

    return keys;
}

void sort_students_keys_table(students_keys_t *keys, size_t count)
{
    for (size_t i = 0; i + 1 < count; i++)
    {
        for (size_t j = 0; j + 1 < count - i; j++)
        {
            if (keys[j].key > keys[j + 1].key)
            {
                students_keys_t tmp = keys[j];
                keys[j] = keys[j + 1];
                keys[j + 1] = tmp;
            }
        }
    }
}

void sort_students_by_avg_grade(student_t *students, size_t count)
{
    if (!students || count < 2)
        return;

    for (size_t i = 0; i < count - 1; i++)
    {
        for (size_t j = 0; j < count - i - 1; j++)
        {
            if (students[j].avg_grade > students[j + 1].avg_grade)
            {
                student_t tmp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = tmp;
            }
        }
    }
}

void bubble_sort_students_by_key(student_t *students, size_t count)
{
    for (size_t i = 0; i < count - 1; i++)
    {
        for (size_t j = 0; j < count - i - 1; j++)
        {
            if (students[j].avg_grade > students[j + 1].avg_grade)
            {
                student_t tmp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = tmp;
            }
        }
    }
}

void bubble_sort_keys(students_keys_t *keys, size_t count)
{
    for (size_t i = 0; i < count - 1; i++)
    {
        for (size_t j = 0; j < count - i - 1; j++)
        {
            if (keys[j].key > keys[j + 1].key)
            {
                students_keys_t tmp = keys[j];
                keys[j] = keys[j + 1];
                keys[j + 1] = tmp;
            }
        }
    }
}

void print_students_keys_table(const students_keys_t *keys, size_t count)
{
    printf("\n================= Таблица ключей =================\n");
    printf("| %-10s | %-10s |\n", "Index", "Avg_Grade");
    printf("----------------------------------------\n");
    for (size_t i = 0; i < count; i++)
    {
        printf("| %-10zu | %-10.2lf |\n", keys[i].index, keys[i].key);
    }
    printf("----------------------------------------\n");
}

void print_students_array_by_index(student_t *students, size_t count, students_keys_t *keys)
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

    for (size_t j = 0; j < count; j++)
    {
        size_t i = keys[j].index;

        printf(" %2zu | %-10s | %-10s | %-10s | %-6c | %4d | %6.2f | "
               "%02s.%02s.%04s | %10d ",
               i, students[i].surname, students[i].name, students[i].group_name, students[i].gender, students[i].age,
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

void quicksort_students_by_avg_grade(student_t *arr, int low, int high)
{
    if (low < high)
    {
        double pivot = arr[high].avg_grade;
        int i = low - 1;
        for (int j = low; j < high; j++)
        {
            if (arr[j].avg_grade <= pivot)
            {
                i++;
                student_t tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
            }
        }
        student_t tmp = arr[i + 1];
        arr[i + 1] = arr[high];
        arr[high] = tmp;

        int pi = i + 1;
        quicksort_students_by_avg_grade(arr, low, pi - 1);
        quicksort_students_by_avg_grade(arr, pi + 1, high);
    }
}

void quicksort_keys(students_keys_t *arr, int low, int high)
{
    if (low < high)
    {
        double pivot = arr[high].key;
        int i = low - 1;
        for (int j = low; j < high; j++)
        {
            if (arr[j].key <= pivot)
            {
                i++;
                students_keys_t tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
            }
        }
        students_keys_t tmp = arr[i + 1];
        arr[i + 1] = arr[high];
        arr[high] = tmp;

        int pi = i + 1;
        quicksort_keys(arr, low, pi - 1);
        quicksort_keys(arr, pi + 1, high);
    }
}

student_t *copy_students_array(student_t *arr, size_t n)
{
    student_t *copy = malloc(n * sizeof(student_t));
    if (!copy)
        return NULL;
    for (size_t i = 0; i < n; i++)
        copy[i] = arr[i];
    return copy;
}

students_keys_t *copy_keys_array(students_keys_t *arr, size_t n)
{
    students_keys_t *copy = malloc(n * sizeof(students_keys_t));
    if (!copy)
        return NULL;
    for (size_t i = 0; i < n; i++)
        copy[i] = arr[i];
    return copy;
}

// Вместо copy_students_array - используем другое имя
student_t* duplicate_students_array(student_t *src, size_t count)
{
    if (!src || count == 0)
        return NULL;

    student_t *copy = malloc(count * sizeof(student_t));
    if (!copy)
        return NULL;

    for (size_t i = 0; i < count; i++)
    {
        if (copy_student(&copy[i], &src[i]) != 0)
        {
            free(copy);
            return NULL;
        }
    }

    return copy;
}

// Вместо copy_keys_array - используем другое имя  
students_keys_t* duplicate_keys_array(students_keys_t *src, size_t count)
{
    if (!src || count == 0)
        return NULL;

    students_keys_t *copy = malloc(count * sizeof(students_keys_t));
    if (!copy)
        return NULL;

    memcpy(copy, src, count * sizeof(students_keys_t));
    return copy;
}

// Функция для создания массива студентов нужного размера путем копирования исходных данных
student_t* create_students_array_of_size(student_t *base_students, size_t base_count, int target_size)
{
    if (!base_students || base_count == 0 || target_size <= 0)
        return NULL;

    student_t *result = malloc(target_size * sizeof(student_t));
    if (!result)
        return NULL;

    // Копируем базовые данные необходимое количество раз
    for (int i = 0; i < target_size; i++)
    {
        int base_idx = i % base_count;
        if (copy_student(&result[i], &base_students[base_idx]) != 0)
        {
            free(result);
            return NULL;
        }
    }

    return result;
}

// Функция для копирования студента
int copy_student(student_t *dest, const student_t *src)
{
    if (!dest || !src)
        return -1;

    memcpy(dest, src, sizeof(student_t));
    return 0;
}