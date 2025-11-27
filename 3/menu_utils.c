#include "menu_utils.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void print_menu(void)
{
    printf("\n=== МЕНЮ ===\n");
    printf("1. Ввод матриц\n");
    printf("2. Вывод текущих матриц\n");
    printf("3. Умножение стандартным алгоритмом\n");
    printf("4. Умножение разреженным алгоритмом\n");
    printf("5. Замеры и сравнение алгоритмов\n");
    printf("6. Выход\n");
    printf("Выберите пункт: ");
}

void print_matrix_input_menu(void)
{
    printf("1. Ввести вручную\n");
    printf("2. Сгенерировать случайно\n");
    printf("3. Ввести координатным способом\n");
    printf("Выберите вариант: ");
}

void print_output_sparse_matrix_menu(int is_huge)
{
    if (is_huge == 0)
    {
        printf("В каком формате вывести исходные матрицы?\n");
        printf("1. В обычной форме\n");
        printf("2. В CSR/CSC форме\n");
        printf("3. В обоих форматах\n");
        printf("4. Не выводить\n");
    }
    else
    {
        printf("В каком формате вывести исходные матрицы?\n");
        printf("1. В CSR/CSC форме\n");
        printf("2. Не выводить\n");
    }

    printf("Выберите вариант: ");
}

void print_output_dense_matrix_menu(int is_huge)
{
    if (is_huge == 0)
    {
        printf("Вывести матрицы?\n");
        printf("1. Вывести\n");
        printf("2. Не выводить\n");
        printf("Выберите вариант: ");
    }
    else
    {
        printf("Матрицы слишком большие, невозможно вывести на экран.\n");
    }
}

void print_dense_result_matrix_menu(int is_huge, int rows, int cols)
{
    if (is_huge == 0)
    {
        printf("\n");
        printf("Вывести результат умножения?\n");
        printf("1. Вывести\n");
        printf("2. Не выводить\n");
        printf("Выберите вариант: ");
    }
    else
    {
        printf("Результирующая матрица слишком большая: %dx%d. Невозможно вывести\n", rows, cols);
    }
}

void print_sparse_result_matrix_menu(int is_huge, int rows, int cols)
{
    if (is_huge == 0)
    {
        printf("\n");
        printf("Вывести результат умножения?\n");
        printf("1. В CSR форме\n");
        printf("2. В обычной форме\n");
        printf("3. В обоих форматах\n");
        printf("4. Не выводить\n");
        printf("Выберите вариант: ");
    }
    else
    {
        printf("Результирующая матрица слишком большая: %dx%d.\n", rows, cols);
        printf("1. Вывести в CSR форме.\n");
        printf("2. Не выводить.\n");
        printf("Выберите вариант: ");
    }
}

void flush_stdin(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

int read_menu_choice(void)
{
    int value;
    if (scanf("%d", &value) != 1)
    {
        flush_stdin();
        return -1;
    }
    flush_stdin();
    return value;
}

double **init_matrix_with_choice(int *rows, int *cols, int num)
{
    while (1)
    {
        printf("Введите %d матрицу:\n", num);
        print_matrix_input_menu();
        int choice = read_menu_choice();
        double **matrix = NULL;

        if (choice == 1)
        {
            matrix = matrix_input(rows, cols);
        }
        else if (choice == 2)
        {
            if (!matrix_input_size(rows, cols))
                continue;
            int fill_ratio;
            printf("Введите степень заполненности матрицы (в процентах, 0-100): ");

            if (scanf("%d", &fill_ratio) != 1)
            {
                flush_stdin();
                printf("Неверный ввод\n");
                continue;
            }

            if (fill_ratio < 0 || fill_ratio > 100)
            {
                printf("Неверный ввод\n");
                continue;
            }

            matrix = matrix_generate(*rows, *cols, fill_ratio);
        }
        else if (choice == 3)
        {
            matrix = input_matrix_coordinate(rows, cols);
        }
        else
        {
            printf("Неверный выбор\n");
            continue;
        }

        if (!matrix)
        {
            printf("Ошибка при вводе или генерации матрицы\n");
            continue;
        }

        return matrix;
    }
}

void print_dense_matrix(double **matrix, int rows, int cols)
{
    if (!matrix)
        return;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
            printf("%8.2lf ", matrix[i][j]);
        printf("\n");
    }
}

