#include "matrix_create.h"

int matrix_input_size(int *rows, int *cols)
{
    if (*rows == 0)
    {
        printf("Введите количество строк и столбцов: \n");
        if (scanf("%d %d", rows, cols) != 2)
        {
            printf("Ошибка ввода\n");
            flush_stdin();
            return 0;
        }
        if (*rows <= 0 || *cols <= 0)
        {
            printf("Ошибка: размеры должны быть положительными\n");
            flush_stdin();
            return 0;
        }
        return 1;
    }
    else
    {
        // Для второй матрицы количество строк фиксировано и равно количеству столбцов первой
        printf("Количество строк второй матрицы фиксировано: %d\n", *rows);
        printf("Введите количество столбцов второй матрицы: ");
        if (scanf("%d", cols) != 1)
        {
            printf("Ошибка ввода\n");
            flush_stdin();
            return 0;
        }
        if (*cols <= 0)
        {
            printf("Ошибка: размеры должны быть положительными\n");
            flush_stdin();
            return 0;
        }
        return 1;
    } 
}

double **matrix_input(int *rows, int *cols)
{
    if (!matrix_input_size(rows, cols))
        return NULL;
    double **matrix = matrix_allocate(*rows, *cols);
    if (!matrix)
    {
        printf("Ошибка при выделении памяти\n");
        return NULL;
    }

    printf("Введите матрицу:\n");
    for (int i = 0; i < *rows; i++)
    {
        for (int j = 0; j < *cols; j++)
        {
            if (scanf("%lf", &matrix[i][j]) != 1)
            {
                printf("Введено не число\n");
                flush_stdin();
                matrix_free(matrix, *rows);
                return NULL;
            }
        }
    }

    return matrix;
}

void matrix_free(double **matrix, int rows)
{
    for (int i = 0; i < rows; i++)
        free(matrix[i]);
    free(matrix);
}

matrix_csr *matrix_to_csr(double **matrix, int rows, int cols)
{
    int elems = matrix_calc_non_zero(matrix, rows, cols);

    matrix_csr *new_matrix = matrix_csr_allocate(elems, rows);
    if (!new_matrix)
        return NULL;

    new_matrix->IA[0] = 0;
    int index = 0;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (fabs(matrix[i][j]) > EPS)
            {
                new_matrix->A[index] = matrix[i][j];
                new_matrix->JA[index] = j;
                index++;
            }
        }

        new_matrix->IA[i + 1] = index;
    }

    return new_matrix;
}

int matrix_calc_non_zero(double **matrix, int rows, int cols)
{
    int count = 0;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (fabs(matrix[i][j]) > EPS)
                count++;
        }
    }
    return count;
}

matrix_csr *matrix_csr_allocate(int elems, int rows)
{
    matrix_csr *matrix = malloc(sizeof(matrix_csr));
    if (!matrix)
        return NULL;

    matrix->A = malloc(elems * sizeof(double));
    matrix->JA = malloc(elems * sizeof(int));
    matrix->IA = malloc((rows + 1) * sizeof(int));

    if (!matrix->A || !matrix->JA || !matrix->IA)
    {
        free(matrix->A);
        free(matrix->JA);
        free(matrix->IA);
        free(matrix);
        return NULL;
    }

    return matrix;
}

matrix_csc *matrix_csc_allocate(int elems, int cols)
{
    matrix_csc *matrix = (matrix_csc *)malloc(sizeof(matrix_csc));
    if (!matrix)
        return NULL;

    matrix->B = malloc(elems * sizeof(double));
    matrix->JB = malloc(elems * sizeof(int));
    matrix->IB = malloc((cols + 1) * sizeof(int));

    if (!matrix->B || !matrix->JB || !matrix->IB)
    {
        free(matrix->B);
        free(matrix->JB);
        free(matrix->IB);
        free(matrix);
        return NULL;
    }

    return matrix;
}

matrix_csc *matrix_to_csc(double **matrix, int rows, int cols)
{
    int elems = matrix_calc_non_zero(matrix, rows, cols);

    matrix_csc *new_matrix = matrix_csc_allocate(elems, cols);
    if (!new_matrix)
        return NULL;

    new_matrix->IB[0] = 0;
    int index = 0;

    for (int j = 0; j < cols; j++)
    {
        for (int i = 0; i < rows; i++)
        {
            if (fabs(matrix[i][j]) > EPS)
            {
                new_matrix->B[index] = matrix[i][j];
                new_matrix->JB[index] = i;
                index++;
            }
        }

        new_matrix->IB[j + 1] = index;
    }

    return new_matrix;
}

double **matrix_allocate(int rows, int cols)
{
    double **matrix = malloc(rows * sizeof(double *));
    if (!matrix)
        return NULL;

    for (int i = 0; i < rows; i++)
    {
        matrix[i] = calloc(cols, sizeof(double));
        if (!matrix[i])
        {
            for (int j = 0; j < i; j++)
                free(matrix[j]);

            free(matrix);
            return NULL;
        }
    }

    return matrix;
}

