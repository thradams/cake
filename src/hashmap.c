
#include <stdlib.h>
#include <string.h>
#include "hashmap.h"
#include "hash.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#if defined _MSC_VER && !defined __POCC__
#include <crtdbg.h>
#endif


void hashmap_remove_all(struct hash_map* map)
{
    if (map->table != NULL)
    {
        for (int i = 0; i < map->capacity; i++)
        {
            struct map_entry* pentry = map->table[i];

            while (pentry != NULL)
            {
                struct map_entry* p_entry_current = pentry;
                pentry = pentry->next;
                free(p_entry_current->key);
                free(p_entry_current);
            }
        }

        free(map->table);
        map->table = NULL;
        map->size = 0;
    }
}

void hashmap_destroy(struct hash_map* map)
{
    hashmap_remove_all(map);
}

struct type_tag_id* hashmap_find(struct hash_map* map, const char* key)
{
    if (map->table == NULL)
      return NULL;

    struct type_tag_id* p = NULL;

    unsigned int hash = stringhash(key);
    int index = hash % map->capacity;

    struct map_entry* pentry = map->table[index];

    for (; pentry != NULL; pentry = pentry->next)
    {
        if (pentry->hash == hash && strcmp(pentry->key, key) == 0) {
            p = pentry->p;
            break;
        }
    }

    return p;
}


struct type_tag_id* hashmap_remove(struct hash_map* map, const char* key)
{
    struct type_tag_id* p = 0;

    if (map->table != NULL)
    {
        unsigned int hash = stringhash(key);
        struct map_entry** preventry = &map->table[hash % map->capacity];
        struct map_entry* pentry = *preventry;

        for (; pentry != NULL; pentry = pentry->next)
        {
            if ((pentry->hash == hash) && (strcmp(pentry->key, key) == 0))
            {
                *preventry = pentry->next;
                p = pentry->p;
                free(pentry->key);
                free(pentry);
                break;
            }
            preventry = &pentry->next;
        }
    }

    return p;
}

int hashmap_set(struct hash_map* map, const char* key, struct type_tag_id* p_new)
{
    int result = 0;

    if (map->table == NULL)
    {
        if (map->capacity < 1) {
            map->capacity = 1000;
        }

        map->table = calloc(map->capacity, sizeof(map->table[0]));
    }

    if (map->table != NULL)
    {
        unsigned int hash = stringhash(key);
        int index = hash % map->capacity;

        struct map_entry* pentry = map->table[index];

        for (; pentry != NULL; pentry = pentry->next) {
            if (pentry->hash == hash && strcmp(pentry->key, key) == 0) {
                break;
            }
        }

        if (pentry == NULL)
        {
            pentry = calloc(1, sizeof(*pentry));
            pentry->hash = hash;
            pentry->p = p_new;
            pentry->key = strdup(key);
            pentry->next = map->table[index];
            map->table[index] = pentry;
            map->size++;
            result = 0;
        }
        else
        {
            result = 1;
            pentry->p = p_new;
        }
    }

    return result;
}
