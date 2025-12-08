#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

// --- Общие Константы ---
#define TABLE_INIT_SIZE 23  // Начальный размер хеш-таблицы (простое число)
#define MAX_KEY_LEN 10      // Максимальная длина ключевого слова
#define MAX_VALUE_LEN 100   // Максимальная длина подсказки (HELP)
#define MAX_COLLISIONS 3    // Максимально допустимое количество сравнений для рехеширования
#define REHASH_FLAG 1000000 // Флаг для указания, что было рехеширование (добавляется к числу сравнений)

#define MAX_LOAD_SIZE 0.95 // макс заполнение при открытом хешировании

// --- 1. Структуры для Метода Цепочек (Закрытая адресация) ---

typedef struct ChainNode {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
    struct ChainNode *next;
} ChainNode;

typedef struct ChainingTable {
    ChainNode **array;
    int size;
    int count;
    int rehash_count;       // Счетчик реструктуризаций
} ChainingTable;


// --- 2. Структуры для Открытой Адресации ---

typedef struct OpenAddressEntry {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
    int is_occupied;        // 1: занято, 0: свободно, -1: удалено (Tombstone)
} OpenAddressEntry;

typedef struct OpenAddressTable {
    OpenAddressEntry *array;
    int size;
    int count;
    int deleted_count;      // Количество удаленных элементов (Tombstone)
    int rehash_count;       // Счетчик реструктуризаций
} OpenAddressTable;


// --- 3. Структуры для Двоичного Дерева Поиска (BST) ---

typedef struct Node {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
    struct Node *left;
    struct Node *right;
} Node;


// --- 4. Структуры для АВЛ-дерева ---

typedef struct AVL_Node {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
    int height;
    struct AVL_Node *left;
    struct AVL_Node *right;
} AVL_Node;


// --- Прототипы общих функций ---
unsigned int hash_function(const char *key, int m);


// --- Прототипы функций (Хеш-таблица: Метод Цепочек) ---
ChainingTable *init_chaining_table();
void free_chaining_table(ChainingTable *table);
int insert_chaining_second_method(ChainingTable *table, const char *key, const char *value); // Возвращает сравнения (+REHASH_FLAG)
const char *search_chaining_second_method(ChainingTable *table, const char *key, int *comparisons);
int search_chain_count(ChainingTable *table, const char *key);
void visualize_chaining(ChainingTable *table);
// Новые/Обновленные функции
int delete_chaining_second_method(ChainingTable *table, const char *key, int *comparisons); // Возвращает статус (0/1), сравнения в *comparisons
size_t memory_chaining_total(ChainingTable *table); // Общая выделенная память
size_t memory_chaining_used(ChainingTable *table); // Память, занятая полезными данными


// --- Прототипы функций (Хеш-таблица: Открытая Адресация) ---
OpenAddressTable *init_open_addressing_table();
void free_open_addressing_table(OpenAddressTable *table);
int insert_open_addressing_first_method(OpenAddressTable *table, const char *key, const char *value); // Возвращает сравнения (+REHASH_FLAG)
const char *search_open_addressing_first_method(OpenAddressTable *table, const char *key, int *comparisons);
int search_open_count(OpenAddressTable *table, const char *key);
void visualize_open_addressing(OpenAddressTable *table);
// Новые/Обновленные функции
int delete_open_addressing_first_method(OpenAddressTable *table, const char *key, int *comparisons); // Возвращает статус (0/1), сравнения в *comparisons
size_t memory_open_addressing_total(OpenAddressTable *table); // Общая выделенная память
size_t memory_open_addressing_used(OpenAddressTable *table); // Память, занятая полезными данными


// --- Прототипы функций (BST) ---
Node *insert_bst(Node *root, const char *key, const char *value); // Старая версия для рехеширования
Node *insert_bst_with_count(Node *root, const char *key, const char *value, int *comparisons); // Новая обертка
Node *search_bst(Node *root, const char *key);
void free_bst(Node *root);
void visualize_bst(Node *root);
int search_bst_count(Node *root, const char *key);
// Новые/Обновленные функции
Node *delete_bst(Node *root, const char *key); // Старая версия
Node *delete_bst_with_count(Node *root, const char *key, int *comparisons); // Новая обертка
size_t memory_bst(Node *root);


// --- Прототипы функций (AVL) ---
AVL_Node *insert_avl(AVL_Node *root, const char *key, const char *value); // Старая версия для рехеширования
AVL_Node *insert_avl_with_count(AVL_Node *root, const char *key, const char *value, int *comparisons); // Новая обертка
AVL_Node *search_avl(AVL_Node *root, const char *key);
void free_avl(AVL_Node *root);
void visualize_avl(AVL_Node *root);
int search_avl_count(AVL_Node *root, const char *key);
// Новые/Обновленные функции
AVL_Node *delete_avl(AVL_Node *root, const char *key); // Старая версия
AVL_Node *delete_avl_with_count(AVL_Node *root, const char *key, int *comparisons); // Новая обертка
size_t memory_avl(AVL_Node *root);

#endif // STRUCTS_H