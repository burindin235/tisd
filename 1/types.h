#ifndef TYPES
#define TYPES

// максимальный размер буферов (мантисса + запас)
#define NMAX 256

// максимальная длина мантиссы
#define MAX_MANTISSA 35

// структура: строка для хранения числа в виде текста
typedef char string_t[NMAX + 1];

// структура: массив (буфер) для хранения цифр числа в виде int
typedef int num_integer_part[NMAX];

// знак числа
typedef enum
{
    positive = 1,
    negative = -1
} num_sign;

// структура: длинное число в нормализованном виде
typedef struct
{
    num_sign sign;            // знак числа
    num_integer_part integer; // массив цифр (мантисса)
    int exp;                  // порядок (экспонента)
} long_t;

#endif
