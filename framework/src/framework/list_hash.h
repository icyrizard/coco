
#ifndef _LIST_HASH_H_
#define _LIST_HASH_H_

#define RETURN_NULL(a) if(!a) return -1;
#define RETURN_NNULL(a) if(!a) return NULL; /* FIXME macro name */
#define SET_LIST(l, v, n) l->value = v; l->next = n;
#define SET_HASH(h, k, v, n) h->key = k; h->value =v; h->next = n;

typedef struct list list;
typedef struct hashmap hashmap;

struct list {
    void *value;

    list *next;
};

struct hashmap {
    void *key;
    void *value;

    void *next;
};

list* list_create();
int list_free(list *head);
int list_empty(list *head);
int list_addtofront(list *head, void *value);
int list_addtoend(list *head, void *value);
int list_remove(list *head, void *value);
int list_length(list *head);
int list_contains(list *map, void *key);

hashmap* hashmap_create();
int hashmap_free(hashmap *map);
int hashmap_add(hashmap *map, void *key, void *value);
int hashmap_remove(hashmap *map, void *key);
int hashmap_contains(hashmap *map, void *key);
void* hashmap_get(hashmap *map, void *key);
int hashmap_empty(hashmap *map);
hashmap* hashmap_pop(hashmap *map);
hashmap* hashmap_pop_last(hashmap *map);

#endif