double **csr_to_matrix(matrix_csr *csr_matrix, int rows, int cols)
{
    double **matrix = matrix_allocate(rows, cols);

    if (!matrix)
        return NULL;

    for (int i = 0; i < rows; i++)
    {
        for (int index = csr_matrix->IA[i]; index < csr_matrix->IA[i + 1]; index++)
        {
            int j = csr_matrix->JA[index];
            matrix[i][j] = csr_matrix->A[index];
        }
    }

    return matrix;
}

double **csc_to_matrix(matrix_csc *csc_matrix, int rows, int cols)
{
    double **matrix = matrix_allocate(rows, cols);

    if (!matrix)
        return NULL;

    for (int i = 0; i < cols; i++)
    {
        for (int index = csc_matrix->IB[i]; index < csc_matrix->IB[i + 1]; index++)
        {
            int j = csc_matrix->JB[index];
            matrix[j][i] = csc_matrix->B[index];
        }
    }

    return matrix;
}

double random_double(double min, double max)
{
    return min + ((double)rand() / RAND_MAX) * (max - min);
}

void matrix_generate_indexes(int rows, int cols, int *rows_indexes, int *cols_indexes, int elems)
{
    int index_count = rows * cols;
    int *temp_array = malloc(index_count * sizeof(int));

    for (int i = 0; i < index_count; i++)
    {
        temp_array[i] = i;
    }

    // перемешивание массива, алгоритм фишера - йетса
    for (int i = index_count - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);

        int temp = temp_array[i];

        temp_array[i] = temp_array[j];
        temp_array[j] = temp;
    }

    for (int i = 0; i < elems; i++)
    {
        rows_indexes[i] = temp_array[i] / cols;
        cols_indexes[i] = temp_array[i] % cols;
    }

    free(temp_array);
}

double **matrix_generate(int rows, int cols, int fill_ratio)
{
    double **matrix = matrix_allocate(rows, cols);
    if (!matrix)
        return NULL;

    int elems_count = (int)(rows * cols * fill_ratio / 100.0);
    int *rows_indexes = malloc(elems_count * sizeof(int));
    int *cols_indexes = malloc(elems_count * sizeof(int));

    if (!rows_indexes || !cols_indexes)
    {
        free(rows_indexes);
        free(cols_indexes);
        matrix_free(matrix, rows);
        return NULL;
    }

    matrix_generate_indexes(rows, cols, rows_indexes, cols_indexes, elems_count);

    for (int k = 0; k < elems_count; k++)
    {
        int i = rows_indexes[k];
        int j = cols_indexes[k];
        matrix[i][j] = random_double(MIN_GENERATED_MATRIX_ELEMENT, MAX_GENERATED_MATRIX_ELEMENT);
    }

    free(rows_indexes);
    free(cols_indexes);

    return matrix;
}

double **input_matrix_coordinate(int *rows, int *cols)
{
    if (!matrix_input_size(rows, cols))
        return NULL;

    int non_zero_elements;
    printf("Введите количество ненулевых элементов: ");
    if (scanf("%d", &non_zero_elements) != 1 || non_zero_elements < 0)
    {
        printf("Ошибка: неверное количество ненулевых элементов\n");
        flush_stdin();
        return NULL;
    }

    double **matrix = matrix_allocate(*rows, *cols);
    if (!matrix)
    {
        printf("Ошибка выделения памяти\n");
        return NULL;
    }

    printf("Формат ввода: i j value (нумерация с 1)\n");

    for (int k = 0; k < non_zero_elements;)
    {
        int i, j;
        double val;
        if (scanf("%d %d %lf", &i, &j, &val) != 3)
        {
            printf("Ошибка ввода значения\n");
            flush_stdin();
            matrix_free(matrix, *rows);
            return NULL;
        }

        i--;
        j--;

        if (i < 0 || i >= *rows || j < 0 || j >= *cols)
        {
            printf("Неверные координаты (%d,%d), пропуск.\n", i + 1, j + 1);
            continue;
        }

        if (fabs(matrix[i][j]) > EPS)
        {
            flush_stdin();
            printf("Элемент (%d,%d) уже задан, введите другую координату.\n", i + 1, j + 1);
            continue;
        }



        matrix[i][j] = val;

        // счетчик увеличивается только в случае валидной записи
        k++;
    }

    return matrix;
}


void matrix_csr_free(matrix_csr *matrix)
{
    if (!matrix)
        return;
    free(matrix->A);
    free(matrix->JA);
    free(matrix->IA);
    free(matrix);
}

void matrix_csc_free(matrix_csc *matrix)
{
    if (!matrix)
        return;
    free(matrix->B);
    free(matrix->JB);
    free(matrix->IB);
    free(matrix);
}
