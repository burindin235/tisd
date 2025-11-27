#include "nums_division.h"

/* Удаляет ведущие нули в массиве цифр.
   Возвращает новую длину; изменяет содержимое массива. */
static int trim_leading_zeros(int *a, int len)
{
    int i = 0;
    while (i < len && a[i] == 0)
        i++;
    if (i == len)
    {
        a[0] = 0;
        return 1;
    }
    if (i > 0)
    {
        for (int j = 0; j < len - i; j++)
            a[j] = a[j + i];
        len -= i;
    }
    return len;
}

/* Сравнивает два массива цифр без изменения исходных массивов:
   возвращает 1 если a > b, -1 если a < b, 0 если равны. */
int compare_arrays(int *a, int len_a, int *b, int len_b)
{
    int ia = 0;
    while (ia < len_a && a[ia] == 0)
        ia++;
    int ib = 0;
    while (ib < len_b && b[ib] == 0)
        ib++;

    int la = len_a - ia;
    int lb = len_b - ib;

    if (la != lb)
        return (la > lb) ? 1 : -1;

    for (int i = 0; i < la; i++)
    {
        int da = a[ia + i];
        int db = b[ib + i];
        if (da > db)
            return 1;
        if (da < db)
            return -1;
    }
    return 0;
}

/* Умножение массива цифр
   Возвращает длину результата */
int multiply_array_by_digit(int *a, int len_a, int digit, int *res)
{
    if (digit == 0)
    {
        res[0] = 0;
        return 1;
    }
    int is_overflow = 0;
    for (int i = len_a - 1; i >= 0; i--)
    {
        int prod = a[i] * digit + is_overflow;
        res[i] = prod % 10;
        is_overflow = prod / 10;
    }
    int res_len = len_a;
    if (is_overflow > 0)
    {
        for (int i = res_len; i > 0; i--)
            res[i] = res[i - 1];
        res[0] = is_overflow;
        res_len++;
    }
    return res_len;
}

/* Вычитание b из a a >= b.
   Возвращает новую длину массива a после вычитания. */
int subtract_arrays(int *a, int len_a, int *b, int len_b)
{
    int ia = len_a - 1;
    int ib = len_b - 1;
    int borrow = 0;
    while (ib >= 0)
    {
        int sub = a[ia] - b[ib] - borrow;
        if (sub < 0)
        {
            sub += 10;
            borrow = 1;
        }
        else
        {
            borrow = 0;
        }
        a[ia] = sub;
        ia--;
        ib--;
    }
    while (ia >= 0 && borrow)
    {
        int sub = a[ia] - borrow;
        if (sub < 0)
        {
            sub += 10;
            borrow = 1;
        }
        else
        {
            borrow = 0;
        }
        a[ia] = sub;
        ia--;
    }
    return trim_leading_zeros(a, len_a);
}

/* Нормализация строки числа: убирает лишние ведущие/хвостовые нули,
   обрабатывает форматы типа ".123", "+.123", сохраняет экспоненту "e..." при наличии. */
void structuring_num(string_t string_num, long_t *long_num, int significant_nums, int *new_size)
{
    string_t normalized;
    normalizing_num(string_num, normalized);

    char *p = normalized;

    if (*p == '-')
    {
        long_num->sign = negative;
        p++;
    }
    else
    {
        long_num->sign = positive;
        if (*p == '+')
            p++;
    }

    char *e_pos = strchr(p, 'e');
    int exp_val = 0;
    if (e_pos != NULL)
    {
        exp_val = atoi(e_pos + 1);
        *e_pos = '\0';
    }

    char *dot = strchr(p, '.');
    int frac_len = 0;
    int idx = 0;

    int leading = 1;
    while (*p && p != dot)
    {
        if (leading && *p == '0')
        {
            p++;
            continue;
        }
        leading = 0;
        long_num->integer[idx++] = *p - '0';
        p++;
    }

    if (dot != NULL)
    {
        p++;
        while (*p)
        {
            if (leading && *p == '0')
            {
                p++;
                frac_len++;
                continue;
            }
            leading = 0;
            long_num->integer[idx++] = *p - '0';
            p++;
            frac_len++;
        }
    }

    *new_size = idx;
    long_num->exp = exp_val - frac_len;
}

