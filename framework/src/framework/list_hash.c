#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "str.h"
#include "list_hash.h"

/* return a new list node */
list* list_create()
{
    list *result;

    result = (list *) MEMmalloc(sizeof(list));
    SET_LIST(result, NULL, NULL);

    return result;
}

/* Free every node of the list. The actual values are not freed */
int list_free(list *head)
{
    list *tmp,
         *curr = head;

    RETURN_NULL(head);

    while(curr) {
        tmp = curr;
        curr = curr->next;
        tmp->next = NULL;
        MEMfree(tmp);
    }
    return 0;
}

/* Free every node of the list. The actual values are not freed */
int list_empty(list *head)
{
    RETURN_NULL(head);

    head->next = NULL;

    return list_free(head->next);
}

int list_is_empty(list *head)
{
    return head == NULL &&
           head->next == NULL;
}

/* adds the new element to the front of the list
 * right after the sentinal head node */
int list_addtofront(list *head, void *value)
{
    list *new;

    RETURN_NULL(head);

    new = list_create();

    SET_LIST(new, value, head->next);

    head->next = new;

    return 0;
}

int list_addtoend(list *head, void *value)
{
    list *new,
         *curr = head;

    RETURN_NULL(head);

    /* traverse to the end of the list */
    while(curr->next && (curr = curr->next));

    /* add new list node to the tail */
    new = list_create();
    SET_LIST(new, value, NULL);
    curr->next = new;

    return 0;
}

void list_print_str(list *head){
    printf("[ ");
    while((head = head->next))
        printf("%s, ", (char*) head->value);
    printf("]\n");
}

int list_remove(list *head, void *value)
{
    list *tmp,
         *curr = head;

    RETURN_NULL(head);

    while(curr->next) {
        tmp = curr->next;

        if(tmp->value == value) {
            curr->next = tmp->next;
            MEMfree(tmp);
            return 0;
        }
        curr = curr->next;
    }

    /* elem not found in list */
    return -2;
}

void *get_from_end(list *head, int index){
    list *stride = head, *curr = head;
    int i = 0;

    while(curr){
        if (i > index){
            stride = stride->next;
        }
        curr = curr->next;
        i++;
    }

    return stride->value;
}

void *list_get_last(list *head)
{
    list *prev;
    RETURN_NULL(head);

    while(head){
        prev = head;
        head = head->next;
    }

    return prev->value;
}

int list_length(list *head)
{
    int length = 0;

    RETURN_NULL(head);

    while((head = head->next) && ++length);

    return length;
}

int list_contains(list *head, void *value)
{
    RETURN_NULL(head);

    while((head = head->next))
        if(head->value == value)
            return 1;
    return 0;
}

int list_get_index_fun(list *head, void *value, bool (*fun)(void *, void *))
{
    int index = 0;
    RETURN_NULL(head);

    while((head = head->next)) {
        if(fun(value, head->value))
            return index;
        index++;
    }

    return -1;
}


int list_contains_fun(list *head, void *value, bool (*fun)(void *, void *))
{
    RETURN_NULL(head);

    while((head = head->next))
        if(fun(value, head->value))
            return 1;
    return 0;
}

void *list_get_elem(list *head, int index)
{
    int i;

    RETURN_NNULL(head);

    if(list_length(head) < index + 1)
        return NULL; // wouw had ik niet aan gedacht xd
    head = head->next;  // sla sentinel node over.
    for(i = 0; i < index; i++)
        head = head->next;

    return head->value;
}

hashmap* hashmap_create()
{
    hashmap *result;

    result = (hashmap *) MEMmalloc(sizeof(hashmap));

    SET_HASH(result, NULL, NULL, NULL);

    return result;
}

int hashmap_free(hashmap *map)
{
    hashmap *tmp;

    RETURN_NULL(map);

    while(map) {
        tmp = map;
        map = map->next;
        tmp->next = NULL;
        MEMfree(tmp);
    }
    return 0;
}

int hashmap_add(hashmap *map, void *key, void *value)
{
    hashmap *new;

    RETURN_NULL(map)

    new = hashmap_create();

    SET_HASH(new, key, value, map->next);
    map->next = new;

    return 0;
}

int hashmap_remove(hashmap *map, void *key)
{
    hashmap *tmp;

    RETURN_NULL(map)

    while(map->next) {
        tmp = map->next;

        if(STReq(tmp->key, key)) {
            map->next = tmp->next;
            MEMfree(tmp);
            return 0;
        }
        map = map->next;
    }
    /* key not found in map */
    return -2;
}

int hashmap_contains(hashmap *map, void *key)
{
    RETURN_NULL(map);

    while((map = map->next))
        if(STReq(map->key, key))
            return 1;
    return 0;
}

void* hashmap_get(hashmap *map, void *key)
{
    RETURN_NNULL(map);

    while((map = map->next))
        if(STReq(map->key, key))
            return map->value;
    return NULL;
}

void hashmap_empty(hashmap *map)
{
    if(map == NULL)
        return;

    map->next = NULL;
}

void hashmap_print(hashmap *map)
{
    if(map == NULL)
        return;

    printf("{ ");
    while((map = map->next)) {
        printf("%s: ", (char *)map->key);
        printf("<%p>, ", map->value);
    }
    printf(" }\n");
}

int hashmap_is_empty(hashmap *map)
{
    RETURN_NULL(map);

    return map->next == NULL;
}

hashmap* hashmap_pop(hashmap *map)
{
    hashmap *tmp;

    RETURN_NNULL(map);
    RETURN_NNULL(map->next);

    tmp = map->next;
    map->next = tmp->next;
    tmp->next = NULL;

    return tmp;
}

hashmap* hashmap_pop_last(hashmap *map)
{
    hashmap *tmp;

    RETURN_NNULL(map);
    RETURN_NNULL(map->next);

    tmp = map->next;
    map->next = tmp->next;
    tmp->next = NULL;

    return tmp;
}
