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

int hashmap_empty(hashmap *map)
{
    RETURN_NULL(map);

    map->next = NULL;

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
