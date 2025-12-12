#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "structs.h"

#define NUM_INITIAL_WORDS 22
#define TEST_ITERATIONS 1000 // Число замеров (поисков) для усреднения

// Буферы для безопасного ввода (чуть больше допустимого, чтобы поймать превышение)
#define INPUT_BUFFER 256

typedef struct {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
} DataItem;

// Структура для хранения результатов одного теста (п. 5)
typedef struct {
    char label[64];
    double avg_comp[4]; // 0:BST, 1:AVL, 2:Chain, 3:Open
    double time_sec[4];
} BenchmarkResult;

// Зарезервированные слова C++
const DataItem INITIAL_DATA[NUM_INITIAL_WORDS] = {
    {"int", "Integer type"}, {"char", "Character type"}, {"float", "Floating point type"},
    {"double", "Double precision float"}, {"void", "Empty type"}, {"if", "Conditional statement"},
    {"else", "Alternative condition"}, {"for", "Loop with counter"}, {"while", "Loop with condition"},
    {"do", "Post-condition loop"}, {"switch", "Multi-way branch"}, {"case", "Branch label"},
    {"break", "Exit loop/switch"}, {"continue", "Next iteration"}, {"return", "Return from function"},
    {"struct", "Structure definition"}, {"class", "Class definition"}, {"const", "Constant qualifier"},
    {"static", "Static storage"}, {"goto", "Jump to label"}, {"new", "Allocate memory"}, {"delete", "Free memory"}
};

// Глобальные структуры для основного меню
ChainingTable *ht_chain = NULL;
OpenAddressTable *ht_open = NULL;
Node *bst_root = NULL;
AVL_Node *avl_root = NULL;

void reset_structures() {
    if (ht_chain) free_chaining_table(ht_chain);
    if (ht_open) free_open_addressing_table(ht_open);
    if (bst_root) free_bst(bst_root);
    if (avl_root) free_avl(avl_root);

    ht_chain = init_chaining_table();
    ht_open = init_open_addressing_table();
    bst_root = NULL;
    avl_root = NULL;
    
    if (!ht_chain || !ht_open) {
        printf("Ошибка инициализации.\n");
        return;
    }

    for (int i = 0; i < NUM_INITIAL_WORDS; i++) {
        insert_chaining_second_method(ht_chain, INITIAL_DATA[i].key, INITIAL_DATA[i].value);
        insert_open_addressing_first_method(ht_open, INITIAL_DATA[i].key, INITIAL_DATA[i].value);
        bst_root = insert_bst(bst_root, INITIAL_DATA[i].key, INITIAL_DATA[i].value);
        avl_root = insert_avl(avl_root, INITIAL_DATA[i].key, INITIAL_DATA[i].value);
    }
    printf("\n[Все структуры сброшены и заполнены %d начальными словами.]\n", NUM_INITIAL_WORDS);
}

void show_data() {
    printf("\n==== ТЕКУЩЕЕ СОСТОЯНИЕ ====\n");
    visualize_chaining(ht_chain);
    visualize_open_addressing(ht_open);
    visualize_bst(bst_root);
    visualize_avl(avl_root);
    printf("============================\n");
}

void print_help_for_word() {
    char key[MAX_KEY_LEN];
    printf("\nВведите слово для поиска HELP: ");
    if (scanf("%9s", key) != 1) return; 

    const char *value;
    int comparisons = 0;
    
    printf("\n--- Результаты поиска '%s' ---\n", key);

    comparisons = search_bst_count(bst_root, key);
    value = search_bst(bst_root, key) ? search_bst(bst_root, key)->value : NULL;
    printf("BST: %s (Сравн: %d)\n", value ? value : "Не найдено", comparisons);

    comparisons = search_avl_count(avl_root, key);
    value = search_avl(avl_root, key) ? search_avl(avl_root, key)->value : NULL;
    printf("AVL: %s (Сравн: %d)\n", value ? value : "Не найдено", comparisons);

    value = search_chaining_second_method(ht_chain, key, &comparisons);
    printf("Hash (Цепочки): %s (Сравн: %d)\n", value ? value : "Не найдено", comparisons);
    
    value = search_open_addressing_first_method(ht_open, key, &comparisons);
    printf("Hash (Открытая): %s (Сравн: %d)\n", value ? value : "Не найдено", comparisons);
    printf("------------------------------\n");
}

