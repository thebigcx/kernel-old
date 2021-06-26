#pragma once

#include <util/types.h>

typedef struct list_node
{
    struct list_node* prev;
    struct list_node* next;
    void* val;

} list_node_t;

typedef struct list
{
    list_node_t* head;
    list_node_t* tail;
    uint32_t cnt;

} list_t;

#define list_foreach(list, ele) for (list_node_t* ele = list->head; ele != NULL; ele = ele->next)

list_t* list_create();
void list_destroy(list_t* list);
list_node_t* list_push_back(list_t* list, void* val);
void* list_pop_back(list_t* list);
list_node_t* list_push_front(list_t* list, void* val);
void* list_pop_front(list_t* list);
list_node_t* list_get(list_t* list, uint32_t idx);