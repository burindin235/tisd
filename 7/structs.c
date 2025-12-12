#include "structs.h"

// --- Вспомогательные функции ---

static int is_prime(int n) {
    if (n <= 1) return 0;
    if (n <= 3) return 1;
    if (n % 2 == 0 || n % 3 == 0) return 0;
    for (int i = 5; i * i <= n; i = i + 6)
        if (n % i == 0 || n % (i + 2) == 0)
            return 0;
    return 1;
}

static int find_next_prime(int current_size) {
    int next = current_size * NEXT_PRIME_MULTIPLIER + 1;
    while (!is_prime(next)) {
        next++;
    }
    return next;
}

unsigned int hash_function(const char *key, int m) {
    unsigned int hash = 5381;
    if (key == NULL || m <= 0) return 0; 
    int c;
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c; 
    }
    return hash % m;
} 

// =================================================================
// --- 1. Метод Цепочек (Списки) ---
// =================================================================

static ChainNode *create_chain_node(const char *key, const char *value) {
    ChainNode *new_node = (ChainNode *)malloc(sizeof(ChainNode));
    if (new_node == NULL) return NULL;
    strncpy(new_node->key, key, MAX_KEY_LEN - 1);
    new_node->key[MAX_KEY_LEN - 1] = '\0';
    strncpy(new_node->value, value, MAX_VALUE_LEN - 1);
    new_node->value[MAX_VALUE_LEN - 1] = '\0';
    new_node->next = NULL;
    return new_node;
}

ChainingTable *init_chaining_table() {
    ChainingTable *table = (ChainingTable *)malloc(sizeof(ChainingTable));
    if (table == NULL) return NULL;
    table->size = TABLE_INIT_SIZE;
    table->count = 0;
    table->rehash_count = 0;
    table->array = (ChainNode **)calloc(table->size, sizeof(ChainNode *));
    if (table->array == NULL) { free(table); return NULL; }
    return table;
}

static void free_chain_list(ChainNode *node) {
    ChainNode *temp;
    while (node != NULL) {
        temp = node;
        node = node->next;
        free(temp);
    }
}

void free_chaining_table(ChainingTable *table) {
    if (table == NULL) return;
    for (int i = 0; i < table->size; i++) {
        free_chain_list(table->array[i]);
    }
    free(table->array);
    free(table);
}