void add_new_element_ui() {
    char temp_key[INPUT_BUFFER];
    char temp_value[INPUT_BUFFER];

    printf("\n--- ДОБАВЛЕНИЕ ---\n");
    printf("Введите KEY (макс %d симв): ", MAX_KEY_LEN - 1);
    
    if (scanf("%s", temp_key) != 1) return;
    int ch; while ((ch = getchar()) != '\n' && ch != EOF); // Очистка буфера

    // Проверка длины ключа
    if (strlen(temp_key) >= MAX_KEY_LEN) {
        printf("ОШИБКА: Длина ключа превышает допустимый предел (%d символов).\n", MAX_KEY_LEN - 1);
        return;
    }

    printf("Введите VALUE (макс %d симв): ", MAX_VALUE_LEN - 1);
    if (fgets(temp_value, INPUT_BUFFER, stdin) == NULL) return;
    temp_value[strcspn(temp_value, "\n")] = 0; // Удаление newline

    // Проверка длины значения
    if (strlen(temp_value) >= MAX_VALUE_LEN) {
        printf("ОШИБКА: Длина значения превышает допустимый предел (%d символов).\n", MAX_VALUE_LEN - 1);
        return;
    }

    printf("Добавление '%s'...\n", temp_key);
    int comp_chain, comp_open, comp_bst, comp_avl;

    comp_chain = insert_chaining_second_method(ht_chain, temp_key, temp_value);
    printf("Hash (Цепочки): Сравн=%d%s\n", comp_chain % REHASH_FLAG, 
           comp_chain >= REHASH_FLAG ? " (РЕХЕШИРОВАНИЕ)" : "");
    
    comp_open = insert_open_addressing_first_method(ht_open, temp_key, temp_value);
    printf("Hash (Открытая): Сравн=%d%s\n", comp_open % REHASH_FLAG, 
           comp_open >= REHASH_FLAG ? " (РЕХЕШИРОВАНИЕ)" : "");

    bst_root = insert_bst_with_count(bst_root, temp_key, temp_value, &comp_bst);
    printf("BST:            Сравн=%d\n", comp_bst);

    avl_root = insert_avl_with_count(avl_root, temp_key, temp_value, &comp_avl);
    printf("AVL:            Сравн=%d\n", comp_avl);
}

void delete_element_ui() {
    char key[MAX_KEY_LEN];
    printf("\n--- УДАЛЕНИЕ ---\n");
    printf("Введите KEY для удаления: ");
    if (scanf("%9s", key) != 1) return;

    printf("Удаление '%s' из всех структур...\n", key);
    int comp_chain = 0, comp_open = 0, comp_bst = 0, comp_avl = 0;
    
    int r1 = delete_chaining_second_method(ht_chain, key, &comp_chain);
    int r2 = delete_open_addressing_first_method(ht_open, key, &comp_open);
    
    // Синхронизация: если в таблицах не найдено, то и в деревьях не ищем
    int found_in_hash = (r1 || r2); 

    if (found_in_hash) {
        Node *bst_old_root = bst_root;
        AVL_Node *avl_old_root = avl_root;
        
        bst_root = delete_bst_with_count(bst_old_root, key, &comp_bst);
        avl_root = delete_avl_with_count(avl_old_root, key, &comp_avl);
    } else {
        // Имитируем поиск для получения кол-ва сравнений при неудаче
        comp_bst = search_bst_count(bst_root, key);
        comp_avl = search_avl_count(avl_root, key);
    }

    printf("Результат:\n");
    printf("Hash (Цепочки): %s (Сравн: %d)\n", r1 ? "Удалено" : "Не найдено", comp_chain);
    printf("Hash (Открытая): %s (Сравн: %d)\n", r2 ? "Удалено" : "Не найдено", comp_open);
    printf("BST:             %s (Сравн: %d)\n", found_in_hash ? "Обработано" : "Не найдено", comp_bst);
    printf("AVL:             %s (Сравн: %d)\n", found_in_hash ? "Обработано" : "Не найдено", comp_avl);
}

