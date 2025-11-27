#include "benchmark.h"
#include "matrix_create.h"
#include "matrix_multiplicate.h"
#include "menu_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


// вывод можно выбирать несколько раз
// количество строк не спрашивается во второй матрице


int main(void)
{
    srand((unsigned int)time(NULL));

    // Текущие матрицы во всём сеансе
    double **matrix1 = NULL;
    double **matrix2 = NULL;
    int rows1 = 0, cols1 = 0, rows2 = 0, cols2 = 0;

    while (1)
    {
        print_menu();
        int menu_choice = read_menu_choice();

        switch (menu_choice)
        {
        case 1: // Ввод матриц
        {
            if (matrix1)
            {
                matrix_free(matrix1, rows1);
                matrix1 = NULL;
            }
            if (matrix2)
            {
                matrix_free(matrix2, rows2);
                matrix2 = NULL;
            }

            rows1 = cols1 = rows2 = cols2 = 0;

            printf("\n -- Ввод первой матрицы: -- \n");
            matrix1 = init_matrix_with_choice(&rows1, &cols1, 1);
            rows2 = cols1; // количество строк второй фиксировано
            printf("\n -- Ввод второй матрицы: -- \n");
            matrix2 = init_matrix_with_choice(&rows2, &cols2, 2);

            if (cols1 != rows2)
            {
                printf("Умножить матрицы невозможно: кол-во столбцов первой не равно кол-во строк второй\n");
                matrix_free(matrix1, rows1);
                matrix_free(matrix2, rows2);
                matrix1 = matrix2 = NULL;
                rows1 = cols1 = rows2 = cols2 = 0;
                break;
            }

            break;
        }

        case 2: // Вывод текущих матриц
        {
            if (!matrix1 || !matrix2)
            {
                printf("Матрицы не введены. Сначала выполните пункт 1.\n");
                break;
            }

            // Спросить формат вывода и при необходимости перевести в разрежённый
            matrix_csr *A_csr = matrix_to_csr(matrix1, rows1, cols1);
            matrix_csc *B_csc = matrix_to_csc(matrix2, rows2, cols2);
            print_sparse_matrices(matrix1, rows1, cols1, matrix2, rows2, cols2, A_csr, B_csc);
            matrix_csr_free(A_csr);
            matrix_csc_free(B_csc);
            break;
        }

        case 3: // Умножение стандартным алгоритмом
        {
            if (!matrix1 || !matrix2)
            {
                printf("Сначала необходимо ввести матрицы (пункт 1).\n");
                break;
            }

            double **C = multiply_standard(matrix1, matrix2, rows1, cols1, cols2);
            // Вывод результата
            print_result_dense_matrix(C, rows1, cols2);
            matrix_free(C, rows1);
            break;
        }

        case 4: // Умножение разрежённым алгоритмом
        {
            if (!matrix1 || !matrix2)
            {
                printf("Сначала необходимо ввести матрицы (пункт 1).\n");
                break;
            }

            matrix_csr *A_csr = matrix_to_csr(matrix1, rows1, cols1);
            matrix_csc *B_csc = matrix_to_csc(matrix2, rows2, cols2);
            matrix_csr *C_csr = multiply_csr_csc(A_csr, B_csc, rows1, cols1, cols2);
            // Вывод результата (CSR/обычной/обеих/не выводить)
            print_result_sparse_matrix(C_csr, rows1, cols2);

            matrix_csr_free(A_csr);
            matrix_csc_free(B_csc);
            matrix_csr_free(C_csr);
            break;
        }

        case 5:
        {
            printf("Выберите, короткие или полные замеры необходимо произвести\n");
            printf("1. Короткие (около 10 секунд)\n");
            printf("2. Полные (около 3 минут)\n");
            int bench_choice;
            do
            {
                printf("Выбор: ");
                bench_choice = read_menu_choice();
                if (bench_choice == -1)
                    printf("Ошибка ввода, попробуйте ещё раз: \n");
            } while (bench_choice != 1 && bench_choice != 2);

            run_benchmarks(bench_choice);
            break;
        }

        case 6:
        {
            if (matrix1)
                matrix_free(matrix1, rows1);
            if (matrix2)
                matrix_free(matrix2, rows2);
            return 0;
        }

        default:
            printf("Неверный выбор\n");
            break;
        }
    }

    return 0;
}

