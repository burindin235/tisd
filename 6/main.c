#include "tree_funcs.h"

// Функция очистки буфера ввода
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Безопасный ввод строки
void get_string_input(char *buffer, int size) {
    if (fgets(buffer, size, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        } else {
            // Если ввод слишком длинный, очищаем буфер
            clear_input_buffer();
        }
    } else {
        buffer[0] = '\0';
    }
}

void print_menu() {
    printf("\n=== МЕНЮ ФАЙЛОВОЙ СИСТЕМЫ (BST) ===\n");
    printf("1. Вывести текущее дерево (Графически)\n");
    printf("2. Добавить файл\n");
    printf("3. Удалить файл (по имени)\n");
    printf("4. Обход дерева (список файлов)\n");
    printf("5. Сравнение сценариев (Тест: Удаление на месте vs Перестроение)\n");
    printf("6. Тест эффективности (Список vs Дерево)\n");
    printf("7. Очистка старых файлов (Удалить из дерева по дате)\n");
    printf("8. Сравнение правосторонних/левосторонних деревьев\n");
    printf("9. Выход\n");
    printf("Ваш выбор: ");
}

void add_file_menu(node **root) {
    int sub_choice;
    char name[STR_LEN];
    char date[STR_LEN];
    
    printf("--- Добавить файл ---\n");
    printf("1. Ввести вручную\n");
    printf("2. Сгенерировать\n");
    printf("Ваш выбор: ");

    if (scanf("%d", &sub_choice) != 1) {
        printf("Ошибка: Неверный ввод.\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    if (sub_choice == 1) {
        printf("Введите имя файла: ");
        get_string_input(name, STR_LEN);
        if (strlen(name) == 0) {
            printf("Ошибка: Пустое имя.\n");
            return;
        }

        printf("Введите дату обращения (DD.MM.YYYY): ");
        get_string_input(date, STR_LEN);
        if (!is_valid_date(date)) {
            printf("Ошибка: Неверный формат или диапазон даты. (DD.MM.YYYY, год < 2025).\n");
            return;
        }
    } else if (sub_choice == 2) {
        generate_random_file_data(name, date);
        printf("Сгенерировано: Имя: %s, Дата: %s\n", name, date);
    } else {
        printf("Ошибка: Неверный пункт.\n");
        return;
    }

    *root = insert_node(*root, name, date);
    printf("Файл успешно добавлен.\n");
}

int has_extension(const char *s) {
    return strchr(s, '.') != NULL;
}


int main() {
    node *root = NULL;
    int choice;
    char buffer_name[STR_LEN];
    char buffer_date[STR_LEN];
    
    // Инициализация генератора случайных чисел
    srand((unsigned int)time(NULL)); 

    while (1) {
        print_menu();
        
        if (scanf("%d", &choice) != 1) {
            printf("Ошибка: Введите число.\n");
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice) {
            case 1: // Вывести текущее дерево (Графически)
            {
                if (root == NULL) {
                    printf("\nТекущее дерево пусто. Сначала добавьте файлы (выбор 2).\n");
                    break;
                }
                
                char filename[STR_LEN] = "tree";
                /*
                // --- 1. Ввод и валидация имени файла ---
                while (1) {
                    printf("\nВведите БАЗОВОЕ название файла (без расширения для .txt и .png): ");
                    
                    get_string_input(filename, STR_LEN);
                    
                    if (strlen(filename) == 0) {
                        printf("Ошибка: Имя файла не может быть пустым. Повторите ввод.\n");
                        continue;
                    }

                    if (has_extension(filename)) {
                        printf("Ошибка: Пожалуйста, введите название файла БЕЗ расширения (символ '.' запрещен).\n");
                        continue; // Повторяем ввод
                    }
                    
                    break; // Имя файла корректно, выходим из цикла
                } */

                // --- 2. Сохранение текстовой информации (.txt) ---
                // Файл перезаписывается, как требовалось (режим "w" в save_tree_to_txt)
                save_tree_to_txt(root, filename);
                
                // --- 3. Создание визуализации Graphviz (.png) ---
                // Файл перезаписывается, как требовалось (режим "w" в generate_dot_file)
                visualize_tree_with_graphviz(root, filename);
            }
            break;

            case 2: add_file_menu(&root); break;

            case 3:
                if (root == NULL) { printf("Ошибка: Дерево пусто.\n"); break; }
                printf("\nВведите имя файла для удаления: ");
                get_string_input(buffer_name, STR_LEN);
                if (search_node(root, buffer_name) == NULL) {
                    printf("Ошибка: Файл '%s' не найден.\n", buffer_name);
                } else {
                    root = delete_node(root, buffer_name);
                    printf("Файл '%s' удален.\n", buffer_name);
                }
                break;

            case 4:
                if (root == NULL) { printf("Дерево пусто.\n"); } 
                else { printf("\n--- Список файлов ---\n"); inorder_traversal(root); }
                break;

            case 5:
                if (root == NULL) { printf("Ошибка: Заполните дерево данными для теста.\n"); break; }
                printf("\nВведите контрольную дату (DD.MM.YYYY, удалить файлы старее чем): ");
                get_string_input(buffer_date, STR_LEN);
                if (!is_valid_date(buffer_date)) {
                     printf("Ошибка формата даты.\n"); break;
                }

                printf("\n--- Запуск тестов ---\n");
                
                node *copy1 = copy_tree(root);
                node *copy2 = copy_tree(root); 
                double t1, t2;

                printf("1. Сценарий 1 (Удаление на месте в алфавитном дереве)...\n");
                run_scenario_1(copy1, buffer_date, &t1);
                
                printf("2. Сценарий 2 (Перестроение по дате + удаление)...\n");
                node *res2 = run_scenario_2(copy2, buffer_date, &t2); 
                
                printf("\nРезультаты:\n");
                printf("Время Сценария 1: %.6f сек\n", t1);
                printf("Время Сценария 2: %.6f сек\n", t2);
                
                destroy_tree(copy1);
                destroy_tree(res2);
                break;

            case 6: run_efficiency_test(); break;
            
            case 8: run_skewed_comparison(); break;

            case 7:
                if (root == NULL) {
                    printf("Ошибка: Дерево пусто.\n");
                    break;
                }
                printf("\n--- Удаление устаревших файлов ---\n");
                printf("ВНИМАНИЕ: Файлы будут удалены из текущего дерева навсегда!\n");
                printf("Введите дату отсечения (DD.MM.YYYY): ");
                get_string_input(buffer_date, STR_LEN);
                
                if (!is_valid_date(buffer_date)) {
                    printf("Ошибка: Некорректная дата.\n");
                    break;
                }

                remove_files_by_date_interactive(&root, buffer_date);
                break;

            case 9: 
                destroy_tree(root);
                printf("Память очищена. \n");
                break;

            case 10:
                printf("тест кейс 10");
                node *generated_tree = create_balanced_tree(10);
                node *copy3 = copy_tree(generated_tree);
                node *copy4 = copy_tree(generated_tree);

                double t3, t4;
                char buffer_date_2[20] = "01.01.2000";

                printf("1. Сценарий 1 (Удаление на месте в алфавитном дереве)...\n");
                run_scenario_1(copy3, buffer_date_2, &t3);
                
                printf("2. Сценарий 2 (Перестроение по дате + удаление)...\n");
                node *res3 = run_scenario_2(copy4, buffer_date_2, &t4); 
                
                printf("\nРезультаты:\n");
                printf("Время Сценария 1: %.6f сек\n", t3);
                printf("Время Сценария 2: %.6f сек\n", t4);
                
                destroy_tree(copy3);
                destroy_tree(res3);
                
                break;
            default: 
                printf("Ошибка: Неверный пункт меню.\n");
        }
    }
    return 0;
}