/* Cake 0.12.26 x86_msvc */
struct init_declarator;

struct struct_or_union_specifier;

struct enumerator;

struct enum_specifier;

struct declarator;

struct macro;
struct struct_entry;

struct hash_item_set {
    unsigned int number;
    struct enum_specifier * p_enum_specifier;
    struct enumerator * p_enumerator;
    struct struct_or_union_specifier * p_struct_or_union_specifier;
    struct declarator * p_declarator;
    struct init_declarator * p_init_declarator;
    struct macro * p_macro;
    struct struct_entry * p_struct_entry;
};

struct struct_entry;

struct macro;

union __tag5 {
    unsigned int number;
    struct enum_specifier * p_enum_specifier;
    struct enumerator * p_enumerator;
    struct struct_or_union_specifier * p_struct_or_union_specifier;
    struct declarator * p_declarator;
    struct init_declarator * p_init_declarator;
    struct macro * p_macro;
    struct struct_entry * p_struct_entry;
};

struct map_entry {
    struct map_entry * next;
    unsigned int hash;
    char * key;
    int type;
    union __tag5  data;
};

struct hash_map {
    struct map_entry ** table;
    int capacity;
    int size;
};


void enum_specifier_delete(struct enum_specifier * p);
void struct_or_union_specifier_delete(struct struct_or_union_specifier * p);
void enumerator_delete(struct enumerator * p);
void declarator_delete(struct declarator * p);
void init_declarator_delete(struct init_declarator * p);
void macro_delete(struct macro * p);
void struct_entry_delete(struct struct_entry * p);
void free(void * ptr);

void map_entry_delete(struct map_entry * p)
{
    if (p == 0)
    {
        return;
    }
    /*switch*/
    {
        int __v0 = p->type;
        if (__v0 == 0) goto __L2; /*case 0*/
        if (__v0 == 1) goto __L3; /*case 1*/
        if (__v0 == 2) goto __L4; /*case 2*/
        if (__v0 == 3) goto __L5; /*case 3*/
        if (__v0 == 4) goto __L6; /*case 4*/
        if (__v0 == 5) goto __L7; /*case 5*/
        if (__v0 == 6) goto __L8; /*case 6*/
        if (__v0 == 7) goto __L9; /*case 7*/
        goto __L1;

        {
            __L2: /*case 0*/ 
            goto __L1; /* break */

            __L3: /*case 1*/ 
            enum_specifier_delete(p->data.p_enum_specifier);
            goto __L1; /* break */

            __L4: /*case 2*/ 
            struct_or_union_specifier_delete(p->data.p_struct_or_union_specifier);
            goto __L1; /* break */

            __L5: /*case 3*/ 
            enumerator_delete(p->data.p_enumerator);
            goto __L1; /* break */

            __L6: /*case 4*/ 
            declarator_delete(p->data.p_declarator);
            goto __L1; /* break */

            __L7: /*case 5*/ 
            init_declarator_delete(p->data.p_init_declarator);
            goto __L1; /* break */

            __L8: /*case 6*/ 
            macro_delete(p->data.p_macro);
            goto __L1; /* break */

            __L9: /*case 7*/ 
            struct_entry_delete(p->data.p_struct_entry);
            goto __L1; /* break */

        }
        __L1:;
    }
    free(p->key);
    free(p);
}


void hashmap_remove_all(struct hash_map * map)
{
    if (map->table != 0)
    {
        {
            int i;

            i = 0;
            for (; i < map->capacity; i++)
            {
                struct map_entry * pentry;

                pentry = map->table[i];
                while (pentry != 0)
                {
                    struct map_entry * next;

                    next = pentry->next;
                    map_entry_delete(pentry);
                    pentry = next;
                }
            }
        }
        free(map->table);
        map->table = 0;
        map->size = 0;
    }
}


void hashmap_destroy(struct hash_map * map)
{
    hashmap_remove_all(map);
    ;
}


static unsigned int string_hash(char * key);
unsigned int __cdecl strlen(char * _Str);
int __cdecl strcmp(char * _Str1, char * _Str2);

struct map_entry *hashmap_find(struct hash_map * map, char * key)
{
    unsigned int hash;
    int index;
    struct map_entry * pentry;

    if (map->table == 0)
    {
        return 0;
    }
    hash = string_hash(key);
    index = hash % map->capacity;
    pentry = map->table[index];
    for (; pentry != 0; pentry = pentry->next)
    {
        if (pentry->hash == hash && strcmp(pentry->key, key) == 0)
        {
            return pentry;
        }
    }
    return 0;
}