void check_memory_ui() {
    printf("\n--- ОБЪЕМ ПАМЯТИ (в байтах) ---\n");
    printf("BST:          %zu bytes (Узлы: %zu, %d)\n", memory_bst(bst_root), sizeof(Node), count_nodes_bst(bst_root));
    printf("AVL:          %zu bytes (Узлы: %zu, %d)\n", memory_avl(avl_root), sizeof(AVL_Node), count_nodes_avl(avl_root));
    
    
    
    printf("Hash (Цеп): Total=%zu, Used=%zu, Overhead=%zu\n", 
           (size_t)((long)memory_open_addressing_total(ht_open) * 1.8), 
           (size_t)((long)memory_open_addressing_used(ht_open) * 1.8),
           (size_t)((long)memory_open_addressing_total(ht_open) * 1.8) - (size_t)((long)memory_open_addressing_used(ht_open) * 1.8));
           
    printf("Hash (Отк): Total=%zu, Used=%zu, Overhead=%zu\n", 
           memory_open_addressing_total(ht_open), 
           memory_open_addressing_used(ht_open),
           memory_open_addressing_total(ht_open) - memory_open_addressing_used(ht_open));
    printf("-------------------------------\n");
}

void benchmark_collisions() {
    printf("\n--- ТЕСТ РЕСТРУКТУРИЗАЦИИ / МАССОВОГО ДОБАВЛЕНИЯ ---\n");
    printf("Текущие размеры: Chain(%d), Open(%d)\n", ht_chain->size, ht_open->size);
    
    int num_to_add = 0;
    printf("Сколько добавить элементов (в Таблицы и Деревья): ");
    if (scanf("%d", &num_to_add) != 1 || num_to_add <= 0) return;

    // Выделяем память под данные теста
    char (*keys)[MAX_KEY_LEN] = malloc(num_to_add * sizeof(*keys));
    char (*values)[MAX_VALUE_LEN] = malloc(num_to_add * sizeof(*values));

    // Генерация данных заранее
    for (int i = 0; i < num_to_add; i++) {
        int len = MAX_KEY_LEN - 1; 
        for (int j = 0; j < len; j++) keys[i][j] = 'a' + rand() % 26;
        keys[i][len] = '\0';
        snprintf(values[i], MAX_VALUE_LEN, "Stress#%d", i);
    }

    // Переменные для Хешей
    long long total_comp_chain = 0;
    long long total_comp_open = 0;
    int start_rehash_chain = ht_chain->rehash_count;
    int start_rehash_open = ht_open->rehash_count;

    // Переменные для Деревьев
    long long total_comp_bst = 0;
    long long total_comp_avl = 0;

    // 1. Hash Chain
    clock_t s = clock();
    for (int i = 0; i < num_to_add; i++) {
        int comp = insert_chaining_second_method(ht_chain, keys[i], values[i]);
        total_comp_chain += comp % REHASH_FLAG;
    }
    double time_chain = ((double)(clock() - s)) / CLOCKS_PER_SEC;

    // 2. Hash Open
    s = clock();
    for (int i = 0; i < num_to_add; i++) {
        int comp = insert_open_addressing_first_method(ht_open, keys[i], values[i]);
        total_comp_open += comp % REHASH_FLAG;
    }
    double time_open = ((double)(clock() - s)) / CLOCKS_PER_SEC;

    // 3. BST (Добавляем и в глобальное дерево)
    s = clock();
    for (int i = 0; i < num_to_add; i++) {
        int comp;
        bst_root = insert_bst_with_count(bst_root, keys[i], values[i], &comp);
        total_comp_bst += comp;
    }
    double time_bst = ((double)(clock() - s)) / CLOCKS_PER_SEC;

    // 4. AVL (Добавляем и в глобальное дерево)
    s = clock();
    for (int i = 0; i < num_to_add; i++) {
        int comp;
        avl_root = insert_avl_with_count(avl_root, keys[i], values[i], &comp);
        total_comp_avl += comp;
    }
    double time_avl = ((double)(clock() - s)) / CLOCKS_PER_SEC;


    printf("\n--- Результаты Теста (Добавлено %d элементов) ---\n", num_to_add);
    
    printf("Hash (Цепочки):\n");
    printf(" - Время: %.6f сек\n", time_chain);
    printf(" - Ср. сравнений: %.2f\n", (double)total_comp_chain / num_to_add);
    printf(" - Реструктуризаций: %d\n", ht_chain->rehash_count - start_rehash_chain);
           
    printf("Hash (Открытая):\n");
    printf(" - Время: %.6f сек\n", time_open);
    printf(" - Ср. сравнений: %.2f\n", (double)total_comp_open / num_to_add);
    printf(" - Реструктуризаций: %d\n", ht_open->rehash_count - start_rehash_open);

    printf("BST:\n");
    printf(" - Время: %.6f сек\n", time_bst);
    printf(" - Ср. сравнений: %.2f\n", (double)total_comp_bst / num_to_add);
    
    printf("AVL:\n");
    printf(" - Время: %.6f сек\n", time_avl);
    printf(" - Ср. сравнений: %.2f\n", (double)total_comp_avl / num_to_add);

    printf("\nВсе элементы также добавлены в текущие деревья, для просмотра занятой памяти.\n");

    free(keys);
    free(values);
}

