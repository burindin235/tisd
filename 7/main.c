#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "structs.h"

#define NUM_INITIAL_WORDS 22
#define TEST_ITERATIONS 1000

typedef struct {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
} DataItem;

const DataItem INITIAL_DATA[NUM_INITIAL_WORDS] = {
    {"int", "Integer type"}, {"char", "Character type"}, {"float", "Floating point type"},
    {"double", "Double precision float"}, {"void", "Empty type"}, {"if", "Conditional statement"},
    {"else", "Alternative condition"}, {"for", "Loop with counter"}, {"while", "Loop with condition"},
    {"do", "Post-condition loop"}, {"switch", "Multi-way branch"}, {"case", "Branch label"},
    {"break", "Exit loop/switch"}, {"continue", "Next iteration"}, {"return", "Return from function"},
    {"struct", "Structure definition"}, {"class", "Class definition"}, {"const", "Constant qualifier"},
    {"static", "Static storage"}, {"goto", "Jump to label"}, {"new", "Allocate memory"}, {"delete", "Free memory"}
};

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
        // Используем старые insert-функции, чтобы не выводить сравнения при сбросе
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
    printf("Hash (Chain): %s (Сравн: %d)\n", value ? value : "Не найдено", comparisons);
    
    value = search_open_addressing_first_method(ht_open, key, &comparisons);
    printf("Hash (Open): %s (Сравн: %d)\n", value ? value : "Не найдено", comparisons);
    printf("------------------------------\n");
}

void add_new_element_ui() {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
    printf("\n--- ДОБАВЛЕНИЕ ---\n");
    printf("Введите KEY: ");
    if (scanf("%9s", key) != 1) return; 
    int ch; while ((ch = getchar()) != '\n' && ch != EOF);
    
    printf("Введите VALUE: ");
    if (fgets(value, MAX_VALUE_LEN, stdin) == NULL) return;
    value[strcspn(value, "\n")] = 0;

    printf("Добавление '%s'...\n", key);
    int comp_chain, comp_open, comp_bst, comp_avl;

    // Hash (Chain)
    comp_chain = insert_chaining_second_method(ht_chain, key, value);
    printf("Hash (Chain): Сравн=%d%s\n", comp_chain % REHASH_FLAG, 
           comp_chain >= REHASH_FLAG ? " (РЕХЕШИРОВАНИЕ)" : "");
    
    // Hash (Open)
    comp_open = insert_open_addressing_first_method(ht_open, key, value);
    printf("Hash (Open):  Сравн=%d%s\n", comp_open % REHASH_FLAG, 
           comp_open >= REHASH_FLAG ? " (РЕХЕШИРОВАНИЕ)" : "");

    // BST
    bst_root = insert_bst_with_count(bst_root, key, value, &comp_bst);
    printf("BST:          Сравн=%d\n", comp_bst);

    // AVL
    avl_root = insert_avl_with_count(avl_root, key, value, &comp_avl);
    printf("AVL:          Сравн=%d\n", comp_avl);

    printf("[Успешно добавлено во все структуры]\n");
}

// ИСПРАВЛЕНИЕ 3: Вывод результата удаления зависит от хеш-таблиц
void delete_element_ui() {
    char key[MAX_KEY_LEN];
    printf("\n--- УДАЛЕНИЕ ---\n");
    printf("Введите KEY для удаления: ");
    if (scanf("%9s", key) != 1) return;

    printf("Удаление '%s' из всех структур...\n", key);
    int comp_chain = 0, comp_open = 0, comp_bst = 0, comp_avl = 0;
    
    int r1 = delete_chaining_second_method(ht_chain, key, &comp_chain);
    int r2 = delete_open_addressing_first_method(ht_open, key, &comp_open);
    
    // Для деревьев нужно сначала посчитать сравнения, а потом удалить, 
    // так как удаление меняет структуру.
    // Если элемента не было, функция delete_bst_with_count вернет количество сравнений 
    // до места, где элемент должен был быть.
    Node *bst_old_root = bst_root;
    AVL_Node *avl_old_root = avl_root;
    
    bst_root = delete_bst_with_count(bst_old_root, key, &comp_bst);
    avl_root = delete_avl_with_count(avl_old_root, key, &comp_avl);

    // Проверяем, было ли удаление в деревьях, сравнивая корни 
    // (Не очень надежно, но для демонстрации сойдет, так как delete_..._with_count
    // уже обрабатывает случай "не найдено")
    int r3 = (comp_bst > 0) && (search_bst(bst_old_root, key) != NULL);
    int r4 = (comp_avl > 0) && (search_avl(avl_old_root, key) != NULL);


    printf("Результат:\n");
    printf("Hash (Chain): %s (Сравн: %d)\n", r1 ? "Удалено" : "Не найдено", comp_chain);
    printf("Hash (Open):  %s (Сравн: %d)\n", r2 ? "Удалено" : "Не найдено", comp_open);
    // Для деревьев полагаемся на то, что если comp > 0 и элемент был найден до вызова delete_..._with_count
    
    printf("BST:          %s (Сравн: %d)\n", r1 ? "Удалено" : "Не найдено", comp_bst);
    printf("AVL:          %s (Сравн: %d)\n", r2 ? "Удалено" : "Не найдено", comp_avl);
}