void print_sparse_matrices(double **matrix1, int rows1, int cols1, double **matrix2, int rows2, int cols2,
                           matrix_csr *A_csr, matrix_csc *B_csc)
{
    int choice;
    int is_huge = (rows1 * cols1 >= 900 || rows2 * cols2 >= 900);

    if (is_huge == 0)
    {
        print_output_sparse_matrix_menu(is_huge);
        choice = read_menu_choice();
        if (choice == 1)
        {
            printf("\nПервая матрица:\n");
            print_dense_matrix(matrix1, rows1, cols1);
            printf("\nВторая матрица:\n");
            print_dense_matrix(matrix2, rows2, cols2);
        }
        else if (choice == 2)
        {
            printf("\nПервая матрица (CSR):\n");
            print_csr_lists(A_csr, rows1);
            printf("\nВторая матрица (CSC):\n");
            print_csc_lists(B_csc, cols2);
        }
        else if (choice == 3)
        {
            printf("\nПервая матрица:\n");
            print_dense_matrix(matrix1, rows1, cols1);
            printf("\nВторая матрица:\n");
            print_dense_matrix(matrix2, rows2, cols2);
            printf("\nПервая матрица (CSR):\n");
            print_csr_lists(A_csr, rows1);
            printf("\nВторая матрица (CSC):\n");
            print_csc_lists(B_csc, cols2);
        }
    }
    else
    {
        print_output_sparse_matrix_menu(is_huge);
        choice = read_menu_choice();
        if (choice == 1)
        {
            printf("\nПервая матрица (CSR):\n");
            print_csr_lists(A_csr, rows1);
            printf("\nВторая матрица (CSC):\n");
            print_csc_lists(B_csc, cols2);
        }
    }
}

void print_dense_matrices(double **matrix1, double **matrix2, int rows1, int cols1, int rows2, int cols2)
{
    int choice;
    int is_huge = (rows1 * cols1 >= 900 || rows2 * cols2 >= 900);

    if (is_huge == 0)
    {
        print_output_dense_matrix_menu(is_huge);
        choice = read_menu_choice();
        if (choice == 1)
        {
            printf("\nПервая матрица:\n");
            print_dense_matrix(matrix1, rows1, cols1);
            printf("\nВторая матрица:\n");
            print_dense_matrix(matrix2, rows2, cols2);
        }
    }
    else
    {
        print_output_dense_matrix_menu(is_huge);
    }
}

void print_result_sparse_matrix(matrix_csr *result_csr, int rows, int cols)
{
    if (!result_csr)
        return;

    int none_zero_elements = result_csr->IA[rows];
    if (none_zero_elements == 0)
    {
        printf("Результат умножения полностью нулевая матрица.\n");
        return;
    }

    int choice;
    int is_huge = (rows * cols >= 900);

    if (is_huge == 0)
    {
        print_sparse_result_matrix_menu(is_huge, rows, cols);
        choice = read_menu_choice();
        if (choice == 1)
        {
            print_csr_lists(result_csr, rows);
        }
        else if (choice == 2)
        {
            double **dense = csr_to_matrix(result_csr, rows, cols);
            print_dense_matrix(dense, rows, cols);
            matrix_free(dense, rows);
        }
        else if (choice == 3)
        {
            print_csr_lists(result_csr, rows);
            double **dense = csr_to_matrix(result_csr, rows, cols);
            printf("\nВ обычной форме:\n");
            print_dense_matrix(dense, rows, cols);
            matrix_free(dense, rows);
        }
    }
    else
    {
        print_sparse_result_matrix_menu(is_huge, rows, cols);
        choice = read_menu_choice();
        if (choice == 1)
        {
            print_csr_lists(result_csr, rows);
        }
        else
        {
            printf("Результат отображен не будет.\n");
        }
    }
}

void print_result_dense_matrix(double **result, int rows, int cols)
{
    if (is_dense_matrix_zero(result, rows, cols))
    {
        printf("Результат умножения полностью нулевая матрица.\n");
        return;
    }

    int choice;
    int is_huge = (rows * cols >= 900);

    if (is_huge == 0)
    {
        print_dense_result_matrix_menu(is_huge, rows, cols);
        choice = read_menu_choice();
        if (choice == 1)
        {
            print_dense_matrix(result, rows, cols);
        }
    }
    else
    {
        print_dense_result_matrix_menu(is_huge, rows, cols);
    }
}

int is_dense_matrix_zero(double **matrix, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (fabs(matrix[i][j]) > EPS)
                return 0;
        }
    }

    return 1;
}


void print_csr_lists(matrix_csr *mat, int rows)
{
    if (!mat)
        return;

    printf("\n--- CSR представление ---\n");

    printf("A: ");
    for (int i = 0; i < mat->IA[rows]; i++)
        printf("%.2lf ", mat->A[i]);
    printf("\n");

    printf("JA: ");
    for (int i = 0; i < mat->IA[rows]; i++)
        printf("%d ", mat->JA[i]);
    printf("\n");

    printf("IA: ");
    for (int i = 0; i <= rows; i++)
        printf("%d ", mat->IA[i]);
    printf("\n");
}

void print_csc_lists(matrix_csc *mat, int cols)
{
    if (!mat)
        return;

    printf("\n--- CSC представление ---\n");

    printf("B: ");
    for (int i = 0; i < mat->IB[cols]; i++)
        printf("%.2lf ", mat->B[i]);
    printf("\n");

    printf("JB: ");
    for (int i = 0; i < mat->IB[cols]; i++)
        printf("%d ", mat->JB[i]);
    printf("\n");

    printf("IB: ");
    for (int i = 0; i <= cols; i++)
        printf("%d ", mat->IB[i]);
    printf("\n");
}

void matrix_print(double **matrix, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
            printf("%8.2lf ", matrix[i][j]);
        printf("\n");
    }
}