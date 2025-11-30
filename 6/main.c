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

void preorder_traversal(node* root) {
    if (root != NULL) {
        printf("Файл: %-20s | Дата: %s\n", root->filename, root->date);
        preorder_traversal(root->left);
        preorder_traversal(root->right);
    }
}

void postorder_traversal(node* root) {
    if (root != NULL) {
        postorder_traversal(root->left);
        postorder_traversal(root->right);
        printf("Файл: %-20s | Дата: %s\n", root->filename, root->date);
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
    printf("9. Сравнение методов удаления\n");
    printf("10. Выход\n");
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
                
                // Используем фиксированное имя файла для сохранения
                char filename[STR_LEN] = "tree";
                
                printf("\nСохранение информации о дереве:\n");
                // 1. Сохранение текстовой информации (.txt)
                save_tree_to_txt(root, filename);
                
                // 2. Создание визуализации Graphviz (.png)
                // 
                visualize_tree_with_graphviz(root, filename);
                break;
            }

            case 2: // Добавить файл
                add_file_menu(&root); 
                break;

            case 3: // Удалить файл (по имени)
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

            case 4: // Обход дерева (список файлов)
                if (root == NULL) { printf("Дерево пусто.\n"); } 
                else { 
                    printf("\n=== ПРЕФИКСНЫЙ ОБХОД (сверху вниз) ===\n");
                    preorder_traversal(root);
                    
                    printf("\n=== ИНФИКСНЫЙ ОБХОД (слева направо) ===\n");
                    inorder_traversal(root);
                    
                    printf("\n=== ПОСТФИКСНЫЙ ОБХОД (снизу вверх) ===\n");
                    postorder_traversal(root);
                }
                break;

            case 5: // Сравнение сценариев (Ручной тест)
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
                // ИСПРАВЛЕНО: Передаем &copy1 (указатель на указатель)
                run_scenario_1(&copy1, buffer_date, &t1); 
                
                printf("2. Сценарий 2 (Перестроение по дате + удаление)...\n");
                node *res2 = run_scenario_2(copy2, buffer_date, &t2); 
                
                printf("\nРезультаты:\n");
                printf("Время Сценария 1: %.6f сек\n", t1);
                printf("Время Сценария 2: %.6f сек\n", t2);
                
                destroy_tree(copy1); // copy1 теперь указывает на актуальный корень
                destroy_tree(res2);
                break;

            case 6: // Тест эффективности
                run_efficiency_test(); 
                break;
            
            case 7: // Очистка старых файлов (Удалить из текущего дерева)
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

            case 8: // Сравнение скошенных деревьев
                run_skewed_comparison(); 
                break;


            case 9: // Комплексный тест (N=100, K=10..100)
                run_scenario_comparison_test(); 
                break;

            
            case 10: // Выход
                destroy_tree(root);
                printf("Память очищена. Программа завершена.\n");
                return 0;
                
            default: 
                printf("Ошибка: Неверный пункт меню.\n");
        }
    }
    return 0;
}