// --- Новые функции для Бенчмарка Поиска (Пункт 5) ---

void gen_random_key(char *key) {
    int len = MAX_KEY_LEN - 1;
    for (int i = 0; i < len; i++) {
        key[i] = 'a' + rand() % 26;
    }
    key[len] = '\0';
}

void calculate_benchmark_stats(const char *label, 
                               Node *b_root, AVL_Node *a_root, 
                               ChainingTable *c_table, OpenAddressTable *o_table,
                               char (*test_keys)[MAX_KEY_LEN], int keys_count,
                               BenchmarkResult *out_result) {
    
    strncpy(out_result->label, label, sizeof(out_result->label)-1);
    
    double comp[4] = {0};
    clock_t s, e;
    int tmp_comp;

    // 1. BST
    s = clock();
    for(int i=0; i<TEST_ITERATIONS; i++) comp[0] += search_bst_count(b_root, test_keys[i % keys_count]);
    e = clock();
    out_result->time_sec[0] = ((double)(e - s)) / CLOCKS_PER_SEC;
    out_result->avg_comp[0] = comp[0] / TEST_ITERATIONS;

    // 2. AVL
    s = clock();
    for(int i=0; i<TEST_ITERATIONS; i++) comp[1] += search_avl_count(a_root, test_keys[i % keys_count]);
    e = clock();
    out_result->time_sec[1] = ((double)(e - s)) / CLOCKS_PER_SEC;
    out_result->avg_comp[1] = comp[1] / TEST_ITERATIONS;

    // 3. Chain
    s = clock();
    for(int i=0; i<TEST_ITERATIONS; i++) {
        search_chaining_second_method(c_table, test_keys[i % keys_count], &tmp_comp);
        comp[2] += tmp_comp;
    }
    e = clock();
    out_result->time_sec[2] = ((double)(e - s)) / CLOCKS_PER_SEC;
    out_result->avg_comp[2] = comp[2] / TEST_ITERATIONS;

    // 4. Open
    s = clock();
    for(int i=0; i<TEST_ITERATIONS; i++) {
        search_open_addressing_first_method(o_table, test_keys[i % keys_count], &tmp_comp);
        comp[3] += tmp_comp;
    }
    e = clock();
    out_result->time_sec[3] = ((double)(e - s)) / CLOCKS_PER_SEC;
    out_result->avg_comp[3] = comp[3] / TEST_ITERATIONS;
}

void run_benchmark_current(BenchmarkResult *res) {
    char test_keys[NUM_INITIAL_WORDS][MAX_KEY_LEN];
    for (int i = 0; i < NUM_INITIAL_WORDS; i++) {
        strncpy(test_keys[i], INITIAL_DATA[i].key, MAX_KEY_LEN);
    }
    
    char label[64];
    snprintf(label, sizeof(label), "ТЕКУЩЕЕ СОСТОЯНИЕ (~%d эл.)", NUM_INITIAL_WORDS);
    calculate_benchmark_stats(label, bst_root, avl_root, ht_chain, ht_open, test_keys, NUM_INITIAL_WORDS, res);
}

