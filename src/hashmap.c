
#include <stdlib.h>
#include <string.h>
#include "hashmap.h"
#include "hash.h"
#ifdef _WIN32 
#include <crtdbg.h>
#endif

extern void node_delete(struct type_tag_id* pNode);




void hashmap_remove_all(struct hash_map* pMap)
{
    if (pMap->table != NULL)
    {
        for (int i = 0; i < pMap->capacity; i++)
        {
            struct map_entry* pentry = pMap->table[i];

            while (pentry != NULL)
            {
                struct map_entry* pentryCurrent = pentry;

                //node_delete(pentryCurrent->p);

                pentry = pentry->next;
                free(pentryCurrent->key);
                free(pentryCurrent);
            }
        }

        free(pMap->table);
        pMap->table = NULL;
        pMap->size = 0;
    }
}

void hashmap_destroy(struct hash_map* pMap)
{
    hashmap_remove_all(pMap);
}

struct type_tag_id* hashmap_find(struct hash_map* pMap, const char* key)
{
    if (pMap->table == NULL)
      return NULL;

    struct type_tag_id* p = NULL;

    unsigned int hash = stringhash(key);
    int index = hash % pMap->capacity;

    struct map_entry* pentry = pMap->table[index];

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

int hashmap_set(struct hash_map* pMap, const char* key, struct type_tag_id* pNew)
{
    int result = 0;

    if (pMap->table == NULL)
    {
        if (pMap->capacity < 1) {
            pMap->capacity = 1000;
        }

        pMap->table = calloc(pMap->capacity, sizeof(pMap->table[0]));
    }

    if (pMap->table != NULL)
    {
        unsigned int hash = stringhash(key);
        int index = hash % pMap->capacity;

        struct map_entry* pentry = pMap->table[index];

        for (; pentry != NULL; pentry = pentry->next) {
            if (pentry->hash == hash && strcmp(pentry->key, key) == 0) {
                break;
            }
        }

        if (pentry == NULL)
        {
            pentry = calloc(1, sizeof(*pentry));
            pentry->hash = hash;
            pentry->p = pNew;
            pentry->key = strdup(key);
            pentry->next = pMap->table[index];
            pMap->table[index] = pentry;
            pMap->size++;
            result = 0;
        }
        else
        {
            result = 1;
            pentry->p = pNew;
        }
    }

    return result;
}
