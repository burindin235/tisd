#include "tree_funcs.h"

static void collect_for_removal_recursive(node *curr, const char *cutoff_date, char (*names_to_delete)[STR_LEN], int *count);
static void store_inorder_ptr(node* root, node_array_t *array);
static node* build_balanced_bst(node_array_t *array, int start, int end);
static node* create_right_skewed_tree(int count);

int date_to_int(const char *date) {
    int d, m, y;
    if (sscanf(date, "%d.%d.%d", &d, &m, &y) != 3) {
        return 0; 
    }
    return y * 10000 + m * 100 + d;
}

int compare_dates_chrono(const char *date1, const char *date2) {
    int val1 = date_to_int(date1);
    int val2 = date_to_int(date2);
    if (val1 < val2) return -1;
    if (val1 > val2) return 1;
    return 0;
}

static node* find_min(node* node_ptr) {
    struct node* current = node_ptr;
    while (current && current->left != NULL) {
        current = current->left;
    }
    return current;
}

node* create_node(const char *filename, const char *date) {
    node *new_node = (node*)malloc(sizeof(node));
    if (new_node == NULL) {
        perror("Ошибка: Не удалось выделить память");
        exit(EXIT_FAILURE);
    }
    
    strncpy(new_node->filename, filename, STR_LEN - 1);
    new_node->filename[STR_LEN - 1] = '\0'; 

    strncpy(new_node->date, date, STR_LEN - 1);
    new_node->date[STR_LEN - 1] = '\0';
    
    new_node->left = NULL;
    new_node->right = NULL;
    return new_node;
}

node* copy_tree(node *root) {
    if (root == NULL) return NULL;
    node *new_node = create_node(root->filename, root->date);
    new_node->left = copy_tree(root->left);
    new_node->right = copy_tree(root->right);
    return new_node;
}

node* insert_node(node *root, const char *filename, const char *date) {
    if (root == NULL) {
        return create_node(filename, date);
    }
    
    int cmp_result = strcmp(filename, root->filename); 

    if (cmp_result == 0) {
        printf("Инфо: Файл с именем '%s' уже существует. Обновление даты.\n", filename);
        strncpy(root->date, date, STR_LEN - 1);
        return root; 
    }

    if (cmp_result < 0) {
        root->left = insert_node(root->left, filename, date);
    } 
    else {
        root->right = insert_node(root->right, filename, date);
    }

    return root;
}

node* search_node(node* root, const char *filename) {
    if (root == NULL) return NULL;
    
    int cmp_result = strcmp(filename, root->filename);

    if (cmp_result == 0) return root;
    
    if (cmp_result < 0) return search_node(root->left, filename);
    
    return search_node(root->right, filename);
}

node* delete_node(node* root, const char *filename) {
    if (root == NULL) return NULL;

    int cmp_result = strcmp(filename, root->filename);

    if (cmp_result < 0) {
        root->left = delete_node(root->left, filename);
    } else if (cmp_result > 0) {
        root->right = delete_node(root->right, filename);
    }
    else {
        if (root->left == NULL) {
            node* temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            node* temp = root->left;
            free(root);
            return temp;
        }

        node* temp = find_min(root->right);
        strncpy(root->filename, temp->filename, STR_LEN);
        strncpy(root->date, temp->date, STR_LEN);
        root->right = delete_node(root->right, temp->filename);
    }
    return root;
}

void destroy_tree(node* root) {
    if (root != NULL) {
        destroy_tree(root->left);
        destroy_tree(root->right);
        free(root);
    }
}

