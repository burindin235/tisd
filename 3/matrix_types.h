#ifndef TYPES_H
#define TYPES_H

#define EPS 1e-9

typedef struct
{
    // ненулевые элементы по порядку
    double *A;

    // индексы столбцов для элеметов
    int *JA;

    // индексы начала элементов строк
    int *IA;

} matrix_csr;

typedef struct
{
    // ненулевые элементы по порядку
    double *B;

    // индексы строк для элеметов
    int *JB;

    // индексы начала элементов столбцов
    int *IB;
} matrix_csc;

#endif
