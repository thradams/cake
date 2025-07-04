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

struct hash_map {
    struct map_entry ** table;
    int capacity;
    int size;
};

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

struct map_entry {
    struct map_entry * next;
    unsigned int hash;
    char * key;
    int   type;
    union _struct_tag_5  data;
};


unsigned int __cdecl strlen(char * _Str);

static unsigned int string_hash(char * key)
{
    unsigned int hash_val;
    unsigned int first;
    unsigned int last;
    unsigned int stride;

    hash_val = 2166136261U;
    first = 0;
    last = (unsigned int)strlen(key);
    stride = 1 + last / 10;
    for (; first < last; first += stride)
    {
        hash_val = 16777619U * hash_val ^ (unsigned int)key[first];
    }
    return (hash_val);
}

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
    if (p == 0U)
    {
        return;
    }
    /*switch*/
    {
        register int   _R0 = p->type;
        if (_R0 == 0) goto _CKL2; /*case 0*/
        if (_R0 == 1) goto _CKL3; /*case 1*/
        if (_R0 == 2) goto _CKL4; /*case 2*/
        if (_R0 == 3) goto _CKL5; /*case 3*/
        if (_R0 == 4) goto _CKL6; /*case 4*/
        if (_R0 == 5) goto _CKL7; /*case 5*/
        if (_R0 == 6) goto _CKL8; /*case 6*/
        if (_R0 == 7) goto _CKL9; /*case 7*/
        goto _CKL1;

        {
            /*case 0*/ _CKL2:
            /*break*/ goto _CKL1;

            /*case 1*/ _CKL3:
            enum_specifier_delete(p->data.p_enum_specifier);
            /*break*/ goto _CKL1;

            /*case 2*/ _CKL4:
            struct_or_union_specifier_delete(p->data.p_struct_or_union_specifier);
            /*break*/ goto _CKL1;

            /*case 3*/ _CKL5:
            enumerator_delete(p->data.p_enumerator);
            /*break*/ goto _CKL1;

            /*case 4*/ _CKL6:
            declarator_delete(p->data.p_declarator);
            /*break*/ goto _CKL1;

            /*case 5*/ _CKL7:
            init_declarator_delete(p->data.p_init_declarator);
            /*break*/ goto _CKL1;

            /*case 6*/ _CKL8:
            macro_delete(p->data.p_macro);
            /*break*/ goto _CKL1;

            /*case 7*/ _CKL9:
            struct_entry_delete(p->data.p_struct_entry);
            /*break*/ goto _CKL1;

        }
        _CKL1:;
    }
    free(p->key);
    free(p);
}

void hashmap_remove_all(struct hash_map * map)
{
    if (map->table != 0U)
    {
        {
            int i;
            i = 0;
            for (; i < map->capacity; i++)
            {
                struct map_entry * pentry;

                pentry = map->table[i];
                while (pentry != 0U)
                {
                    struct map_entry * next;

                    next = pentry->next;
                    map_entry_delete(pentry);
                    pentry = next;
                }
            }
        }
        free(map->table);
        map->table = 0U;
        map->size = 0;
    }
}

void hashmap_destroy(struct hash_map * map)
{
    hashmap_remove_all(map);
    ;
}

int __cdecl strcmp(char * _Str1, char * _Str2);

struct map_entry *hashmap_find(struct hash_map * map, char * key)
{
    unsigned int hash;
    int index;
    struct map_entry * pentry;

    if (map->table == 0U)
    {
        return 0U;
    }
    hash = string_hash(key);
    index = hash % map->capacity;
    pentry = map->table[index];
    for (; pentry != 0U; pentry = pentry->next)
    {
        if (pentry->hash == hash && strcmp(pentry->key, key) == 0)
        {
            return pentry;
        }
    }
    return 0U;
}