int is_valid_date(const char *date) {
    int d, m, y;
    if (sscanf(date, "%d.%d.%d", &d, &m, &y) != 3) return 0;
    if (y < 1900 || y >= 2025) return 0; 
    if (m < 1 || m > 12) return 0;
    if (d < 1 || d > 31) return 0; 
    
    if (d == 31 && (m == 4 || m == 6 || m == 9 || m == 11)) return 0;
    if (m == 2) {
        int is_leap = (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
        if ((is_leap && d > 29) || (!is_leap && d > 28)) return 0;
    }
    return 1;
}

void generate_random_file_data(char *filename, char *date) {
    int len = 8 + (rand() % 5);
    for (int i = 0; i < len; i++) {
        int char_type = 1;
        if (char_type == 0) filename[i] = 'A' + (rand() % 26);
        else if (char_type == 1) filename[i] = 'a' + (rand() % 26);
        else filename[i] = '0' + (rand() % 10);
    }
    filename[len] = '\0';

    int y = 2000 + (rand() % 25); 
    int m = 1 + (rand() % 12);
    int d = 1 + (rand() % 28);
    
    sprintf(date, "%02d.%02d.%d", d, m, y);
}

void inorder_traversal(node* root) {
    if (root != NULL) {
        inorder_traversal(root->left);
        printf("Файл: %-20s | Дата: %s\n", root->filename, root->date);
        inorder_traversal(root->right);
    }
}

static void print_tree_structure(node *root, const char *prefix, int is_left) {
    if (root == NULL) return;

    printf("%s%s%s (Дата: %s)\n", 
           prefix, 
           (is_left ? "├── " : "└── "), 
           root->filename, 
           root->date);

    char new_prefix[256];
    strncpy(new_prefix, prefix, 200); 
    strcat(new_prefix, (is_left ? "│   " : "    "));
    
    print_tree_structure(root->right, new_prefix, 0); 
    print_tree_structure(root->left, new_prefix, 1);
}

void visualize_tree(node *root) {
    if (root == NULL) {
        printf("[Дерево пусто]\n");
        return;
    }
    printf("\n--- ГРАФИЧЕСКОЕ ПРЕДСТАВЛЕНИЕ ---\n");
    printf("%s (Дата: %s)\n", root->filename, root->date); 
    print_tree_structure(root->right, "", 0); 
    print_tree_structure(root->left, "", 1); 
    printf("---------------------------------\n");
}

static void collect_nodes_to_delete(node *root, const char *cutoff, char list[][STR_LEN], int *count) {
    if (root == NULL) return;
    collect_nodes_to_delete(root->left, cutoff, list, count);
    
    if (*count < MAX_NODES && compare_dates_chrono(root->date, cutoff) < 0) { 
        strncpy(list[*count], root->filename, STR_LEN - 1);
        list[*count][STR_LEN - 1] = '\0';
        (*count)++;
    }
    
    collect_nodes_to_delete(root->right, cutoff, list, count);
}

void run_scenario_1(node **root, const char *cutoff_date, double *time_taken) {
    clock_t start = clock();
    static char names_to_delete[MAX_NODES][STR_LEN];
    int count = 0;

    collect_nodes_to_delete(*root, cutoff_date, names_to_delete, &count);

    for (int i = 0; i < count; i++) {
        *root = delete_node(*root, names_to_delete[i]);
    }

    clock_t end = clock();
    *time_taken = (double)(end - start) / CLOCKS_PER_SEC;
}

int compare_dates(const void *a, const void *b) {
    node *node_a = *(node**)a;
    node *node_b = *(node**)b;
    return compare_dates_chrono(node_a->date, node_b->date); 
}

static void store_inorder_ptr(node* root, node_array_t *array) {
    if (root == NULL || array->count >= MAX_NODES) return;
    store_inorder_ptr(root->left, array);
    
    node *temp_right = root->right;
    root->left = NULL;
    root->right = NULL;
    array->arr[array->count++] = root;
    
    store_inorder_ptr(temp_right, array);
}

static node* build_balanced_bst(node_array_t *array, int start, int end) {
    if (start > end) return NULL;
    int mid = start + (end - start) / 2;
    node *root = array->arr[mid];
    root->left = build_balanced_bst(array, start, mid - 1);
    root->right = build_balanced_bst(array, mid + 1, end);
    return root;
}

int compare_filenames_for_qsort(const void *a, const void *b) {
    node *node_a = *(node**)a;
    node *node_b = *(node**)b;
    return strcmp(node_a->filename, node_b->filename);
}

node* run_scenario_2(node *root_old, const char *cutoff_date, double *time_taken) 
{
    clock_t start = clock();
    node_array_t array = { .count = 0 };
    
    store_inorder_ptr(root_old, &array);

    qsort(array.arr, array.count, sizeof(node*), compare_filenames_for_qsort); 
    
    node *new_root = NULL;
    if (array.count > 0) {
        new_root = build_balanced_bst(&array, 0, array.count - 1);
    }
    
    char names_to_delete[MAX_NODES][STR_LEN];
    int count = 0;
    collect_for_removal_recursive(new_root, cutoff_date, names_to_delete, &count); 
    
    for (int i = 0; i < count; i++) {
        new_root = delete_node(new_root, names_to_delete[i]); 
    }
    
    clock_t end = clock();
    *time_taken = (double)(end - start) / CLOCKS_PER_SEC;
    
    return new_root; 
}

static double measure_search_time(node *root, int iterations, int node_count, int degenerate_type) {
    if (root == NULL || iterations == 0) return 0.0;
    
    clock_t start = clock();
    char search_name[STR_LEN];
    
    for (int k = 0; k < iterations; k++) {
        if (degenerate_type == 0) {
             sprintf(search_name, "File_%05d_%d", rand() % (node_count * 10), rand() % node_count);
        } else if (degenerate_type == 1) {
             sprintf(search_name, "File_%05d", node_count - 1);
        } else if (degenerate_type == 2) {
             sprintf(search_name, "File_%05d", 0);
        }
        search_node(root, search_name);
    }
    
    clock_t end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC;
}

void inorder_no_print(node* current) {
    if (current != NULL) {
        inorder_no_print(current->left);
        (void)current->filename; 
        inorder_no_print(current->right);
    }
}

static double measure_inorder_time(node *root) {
    if (root == NULL) return 0.0;
    
    clock_t start = clock();
    
    inorder_no_print(root);
    
    clock_t end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC;
}

static void run_sorting_comparison() {
    int N_values[] = {100, 500, 1000, 5000, 10000};
    int num_sizes = sizeof(N_values) / sizeof(int);

    printf("\n--- СРАВНЕНИЕ ЭФФЕКТИВНОСТИ СОРТИРОВКИ (Обход Inorder) ---\n");
    printf("| %-10s | %-20s | %-25s | %-15s |\n", 
           "N (Элем.) ", "Время (Вырожденное) ", "Время (Сбалансированное) ", "Разница (раз)   ");
    printf("| %-10s | %-20s | %-25s | %-13s |\n", 
           "----------", "--------------------", "-------------------------", "----------------");

    for (int i = 0; i < num_sizes; i++) {
        int N = N_values[i];
        
        node *degenerate_root = create_right_skewed_tree(N); 
        node *balanced_root = create_balanced_tree(N);
        
        double t_degenerate = measure_inorder_time(degenerate_root);
        double t_balanced = measure_inorder_time(balanced_root); 
        
        double diff = t_degenerate / (t_balanced + 1e-9);
        char diff_str[30];
        
        if (diff > 1.05) {
            sprintf(diff_str, "Выр. > в %.2f", diff);
        } else if (diff < 0.95) {
            sprintf(diff_str, "Сбалан. > в %.2f", 1.0 / diff);
        } else {
            sprintf(diff_str, "Схожи");
        }

        printf("| %-11d| %-20.6f | %-25.6f | %-21s |\n", N, t_degenerate, t_balanced, diff_str);

        destroy_tree(degenerate_root);
        destroy_tree(balanced_root);
    }
    printf("--------------------------------------------------------------------------------------\n");
}

node* create_linked_list_tree(int count) {
    node *root = NULL;
    char name[STR_LEN];
    char date[STR_LEN];
    
    for (int i = 0; i < count; i++) {
        sprintf(name, "File_%05d", i);
        sprintf(date, "01.01.2020");
        root = insert_node(root, name, date);
    }
    return root;
}

node* create_balanced_tree(int count) {
    node *root = NULL;
    char name[STR_LEN];
    char date[STR_LEN];
    
    for (int i = 0; i < count; i++) {
        sprintf(name, "File_%05d_%d", rand() % (count * 10), i); 
        sprintf(date, "01.01.2020");
        root = insert_node(root, name, date);
    }
    return root;
}

void run_efficiency_test() {
    int N_values[] = {100, 500, 1000, 5000, 10000};
    int I_values[] = {100, 100, 100, 20, 10};
    int num_sizes = sizeof(N_values) / sizeof(int);

    printf("\n--- СРАВНЕНИЕ ЭФФЕКТИВНОСТИ ПОИСКА (h=N vs h=logN) ---\n");
    printf("| %-10s | %-20s | %-25s | %-15s |\n", 
           "N (Элем.)", "Время (Вырожденное)", "Время (Сбалансированное)", "Разница (раз)   ");
    printf("| %-9s | %-19s | %-24s | %-15s |\n", 
           "---------", "-------------------", "------------------------", "----------------");

    for (int i = 0; i < num_sizes; i++) {
        int N = N_values[i];
        int I = I_values[i];
        
        node *degenerate_root = create_linked_list_tree(N);
        node *balanced_root = create_balanced_tree(N);
        
        double t_degenerate = measure_search_time(degenerate_root, I, N, 1);
        double t_balanced = measure_search_time(balanced_root, I, N, 0); 
        
        double diff = t_degenerate / (t_balanced + 1e-9); 
        char diff_str[30];
        
        if (diff > 1.05) {
            sprintf(diff_str, "Выр. > в %.2f", diff);
        } else if (diff < 0.95) {
            sprintf(diff_str, "Сбалан. > в %.2f", 1.0 / diff);
        } else {
            sprintf(diff_str, "Схожи");
        }

        printf("| %-10d| %-19.6f | %-24.6f | %-20s |\n", N, t_degenerate, t_balanced, diff_str);

        destroy_tree(degenerate_root);
        destroy_tree(balanced_root);
    }
    printf("----------------------------------------------------------------------------------\n");

    run_sorting_comparison();
}

static node* create_left_skewed_tree(int count) {
    node *root = NULL;
    char name[STR_LEN];
    char date[STR_LEN];
    
    for (int i = count - 1; i >= 0; i--) {
        sprintf(name, "File_%05d", i);
        sprintf(date, "01.01.2020");
        root = insert_node(root, name, date);
    }
    return root;
}

static node* create_right_skewed_tree(int count) {
    node *root = NULL;
    char name[STR_LEN];
    char date[STR_LEN];
    
    for (int i = 0; i < count; i++) {
        sprintf(name, "File_%05d", i);
        sprintf(date, "01.01.2020");
        root = insert_node(root, name, date);
    }
    return root;
}

static void collect_for_removal_recursive(node *curr, const char *cutoff_date, char (*names_to_delete)[STR_LEN], int *count) {
    if (curr == NULL) return;
    collect_for_removal_recursive(curr->left, cutoff_date, names_to_delete, count);
    
    if (*count < MAX_NODES && compare_dates_chrono(curr->date, cutoff_date) < 0) {
        strncpy(names_to_delete[*count], curr->filename, STR_LEN - 1);
        names_to_delete[*count][STR_LEN - 1] = '\0';
        (*count)++;
    }
    collect_for_removal_recursive(curr->right, cutoff_date, names_to_delete, count);
}

void run_skewed_comparison() {
    int N_values[] = {100, 500, 1000, 5000, 10000};
    int I_values[] = {100, 100, 100, 20, 10};
    int num_sizes = sizeof(N_values) / sizeof(int);

    printf("\n--- Сравнение левосторонних и правосторонних деревьев ---\n");
    printf("| %-10s | %-18s | %-18s | %-15s |\n", 
           "N (Элем.)", "Время (Правостороннее)", "Время (Левостороннее)", "Разница (раз)");
    printf("| %-10s | %-18s | %-18s | %-15s |\n", 
           "----------", "------------------", "------------------", "---------------");

    for (int i = 0; i < num_sizes; i++) {
        int N = N_values[i];
        int I = I_values[i];
        
        node *right_root = create_right_skewed_tree(N);
        node *left_root = create_left_skewed_tree(N);
        
        double t_right = measure_search_time(right_root, I, N, 1);
        double t_left = measure_search_time(left_root, I, N, 2);
        
        double diff = t_right / (t_left + 1e-9);
        char diff_str[30];
        
        if (diff > 1.05) {
            sprintf(diff_str, "Право > %.2f", diff);
        } else if (diff < 0.95) {
            sprintf(diff_str, "Лево > %.2f", 1.0 / diff);
        } else {
            sprintf(diff_str, "Схожи");
        }

        printf("| %-10d | %-18.8f | %-18.8f | %-15s |\n", N, t_right, t_left, diff_str);

        destroy_tree(right_root);
        destroy_tree(left_root);
    }
    printf("------------------------------------------------------------------------\n");
}

void remove_files_by_date_interactive(node **root, const char *cutoff_date) {
    if (*root == NULL) {
        printf("Дерево пусто.\n");
        return;
    }

    char (*names_to_delete)[STR_LEN] = malloc(MAX_NODES * sizeof(*names_to_delete));
    if (names_to_delete == NULL) {
        printf("Ошибка: Нехватка памяти для операции.\n");
        return;
    }

    int count = 0;
    collect_for_removal_recursive(*root, cutoff_date, names_to_delete, &count);

    if (count == 0) {
        printf("Файлов старее %s не найдено.\n", cutoff_date);
        free(names_to_delete);
        return;
    }

    printf("Найдено %d файлов для удаления. Удаляем...\n", count);

    clock_t start = clock();
    for (int i = 0; i < count; i++) {
        *root = delete_node(*root, names_to_delete[i]);
    }
    clock_t end = clock();

    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;
    
    printf("Успешно удалено %d файлов.\n", count);
    printf("Время операции: %.6f сек.\n", time_taken);

    free(names_to_delete);
}

static void write_dot_nodes(node *current, FILE *fp) {
    if (current == NULL) return;

    fprintf(fp, "  \"%p\" [label=\"{%s | %s}\"];\n", 
            (void*)current, current->filename, current->date);

    if (current->left != NULL) {
        fprintf(fp, "  \"%p\" -> \"%p\" [label=\"L\"];\n", 
                (void*)current, (void*)current->left);
        write_dot_nodes(current->left, fp);
    }

    if (current->right != NULL) {
        fprintf(fp, "  \"%p\" -> \"%p\" [label=\"R\"];\n", 
                (void*)current, (void*)current->right);
        write_dot_nodes(current->right, fp);
    }
}

static void generate_dot_file(node *root, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("Ошибка открытия файла для DOT");
        return;
    }

    fprintf(fp, "digraph BST {\n");
    fprintf(fp, "  node [shape=record, style=filled, fillcolor=\"#E6E6FF\"];\n"); 
    
    write_dot_nodes(root, fp);
    
    fprintf(fp, "}\n");
    fclose(fp);
}

