/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake 
*/

#pragma safety enable

#include "ownership.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "hashmap.h"

#include "error.h"
#include <assert.h>
#ifdef _WIN32
#include <Windows.h>
#endif

#if defined _MSC_VER
#include <crtdbg.h>
#endif

static unsigned int string_hash(const char* key)
{
    // hash key to unsigned int value by pseudorandomizing transform
    // (algorithm copied from STL char hash in xfunctional)
    unsigned int hash_val = 2166136261U;
    unsigned int first = 0;
    unsigned int last = (unsigned int)strlen(key);
    unsigned int stride = 1 + last / 10;

    for (; first < last; first += stride)
    {
        hash_val = 16777619U * hash_val ^ (unsigned int)key[first];
    }

    return (hash_val);
}


void map_entry_delete(struct map_entry* _Owner _Opt p)
{
    if (p == NULL)
        return;

    switch (p->type)
    {
    case TAG_TYPE_NUMBER:break;

    case TAG_TYPE_ENUN_SPECIFIER:
        enum_specifier_delete(p->data.p_enum_specifier);
        break;
    case TAG_TYPE_STRUCT_OR_UNION_SPECIFIER:
        struct_or_union_specifier_delete(p->data.p_struct_or_union_specifier);
        break;

    case TAG_TYPE_ENUMERATOR:
        enumerator_delete(p->data.p_enumerator);
        break;
    case TAG_TYPE_DECLARATOR:
        declarator_delete(p->data.p_declarator);
        break;
    case TAG_TYPE_INIT_DECLARATOR:
        init_declarator_delete(p->data.p_init_declarator);
        break;
    case TAG_TYPE_MACRO:
        macro_delete(p->data.p_macro);
        break;

    case TAG_TYPE_STRUCT_ENTRY:
        struct_entry_delete(p->data.p_struct_entry);
        break;
    }

    free(p->key);
    free(p);
}

void hashmap_remove_all(struct hash_map* map)
{

    if (map->table != NULL)
    {
        for (int i = 0; i < map->capacity; i++)
        {
            struct map_entry* _Owner _Opt pentry = map->table[i];

            while (pentry != NULL)
            {
                struct map_entry* _Owner _Opt next = pentry->next;
                map_entry_delete(pentry);
                pentry = next;
            }
        }

        free(map->table);
        map->table = NULL;
        map->size = 0;
    }
}

void hashmap_destroy(_Dtor struct hash_map* map)
{
    hashmap_remove_all(map);
    assert(map->table == NULL);
}

struct map_entry* _Opt hashmap_find(struct hash_map* map, const char* key)
{
    if (map->table == NULL)
        return NULL;

    const unsigned int hash = string_hash(key);
    const int index = hash % map->capacity;

    struct map_entry* _Opt pentry = map->table[index];

    for (; pentry != NULL; pentry = pentry->next)
    {
        if (pentry->hash == hash && strcmp(pentry->key, key) == 0)
        {
            return pentry;
        }
    }

    return NULL;
}


void* _Opt hashmap_remove(struct hash_map* map, const char* key, enum tag* _Opt p_type_opt)
{
    if (map->table != NULL)
    {
        const unsigned int hash = string_hash(key);
        struct map_entry** pp_entry = &map->table[hash % map->capacity];
        struct map_entry* _Opt p_entry = *pp_entry;

        for (; p_entry != NULL; p_entry = p_entry->next)
        {
            if ((p_entry->hash == hash) && (strcmp(p_entry->key, key) == 0))
            {
                *pp_entry = p_entry->next;

                if (p_type_opt)
                    *p_type_opt = p_entry->type;

                void* _Opt p = p_entry->data.p_declarator;
                free((void* _Owner)p_entry->key);
                free((void* _Owner)p_entry);

                return p;
            }
            pp_entry = &p_entry->next;
        }
    }

    return NULL;
}

void hash_item_set_destroy(_Dtor struct hash_item_set* p)
{
    declarator_delete(p->p_declarator);
    enumerator_delete(p->p_enumerator);
    enum_specifier_delete(p->p_enum_specifier);
    init_declarator_delete(p->p_init_declarator);
    struct_or_union_specifier_delete(p->p_struct_or_union_specifier);
    macro_delete(p->p_macro);

}