void normalizing_num(string_t num, string_t normalized_num)
{
    char *e_pos = strchr(num, 'e');

    /* буферы для хранения мантиссы и экспоненциальной части*/
    char mantissa[NMAX + 1];
    char exponent[NMAX + 1];

    if (e_pos != NULL)
    {
        strcpy(exponent, e_pos);
        *e_pos = '\0';
    }
    else
    {
        exponent[0] = '\0';
    }

    strcpy(mantissa, num);

    if (mantissa[0] == '.')
    {
        char temp[NMAX + 1];
        strcpy(temp, mantissa);
        strcpy(mantissa, "0");
        strcat(mantissa, temp);
    }

    /* буфер для хранения вещественной части числа*/
    char result[NMAX + 1];
    char *dst = result;
    char *src = mantissa;

    if (*src == '+' || *src == '-')
        *dst++ = *src++;

    int leading = 1;
    char *dot = strchr(src, '.');

    while (*src)
    {
        if (leading)
        {
            if (*src == '0' && (dot == NULL || src < dot - 1))
            {
                src++;
                continue;
            }
            leading = 0;
        }
        *dst++ = *src++;
    }
    *dst = '\0';

    dot = strchr(result, '.');
    if (dot != NULL)
    {
        char *end = result + strlen(result) - 1;
        while (end > dot && *end == '0')
        {
            *end = '\0';
            end--;
        }
        if (end == dot)
            *end = '\0';
    }

    strcpy(normalized_num, result);
    strcat(normalized_num, exponent);
}

/* Проверка: все ли цифры в long_t нулевые (len — количество значащих цифр). */
int is_zero(long_t *num, int len)
{
    for (int i = 0; i < len; i++)
    {
        if (num->integer[i] != 0)
            return 0;
    }
    return 1;
}

