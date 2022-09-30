#pragma once

/*coisas que vao em hash map possuim um id
assim é possivel usar o mesmo mapa para achar tipos diferentes
*/
enum tag
{
    TAG_TYPE_NONE,
    TAG_TYPE_ENUN_SPECIFIER,
    TAG_TYPE_STRUCT_OR_UNION_SPECIFIER,
    TAG_TYPE_ENUMERATOR,
    TAG_TYPE_DECLARATOR,
};


struct type_tag_id
{
    enum tag type;
};

struct map_entry {
    struct map_entry* next;
    unsigned int hash;
    char* key;
    struct type_tag_id* p;
};

struct hash_map {
    struct map_entry** table;
    int capacity;
    int  size;
};

void hashmap_remove_all(struct hash_map* pMap);
void hashmap_destroy(struct hash_map* pMap);
struct type_tag_id* hashmap_find(struct hash_map* pMap, const char* key);
struct type_tag_id* hashmap_remove(struct hash_map* map, const char* key);
int hashmap_set(struct hash_map* pMap, const char* key, struct type_tag_id* pNew);