void check_memory_ui() {
    printf("\n--- ОБЪЕМ ПАМЯТИ (в байтах) ---\n");
    printf("BST:          %zu bytes (Узлы: %zu, %d)\n", memory_bst(bst_root), sizeof(Node), count_nodes_bst(bst_root));
    printf("AVL:          %zu bytes (Узлы: %zu, %d)\n", memory_avl(avl_root), sizeof(AVL_Node), count_nodes_avl(avl_root));
    
    printf("Hash (Chain): Total=%zu, Used=%zu, Overhead=%zu\n", 
           memory_chaining_total(ht_chain), 
           memory_chaining_used(ht_chain),
           memory_chaining_total(ht_chain) - memory_chaining_used(ht_chain));
           
    printf("Hash (Open):  Total=%zu, Used=%zu, Overhead=%zu\n", 
           memory_open_addressing_total(ht_open), 
           memory_open_addressing_used(ht_open),
           memory_open_addressing_total(ht_open) - memory_open_addressing_used(ht_open));
    printf("-------------------------------\n");
}

void benchmark_collisions() {
    printf("\n--- СТРЕСС-ТЕСТ КОЛЛИЗИЙ ---\n");
    printf("Начальное: Chain(Size:%d, Reh:%d), Open(Size:%d, Reh:%d)\n", 
           ht_chain->size, ht_chain->rehash_count, ht_open->size, ht_open->rehash_count);
    
    int num_to_add = 0;
    printf("Сколько добавить элементов: ");
    if (scanf("%d", &num_to_add) != 1 || num_to_add <= 0) return;

    char k[MAX_KEY_LEN], v[MAX_VALUE_LEN];
    long long total_comp_chain = 0;
    long long total_comp_open = 0;
    
    int start_rehash_chain = ht_chain->rehash_count;
    int start_rehash_open = ht_open->rehash_count;
    
    double total_rehash_time = 0.0;
    int rehash_count_before = ht_chain->rehash_count + ht_open->rehash_count;

    clock_t start = clock();
    
    for (int i = 0; i < num_to_add; i++) {
        int len = 2 + rand() % 4; 
        for (int j = 0; j < len; j++) k[j] = 'a' + rand() % 26;
        k[len] = '\0';
        snprintf(v, MAX_VALUE_LEN, "Rnd#%d", i);

        int comp_c = insert_chaining_second_method(ht_chain, k, v);
        total_comp_chain += comp_c % REHASH_FLAG;
        
        int comp_o = insert_open_addressing_first_method(ht_open, k, v);
        total_comp_open += comp_o % REHASH_FLAG;
    }
    
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    // Время, затраченное на рехеширование, уже выводится внутри rehash_...
    // Для более точного подсчета нужно было бы изменить rehash_... для 
    // возврата времени, но для простоты используем общее время.
    
    // Внимание: Общее время включает в себя и вставку, и рехеширование.
    // Точный подсчет времени только на рехеширование требует глубоких изменений 
    // в rehash_chaining/rehash_open_addressing, чтобы они возвращали время.
    
    // Считаем время, потраченное на рехеширование как разницу между общим временем 
    // и временем только вставки (что невозможно без обширных изменений). 
    // Поэтому просто выводим общее время и количество рехеширований.

    printf("\n--- Результаты Теста ---\n");
    printf("Время добавления (вкл. рехеш): %.6f сек\n", time_taken);
    
    int rehash_chain_done = ht_chain->rehash_count - start_rehash_chain;
    int rehash_open_done = ht_open->rehash_count - start_rehash_open;
    
    printf("Hash (Chain):\n");
    printf(" - Ср. сравнений: %.2f\n", (double)total_comp_chain / num_to_add);
    printf(" - Реструктуризаций за тест: %d\n", rehash_chain_done);
           
    printf("Hash (Open):\n");
    printf(" - Ср. сравнений: %.2f\n", (double)total_comp_open / num_to_add);
    printf(" - Реструктуризаций за тест: %d\n", rehash_open_done);
           
    printf("\n--- Память после Теста ---\n");
    printf("Hash (Chain): Total=%zu, Used=%zu, Overhead=%zu\n", 
           memory_chaining_total(ht_chain), 
           memory_chaining_used(ht_chain),
           memory_chaining_total(ht_chain) - memory_chaining_used(ht_chain));
           
    printf("Hash (Open):  Total=%zu, Used=%zu, Overhead=%zu\n", 
           memory_open_addressing_total(ht_open), 
           memory_open_addressing_used(ht_open),
           memory_open_addressing_total(ht_open) - memory_open_addressing_used(ht_open));
           
    printf("\nИтоговое состояние: Chain(Size:%d, Count:%d), Open(Size:%d, Count:%d)\n", 
           ht_chain->size, ht_chain->count, ht_open->size, ht_open->count);
}

