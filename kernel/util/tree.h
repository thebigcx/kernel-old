#pragma once

#include <util/list.h>

typedef struct tree_node
{
    list_t* children;
    void* data; // User data

} tree_node_t;

typedef struct tree
{
    tree_node_t* root;

} tree_t;

tree_t* tree_create();
void tree_destroy(tree_t* tree);
void tree_insert(tree_t* tree, tree_node_t* parent, void* data);
void* tree_remove(tree_t* tree, tree_node_t* node);
tree_node_t* tree_node_create(void* data);
void* tree_node_destroy(tree_t* tree, tree_node_t* node);