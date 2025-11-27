#ifndef MENU_UTILS_H
#define MENU_UTILS_H

#include <stdio.h>

// Печать меню
void show_info(void);
void show_menu(void);
void show_operations(int is_list);

// Ввод и проверка выражения
int validate_expression(const char *expr);
void read_expression(char *buffer, int size);

#endif