void visualize_tree_with_graphviz(node *root, const char *base_filename) {
    if (root == NULL) {
        printf("Дерево пусто, визуализация невозможна.\n");
        return;
    }

    char dot_filename[256];
    char png_filename[256];
    char command[512];
    
    sprintf(dot_filename, "%s.dot", base_filename);
    sprintf(png_filename, "%s.png", base_filename);

    printf("\n-> Генерируется DOT-файл: %s\n", dot_filename);
    generate_dot_file(root, dot_filename);
    
    sprintf(command, "dot -Tpng %s -o %s", dot_filename, png_filename);

    printf("-> Выполняется команда Graphviz:\n   %s\n", command);

    int status = system(command);

    if (status == 0) {
        printf("Визуализация успешно создана: %s\n", png_filename);
    } else {
        fprintf(stderr, "Ошибка при вызове Graphviz. Код выхода: %d\n", status);
        fprintf(stderr, "Убедитесь, что Graphviz установлен и 'dot' доступен в PATH.\n");
    }
}

static void write_nodes_to_txt_recursive(node *current, FILE *fp) {
    if (current == NULL) return;

    write_nodes_to_txt_recursive(current->left, fp);
    
    fprintf(fp, "%s, %s\n", current->filename, current->date);
    
    write_nodes_to_txt_recursive(current->right, fp);
}

