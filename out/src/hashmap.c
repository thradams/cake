struct struct_entry12;
union _struct_tag_5 {
    unsigned int number;
    struct enum_specifier * p_enum_specifier;
    struct enumerator * p_enumerator;
    struct struct_or_union_specifier * p_struct_or_union_specifier;
    struct declarator * p_declarator;
    struct init_declarator * p_init_declarator;
    struct macro * p_macro;
    struct struct_entry * p_struct_entry;
};

struct enumerator2;
struct struct_entry;
struct macro11;
struct map_entry {
    struct map_entry * next;
    unsigned int hash;
    char * key;
    int   type;
    union _struct_tag_5  data;
};

struct enum_specifier;
struct struct_or_union_specifier;
struct macro;
struct init_declarator;
struct declarator;
struct enumerator7;
struct hash_map {
    struct map_entry ** table;
    int capacity;
    int size;
};

struct enum_specifier0;
struct hash_item_set {
    unsigned int number;
    struct enum_specifier6 * p_enum_specifier;
    struct enumerator7 * p_enumerator;
    struct struct_or_union_specifier8 * p_struct_or_union_specifier;
    struct declarator9 * p_declarator;
    struct init_declarator10 * p_init_declarator;
    struct macro11 * p_macro;
    struct struct_entry12 * p_struct_entry;
};

struct struct_or_union_specifier1;
struct init_declarator4;
struct enumerator;
struct init_declarator10;
struct enum_specifier6;
struct macro5;
struct struct_or_union_specifier8;
struct declarator9;
struct declarator3;

unsigned int strlen(char * _Str);

static unsigned int string_hash(char * key)
{
    unsigned int hash_val = 2166136261U;
    unsigned int first = 0;
    unsigned int last = (unsigned int)strlen(key);
    unsigned int stride = 1 + last / 10;
    for (; first < last; first = stride)
    {
        hash_val = 16777619U * hash_val ^ (unsigned int)key[first];
    }
    return (hash_val);
}

void enum_specifier_delete(struct enum_specifier0 * p);
void struct_or_union_specifier_delete(struct struct_or_union_specifier1 * p);
void enumerator_delete(struct enumerator2 * p);
void declarator_delete(struct declarator3 * p);
void init_declarator_delete(struct init_declarator4 * p);
void macro_delete(struct macro5 * p);
void free(void * ptr);

void map_entry_delete(struct map_entry * p)
{
    if (p == ((void *)0))
    {
        return;
    }
    switch (p->type)
    {
        case 0 :
        break;
        case 1 :
        enum_specifier_delete(p->data.p_enum_specifier);
        break;
        case 2 :
        struct_or_union_specifier_delete(p->data.p_struct_or_union_specifier);
        break;
        case 3 :
        enumerator_delete(p->data.p_enumerator);
        break;
        case 4 :
        declarator_delete(p->data.p_declarator);
        break;
        case 5 :
        init_declarator_delete(p->data.p_init_declarator);
        break;
        case 6 :
        macro_delete(p->data.p_macro);
        break;
    }
    free(p->key);
    free(p);
}

void hashmap_remove_all(struct hash_map * map)
{
    if (map->table != ((void *)0))
    {
        {
            int i = 0;
            for (; i < map->capacity; i++)
            {
                struct map_entry * pentry = map->table[i];
                while (pentry != ((void *)0))
                {
                    struct map_entry * next = pentry->next;
                    map_entry_delete(pentry);
                    pentry = next;
                }
            }
        }
        free(map->table);
        map->table = ((void *)0);
        map->size = 0;
    }
}

void hashmap_destroy(struct hash_map * map)
{
    hashmap_remove_all(map);
    ;
}

int strcmp(char * _Str1, char * _Str2);

struct map_entry *hashmap_find(struct hash_map * map, char * key)
{
    if (map->table == ((void *)0))
    {
        return ((void *)0);
    }
    unsigned int hash = string_hash(key);
    int index = hash % map->capacity;
    struct map_entry * pentry = map->table[index];
    for (; pentry != ((void *)0); pentry = pentry->next)
    {
        if (pentry->hash == hash && strcmp(pentry->key, key) == 0)
        {
            return pentry;
        }
    }
    return ((void *)0);
}

