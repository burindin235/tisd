#include "benchmark.h"
#include "matrix_create.h"
#include "matrix_multiplicate.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_FILL_COUNT 100

double get_time_s(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9; // нано секуны / 1e9 = секунды
}

size_t calc_dense_memory(int rows, int cols)
{
    return sizeof(double *) * rows + sizeof(double) * rows * cols;
}

size_t calc_sparse_memory(matrix_csr *matrix, int rows)
{
    if (!matrix)
        return 0;
    return sizeof(matrix_csr) + sizeof(double) * matrix->IA[rows] + sizeof(int) * matrix->IA[rows] + sizeof(int) * (rows + 1);
}

void run_benchmarks(int is_fast)
{
    int sizes[MAX_SIZES_TO_BENCHMARK];
    int actual_size;

    if (is_fast == 1)
    {
        int temp[] = {10, 50, 100, 150, 200, 250, 300, 350, 400, 450, 500};
        actual_size = sizeof(temp) / sizeof(temp[0]);
        for (int i = 0; i < actual_size; i++)
            sizes[i] = temp[i];
    }
    else
    {
        int temp[] = {10,  50,   100,  150,  200,  250,  300,  400,  500,  600,  700,
                      800, 900, 1000};
        actual_size = sizeof(temp) / sizeof(temp[0]);
        for (int i = 0; i < actual_size; i++)
            sizes[i] = temp[i];
    }

    int fills[MAX_FILL_COUNT];
    int num_fills = 0;

    if (is_fast == 1)
    {
        int temp[] = {1, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100};
        num_fills = sizeof(temp) / sizeof(temp[0]);
        for (int i = 0; i < num_fills; i++)
            fills[i] = temp[i];
    }
    else
    {
        for (int i = 0; i < MAX_FILL_COUNT; i++)
            fills[i] = i + 1;
        num_fills = MAX_FILL_COUNT;
    }

    int num_sizes = actual_size;

    printf("\nКоличество матриц для прогона: %d\n\n", num_sizes);
    
    int *better_time_fill = malloc(sizeof(int) * num_sizes);
    int *better_mem_fill = malloc(sizeof(int) * num_sizes);
    array_init_by_num(better_time_fill, num_sizes, -1);
    array_init_by_num(better_mem_fill, num_sizes, -1);

    printf("Размеры | Заполнение%% | Время стандарт,с | Время разряж,с | Память стандарт,Б | Память разряж,Б | Разряж "
           "быстрее? | Кол-во замеров\n");
    printf("--------|-------------|------------------|----------------|-------------------|-----------------|----------"
           "-------|----------------\n");

    for (int size_i = 0; size_i < num_sizes; size_i++)
    {
        int n = sizes[size_i];
        int m = n;

        int repeats;
        if (n <= 50)
            repeats = 50;
        else if (n <= 150)
            repeats = 15;
        else if (n <= 300)
            repeats = 3;
        else
            repeats = 1;


        for (int fill_i = 0; fill_i < num_fills; fill_i++)
        {
            int cur_fill = fills[fill_i];

            // если пороги по памяти и времени найдены, то для текущих размеров замеры прекращаются
            if (better_time_fill[size_i] != -1 && better_mem_fill[size_i] != -1)
                break;

            double total_dense_time = 0.0;
            double total_sparse_time = 0.0;
            size_t mem_dense = 0;
            size_t mem_sparse = 0;

            for (int r = 0; r < repeats; r++)
            {
                double **A = matrix_generate(n, m, cur_fill);
                double **B = matrix_generate(m, n, cur_fill);
                if (!A || !B)
                {
                    printf("Ошибка выделения памяти для %dx%d c заполнением %d%%\n", n, m, cur_fill);
                    matrix_free(A, n);
                    matrix_free(B, m);
                    continue;
                }

                double t0 = get_time_s();
                double **C_std = multiply_standard(A, B, n, m, n);
                double t1 = get_time_s();
                total_dense_time += (t1 - t0);
                mem_dense = calc_dense_memory(n, m);

                matrix_csr *A_csr = matrix_to_csr(A, n, m);
                matrix_csc *B_csc = matrix_to_csc(B, m, n);

                t0 = get_time_s();
                matrix_csr *C_sparse = multiply_csr_csc(A_csr, B_csc, n, m, n);
                t1 = get_time_s();
                total_sparse_time += (t1 - t0);

                mem_sparse = calc_sparse_memory(C_sparse, n);

                matrix_free(A, n);
                matrix_free(B, m);
                matrix_free(C_std, n);
                matrix_csr_free(A_csr);
                matrix_csc_free(B_csc);
                matrix_csr_free(C_sparse);
            }

            double avg_dense_time = total_dense_time / repeats;
            double avg_sparse_time = total_sparse_time / repeats;

            int is_sprase_faster = avg_sparse_time < avg_dense_time;
            int is_sparse_more_mem = mem_sparse > mem_dense;

            printf("%3dx%-3d | %10d%% | %16.8f | %14.8f | %17zu | %15zu | %15s | %14d\n", n, n, cur_fill, avg_dense_time,
                   avg_sparse_time, mem_dense, mem_sparse, is_sprase_faster ? "Yes" : "No", repeats);

            if (better_time_fill[size_i] == -1 && is_sprase_faster == 0)
                better_time_fill[size_i] = cur_fill;
            if (better_mem_fill[size_i] == -1 && is_sparse_more_mem == 1)
                better_mem_fill[size_i] = cur_fill;
        }
    }

    printf("\n\n=== Итоговая таблица: пороги заполненности, когда обычная становится лучше разреженной ===\n");
    printf("   Размер   |  Порог по времени (%%)  | Порог по памяти (%%)\n");
    printf("------------|------------------------|----------------------\n");

    for (int i = 0; i < num_sizes; i++)
    {
        printf("%3dx%-3d     | %22d | %20d\n", sizes[i], sizes[i], better_time_fill[i], better_mem_fill[i]);
    }
}

void array_init_by_num(int *arr, int arr_len, int num)
{
    for (int i = 0; i < arr_len; i++)
        arr[i] = num;
}
