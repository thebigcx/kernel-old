#include <util/list.h>

list_t* list_create()
{
    list_t* list = (list_t*)kmalloc(sizeof(list_t));
    memset(list, 0, sizeof(list_t));
    return list;
}

void list_destroy(list_t* list)
{
    for (int i = 0; i < list->cnt; i++)
    {
        list_pop_back(list);
    }

    kfree(list);
}

list_node_t* list_push_back(list_t* list, void* val)
{
    list_node_t* node = kmalloc(sizeof(list_node_t));

    if (!list->head && !list->tail)
    {
        list->head = node;
        list->tail = node;
        node->prev = NULL;
        node->next = NULL;
        node->val = val;
        list->cnt++;
        return node;
    }

    node->prev = list->tail;
    node->next = NULL;
    node->val = val;
    list->tail->next = node;
    list->tail = node;
    list->cnt++;
    return node;
}

void* list_pop_back(list_t* list)
{
    if (list->cnt == 0)
        return;

    list_node_t* node = list->tail;
    list->tail = node->prev;
    list->tail->next = NULL;
    void* val = node->val;
    kfree(node);

    return val;
}