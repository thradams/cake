/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake 
*/

#pragma once
#include "ownership.h"

struct declarator;
struct enumerator;
struct enum_specifier;
struct init_declarator;
struct struct_or_union_specifier;
struct macro;
struct struct_entry;

void declarator_delete(struct declarator* _Owner _Opt p);
void init_declarator_delete(struct init_declarator* _Owner _Opt p);

void enumerator_delete(struct enumerator* _Owner _Opt p);
void enum_specifier_delete(struct enum_specifier* _Owner _Opt p);
void struct_or_union_specifier_delete(struct struct_or_union_specifier* _Owner _Opt p);
void struct_entry_delete(struct struct_entry* _Opt _Owner p);

void macro_delete(struct macro* _Owner _Opt p);


/*
* tag allow more than one type of object be inserted at the same map
*/
enum tag
{
    TAG_TYPE_NUMBER,

    TAG_TYPE_ENUN_SPECIFIER,
    TAG_TYPE_STRUCT_OR_UNION_SPECIFIER,

    TAG_TYPE_ENUMERATOR,
    TAG_TYPE_DECLARATOR,
    TAG_TYPE_INIT_DECLARATOR,
    TAG_TYPE_MACRO,
    TAG_TYPE_STRUCT_ENTRY
};


struct map_entry 
{
    struct map_entry* _Owner _Opt next;
    unsigned int hash;
    char* _Owner key;

    enum tag type; /*type of the object pointed by p*/

    union 
    {
        size_t number;
        struct enum_specifier* _Opt _Owner p_enum_specifier;
        struct enumerator* _Opt _Owner p_enumerator;
        struct struct_or_union_specifier* _Opt _Owner p_struct_or_union_specifier;
        struct declarator* _Opt _Owner p_declarator;
        struct init_declarator* _Opt _Owner p_init_declarator;
        struct macro* _Opt _Owner p_macro;
        struct struct_entry* _Opt _Owner p_struct_entry;
    } data;
    
};

struct hash_map 
{
    struct map_entry* _Owner _Opt * _Owner _Opt table;
    int capacity;
    int  size;
};

void hashmap_remove_all(struct hash_map* map);
void hashmap_destroy(_Dtor struct hash_map* map);
struct map_entry* _Opt hashmap_find(struct hash_map* map, const char* key);
void* _Opt hashmap_remove(struct hash_map* map, const char* key, enum tag* _Opt p_type_opt);

/*
  hash_item_set is used to insert pointer with it type into a hashmap and also
  used to undo the map insertion using map and key info.
*/
struct hash_item_set
{
    size_t  number;
    struct enum_specifier* _Owner _Opt p_enum_specifier;
    struct enumerator* _Owner _Opt p_enumerator;
    struct struct_or_union_specifier* _Owner _Opt p_struct_or_union_specifier;
    struct declarator* _Owner _Opt p_declarator;
    struct init_declarator* _Owner _Opt p_init_declarator;
    struct macro* _Owner _Opt p_macro;
    struct struct_entry* _Owner _Opt p_struct_entry;
};
void hash_item_set_destroy(_Dtor struct hash_item_set* p);

int hashmap_set(struct hash_map* map, const char* key, struct hash_item_set * item);
