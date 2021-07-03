#include <util/tree.h>
#include <mem/kheap.h>

tree_t* tree_create()
{
    tree_t* tree = kmalloc(sizeof(tree_t));
    tree->root = kmalloc(sizeof(tree_node_t));
    tree->root->children = list_create();
    return tree;
}

void tree_destroy(tree_t* tree)
{
    // TODO: implement destroying tree
    kfree(tree);
}

tree_node_t* tree_insert(tree_t* tree, tree_node_t* parent, void* data)
{
    tree_node_t* node = tree_node_create(data);
    list_push_back(parent->children, node);
    return node;
}

void* tree_remove_recur(tree_t* tree, tree_node_t* parent, tree_node_t* remove_node)
{
    list_foreach(parent->children, node)
    {
        if (node == remove_node)
        {

        }
    }
}

void* tree_remove(tree_t* tree, tree_node_t* node)
{
    tree_remove_recur(tree, tree->root, node);
}

tree_node_t* tree_node_create(void* data)
{
    tree_node_t* node = kmalloc(sizeof(tree_node_t));
    node->data = data;
    node->children = list_create();
    return node;
}

void* tree_node_destroy(tree_t* tree, tree_node_t* node)
{
    list_foreach(node->children, child)
    {
        tree_node_destroy(tree, child);
    }

    void* data = node->data;
    list_destroy(node->children);
    kfree(node);

    return data;
}