void *hashmap_remove(struct hash_map * map, char * key, int  * p_type_opt)
{
    if (map->table != ((void *)0))
    {
        unsigned int hash = string_hash(key);
        struct map_entry ** pp_entry = &map->table[hash % map->capacity];
        struct map_entry * p_entry =  *pp_entry;
        for (; p_entry != ((void *)0); p_entry = p_entry->next)
        {
            if ((p_entry->hash == hash) && (strcmp(p_entry->key, key) == 0))
            {
                 *pp_entry = p_entry->next;
                if (p_type_opt)
                {
                     *p_type_opt = p_entry->type;
                }
                void * p = p_entry->data.p_declarator;
                free((void *)p_entry->key);
                free((void *)p_entry);
                return p;
            }
            pp_entry = &p_entry->next;
        }
    }
    return ((void *)0);
}

void hash_item_set_destroy(struct hash_item_set * p)
{
    declarator_delete(p->p_declarator);
    enumerator_delete(p->p_enumerator);
    enum_specifier_delete(p->p_enum_specifier);
    init_declarator_delete(p->p_init_declarator);
    struct_or_union_specifier_delete(p->p_struct_or_union_specifier);
    macro_delete(p->p_macro);
}

void *calloc(int nmemb, unsigned int size);
char *strdup(char * src);

int hashmap_set(struct hash_map * map, char * key, struct hash_item_set * item)
{
    int result = 0;
    void * p = ((void *)0);
    int   type = 0;
    if (item->p_declarator)
    {
        type = 4;
        p = item->p_declarator;
        item->p_declarator = ((void *)0);
    }
    else
    {
        if (item->p_enumerator)
        {
            type = 3;
            p = item->p_enumerator;
            item->p_enumerator = ((void *)0);
        }
        else
        {
            if (item->p_enum_specifier)
            {
                type = 1;
                p = item->p_enum_specifier;
                item->p_enum_specifier = ((void *)0);
            }
            else
            {
                if (item->p_init_declarator)
                {
                    type = 5;
                    p = item->p_init_declarator;
                    item->p_init_declarator = ((void *)0);
                }
                else
                {
                    if (item->p_struct_or_union_specifier)
                    {
                        type = 2;
                        p = item->p_struct_or_union_specifier;
                        item->p_struct_or_union_specifier = ((void *)0);
                    }
                    else
                    {
                        if (item->p_macro)
                        {
                            type = 6;
                            p = item->p_macro;
                            item->p_macro = ((void *)0);
                        }
                        else
                        {
                            if (item->p_struct_entry)
                            {
                                type = 7;
                                p = item->p_struct_entry;
                                item->p_struct_entry = ((void *)0);
                            }
                            else
                            {
                                type = 0;
                                p = (void *)item->number;
                            }
                        }
                    }
                }
            }
        }
    }
    if (1)
    {
        if (map->table == ((void *)0))
        {
            if (map->capacity < 1)
            {
                map->capacity = 1000;
            }
            map->table = calloc(map->capacity, sizeof (map->table[0]));
            if (map->table == ((void *)0))
            {
                goto _catch_label_1;
            }
        }
        if (map->table != ((void *)0))
        {
            unsigned int hash = string_hash(key);
            int index = hash % map->capacity;
            struct map_entry * pentry = map->table[index];
            for (; pentry != ((void *)0); pentry = pentry->next)
            {
                if (pentry->hash == hash && strcmp(pentry->key, key) == 0)
                {
                    break;
                }
            }
            if (pentry == ((void *)0))
            {
                struct map_entry * p_new_entry = calloc(1, sizeof ( *pentry));
                if (p_new_entry == ((void *)0))
                {
                    goto _catch_label_1;
                }
                p_new_entry->hash = hash;
                p_new_entry->data.p_declarator = (void *)p;
                p_new_entry->type = type;
                char * temp_key = strdup(key);
                if (temp_key == ((void *)0))
                {
                    map_entry_delete(p_new_entry);
                    goto _catch_label_1;
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
                    case 0 :
                    break;
                    case 1 :
                    item->p_enum_specifier = pentry->data.p_enum_specifier;
                    break;
                    case 2 :
                    item->p_struct_or_union_specifier = pentry->data.p_struct_or_union_specifier;
                    break;
                    case 3 :
                    item->p_enumerator = pentry->data.p_enumerator;
                    break;
                    case 4 :
                    item->p_declarator = pentry->data.p_declarator;
                    break;
                    case 5 :
                    item->p_init_declarator = pentry->data.p_init_declarator;
                    break;
                    case 6 :
                    item->p_macro = pentry->data.p_macro;
                    break;
                    case 7 :
                    item->p_struct_entry = pentry->data.p_struct_entry;
                    break;
                }
                result = 1;
                pentry->data.p_declarator = (void *)p;
                pentry->type = type;
            }
        }
    }
    else _catch_label_1:
    {
    }
    return result;
}