void save_tree_to_txt(node *root, const char *base_filename) {
    char txt_filename[256];
    
    sprintf(txt_filename, "%s.txt", base_filename);
    
    FILE *fp = fopen(txt_filename, "w");
    if (fp == NULL) {
        perror("Ошибка открытия файла для записи данных");
        return;
    }
    
    printf("-> Сохраняются данные в текстовый файл: %s\n", txt_filename);
    
    write_nodes_to_txt_recursive(root, fp);
    
    fclose(fp);
    printf("Данные дерева успешно сохранены.\n");
}

node* create_test_tree_with_dates(int count) {
    node *root = NULL;
    char name[STR_LEN];
    char date[STR_LEN];
    
    for (int i = 1; i <= count; i++) {
        sprintf(name, "File_%03d_%d", rand() % (count * 10), i);
        
        int sequential_day = (i - 1) % 30 + 1;
        int sequential_month = (i - 1) / 30 + 1;
        
        sprintf(date, "%02d.%02d.2001", sequential_day, sequential_month); 
        
        root = insert_node(root, name, date);
    }
    return root;
}

void get_cutoff_date(int N, int k, char *cutoff_date) {
    if (k < N) {
        int cutoff_index = k + 1;
        int cutoff_day = (cutoff_index - 1) % 30 + 1; 
        int cutoff_month = (cutoff_index - 1) / 30 + 1;
        sprintf(cutoff_date, "%02d.%02d.2001", cutoff_day, cutoff_month);
    } else { 
        sprintf(cutoff_date, "01.01.3000"); 
    }
}