void *hashmap_remove(struct hash_map * map, char * key, int  * p_type_opt)
{
    if (map->table != 0U)
    {
        unsigned int hash;
        struct map_entry ** pp_entry;
        struct map_entry * p_entry;

        hash = string_hash(key);
        pp_entry = &map->table[hash % map->capacity];
        p_entry = *pp_entry;
        for (; p_entry != 0U; p_entry = p_entry->next)
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
    return 0U;
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
    int   type;

    result = 0;
    p = 0U;
    type = 0;
    if (item->p_declarator)
    {
        type = 4;
        p = item->p_declarator;
        item->p_declarator = 0U;
    }
    else
    {
        if (item->p_enumerator)
        {
            type = 3;
            p = item->p_enumerator;
            item->p_enumerator = 0U;
        }
        else
        {
            if (item->p_enum_specifier)
            {
                type = 1;
                p = item->p_enum_specifier;
                item->p_enum_specifier = 0U;
            }
            else
            {
                if (item->p_init_declarator)
                {
                    type = 5;
                    p = item->p_init_declarator;
                    item->p_init_declarator = 0U;
                }
                else
                {
                    if (item->p_struct_or_union_specifier)
                    {
                        type = 2;
                        p = item->p_struct_or_union_specifier;
                        item->p_struct_or_union_specifier = 0U;
                    }
                    else
                    {
                        if (item->p_macro)
                        {
                            type = 6;
                            p = item->p_macro;
                            item->p_macro = 0U;
                        }
                        else
                        {
                            if (item->p_struct_entry)
                            {
                                type = 7;
                                p = item->p_struct_entry;
                                item->p_struct_entry = 0U;
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
    /*try*/ if (1)
    {
        if (map->table == 0U)
        {
            if (map->capacity < 1)
            {
                map->capacity = 1000;
            }
            map->table = calloc(map->capacity, 4U);
            if (map->table == 0U)
            {
                /*throw*/ goto _CKL7;
            }
        }
        if (map->table != 0U)
        {
            unsigned int hash;
            int index;
            struct map_entry * pentry;

            hash = string_hash(key);
            index = hash % map->capacity;
            pentry = map->table[index];
            for (; pentry != 0U; pentry = pentry->next)
            {
                if (pentry->hash == hash && strcmp(pentry->key, key) == 0)
                {
                    break;
                }
            }
            if (pentry == 0U)
            {
                struct map_entry * p_new_entry;
                char * temp_key;

                p_new_entry = calloc(1, 20U);
                if (p_new_entry == 0U)
                {
                    /*throw*/ goto _CKL7;
                }
                p_new_entry->hash = hash;
                p_new_entry->data.p_declarator = (void *)p;
                p_new_entry->type = type;
                temp_key = strdup(key);
                if (temp_key == 0U)
                {
                    map_entry_delete(p_new_entry);
                    /*throw*/ goto _CKL7;
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
                    register int   _R1 = pentry->type;
                    if (_R1 == 0) goto _CKL17; /*case 0*/
                    if (_R1 == 1) goto _CKL18; /*case 1*/
                    if (_R1 == 2) goto _CKL19; /*case 2*/
                    if (_R1 == 3) goto _CKL20; /*case 3*/
                    if (_R1 == 4) goto _CKL21; /*case 4*/
                    if (_R1 == 5) goto _CKL22; /*case 5*/
                    if (_R1 == 6) goto _CKL23; /*case 6*/
                    if (_R1 == 7) goto _CKL24; /*case 7*/
                    goto _CKL16;

                    {
                        /*case 0*/ _CKL17:
                        /*break*/ goto _CKL16;

                        /*case 1*/ _CKL18:
                        ;
                        item->p_enum_specifier = pentry->data.p_enum_specifier;
                        /*break*/ goto _CKL16;

                        /*case 2*/ _CKL19:
                        ;
                        item->p_struct_or_union_specifier = pentry->data.p_struct_or_union_specifier;
                        /*break*/ goto _CKL16;

                        /*case 3*/ _CKL20:
                        ;
                        item->p_enumerator = pentry->data.p_enumerator;
                        /*break*/ goto _CKL16;

                        /*case 4*/ _CKL21:
                        ;
                        item->p_declarator = pentry->data.p_declarator;
                        /*break*/ goto _CKL16;

                        /*case 5*/ _CKL22:
                        ;
                        item->p_init_declarator = pentry->data.p_init_declarator;
                        /*break*/ goto _CKL16;

                        /*case 6*/ _CKL23:
                        ;
                        item->p_macro = pentry->data.p_macro;
                        /*break*/ goto _CKL16;

                        /*case 7*/ _CKL24:
                        ;
                        item->p_struct_entry = pentry->data.p_struct_entry;
                        /*break*/ goto _CKL16;

                    }
                    _CKL16:;
                }
                result = 1;
                pentry->data.p_declarator = (void *)p;
                pentry->type = type;
            }
        }
    }
    /*catch*/ else _CKL7:
    {
    }
    return result;
}