static unsigned int string_hash(char * key)
{
    unsigned int hash_val;
    unsigned int first;
    unsigned int last;
    unsigned int stride;

    hash_val = 2166136261L;
    first = 0;
    last = (unsigned int)strlen(key);
    stride = 1 + last / 10;
    for (; first < last; first += stride)
    {
        hash_val = 16777619 * hash_val ^ (unsigned int)key[first];
    }
    return (hash_val);
}
void *hashmap_remove(struct hash_map * map, char * key, int * p_type_opt)
{
    if (map->table != 0)
    {
        unsigned int hash;
        struct map_entry ** pp_entry;
        struct map_entry * p_entry;

        hash = string_hash(key);
        pp_entry = &map->table[hash % map->capacity];
        p_entry = *pp_entry;
        for (; p_entry != 0; p_entry = p_entry->next)
        {
            if ((p_entry->hash == hash) && (strcmp(p_entry->key, key) == 0))
            {
                void * p;

                *pp_entry = p_entry->next;
                if (p_type_opt)
                {
                    *p_type_opt = p_entry->type;
                }
                p = p_entry->data.p_declarator;
                free((void *)p_entry->key);
                free((void *)p_entry);
                return p;
            }
            pp_entry = &p_entry->next;
        }
    }
    return 0;
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


void *calloc(unsigned int nmemb, unsigned int size);
char *strdup(char * src);

int hashmap_set(struct hash_map * map, char * key, struct hash_item_set * item)
{
    int result;
    void * p;
    int type;

    result = 0;
    p = 0;
    type = 0;
    if (item->p_declarator)
    {
        type = 4;
        p = item->p_declarator;
        item->p_declarator = 0;
    }
    else
    {
        if (item->p_enumerator)
        {
            type = 3;
            p = item->p_enumerator;
            item->p_enumerator = 0;
        }
        else
        {
            if (item->p_enum_specifier)
            {
                type = 1;
                p = item->p_enum_specifier;
                item->p_enum_specifier = 0;
            }
            else
            {
                if (item->p_init_declarator)
                {
                    type = 5;
                    p = item->p_init_declarator;
                    item->p_init_declarator = 0;
                }
                else
                {
                    if (item->p_struct_or_union_specifier)
                    {
                        type = 2;
                        p = item->p_struct_or_union_specifier;
                        item->p_struct_or_union_specifier = 0;
                    }
                    else
                    {
                        if (item->p_macro)
                        {
                            type = 6;
                            p = item->p_macro;
                            item->p_macro = 0;
                        }
                        else
                        {
                            if (item->p_struct_entry)
                            {
                                type = 7;
                                p = item->p_struct_entry;
                                item->p_struct_entry = 0;
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
    if (1) /*try*/
    {
        if (map->table == 0)
        {
            if (map->capacity < 1)
            {
                map->capacity = 1000;
            }
            map->table = calloc(map->capacity, 4);
            if (map->table == 0)
            {
                goto __L7; /* throw */
            }
        }
        if (map->table != 0)
        {
            unsigned int hash;
            int index;
            struct map_entry * pentry;

            hash = string_hash(key);
            index = hash % map->capacity;
            pentry = map->table[index];
            for (; pentry != 0; pentry = pentry->next)
            {
                if (pentry->hash == hash && strcmp(pentry->key, key) == 0)
                {
                    break;
                }
            }
            if (pentry == 0)
            {
                struct map_entry * p_new_entry;
                char * temp_key;

                p_new_entry = calloc(1, 20);
                if (p_new_entry == 0)
                {
                    goto __L7; /* throw */
                }
                p_new_entry->hash = hash;
                p_new_entry->data.p_declarator = (void *)p;
                p_new_entry->type = type;
                temp_key = strdup(key);
                if (temp_key == 0)
                {
                    map_entry_delete(p_new_entry);
                    goto __L7; /* throw */
                }
                p_new_entry->key = temp_key;
                p_new_entry->next = map->table[index];
                map->table[index] = p_new_entry;
                map->size++;
                result = 0;
            }
            else
            {
                /*switch*/
                {
                    int __v0 = pentry->type;
                    if (__v0 == 0) goto __L17; /*case 0*/
                    if (__v0 == 1) goto __L18; /*case 1*/
                    if (__v0 == 2) goto __L19; /*case 2*/
                    if (__v0 == 3) goto __L20; /*case 3*/
                    if (__v0 == 4) goto __L21; /*case 4*/
                    if (__v0 == 5) goto __L22; /*case 5*/
                    if (__v0 == 6) goto __L23; /*case 6*/
                    if (__v0 == 7) goto __L24; /*case 7*/
                    goto __L16;

                    {
                        __L17: /*case 0*/ 
                        goto __L16; /* break */

                        __L18: /*case 1*/ 
                        ;
                        item->p_enum_specifier = pentry->data.p_enum_specifier;
                        goto __L16; /* break */

                        __L19: /*case 2*/ 
                        ;
                        item->p_struct_or_union_specifier = pentry->data.p_struct_or_union_specifier;
                        goto __L16; /* break */

                        __L20: /*case 3*/ 
                        ;
                        item->p_enumerator = pentry->data.p_enumerator;
                        goto __L16; /* break */

                        __L21: /*case 4*/ 
                        ;
                        item->p_declarator = pentry->data.p_declarator;
                        goto __L16; /* break */

                        __L22: /*case 5*/ 
                        ;
                        item->p_init_declarator = pentry->data.p_init_declarator;
                        goto __L16; /* break */

                        __L23: /*case 6*/ 
                        ;
                        item->p_macro = pentry->data.p_macro;
                        goto __L16; /* break */

                        __L24: /*case 7*/ 
                        ;
                        item->p_struct_entry = pentry->data.p_struct_entry;
                        goto __L16; /* break */

                    }
                    __L16:;
                }
                result = 1;
                pentry->data.p_declarator = (void *)p;
                pentry->type = type;
            }
        }
    }
    else __L7: /*catch*/ 
    {
    }
    return result;
}