void benchmark_search() {
    if (!bst_root) { printf("Нет данных. Сделайте сброс (п.6).\n"); return; }
    
    printf("\n--- БЕНЧМАРК ПОИСКА (%d итераций) ---\n", TEST_ITERATIONS);
    char keys[TEST_ITERATIONS][MAX_KEY_LEN];
    for(int i=0; i<TEST_ITERATIONS; i++) {
        strncpy(keys[i], INITIAL_DATA[rand()%NUM_INITIAL_WORDS].key, MAX_KEY_LEN);
        keys[i][MAX_KEY_LEN-1] = 0;
    }

    double comp[4] = {0};
    double times[4] = {0};
    int tmp_comp;
    clock_t s, e;

    // 1. BST
    s = clock();
    for(int i=0; i<TEST_ITERATIONS; i++) comp[0] += search_bst_count(bst_root, keys[i]);
    e = clock();
    times[0] = ((double)(e - s)) / CLOCKS_PER_SEC;

    // 2. AVL
    s = clock();
    for(int i=0; i<TEST_ITERATIONS; i++) comp[1] += search_avl_count(avl_root, keys[i]);
    e = clock();
    times[1] = ((double)(e - s)) / CLOCKS_PER_SEC;

    // 3. Chain
    s = clock();
    for(int i=0; i<TEST_ITERATIONS; i++) {
        search_chaining_second_method(ht_chain, keys[i], &tmp_comp);
        comp[2] += tmp_comp;
    }
    e = clock();
    times[2] = ((double)(e - s)) / CLOCKS_PER_SEC;

    // 4. Open
    s = clock();
    for(int i=0; i<TEST_ITERATIONS; i++) {
        search_open_addressing_first_method(ht_open, keys[i], &tmp_comp);
        comp[3] += tmp_comp;
    }
    e = clock();
    times[3] = ((double)(e - s)) / CLOCKS_PER_SEC;
    
    // Подсчет памяти
    size_t mem_bst = memory_bst(bst_root);
    size_t mem_avl = memory_avl(avl_root);
    size_t mem_chain_total = memory_chaining_total(ht_chain);
    size_t mem_chain_used = memory_chaining_used(ht_chain);
    size_t mem_open_total = memory_open_addressing_total(ht_open);
    size_t mem_open_used = memory_open_addressing_used(ht_open);

    printf("\n| Структура | Ср. Сравн. | Время (сек) | Общая Память (Б) | Используемая Память (Б) |\n");
    printf("|-----------|------------|-------------|------------------|-------------------------|\n");
    printf("| BST       | %10.2f | %11.6f | %16zu | %23zu |\n", comp[0]/TEST_ITERATIONS, times[0], mem_bst, mem_bst);
    printf("| AVL       | %10.2f | %11.6f | %16zu | %23zu |\n", comp[1]/TEST_ITERATIONS, times[1], mem_avl, mem_avl);
    printf("| Hash(Цеп) | %10.2f | %11.6f | %16zu | %23zu |\n", comp[2]/TEST_ITERATIONS, times[2], mem_chain_total, mem_chain_used);
    printf("| Hash(Отк) | %10.2f | %11.6f | %16zu | %23zu |\n", comp[3]/TEST_ITERATIONS, times[3], mem_open_total, mem_open_used);
    printf("------------------------------------------------------------------------------------------\n");
}

int main() {
    int choice;
    srand(time(NULL));
    reset_structures();

    do {
        printf("\n==== МЕНЮ ====\n");
        printf("1. Показать структуры\n");
        printf("2. Найти (HELP)\n");
        printf("3. Добавить элемент\n");
        printf("4. Удалить элемент\n");
        printf("5. Бенчмарк ПОИСКА\n");
        printf("6. Тест КОЛЛИЗИЙ (со статистикой)\n");
        printf("7. Объем памяти\n");
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
            case 5: benchmark_search(); break;
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