int hashmap_set(struct hash_map* map, const char* key, struct hash_item_set* item /*in out*/)
{
    int result = 0;

    void* _Opt p = NULL;
    enum tag type = TAG_TYPE_NUMBER;
    if (item->p_declarator)
    {
        type = TAG_TYPE_DECLARATOR;
        p = item->p_declarator;
        item->p_declarator = NULL;//

    }
    else if (item->p_enumerator)
    {
        type = TAG_TYPE_ENUMERATOR;
        p = item->p_enumerator;
        item->p_enumerator = NULL;

    }
    else if (item->p_enum_specifier)
    {
        type = TAG_TYPE_ENUN_SPECIFIER;
        p = item->p_enum_specifier;
        item->p_enum_specifier = NULL;

    }
    else if (item->p_init_declarator)
    {
        type = TAG_TYPE_INIT_DECLARATOR;
        p = item->p_init_declarator;
        item->p_init_declarator = NULL;

    }
    else if (item->p_struct_or_union_specifier)
    {
        type = TAG_TYPE_STRUCT_OR_UNION_SPECIFIER;
        p = item->p_struct_or_union_specifier;
        item->p_struct_or_union_specifier = NULL;

    }
    else if (item->p_macro)
    {
        type = TAG_TYPE_MACRO;
        p = item->p_macro;
        item->p_macro = NULL;
    }
    else if (item->p_struct_entry)
    {
        type = TAG_TYPE_STRUCT_ENTRY;
        p = item->p_struct_entry;
        item->p_struct_entry = NULL;
    }
    else //if (item->number)
    {
        type = TAG_TYPE_NUMBER;
        p = (void*)item->number;
    }
    // else
    // {
     //    assert(false);
     //}

    try
    {
        if (map->table == NULL)
        {
            if (map->capacity < 1)
            {
                map->capacity = 1000;
            }

            map->table = calloc(map->capacity, sizeof(map->table[0]));
            if (map->table == NULL) throw;
        }

        if (map->table != NULL)
        {
            unsigned int hash = string_hash(key);
            int index = hash % map->capacity;

            struct map_entry* _Opt pentry = map->table[index];

            for (; pentry != NULL; pentry = pentry->next)
            {
                if (pentry->hash == hash && strcmp(pentry->key, key) == 0)
                {
                    break;
                }
            }

            if (pentry == NULL)
            {
                struct map_entry* _Owner _Opt p_new_entry = calloc(1, sizeof(*pentry));
                if (p_new_entry == NULL) throw;

                p_new_entry->hash = hash;

                p_new_entry->data.p_declarator = (void*)p;

                p_new_entry->type = type;

                char* _Opt _Owner temp_key = strdup(key);
                if (temp_key == NULL)
                {
                    map_entry_delete(p_new_entry);
                    throw;
                }

                p_new_entry->key = temp_key;
                p_new_entry->next = map->table[index];
                map->table[index] = p_new_entry;
                map->size++;
                result = 0;
            }
            else
            {
                switch (pentry->type)
                {
                case TAG_TYPE_NUMBER:break;

                case TAG_TYPE_ENUN_SPECIFIER:
                    assert(pentry->data.p_enum_specifier != NULL);
                    item->p_enum_specifier = pentry->data.p_enum_specifier;
                    break;
                case TAG_TYPE_STRUCT_OR_UNION_SPECIFIER:
                    assert(pentry->data.p_struct_or_union_specifier != NULL);
                    item->p_struct_or_union_specifier = pentry->data.p_struct_or_union_specifier;
                    break;

                case TAG_TYPE_ENUMERATOR:
                    assert(pentry->data.p_enumerator != NULL);
                    item->p_enumerator = pentry->data.p_enumerator;
                    break;
                case TAG_TYPE_DECLARATOR:
                    assert(pentry->data.p_declarator != NULL);
                    item->p_declarator = pentry->data.p_declarator;
                    break;
                case TAG_TYPE_INIT_DECLARATOR:
                    assert(pentry->data.p_init_declarator != NULL);
                    item->p_init_declarator = pentry->data.p_init_declarator;
                    break;
                case TAG_TYPE_MACRO:
                    assert(pentry->data.p_macro != NULL);
                    item->p_macro = pentry->data.p_macro;
                    break;
                case TAG_TYPE_STRUCT_ENTRY:
                    assert(pentry->data.p_struct_entry != NULL);
                    item->p_struct_entry = pentry->data.p_struct_entry;
                    break;
                }

                result = 1;
                pentry->data.p_declarator = (void*)p;
                pentry->type = type;
            }
        }
    }
    catch
    {
    }
    return result;
}

