#ifndef TREE_FUNCS_H
#define TREE_FUNCS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define STR_LEN 50
#define MAX_NODES 10000

typedef char string_t[STR_LEN];

typedef struct node {
    string_t filename;
    string_t date;
    struct node *left;
    struct node *right;
} node;

typedef struct {
    node *arr[MAX_NODES];
    int count;
} node_array_t;

// --- Базовые функции ---
node* create_node(const char *filename, const char *date);
node* copy_tree(node *root); 
node* insert_node(node *root, const char *filename, const char *date);
node* search_node(node *root, const char *filename); 
node* delete_node(node *root, const char *filename); 
void destroy_tree(node *root);

// --- Вывод и утилиты ---
void inorder_traversal(node *root);
void visualize_tree_with_graphviz(node *root, const char *base_filename);
void save_tree_to_txt(node *root, const char *base_filename);
int is_valid_date(const char *date);
void generate_random_file_data(char *filename, char *date);
int compare_dates_chrono(const char *date1, const char *date2);

// --- Сценарии и Тесты ---
void run_scenario_1(node **root, const char *cutoff_date, double *time_taken);
node* run_scenario_2(node *root_old, const char *cutoff_date, double *time_taken);

void run_efficiency_test();
void run_skewed_comparison();
void run_scenario_comparison_test();

void remove_files_by_date_interactive(node **root, const char *cutoff_date); 

node* create_balanced_tree(int count);
node* create_linked_list_tree(int count);

#endif