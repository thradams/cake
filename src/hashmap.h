#pragma once
#include "ownership.h"
/*
* tag allow more than one type of object be inserted at the same map
*/
enum tag
{
    TAG_TYPE_NONE,
    
    TAG_TYPE_ENUN_SPECIFIER,
    TAG_TYPE_STRUCT_OR_UNION_SPECIFIER,

    TAG_TYPE_ENUMERATOR,
    TAG_TYPE_ONLY_DECLARATOR,
    TAG_TYPE_INIT_DECLARATOR,
};


struct map_entry {
    struct map_entry* owner next;
    unsigned int hash;
    char* owner key;
    void* p;
    enum tag type; /*type of the object pointed by p*/
};

struct hash_map {
    struct map_entry *owner * owner table;
    int capacity;
    int  size;
};

void hashmap_remove_all(struct hash_map* map);
void hashmap_destroy( struct hash_map* obj_owner map);
struct map_entry* hashmap_find(struct hash_map* map, const char* key);
void* hashmap_remove(struct hash_map* map, const char* key, enum tag* p_type_opt);
int hashmap_set(struct hash_map* map, const char* key, void* p, enum tag type);


struct owner_map_entry {
    struct owner_map_entry* owner next;
    unsigned int hash;
    char* owner key;
    void* owner p;
    enum tag type; /*type of the object pointed by p*/
};

struct owner_hash_map {
    struct owner_map_entry *owner * owner table;
    int capacity;
    int  size;
};

void owner_hashmap_remove_all(struct owner_hash_map* map, void (*pf)(void* owner ));
void owner_hashmap_destroy( struct owner_hash_map* obj_owner map, void (*pf)(void*));
struct owner_map_entry* owner_hashmap_find(struct owner_hash_map* map, const char* key);
void*  owner owner_hashmap_remove(struct owner_hash_map* map, const char* key, enum tag* p_type_opt);
int owner_hashmap_set(struct owner_hash_map* map, const char* key, void* owner p, enum tag type);