node* create_worst_case_tree(int count) {
    node *root = NULL;
    char name[STR_LEN];
    char date[STR_LEN];
    
    for (int i = 1; i <= count; i++) {
        sprintf(name, "FileA_%05d", i); 
        
        int sequential_day = (i - 1) % 30 + 1;
        int sequential_month = (i - 1) / 30 + 1;
        sprintf(date, "%02d.%02d.2001", sequential_day, sequential_month); 
        
        root = insert_node(root, name, date);
    }
    return root;
}

static node* create_worst_case_tree_smart(int total_N, int files_to_delete) {
    node *root = NULL;
    char name[STR_LEN];
    char date[STR_LEN];
    
    for (int i = 0; i < total_N; i++) {
        sprintf(name, "File_%05d", i); 
        
        if (i >= (total_N - files_to_delete)) {
            sprintf(date, "01.01.2000");
        } else {
            sprintf(date, "01.01.2030");
        }
        
        root = insert_node(root, name, date);
    }
    return root;
}

void run_scenario_comparison_test() {
    int N_values[] = {100, 1000, 5000}; 
    int num_N = sizeof(N_values) / sizeof(int);
    
    const char *TEST_CUTOFF = "01.01.2001"; 
    
    char cutoff_dyn[STR_LEN]; 

    char *test_names[] = {"СЛУЧАЙНОЕ (Average)", "ВЫРОЖДЕННОЕ (Worst: Удаление снизу)"};
    
    for (int test_type = 0; test_type < 2; test_type++) {
        printf("\n\n=== %s ===\n", test_names[test_type]);
        printf("| %-7s | %-7s | %-12s | %-12s | %-15s |\n", "N", "K", "T1 (In-place)", "T2 (Rebuild)", "Result");
        printf("|---------|---------|--------------|--------------|-----------------|\n");

        for (int i_N = 0; i_N < num_N; i_N++) {
            int N = N_values[i_N];
            
            int REPEAT_COUNT = (N == 5000) ? 5 : 10; 
            
            for (int step = 1; step <= 10; step++) {
                int k = (N * step) / 10;
                
                double total_t1 = 0.0;
                double total_t2 = 0.0;
                
                node *base_root = NULL;

                if (test_type == 0) {
                    base_root = create_test_tree_with_dates(N);
                    if (k==N) sprintf(cutoff_dyn, "01.01.3000");
                    else {
                        int day = (k % 30) + 1; int month = (k / 30) + 1;
                        sprintf(cutoff_dyn, "%02d.%02d.2001", day, month);
                    }
                } 
                else {
                    base_root = create_worst_case_tree_smart(N, k);
                    strncpy(cutoff_dyn, TEST_CUTOFF, STR_LEN); 
                }
                
                for (int r = 0; r < REPEAT_COUNT; r++) {
                    double t1_single, t2_single;
                    
                    node *copy1 = copy_tree(base_root);
                    run_scenario_1(&copy1, cutoff_dyn, &t1_single);
                    destroy_tree(copy1);
                    total_t1 += t1_single;

                    node *copy2 = copy_tree(base_root);
                    node *res2 = run_scenario_2(copy2, cutoff_dyn, &t2_single);
                    
                    if (res2 != NULL) {
                        destroy_tree(res2); 
                    } 
                    
                    total_t2 += t2_single;
                }
                
                destroy_tree(base_root);
                
                double t1_avg = total_t1 / REPEAT_COUNT;
                double t2_avg = total_t2 / REPEAT_COUNT;

                char cmp[30];
                if (t1_avg > t2_avg * 1.1) sprintf(cmp, "T2 faster %.1fx", t1_avg/t2_avg);
                else if (t2_avg > t1_avg * 1.1) sprintf(cmp, "T1 faster %.1fx", t2_avg/t1_avg);
                else sprintf(cmp, "Similar");

                printf("| %-7d | %-7d | %-12.6f | %-12.6f | %-15s |\n", N, k, t1_avg, t2_avg, cmp);
            }
            printf("|---------|---------|--------------|--------------|-----------------|\n");
        }
    }
}