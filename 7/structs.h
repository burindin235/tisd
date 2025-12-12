#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

// --- Константы ---
#define TABLE_INIT_SIZE 23  
#define MAX_KEY_LEN 10      
#define MAX_VALUE_LEN 100   

// Условие для Метод Цепочек: Макс. длина цепочки перед рехешированием
#define MAX_COLLISIONS 3

// Условие для Открытой Адресации: % заполнения
#define MAX_LOAD_SIZE 0.70 

#define NEXT_PRIME_MULTIPLIER 1.2

#define REHASH_FLAG 1000000 

// --- Структуры: Метод Цепочек ---
typedef struct ChainNode {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
    struct ChainNode *next;
} ChainNode;

typedef struct ChainingTable {
    ChainNode **array;
    int size;
    int count;
    int rehash_count;
} ChainingTable;

// --- Структуры: Открытая Адресация ---
typedef struct OpenAddressEntry {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
    int is_occupied;        // 1: занято, 0: свободно, -1: удалено
} OpenAddressEntry;

typedef struct OpenAddressTable {
    OpenAddressEntry *array;
    int size;
    int count;
    int deleted_count;      
    int rehash_count;       
} OpenAddressTable;

// --- Структуры: Деревья ---
typedef struct Node {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
    struct Node *left;
    struct Node *right;
} Node;

typedef struct AVL_Node {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
    int height;
    struct AVL_Node *left;
    struct AVL_Node *right;
} AVL_Node;

// --- Прототипы ---
unsigned int hash_function(const char *key, int m);

// Метод цепочек
ChainingTable *init_chaining_table();
void free_chaining_table(ChainingTable *table);
int insert_chaining_second_method(ChainingTable *table, const char *key, const char *value); 
const char *search_chaining_second_method(ChainingTable *table, const char *key, int *comparisons);
int delete_chaining_second_method(ChainingTable *table, const char *key, int *comparisons); 
void visualize_chaining(ChainingTable *table);
size_t memory_chaining_total(ChainingTable *table);
size_t memory_chaining_used(ChainingTable *table);

// Открытая адресация
OpenAddressTable *init_open_addressing_table();
void free_open_addressing_table(OpenAddressTable *table);
int insert_open_addressing_first_method(OpenAddressTable *table, const char *key, const char *value); 
const char *search_open_addressing_first_method(OpenAddressTable *table, const char *key, int *comparisons);
int delete_open_addressing_first_method(OpenAddressTable *table, const char *key, int *comparisons); 
void visualize_open_addressing(OpenAddressTable *table);
size_t memory_open_addressing_total(OpenAddressTable *table);
size_t memory_open_addressing_used(OpenAddressTable *table);

// BST
Node *insert_bst(Node *root, const char *key, const char *value); 
Node *insert_bst_with_count(Node *root, const char *key, const char *value, int *comparisons);
Node *search_bst(Node *root, const char *key);
Node *delete_bst_with_count(Node *root, const char *key, int *comparisons);
int search_bst_count(Node *root, const char *key);
void free_bst(Node *root);
void visualize_bst(Node *root);
int count_nodes_bst(Node *root);
size_t memory_bst(Node *root);

// AVL
AVL_Node *insert_avl(AVL_Node *root, const char *key, const char *value);
AVL_Node *insert_avl_with_count(AVL_Node *root, const char *key, const char *value, int *comparisons);
AVL_Node *search_avl(AVL_Node *root, const char *key);
AVL_Node *delete_avl_with_count(AVL_Node *root, const char *key, int *comparisons);
int search_avl_count(AVL_Node *root, const char *key);
void free_avl(AVL_Node *root);
void visualize_avl(AVL_Node *root);
int count_nodes_avl(AVL_Node *root);
size_t memory_avl(AVL_Node *root);

#endif