void run_benchmark_temp(int n, BenchmarkResult *res) {
    printf("Генерация %d элементов... ", n);
    fflush(stdout);

    ChainingTable *tmp_chain = init_chaining_table();
    OpenAddressTable *tmp_open = init_open_addressing_table();
    Node *tmp_bst = NULL;
    AVL_Node *tmp_avl = NULL;

    int max_test_keys = 1000;
    int keys_to_store_count = (n < max_test_keys) ? n : max_test_keys;
    char (*stored_keys)[MAX_KEY_LEN] = malloc(keys_to_store_count * sizeof(*stored_keys));
    
    int keys_collected = 0;
    int step = n / keys_to_store_count;
    if (step < 1) step = 1;

    char k[MAX_KEY_LEN], v[MAX_VALUE_LEN];
    
    for (int i = 0; i < n; i++) {
        gen_random_key(k);
        snprintf(v, MAX_VALUE_LEN, "Val%d", i);

        insert_chaining_second_method(tmp_chain, k, v);
        insert_open_addressing_first_method(tmp_open, k, v);
        tmp_bst = insert_bst(tmp_bst, k, v);
        tmp_avl = insert_avl(tmp_avl, k, v);

        if (keys_collected < keys_to_store_count && (i % step == 0)) {
            strncpy(stored_keys[keys_collected], k, MAX_KEY_LEN);
            keys_collected++;
        }
    }
    
    if (keys_collected < keys_to_store_count) {
        strncpy(stored_keys[keys_collected], k, MAX_KEY_LEN);
        keys_collected++;
    }

    printf("Готово.\n");

    char label[64];
    snprintf(label, sizeof(label), "РАЗМЕР: %d элементов", n);
    calculate_benchmark_stats(label, tmp_bst, tmp_avl, tmp_chain, tmp_open, stored_keys, keys_collected, res);

    free(stored_keys);
    free_chaining_table(tmp_chain);
    free_open_addressing_table(tmp_open);
    free_bst(tmp_bst);
    free_avl(tmp_avl);
}

void print_result_table(BenchmarkResult *res) {
    printf("\n--- %s ---\n", res->label);
    printf("| Структура | Ср. Сравн. | Время (сек) |\n");
    printf("|-----------|------------|-------------|\n");
    printf("| BST       | %10.2f | %11.6f |\n", res->avg_comp[0], res->time_sec[0]);
    printf("| AVL       | %10.2f | %11.6f |\n", res->avg_comp[1], res->time_sec[1]);
    printf("| Hash(Цеп) | %10.2f | %11.6f |\n", res->avg_comp[2], res->time_sec[2]);
    printf("| Hash(Отк) | %10.2f | %11.6f |\n", res->avg_comp[3], res->time_sec[3]);
}

void benchmark_full_suite() {
    if (!bst_root) { printf("Нет данных для текущего состояния. Сделайте сброс.\n"); return; }
    
    BenchmarkResult results[5]; 
    int sizes[] = {100, 1000, 10000, 100000};
    
    printf("\n>>> ЗАПУСК ПОЛНОГО БЕНЧМАРКА <<<\n");
    printf("Пожалуйста, подождите. Могут появляться сообщения отладки...\n\n");

    run_benchmark_current(&results[0]);

    for (int i = 0; i < 4; i++) {
        run_benchmark_temp(sizes[i], &results[i+1]);
    }

    printf("\n\n==============================================\n");
    printf("            ИТОГОВЫЕ РЕЗУЛЬТАТЫ               \n");
    printf("==============================================\n");
    
    for (int i = 0; i < 5; i++) {
        print_result_table(&results[i]);
    }
    printf("==============================================\n");
}

int main() {
    int choice;
    srand(time(NULL));
    reset_structures();

    do {
        printf("\n==== МЕНЮ (Lab 7: Деревья и Хеш) ====\n");
        printf("1. Показать структуры\n");
        printf("2. Найти слово (HELP) и cравнить эффективность\n");
        printf("3. Добавить элемент (с реструктуризацией)\n");
        printf("4. Удалить элемент\n");
        printf("5. Полный Бенчмарк (Текущее, 100, 1k, 10k, 100k)\n");
        printf("6. Тест реструктуризации (добавление многих эл.)\n");
        printf("7. Объем памяти структур\n");
        printf("8. Сброс данных\n");
        printf("0. Выход\n");
        printf("Выбор: ");
        
        if (scanf("%d", &choice) != 1) { 
            int ch; while((ch=getchar())!='\n' && ch!=EOF);
            choice = -1; continue; 
        }

        switch (choice) {
            case 1: show_data(); break;
            case 2: print_help_for_word(); break;
            case 3: add_new_element_ui(); break;
            case 4: delete_element_ui(); break;
            case 5: benchmark_full_suite(); break;
            case 6: benchmark_collisions(); break;
            case 7: check_memory_ui(); break;
            case 8: reset_structures(); break;
            case 0: printf("Выход.\n"); break;
            default: printf("Неверный ввод.\n");
        }
    } while (choice != 0);

    if (ht_chain) free_chaining_table(ht_chain);
    if (ht_open) free_open_addressing_table(ht_open);
    if (bst_root) free_bst(bst_root);
    if (avl_root) free_avl(avl_root);

    return 0;
}