static void rehash_chaining(ChainingTable *table) {
    int old_size = table->size;
    int new_size = find_next_prime(old_size);
    ChainNode **old_array = table->array;
    
    table->array = (ChainNode **)calloc(new_size, sizeof(ChainNode *));
    if (table->array == NULL) {
        table->array = old_array; 
        return;
    }
    table->size = new_size;
    table->count = 0;
    table->rehash_count++; 

    for (int i = 0; i < old_size; i++) {
        ChainNode *current = old_array[i];
        while (current != NULL) {
            insert_chaining_second_method(table, current->key, current->value); 
            ChainNode *temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(old_array);
    printf("[DEBUG] Rehash Chain -> New Size: %d\n", new_size);
}

int insert_chaining_second_method(ChainingTable *table, const char *key, const char *value) {
    if (table == NULL || key == NULL) return 0;
    int comparisons = 0;
    unsigned int index;
    ChainNode *head, *current;

    // Вставка
    while (1) {
        comparisons = 0;
        index = hash_function(key, table->size);
        head = table->array[index];
        current = head;

        // Ищем место для вставки
        while (current != NULL) {
            comparisons++;
            if (strcmp(current->key, key) == 0) {
                strncpy(current->value, value, MAX_VALUE_LEN - 1);
                current->value[MAX_VALUE_LEN - 1] = '\0';
                return comparisons;
            }
            current = current->next;
        }

        // ПРОВЕРКА ДЛЯ РЕСТРУКТУРИЗАЦИИ:
        // Если количество шагов поиска места (длина цепочки) превышает константу
        if (comparisons >= MAX_COLLISIONS && table->size < 200000000) {
            rehash_chaining(table);
            return comparisons + REHASH_FLAG; // Сигнал о рехешировании
        }

        ChainNode *new_node = create_chain_node(key, value);
        if (new_node == NULL) return comparisons;
        new_node->next = head;
        table->array[index] = new_node;
        table->count++;
        return comparisons + 1; 
    }
}

const char *search_chaining_second_method(ChainingTable *table, const char *key, int *comparisons) {
    if (!table || !key || !comparisons) return NULL;
    *comparisons = 0;
    unsigned int index = hash_function(key, table->size);
    ChainNode *current = table->array[index];
    while (current != NULL) {
        (*comparisons)++;
        if (strcmp(current->key, key) == 0) return current->value;
        current = current->next;
    }
    return NULL;
}

int delete_chaining_second_method(ChainingTable *table, const char *key, int *comparisons) {
    if (!table || !key || !comparisons) return 0;
    unsigned int index = hash_function(key, table->size);
    ChainNode *current = table->array[index];
    ChainNode *prev = NULL;
    *comparisons = 0;

    while (current != NULL) {
        (*comparisons)++;
        if (strcmp(current->key, key) == 0) {
            if (prev == NULL) table->array[index] = current->next;
            else prev->next = current->next;
            free(current);
            table->count--;
            return 1;
        }
        prev = current;
        current = current->next;
    }
    return 0;
}

size_t memory_chaining_total(ChainingTable *table) {
    if (!table) return 0;
    return sizeof(ChainingTable) + (table->size * sizeof(ChainNode *)) + (table->count * sizeof(ChainNode));
}

size_t memory_chaining_used(ChainingTable *table) {
    if (!table) return 0;
    return (table->count * sizeof(ChainNode));
}

void visualize_chaining(ChainingTable *table) {
    if (table == NULL) { printf("Empty.\n"); return; }
    printf("\n--- Chaining Hash Table (Size: %d, Count: %d, Rehashes: %d) ---\n", table->size, table->count, table->rehash_count);
    for (int i = 0; i < table->size; i++) {
        printf("Bucket %3d:", i);
        ChainNode *curr = table->array[i];
        if (curr == NULL) {
            printf(" [EMPTY]\n");
        } else {
            while (curr) { 
                printf(" [%s] -> ", curr->key); 
                curr = curr->next; 
            }
            printf("NULL");
            printf("\n");
        }
    }
}

// =================================================================
// --- 2. Открытая Адресация (Пробивание) ---
// =================================================================

OpenAddressTable *init_open_addressing_table() {
    OpenAddressTable *table = (OpenAddressTable *)malloc(sizeof(OpenAddressTable));
    if (!table) return NULL;
    table->size = TABLE_INIT_SIZE;
    table->count = 0;
    table->deleted_count = 0;
    table->rehash_count = 0;
    table->array = (OpenAddressEntry *)calloc(table->size, sizeof(OpenAddressEntry));
    if (!table->array) { free(table); return NULL; }
    return table;
}

void free_open_addressing_table(OpenAddressTable *table) {
    if (table) { free(table->array); free(table); }
}

static void rehash_open_addressing(OpenAddressTable *table) {
    int old_size = table->size;
    OpenAddressEntry *old_array = table->array;
    int new_size = find_next_prime(old_size);
    
    table->array = (OpenAddressEntry *)calloc(new_size, sizeof(OpenAddressEntry));
    if (!table->array) { 
        table->array = old_array; 
        return; 
    }
    
    table->size = new_size;
    table->count = 0;
    table->deleted_count = 0;
    table->rehash_count++; 

    for (int i = 0; i < old_size; i++) {
        if (old_array[i].is_occupied == 1) {
            insert_open_addressing_first_method(table, old_array[i].key, old_array[i].value); 
        }
    }
    free(old_array);
    printf("[DEBUG] Rehash OpenAddr -> New Size: %d\n", new_size);
}

int insert_open_addressing_first_method(OpenAddressTable *table, const char *key, const char *value) {
    if (!table || !key) return 0;
    
    // ПРОВЕРКА ДЛЯ РЕСТРУКТУРИЗАЦИИ:
    // Если заполненность (включая удаленные) > 70%
    double load_factor = (double)(table->count + table->deleted_count) / table->size;
    if (load_factor >= MAX_LOAD_SIZE && table->size < 2000000) {
        rehash_open_addressing(table);
    }
    
    unsigned int initial_hash = hash_function(key, table->size);
    unsigned int index = initial_hash;
    int comparisons = 0;
    int first_deleted_index = -1;
    int i = 0; 

    do {
        comparisons++;
        
        if (table->array[index].is_occupied == 1) {
            if (strcmp(table->array[index].key, key) == 0) {
                strncpy(table->array[index].value, value, MAX_VALUE_LEN - 1);
                table->array[index].value[MAX_VALUE_LEN - 1] = '\0';
                return comparisons;
            }
        } else if (table->array[index].is_occupied == -1) {
            if (first_deleted_index == -1) first_deleted_index = index;
        } else {
            // Пустое место (0)
            int insert_idx = (first_deleted_index != -1) ? first_deleted_index : index;

            strncpy(table->array[insert_idx].key, key, MAX_KEY_LEN - 1);
            table->array[insert_idx].key[MAX_KEY_LEN - 1] = '\0';
            strncpy(table->array[insert_idx].value, value, MAX_VALUE_LEN - 1);
            table->array[insert_idx].value[MAX_VALUE_LEN - 1] = '\0';
            
            if (table->array[insert_idx].is_occupied == 0) {
                table->count++;
            } else if (table->array[insert_idx].is_occupied == -1) {
                table->deleted_count--;
                table->count++;
            }
            table->array[insert_idx].is_occupied = 1;
            return comparisons;
        }
        
        // Квадратичное пробирование
        i++;
        index = (initial_hash + (unsigned int)i * i) % table->size;
        
    } while (index != initial_hash && i < table->size); // Защита от вечного цикла

    return comparisons;
}

const char *search_open_addressing_first_method(OpenAddressTable *table, const char *key, int *comparisons) {
    if (!table || !key || !comparisons) return NULL;
    unsigned int initial_hash = hash_function(key, table->size);
    unsigned int index = initial_hash;
    int i = 0;
    *comparisons = 0;

    do {
        (*comparisons)++;
        if (table->array[index].is_occupied == 0) return NULL;
        
        if (table->array[index].is_occupied == 1) {
            if (strcmp(table->array[index].key, key) == 0) return table->array[index].value;
        }
        i++;
        index = (initial_hash + (unsigned int)i * i) % table->size;
    } while (index != initial_hash && i < table->size);
    return NULL;
}

int delete_open_addressing_first_method(OpenAddressTable *table, const char *key, int *comparisons) {
    if (!table || !key || !comparisons) return 0;
    unsigned int initial_hash = hash_function(key, table->size);
    unsigned int index = initial_hash;
    int i = 0;
    *comparisons = 0;

    do {
        (*comparisons)++;
        if (table->array[index].is_occupied == 0) return 0;
        
        if (table->array[index].is_occupied == 1) {
            if (strcmp(table->array[index].key, key) == 0) {
                table->array[index].is_occupied = -1; // Tombstone
                table->count--;
                table->deleted_count++;
                return 1;
            }
        }
        i++;
        index = (initial_hash + (unsigned int)i * i) % table->size;
    } while (index != initial_hash && i < table->size);
    return 0;
}

size_t memory_open_addressing_total(OpenAddressTable *table) {
    if (!table) return 0;
    return sizeof(OpenAddressTable) + (table->size * sizeof(OpenAddressEntry));
}

size_t memory_open_addressing_used(OpenAddressTable *table) {
    if (!table) return 0;
    return (table->count * sizeof(OpenAddressEntry)); 
}

void visualize_open_addressing(OpenAddressTable *table) {
    if (!table) { printf("Empty.\n"); return; }
    printf("\n--- Open Addressing (Size: %d, Count: %d, Rehashes: %d, Deleted: %d) ---\n", 
           table->size, table->count, table->rehash_count, table->deleted_count);
    for (int i = 0; i < table->size; i++) {
        printf("Bucket %3d: ", i);
        if (table->array[i].is_occupied == 1) {
            printf("[KEY: %s]\n", table->array[i].key);
        } else if (table->array[i].is_occupied == -1) {
            printf("[DEL]\n");
        } else {
            printf("[EMPTY]\n");
        }
    }
}

// =================================================================
// --- 3. BST ---
// =================================================================

static void write_dot_recursive_bst(Node *node, FILE *fp, int *null_count) {
    if (!node) return;
    fprintf(fp, "  \"%p\" [label=\"%s\"];\n", (void *)node, node->key);
    if (node->left) {
        fprintf(fp, "  \"%p\" -> \"%p\";\n", (void *)node, (void *)node->left);
        write_dot_recursive_bst(node->left, fp, null_count);
    } else {
        fprintf(fp, "  null%d [shape=point];\n", *null_count);
        fprintf(fp, "  \"%p\" -> null%d [style=invis];\n", (void *)node, *null_count);
        (*null_count)++;
    }
    if (node->right) {
        fprintf(fp, "  \"%p\" -> \"%p\";\n", (void *)node, (void *)node->right);
        write_dot_recursive_bst(node->right, fp, null_count);
    } else {
        fprintf(fp, "  null%d [shape=point];\n", *null_count);
        fprintf(fp, "  \"%p\" -> null%d [style=invis];\n", (void *)node, *null_count);
        (*null_count)++;
    }
}

static Node *create_node(const char *key, const char *value) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (!new_node) return NULL;
    strncpy(new_node->key, key, MAX_KEY_LEN - 1);
    new_node->key[MAX_KEY_LEN - 1] = '\0';
    strncpy(new_node->value, value, MAX_VALUE_LEN - 1);
    new_node->value[MAX_VALUE_LEN - 1] = '\0';
    new_node->left = NULL; new_node->right = NULL;
    return new_node;
}

Node *insert_bst_recursive_count(Node *root, const char *key, const char *value, int *comparisons) {
    if (root == NULL) {
        (*comparisons)++;
        return create_node(key, value);
    }
    (*comparisons)++;
    int cmp = strcmp(key, root->key);
    if (cmp < 0) root->left = insert_bst_recursive_count(root->left, key, value, comparisons);
    else if (cmp > 0) root->right = insert_bst_recursive_count(root->right, key, value, comparisons);
    else {
        strncpy(root->value, value, MAX_VALUE_LEN - 1);
        root->value[MAX_VALUE_LEN - 1] = '\0';
    }
    return root;
}

Node *insert_bst_with_count(Node *root, const char *key, const char *value, int *comparisons) {
    *comparisons = 0;
    return insert_bst_recursive_count(root, key, value, comparisons);
}

Node *insert_bst(Node *root, const char *key, const char *value) {
    int dummy = 0;
    return insert_bst_recursive_count(root, key, value, &dummy);
}

Node *search_bst(Node *root, const char *key) {
    if (!root || strcmp(key, root->key) == 0) return root;
    if (strcmp(key, root->key) < 0) return search_bst(root->left, key);
    return search_bst(root->right, key);
}

int search_bst_count_recursive(Node *root, const char *key, int count) {
    if (!root) return count + 1;
    int cmp = strcmp(key, root->key);
    if (cmp == 0) return count + 1;
    if (cmp < 0) return search_bst_count_recursive(root->left, key, count + 1);
    return search_bst_count_recursive(root->right, key, count + 1);
}

int search_bst_count(Node *root, const char *key) {
    if (!root) return 0;
    return search_bst_count_recursive(root, key, 0);
}

void free_bst(Node *root) {
    if (!root) return;
    free_bst(root->left);
    free_bst(root->right);
    free(root);
}

static Node *min_value_node(Node *node) {
    Node *current = node;
    while (current && current->left != NULL) current = current->left;
    return current;
}

Node *delete_bst_recursive_count(Node *root, const char *key, int *comparisons) {
    if (root == NULL) {
        (*comparisons)++;
        return root;
    }
    (*comparisons)++;
    int cmp = strcmp(key, root->key);
    if (cmp < 0) root->left = delete_bst_recursive_count(root->left, key, comparisons);
    else if (cmp > 0) root->right = delete_bst_recursive_count(root->right, key, comparisons);
    else {
        if (root->left == NULL) {
            Node *temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            Node *temp = root->left;
            free(root);
            return temp;
        }
        Node *temp = min_value_node(root->right);
        strncpy(root->key, temp->key, MAX_KEY_LEN);
        strncpy(root->value, temp->value, MAX_VALUE_LEN);
        root->right = delete_bst_recursive_count(root->right, temp->key, comparisons); 
    }
    return root;
}

Node *delete_bst_with_count(Node *root, const char *key, int *comparisons) {
    *comparisons = 0;
    Node *found = search_bst(root, key);
    if (!found) {
        *comparisons = search_bst_count(root, key);
        return root;
    }
    return delete_bst_recursive_count(root, key, comparisons);
}

int count_nodes_bst(Node *root) {
    if (!root) return 0;
    return 1 + count_nodes_bst(root->left) + count_nodes_bst(root->right);
}

size_t memory_bst(Node *root) {
    return count_nodes_bst(root) * sizeof(Node);
}

void visualize_bst(Node *root) {
    if (!root) { printf("Empty BST.\n"); return; }
    FILE *fp = fopen("bst.dot", "w");
    if (!fp) return;
    fprintf(fp, "digraph BST { node [shape=circle, style=filled, fillcolor=lightblue];\n");
    int null = 0;
    write_dot_recursive_bst(root, fp, &null);
    fprintf(fp, "}\n");
    fclose(fp);
}

// =================================================================
// --- 4. AVL ---
// =================================================================

static int get_height(AVL_Node *n) { return n ? n->height : 0; }
static int max(int a, int b) { return (a > b) ? a : b; }
static void update_height(AVL_Node *n) { n->height = 1 + max(get_height(n->left), get_height(n->right)); }
static int get_balance(AVL_Node *n) { return n ? get_height(n->left) - get_height(n->right) : 0; }

static AVL_Node *create_avl_node(const char *key, const char *value) {
    AVL_Node *node = (AVL_Node *)malloc(sizeof(AVL_Node));
    if (!node) return NULL;
    strncpy(node->key, key, MAX_KEY_LEN - 1); node->key[MAX_KEY_LEN - 1] = 0;
    strncpy(node->value, value, MAX_VALUE_LEN - 1); node->value[MAX_VALUE_LEN - 1] = 0;
    node->height = 1; node->left = NULL; node->right = NULL;
    return node;
}

static AVL_Node *right_rotate(AVL_Node *y) {
    AVL_Node *x = y->left;
    AVL_Node *T2 = x->right;
    x->right = y; y->left = T2;
    update_height(y); update_height(x);
    return x;
}

static AVL_Node *left_rotate(AVL_Node *x) {
    AVL_Node *y = x->right;
    AVL_Node *T2 = y->left;
    y->left = x; x->right = T2;
    update_height(x); update_height(y);
    return y;
}

AVL_Node *insert_avl_recursive_count(AVL_Node *root, const char *key, const char *value, int *comparisons) {
    if (!root) {
        (*comparisons)++;
        return create_avl_node(key, value);
    }
    (*comparisons)++;
    int cmp = strcmp(key, root->key);
    if (cmp < 0) root->left = insert_avl_recursive_count(root->left, key, value, comparisons);
    else if (cmp > 0) root->right = insert_avl_recursive_count(root->right, key, value, comparisons);
    else { strncpy(root->value, value, MAX_VALUE_LEN - 1); return root; }

    update_height(root);
    int bal = get_balance(root);

    if (bal > 1 && strcmp(key, root->left->key) < 0) return right_rotate(root);
    if (bal < -1 && strcmp(key, root->right->key) > 0) return left_rotate(root);
    if (bal > 1 && strcmp(key, root->left->key) > 0) {
        root->left = left_rotate(root->left); return right_rotate(root);
    }
    if (bal < -1 && strcmp(key, root->right->key) < 0) {
        root->right = right_rotate(root->right); return left_rotate(root);
    }
    return root;
}

AVL_Node *insert_avl_with_count(AVL_Node *root, const char *key, const char *value, int *comparisons) {
    *comparisons = 0;
    return insert_avl_recursive_count(root, key, value, comparisons);
}

AVL_Node *insert_avl(AVL_Node *root, const char *key, const char *value) {
    int dummy = 0;
    return insert_avl_recursive_count(root, key, value, &dummy);
}

AVL_Node *search_avl(AVL_Node *root, const char *key) {
    if (!root || strcmp(key, root->key) == 0) return root;
    if (strcmp(key, root->key) < 0) return search_avl(root->left, key);
    return search_avl(root->right, key);
}

int search_avl_count_recursive(AVL_Node *root, const char *key, int count) {
    if (!root) return count + 1;
    int cmp = strcmp(key, root->key);
    if (cmp == 0) return count + 1;
    if (cmp < 0) return search_avl_count_recursive(root->left, key, count + 1);
    return search_avl_count_recursive(root->right, key, count + 1);
}

int search_avl_count(AVL_Node *root, const char *key) {
    if (!root) return 0;
    return search_avl_count_recursive(root, key, 0);
}

void free_avl(AVL_Node *root) {
    if (!root) return;
    free_avl(root->left); free_avl(root->right); free(root);
}

static AVL_Node *min_value_avl(AVL_Node *node) {
    AVL_Node *current = node;
    while (current->left != NULL) current = current->left;
    return current;
}

AVL_Node *delete_avl_recursive_count(AVL_Node *root, const char *key, int *comparisons) {
    if (!root) {
        (*comparisons)++;
        return root;
    }
    (*comparisons)++;
    int cmp = strcmp(key, root->key);
    if (cmp < 0) root->left = delete_avl_recursive_count(root->left, key, comparisons);
    else if (cmp > 0) root->right = delete_avl_recursive_count(root->right, key, comparisons);
    else {
        if (!root->left || !root->right) {
            AVL_Node *temp = root->left ? root->left : root->right;
            if (!temp) { temp = root; root = NULL; }
            else *root = *temp;
            free(temp);
        } else {
            AVL_Node *temp = min_value_avl(root->right);
            strncpy(root->key, temp->key, MAX_KEY_LEN);
            strncpy(root->value, temp->value, MAX_VALUE_LEN);
            root->right = delete_avl_recursive_count(root->right, temp->key, comparisons); 
        }
    }
    if (!root) return root;

    update_height(root);
    int bal = get_balance(root);

    if (bal > 1 && get_balance(root->left) >= 0) return right_rotate(root);
    if (bal > 1 && get_balance(root->left) < 0) {
        root->left = left_rotate(root->left); return right_rotate(root);
    }
    if (bal < -1 && get_balance(root->right) <= 0) return left_rotate(root);
    if (bal < -1 && get_balance(root->right) > 0) {
        root->right = right_rotate(root->right); return left_rotate(root);
    }
    return root;
}

AVL_Node *delete_avl_with_count(AVL_Node *root, const char *key, int *comparisons) {
    *comparisons = 0;
    AVL_Node *found = search_avl(root, key);
    if (!found) {
        *comparisons = search_avl_count(root, key);
        return root;
    }
    return delete_avl_recursive_count(root, key, comparisons);
}

int count_nodes_avl(AVL_Node *root) {
    if (!root) return 0;
    return 1 + count_nodes_avl(root->left) + count_nodes_avl(root->right);
}

size_t memory_avl(AVL_Node *root) {
    return count_nodes_avl(root) * sizeof(AVL_Node);
}

static void write_dot_recursive_avl(AVL_Node *node, FILE *fp, int *null_count) {
    if (!node) return;
    fprintf(fp, "  \"%p\" [label=\"%s\\n(H:%d)\"];\n", (void*)node, node->key, node->height);
    if (node->left) {
        fprintf(fp, "  \"%p\" -> \"%p\";\n", (void*)node, (void*)node->left);
        write_dot_recursive_avl(node->left, fp, null_count);
    } else {
        fprintf(fp, "  null%d [shape=point];\n", *null_count);
        fprintf(fp, "  \"%p\" -> null%d [style=invis];\n", (void*)node, *null_count);
        (*null_count)++;
    }
    if (node->right) {
        fprintf(fp, "  \"%p\" -> \"%p\";\n", (void*)node, (void *)node->right);
        write_dot_recursive_avl(node->right, fp, null_count);
    } else {
        fprintf(fp, "  null%d [shape=point];\n", *null_count);
        fprintf(fp, "  \"%p\" -> null%d [style=invis];\n", (void*)node, *null_count);
        (*null_count)++;
    }
}

void visualize_avl(AVL_Node *root) {
    if (!root) { printf("Empty AVL.\n"); return; }
    FILE *fp = fopen("avl.dot", "w");
    if (!fp) return;
    fprintf(fp, "digraph AVL { node [shape=circle, style=filled, fillcolor=lightgreen];\n");
    int null = 0;
    write_dot_recursive_avl(root, fp, &null);
    fprintf(fp, "}\n");
    fclose(fp);
}