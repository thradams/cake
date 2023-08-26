#include "ownership.h"
#include <stdlib.h>
#include <string.h>
#include "hashmap.h"
#include "hash.h"
#include <assert.h>
#ifdef _WIN32
#include <Windows.h>
#endif

#if defined _MSC_VER
#include <crtdbg.h>
#endif


void hashmap_remove_all(struct hash_map* map)
{
 
    if (map->table != NULL)
    {
        for (int i = 0; i < map->capacity; i++)
        {
            struct map_entry* owner pentry = map->table[i];

            while (pentry != NULL)
            {
                struct map_entry* owner next = pentry->next;                
                free(pentry->key);
                free(pentry);
                pentry = next;
            }
        }
 
        free(map->table);
        map->table = NULL;
        map->size = 0;
    }
}

void hashmap_destroy( struct hash_map* obj_owner map)
{
    hashmap_remove_all(map);
    assert(map->table == NULL);
}

struct map_entry* hashmap_find(struct hash_map* map, const char* key)
{
    if (map->table == NULL)
      return NULL;

    const unsigned int hash = string_hash(key);
    const int index = hash % map->capacity;

    struct map_entry* pentry = map->table[index];

    for (; pentry != NULL; pentry = pentry->next)
    {
        if (pentry->hash == hash && strcmp(pentry->key, key) == 0)
        {
            return pentry;
        }
    }

    return NULL;
}


void * view hashmap_remove(struct hash_map* map, const char* key, enum tag * p_type_opt)
{
    if (map->table != NULL)
    {
        const unsigned int hash = string_hash(key);
        struct map_entry* * pp_entry = &map->table[hash % map->capacity];
        struct map_entry* p_entry = *pp_entry;

        for (; p_entry != NULL; p_entry = p_entry->next)
        {
            if ((p_entry->hash == hash) && (strcmp(p_entry->key, key) == 0))
            {
                *pp_entry = p_entry->next;

                if (p_type_opt)
                    *p_type_opt = p_entry->type;

                void* view p = p_entry->p;
                free((void* owner)p_entry->key);
                free((void* owner)p_entry);
                
                return p;
            }
            pp_entry = &p_entry->next;
        }
    }

    return NULL;
}


int hashmap_set(struct hash_map* map, const char* key, void* view p, enum tag type)
{
    int result = 0;

    if (map->table == NULL)
    {
        if (map->capacity < 1)
        {
            map->capacity = 1000;
        }

        map->table = calloc(map->capacity, sizeof(map->table[0]));
    }

    if (map->table != NULL)
    {
        unsigned int hash = string_hash(key);
        int index = hash % map->capacity;

        struct map_entry* pentry = map->table[index];

        for (; pentry != NULL; pentry = pentry->next)
        {
            if (pentry->hash == hash && strcmp(pentry->key, key) == 0)
            {
                break;
            }
        }

        if (pentry == NULL)
        {
            struct map_entry* owner p_new_entry = calloc(1, sizeof(*pentry));
            p_new_entry->hash = hash;
            p_new_entry->p = p;
            p_new_entry->type = type;
            p_new_entry->key = strdup(key);
            p_new_entry->next = map->table[index];
            map->table[index] = p_new_entry;
            map->size++;
            result = 0;
        }
        else
        {
            result = 1;            
            pentry->p =  p;
            pentry->type = type;
        }
    }

    return result;
}


/////////////



void owner_hashmap_remove_all(struct owner_hash_map* map, void (*pf)(void* owner))
{
    if (map->table != NULL)
    {
        for (int i = 0; i < map->capacity; i++)
        {
            struct owner_map_entry* owner pentry = map->table[i];

            while (pentry != NULL)
            {
                struct owner_map_entry* owner next = pentry->next;                
                
                pf(pentry->p); //TODO

                free(pentry->key);
                free(pentry);

                pentry = next;
            }
        }

        free(map->table);
        map->table = NULL;
        map->size = 0;
    }
}

void owner_hashmap_destroy( struct owner_hash_map* obj_owner map, void (*pf)(void* owner))
{
    owner_hashmap_remove_all(map, pf );
    assert(map->table == NULL);
}

struct owner_map_entry* owner_hashmap_find(struct owner_hash_map* map, const char* key)
{
    if (map->table == NULL)
      return NULL;

    const unsigned int hash = string_hash(key);
    const int index = hash % map->capacity;

    struct owner_map_entry* pentry = map->table[index];

    for (; pentry != NULL; pentry = pentry->next)
    {
        if (pentry->hash == hash && strcmp(pentry->key, key) == 0)
        {
            return pentry;
        }
    }

    return NULL;
}


void * owner owner_hashmap_remove(struct owner_hash_map* map, const char* key, enum tag * p_type_opt)
{
    if (map->table != NULL)
    {
        const unsigned int hash = string_hash(key);
        struct owner_map_entry* * pp_entry = &map->table[hash % map->capacity];
        struct owner_map_entry* p_entry = *pp_entry;

        for (; p_entry != NULL; p_entry = p_entry->next)
        {
            if ((p_entry->hash == hash) && (strcmp(p_entry->key, key) == 0))
            {
                *pp_entry = p_entry->next;

                if (p_type_opt)
                    *p_type_opt = p_entry->type;

                void* owner p = p_entry->p;
                free(p_entry->key);
                free((void  * owner)p_entry);
                
                return p;
            }
            pp_entry = &p_entry->next;
        }
    }

    return NULL;
}


int owner_hashmap_set(struct owner_hash_map* map, const char* key, void* owner p, enum tag type)
{
    int result = 0;

    if (map->table == NULL)
    {
        if (map->capacity < 1)
        {
            map->capacity = 1000;
        }

        map->table = calloc(map->capacity, sizeof(map->table[0]));
    }

    if (map->table != NULL)
    {
        unsigned int hash = string_hash(key);
        int index = hash % map->capacity;

        struct owner_map_entry* pentry = map->table[index];

        for (; pentry != NULL; pentry = pentry->next)
        {
            if (pentry->hash == hash && strcmp(pentry->key, key) == 0)
            {
                break;
            }
        }

        if (pentry == NULL)
        {
            struct owner_map_entry* owner p_new_entry = calloc(1, sizeof(*pentry));
            p_new_entry->hash = hash;
            p_new_entry->p = p;
            p_new_entry->type = type;
            p_new_entry->key = strdup(key);
            p_new_entry->next = map->table[index];
            map->table[index] = p_new_entry;
            map->size++;
            result = 0;
        }
        else
        {
            result = 1;            
            pentry->p =  p;
            pentry->type = type;
        }
    }
    else
    {
        static_set(p, "moved"); //this is a leak actually
    }
    return result;
}