/*
  division_big_nums:
  - Возвращает >0 — количество значащих цифр результата (до 35 (MAX_MANTISSA)).
  - Возвращает 1 — результат равен нулю (мантисса {0}, exp = 0).
  - Возвращает отрицательные коды ошибок:
      -1 — переполнение буфера
      -2 — деление на ноль
*/
int division_big_nums(long_t numerator, int numerator_len, long_t denominator, int denominator_len,
                      long_t *result, int *rc)
{
    int num_buf[WORK_MAX];           // буфер для делимого    
    int denom_buf[WORK_MAX];         // буфер для делителя  
    int quotient_buf[WORK_MAX + 1];  // буфер для частного

    int num_len = 0;      
    int denom_len = 0;    
    int quotient_len = 0; 

    //if (atoi(denominator.integer) == 1)

    
    // заполнение временных буферов цифрами числителя
    for (int i = 0; i < numerator_len; i++)
        num_buf[i] = numerator.integer[i];
    num_len = numerator_len;

    // заполнение временных буферов цифрами знаменателя
    for (int i = 0; i < denominator_len; i++)
        denom_buf[i] = denominator.integer[i];
    denom_len = denominator_len;

    // учёт порядка (exp) у числителя
    if (numerator.exp > 0)
    {
        // слишком большое число после сдвига
        if (num_len + numerator.exp >= WORK_MAX)
        {
            *rc = -10;
            return -1;
        }
        // домножаем на 10^x добавляем нули справа
        for (int i = 0; i < numerator.exp; i++)
            num_buf[num_len + i] = 0;
        num_len += numerator.exp;
    }
    else if (numerator.exp < 0)
    {
        int shift = -numerator.exp;
        // слишком маленькое число после сдвига
        if (denom_len + shift >= WORK_MAX)
        {
            *rc = -11;
            return -1;
        }
        // домножаем знаменатель на 10^(-x) добавляем нули справа
        for (int i = 0; i < shift; i++)
            denom_buf[denom_len + i] = 0;

        denom_len += shift;
    }

    // удаление ведущих нулей у числителя и знаменателя
    num_len = trim_leading_zeros(num_buf, num_len);
    denom_len = trim_leading_zeros(denom_buf, denom_len);

    // частный случай числитель меньше знаменателя
    if (compare_arrays(num_buf, num_len, denom_buf, denom_len) < 0)
    {
        quotient_buf[quotient_len++] = 0;
    }
    else
    {
        // иначе деление
        int shift_pos = num_len - denom_len;
        for (int shift = shift_pos; shift >= 0; shift--)
        {
            int digit;
            int prod[WORK_MAX];
            int prod_len = 0;

            // подбор цифры digit (от 9 до 0)
            for (digit = 9; digit >= 0; digit--)
            {
                prod_len = multiply_array_by_digit(denom_buf, denom_len, digit, prod);
                int prod_shift_len = prod_len + shift;
                int prod_shift[WORK_MAX];
                // копируем произведение
                for (int i = 0; i < prod_len; i++)
                    prod_shift[i] = prod[i];
                // добавляем нули для сдвига
                for (int i = prod_len; i < prod_shift_len; i++)
                    prod_shift[i] = 0;
                // проверяем, помещается ли в остаток
                if (compare_arrays(prod_shift, prod_shift_len, num_buf, num_len) <= 0)
                    break;
            }

            // вычитаем найденное лишние из остатка
            int prod_len2 = multiply_array_by_digit(denom_buf, denom_len, digit, prod);
            int prod_shift_len2 = prod_len2 + shift;
            int prod_shift2[WORK_MAX];
            for (int i = 0; i < prod_len2; i++)
                prod_shift2[i] = prod[i];
            for (int i = prod_len2; i < prod_shift_len2; i++)
                prod_shift2[i] = 0;

            num_len = subtract_arrays(num_buf, num_len, prod_shift2, prod_shift_len2);
            quotient_buf[quotient_len++] = digit; // записываем цифру частного
        }
    }

    int integer_digits = quotient_len; // количество цифр в целой части

    // вычисляем дробную часть
    while (quotient_len < FRACTION_LIMIT && !(num_len == 1 && num_buf[0] == 0))
    {
        num_buf[num_len++] = 0; // сноска нуля
        int digit;
        int prod[WORK_MAX];
        // подбор очередной цифры  
        for (digit = 9; digit >= 0; digit--)
        {
            int prod_len = multiply_array_by_digit(denom_buf, denom_len, digit, prod);
            if (compare_arrays(prod, prod_len, num_buf, num_len) <= 0)
                break;
        }
        int prod_len2 = multiply_array_by_digit(denom_buf, denom_len, digit, prod);
        num_len = subtract_arrays(num_buf, num_len, prod, prod_len2);
        quotient_buf[quotient_len++] = digit; // добавляем цифру в результат
    }

    // убираем ведущие нули в результате
    int first_nonzero = 0;
    while (first_nonzero < quotient_len && quotient_buf[first_nonzero] == 0)
        first_nonzero++;

    // частный случай если результат деления равен нулю
    if (first_nonzero == quotient_len)
    {
        result->sign = positive;
        result->integer[0] = 0;
        result->exp = 0;
        return 1;
    }

    // вычисление порядка
    int K1 = integer_digits - first_nonzero;

    // перенос значащих цифр в результат
    int res_len = 0;
    int i;
    for (i = first_nonzero; i < quotient_len && res_len < MAX_MANTISSA; i++)
        result->integer[res_len++] = quotient_buf[i];

    // округление 
    if (i < quotient_len && quotient_buf[i] >= 5)
    {
        int is_overflow = 1;
        for (int j = res_len - 1; j >= 0 && is_overflow; j--)
        {
            int tmp = result->integer[j] + is_overflow;
            result->integer[j] = tmp % 10;
            is_overflow = tmp / 10;
        }
        if (is_overflow) // переполнение
        {
            for (int j = res_len; j > 0; j--)
                result->integer[j] = result->integer[j - 1];
            result->integer[0] = 1;
            res_len++;
            K1++; // сдвигаем порядок
        }
    }

    result->exp = K1; // порядок в структуру

    // проверка на переполнение экспоненты
    if (result->exp > 99999)
        return -10;  // слишком большое число
    if (result->exp < -99999)
        return -11;  // слишком маленькое число

    //  знак
    if (numerator.sign == denominator.sign)
        result->sign = positive;
    else
        result->sign = negative;

    return res_len;
}



void normalize_and_trim_result(long_t *result, int *len)
{
    int first_nonzero = 0;
    while (first_nonzero < *len && result->integer[first_nonzero] == 0)
        first_nonzero++;

    if (first_nonzero == *len)
    {
        result->sign = positive;
        result->integer[0] = 0;
        result->exp = 0;
        *len = 1;
        return;
    }

    int new_len = 0;
    for (int i = first_nonzero; i < *len && new_len < MAX_MANTISSA; i++)
    {
        result->integer[new_len++] = result->integer[i];
    }

    result->exp += first_nonzero;
    *len = new_len;
}
