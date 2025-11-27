#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "file_process.h"
#include "input.h"
#include "key_table.h"
#include "types.h"

#define DATAFILE "students.txt"

double get_time_ms();

// Функция для сохранения результата в файл
void save_students_to_file_if_needed(student_t *students, size_t count);

void menu()
{
    printf("\n\n\n===== МЕНЮ =====\n");
    printf("1. Показать всех студентов\n");
    printf("2. Добавить студента\n");
    printf("3. Удалить студентов по группе\n");
    printf("4. Вывести студентов (год + съём дешевле X)\n");
    printf("5. Поиск по вариантному полю (Дом/Общежитие/Съём)\n");
    printf("6. Вывод отсортированной таблицы ключей (по оценке) при неотсортированной таблице\n");
    printf("7. Сортировка исходной таблицы (по средней оценке)\n");
    printf("8. Сортировка исходной таблице по ключам\n");
    printf("9. Быстрая сортировка исходной таблицы\n");
    printf("10. Быстрая сортировка ключей\n");
    printf("11. Сравнение эффективности пузырька и быстрой сортировок\n");
    printf("0. Выход\n");
    printf("\nВаш выбор: ");
}

int main(void)
{
    int choice;

    while (1)
    {
        menu();
        if (scanf("%d", &choice) != 1)
        {
            printf("Ошибка ввода!\n");
            flush_stdin();
            continue;
        }
        flush_stdin();

        if (choice == 0)
        {
            printf("Выход.\n\n");
            break;
        }

        printf("\n");
        switch (choice)
        {
        case 1: // Показать всех студентов
        {
            if (print_students_from_file_formatted(DATAFILE) != 0)
            {
                printf("Ошибка чтения файла.\n");
            }
        }
        break;

        case 2: // Добавить студента
        {
            int res = input_and_print_students_in_file(DATAFILE, 1);
            if (res == 0)
            {
                printf("Студент добавлен.\n");
            }
            else
            {
                printf("Ошибка добавления, code(%d)\n", res);
            }
        }
        break;

        case 3: // Удалить студентов по группе
        {
            char group[MAX_STRING_LEN];
            printf("Введите группу для удаления: ");
            scanf("%s", group);

            int result = delete_students_by_group_in_file(DATAFILE, group);

            if (result == 0)
                printf("Удалено.\n");

            else if (result == -1)
                printf("Нет студентов с такой группой");
            else
                printf("Ошибка.\n");
        }
        break;

        case 4: // Вывести студентов (год + съём дешевле X)
        {
            char year[5];
            double max_rent;

            printf("Введите год поступления: ");
            scanf("%s", year);
            printf("Введите макс. стоимость: ");
            scanf("%lf", &max_rent);

            print_students_rent_cheaper_than(DATAFILE, year, max_rent);
        }
        break;

        case 5: // Поиск по вариантному полю (Дом/Общежитие/Съём)
        {
            size_t count;
            student_t *students = read_students_from_file(DATAFILE, &count);
            if (!students)
            {
                printf("Ошибка чтения файла.\n");
                break;
            }
            find_students_by_variant(students, count);
            free(students);
        }
        break;

        case 6: // Вывод отсортированной таблицы ключей (по оценке) при неотсортированной таблице
        {
            size_t count;
            students_keys_t *keys = build_students_keys_table_from_file(DATAFILE, &count);
            if (!keys)
            {
                printf("Ошибка чтения файла или файл пуст.\n");
                break;
            }

            printf("Таблица ключей до сортировки:\n");
            print_students_keys_table(keys, count);

            sort_students_keys_table(keys, count);

            printf("\nТаблица ключей после сортировки:\n");
            print_students_keys_table(keys, count);

            char ans;
            printf("\nВывести исходную таблицу студентов? (y/n): ");
            scanf(" %c", &ans);
            if (ans == 'y' || ans == 'Y')
            {
                print_students_from_file_formatted(DATAFILE);
            }

            free(keys);
        }
        break;

        case 7: // Сортировка исходной таблицы (по средней оценке)
        {
            size_t count;
            student_t *students = read_students_from_file(DATAFILE, &count);
            if (!students)
            {
                printf("Ошибка чтения файла.\n");
                break;
            }

            sort_students_by_avg_grade(students, count);
            print_students_array(students, count);
            free(students);
        }
        break;

        case 8: // Сортировка исходной таблице по ключам
        {
            size_t count;
            students_keys_t *keys = build_students_keys_table_from_file(DATAFILE, &count);
            if (!keys)
            {
                printf("Ошибка создания ключей.\n");
                break;
            }

            sort_students_keys_table(keys, count);

            student_t *students = read_students_from_file(DATAFILE, &count);
            if (!students)
            {
                printf("Ошибка чтения студентов.\n");
                free(keys);
                break;
            }

            print_students_array_by_index(students, count, keys);

            free(students);
            free(keys);
        }
        break;

        case 9: // Быстрая сортировка исходной таблицы
        {
            size_t count;
            student_t *students = read_students_from_file(DATAFILE, &count);
            if (!students)
            {
                printf("Ошибка чтения файла.\n");
                break;
            }

            quicksort_students_by_avg_grade(students, 0, count - 1);
            print_students_array(students, count);

            free(students);
        }
        break;

        case 10: // Быстрая сортировка по ключам
        {
            size_t count;
            students_keys_t *keys = build_students_keys_table_from_file(DATAFILE, &count);
            if (!keys)
            {
                printf("Ошибка чтения ключей.\n");
                break;
            }

            quicksort_keys(keys, 0, count - 1);

            student_t *students = read_students_from_file(DATAFILE, &count);
            if (!students)
            {
                printf("Ошибка чтения студентов.\n");
                free(keys);
                break;
            }

            print_students_array_by_index(students, count, keys);

            free(students);
            free(keys);
        }
        break;
        case 11: // Сравнение эффективности пузырька и быстрой сортировки
        {
            // Список тестовых файлов
            const char *test_files[] = 
            {
                "tests/file_50.txt",
                "tests/file_100.txt", 
                "tests/file_200.txt",
                "tests/file_300.txt",
                "tests/file_400.txt",
                "tests/file_500.txt",
                "tests/file_600.txt",
                "tests/file_700.txt",
                "tests/file_800.txt",
                "tests/file_900.txt",
                "tests/file_1000.txt",
                "tests/file_1100.txt",
                "tests/file_1200.txt",
                "tests/file_1300.txt",
                "tests/file_1400.txt",
                "tests/file_1500.txt"
            };
            
            int num_files = 16;
            int num_runs = 15; // Количество запусков для каждого замера

            // Размеры структур (берем из реальных типов)
            size_t student_size = sizeof(student_t);
            size_t key_size = sizeof(students_keys_t);

            printf("=== ЗАМЕРНЫЙ ЭКСПЕРИМЕНТ ===\n");
            printf("Размер одной записи студента: %zu байт\n", student_size);
            printf("Размер одного ключа: %zu байт\n\n", key_size);

            for (int file_idx = 0; file_idx < num_files; file_idx++)
            {
                const char *filename = test_files[file_idx];
                printf("=== ФАЙЛ: %s ===\n", filename);

                // Читаем данные из файла
                size_t count;
                student_t *test_students = read_students_from_file(filename, &count);
                if (!test_students || count == 0)
                {
                    printf("Ошибка чтения файла или файл пуст: %s\n", filename);
                    continue;
                }

                // Расчет использования памяти
                size_t total_memory = (student_size * count) + (key_size * count);
                size_t table_memory = student_size * count;
                size_t keys_memory = key_size * count;
                
                double table_memory_percent = (double)table_memory / total_memory * 100;
                double keys_memory_percent = (double)keys_memory / total_memory * 100;

                double total_bubble_students = 0, total_quick_students = 0;
                double total_bubble_keys = 0, total_quick_keys = 0;

                for (int run = 0; run < num_runs; run++)
                {
                    // 1. Сортировка исходной таблицы пузырьком
                    student_t *copy1 = duplicate_students_array(test_students, count);
                    double start = get_time_ms();
                    bubble_sort_students_by_key(copy1, count);
                    double end = get_time_ms();
                    total_bubble_students += (end - start);
                    free(copy1);

                    // 2. Сортировка исходной таблицы квиксортом
                    student_t *copy2 = duplicate_students_array(test_students, count);
                    start = get_time_ms();
                    quicksort_students_by_avg_grade(copy2, 0, count - 1);
                    end = get_time_ms();
                    total_quick_students += (end - start);
                    free(copy2);

                    // 3. Сортировка массива ключей пузырьком
                    students_keys_t *keys1 = build_students_keys_table_from_students(test_students, count);
                    start = get_time_ms();
                    bubble_sort_keys(keys1, count);
                    end = get_time_ms();
                    total_bubble_keys += (end - start);
                    free(keys1);

                    // 4. Сортировка массива ключей квиксортом
                    students_keys_t *keys2 = build_students_keys_table_from_students(test_students, count);
                    start = get_time_ms();
                    quicksort_keys(keys2, 0, count - 1);
                    end = get_time_ms();
                    total_quick_keys += (end - start);
                    free(keys2);
                }

                // Вычисляем среднее время
                double avg_bubble_students = total_bubble_students / num_runs;
                double avg_quick_students = total_quick_students / num_runs;
                double avg_bubble_keys = total_bubble_keys / num_runs;
                double avg_quick_keys = total_quick_keys / num_runs;

                // Выводим результаты для текущего файла
                printf("Записей: %ld\n", count);
                printf("Использование памяти:\n");
                printf("  Всего: %zu байт (100%%)\n", total_memory);
                printf("  Таблица: %zu байт (%.1f%%)\n", table_memory, table_memory_percent);
                printf("  Ключи: %zu байт (%.1f%%)\n", keys_memory, keys_memory_percent);
                
                printf("\nСреднее время за %d запусков:\n", num_runs);
                printf("Пузырёк (студенты):   %.6f сек (100%%)\n", avg_bubble_students);
                printf("QuickSort (студенты): %.6f сек (%.1f%%)\n", 
                    avg_quick_students, (avg_bubble_students / avg_quick_students) * 100);
                printf("Пузырёк (ключи):      %.6f сек (%.1f%%)\n", 
                    avg_bubble_keys, (avg_bubble_students / avg_bubble_keys) * 100);
                printf("QuickSort (ключи):    %.6f сек (%.1f%%)\n", 
                    avg_quick_keys, (avg_bubble_students / avg_quick_keys) * 100);
                
                // Разделительная линия
                printf("\n");
                for (int i = 0; i < 50; i++) printf("=");
                printf("\n\n");

                free(test_students);
            }

            break;
        }

        default:
            printf("Нет такого пункта меню.\n");
        }
    }

    return 0;
}

double get_time_ms()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
}

void save_students_to_file_if_needed(student_t *students, size_t count)
{
    char ans;
    printf("\nСохранить результат в файл? (y/n): ");
    scanf(" %c", &ans);
    if (ans == 'y' || ans == 'Y')
    {
        char filename[64];
        printf("Введите имя файла: ");
        scanf("%63s", filename);
        if (rewrite_students_to_file(filename, students, count) == 0)
            printf("Файл успешно сохранён: %s\n", filename);
        else
            printf("Ошибка при сохранении файла.\n");
    }
}
