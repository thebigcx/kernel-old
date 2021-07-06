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
    list->cnt--;
    void* val = node->val;
    kfree(node);

    return val;
}

list_node_t* list_push_front(list_t* list, void* val)
{
    if (!list->head && !list->tail)
    {
        return list_push_back(list, val); // Doesn't matter if front or back
    }

    list_node_t* node = kmalloc(sizeof(list_node_t));

    node->prev = NULL;
    node->next = list->head;
    node->val = val;
    list->head->prev = node;
    list->head = node;
    list->cnt++;

    return node;
}

void* list_pop_front(list_t* list)
{
    if (list->cnt == 0)
        return;

    list_node_t* node = list->head;
    list->head = list->head->next;
    list->head->prev = NULL;
    list->cnt--;
    void* val = node->val;
    kfree(node);

    return val;
}

list_node_t* list_get(list_t* list, uint32_t idx)
{
    if (idx >= list->cnt)
        return NULL;

    uint32_t i = 0;
    list_foreach(list, node)
    {
        if (i++ == idx)
        {
            return node;
        }
    }

    return NULL;
}

void list_remove(list_t* list, uint32_t idx)
{
    list_node_t* node = list_get(list, idx);

    if (node->prev)
        node->prev->next = node->next;

    if (node->next)
        node->next->prev = node->prev;

    if (node == list->head)
    {
        list->head = node->next;
    }
    if (node == list->tail)
    {
        list->tail = node->prev;
    }

    list->cnt--;
    kfree(node);
}