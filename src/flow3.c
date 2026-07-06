#pragma safety enable

#include "ownership.h"
#include <assert.h>
#include <string.h>
#include "flow3.h"
#include "expressions.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include "console.h"
#include <stdio.h>
#include "osstream.h"

enum
{
    ANY_VALUE = 0xCAFE,
    UNINITIALIZED_VALUE = 0xBAD,
};

enum flow3_relation
{
    FLOW3_UNINITIALIZED = 0, // value == trash
    FLOW3_EQUAL,             // == value
    FLOW3_NOT_EQUAL,         // != value
    FLOW3_ANY,               // value >= TYPE_MIN and value <= TYPE_MAX    
};

enum flow3_value_kind
{
    FLOW3_VALUE_SIGNED = 0, /* signed long long .i  (default, zero-init safe) */
    FLOW3_VALUE_UNSIGNED,    /* unsigned long long .u                           */
    FLOW3_VALUE_PTR,     /* struct object*   .p  (concrete arena pointer)  */
    FLOW3_VALUE_REF,     /* struct object*   .p  (reference — always non-null) */

};

struct flow3_value
{
    enum flow3_value_kind kind;
    union {
        unsigned long long u;
        signed long long   i;
        const struct object* p;
    } as;
};

enum flow3_imaginary
{
    FLOW3_IMAGINARY_NONE = 0,   // no imaginary state (default, zero-init safe)
    FLOW3_IMAGINARY_MOVED,      // target was moved elsewhere, target is alive
    FLOW3_IMAGINARY_ENDED,      // this object's lifetime ended
    FLOW3_IMAGINARY_ABSENT,     // no object is present
};

struct flow3_alternative
{
    struct flow3_value value;
    enum flow3_relation relation;
    enum flow3_imaginary imaginary;   // MOVED, ENDED, or ABSENT (or NONE)

    const struct flow3_map* origin;   /* which map arm set this value */
    int                line;     /* source line for reference */
};

struct flow3_alternatives
{
    struct flow3_alternative* data;
    int                 size;
    int                 capacity;
};

struct flow3_key_alternatives
{
    const struct object* p_obj_key; /*key*/
    struct flow3_alternatives alternatives;
    struct flow3_key_alternatives* next;
};

struct flow3_map
{
    struct flow3_key_alternatives** buckets;  /* NULL until first write (lazily allocated) */
    int num_of_buckets;
    struct flow3_map* p_parent_map;
    const char* name; /* for debugging */
};

struct flow3_branch_pair
{
    struct flow3_map* p_true;
    struct flow3_map* p_false;
};

struct object_set
{
    const struct object** items;
    int             size;
    int             capacity;
};

static void object_set_add(struct object_set* l, const struct object* obj);
static void object_set_destroy(struct object_set* l);

static void flow3_check_dianostic_suppression(struct flow3_visit_ctx* ctx, struct token* p_token);

static void flow3_visit_unlabeled_statement(struct flow3_visit_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement);
static void flow3_visit_static_assertion(struct flow3_visit_ctx* ctx, struct static_assertion* p_static_assertion);
static void flow3_visit_declaration(struct flow3_visit_ctx* ctx, struct declaration* p_declaration);
static void flow3_visit_secondary_block(struct flow3_visit_ctx* ctx, struct secondary_block* p_secondary_block);
static void flow3_visit_struct_or_union_specifier(struct flow3_visit_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier);
static void flow3_visit_statement(struct flow3_visit_ctx* ctx, struct statement* p_statement);
static void flow3_visit_enum_specifier(struct flow3_visit_ctx* ctx, struct enum_specifier* p_enum_specifier);
static void flow3_visit_type_specifier(struct flow3_visit_ctx* ctx, struct type_specifier* p_type_specifier);
static void flow3_visit_bracket_initializer_list(struct flow3_visit_ctx* ctx, struct braced_initializer* p_bracket_initializer_list);
static void flow3_visit_expression_statement(struct flow3_visit_ctx* ctx, struct expression_statement* p_expression_statement);
static void flow3_visit_block_item(struct flow3_visit_ctx* ctx, struct block_item* p_block_item);
static void flow3_visit_initializer(struct flow3_visit_ctx* ctx, struct initializer* p_initializer);
static void flow3_visit_declarator(struct flow3_visit_ctx* ctx, struct declarator* p_declarator);
static void flow3_visit_label(struct flow3_visit_ctx* ctx, struct label* p_label);

static struct flow3_branch_pair flow3_visit_full_expression(struct flow3_visit_ctx* ctx, struct expression* p_expression);

static void flow3_check_file_scope_objects_at_function_exit(struct flow3_visit_ctx* ctx, const struct marker* marker);

static struct flow3_branch_pair flow3_visit_expression(struct flow3_visit_ctx* ctx, const struct expression* p_expression);
static void object_static_debug(struct flow3_visit_ctx* ctx, const struct object* p_object, struct token* first_token, struct token* last_token);

static void flow3_check_object_at_exit(struct flow3_visit_ctx* ctx, const struct type* p_type, const struct object* p_obj, const struct marker* marker, const struct token* p_exit_token);
static void flow3_check_arena_objects_at_function_exit(struct flow3_visit_ctx* ctx);

enum init_type
{
    INIT_PARAMETER,
    INIT_RETURN,
    INIT_OBJ
};

static void flow3_check_object_init_assigment(struct flow3_visit_ctx* ctx,
    struct expression* p_expression,
     const struct object* p_object_dest, //uninitialized alawys
     const struct object* p_object_src,
    enum init_type init_type);

static void object_set_add(struct object_set* l, const struct object* obj)
{
    try
    {
        /*linear search*/
        for (int i = 0; i < l->size; i++)
        {
            if (l->items[i] == obj)
            {
                return;
            }
        }
        if (l->size == l->capacity)
        {
            int new_capacity = l->capacity ? l->capacity * 2 : 8;
            const struct object** p = realloc(l->items, new_capacity * sizeof(struct object*));
            if (p == NULL)
            {
                throw;
            }
            l->items = p;
            l->capacity = new_capacity;
        }
        l->items[l->size++] = obj;
    }
    catch {
    }
}

static void object_set_destroy(struct object_set* l)
{
    free(l->items);
    l->items = NULL;
    l->size = 0;
    l->capacity = 0;
}

static bool object_is_file_scope(const struct object* p_object)
{
    const enum storage_class_specifier_flags flags =
        p_object->type.storage_class_specifier_flags;

    if (flags & (STORAGE_SPECIFIER_PARAMETER | STORAGE_SPECIFIER_BLOCK_SCOPE))
    {
        return false;
    }

    if (flags & (STORAGE_SPECIFIER_EXTERN | STORAGE_SPECIFIER_STATIC))
    {
        return true;
    }

    return false;
}

static long long flow3_cast_integer_value(struct flow3_visit_ctx* ctx, long long value, const struct type* target_type)
{
    if (target_type == NULL) return value;
    if (!type_is_integer(target_type)) return value;


    size_t width = 1;
    enum sizeof_result r = type_get_sizeof(target_type, &width, ctx->ctx->options.target) * 8;
    if (r != SIZEOF_RESULT_OK)
        return 0;

    if (width >= (int)(sizeof(long long) * 8))
    {
        /* Target can represent the full range of long long, no change. */
        return value;
    }

    unsigned long long mask = (width == 64) ? ~0ULL : ((1ULL << width) - 1);
    unsigned long long uval = (unsigned long long)value & mask;

    if (type_is_unsigned_integer(target_type))
    {
        return (long long)uval;
    }
    else
    {
        /* Signed: interpret as width-bit two's complement. */
        long long sign_bit = 1LL << (width - 1);
        if (uval & sign_bit)
        {
            return (long long)(uval - (1ULL << width));
        }
        else
        {
            return (long long)uval;
        }
    }
}

static inline bool flow3_value_is_same(struct flow3_value a, struct flow3_value b)
{
    if (a.kind != b.kind)
    {
        return false;
    }
    switch (a.kind)
    {
    case FLOW3_VALUE_SIGNED:  return a.as.i == b.as.i;
    case FLOW3_VALUE_UNSIGNED: return a.as.u == b.as.u;
    case FLOW3_VALUE_PTR:  return a.as.p == b.as.p;
    case FLOW3_VALUE_REF:  return a.as.p == b.as.p;
    }
    return false;
}

static bool flow3_value_is_true(struct flow3_value a)
{
    switch (a.kind)
    {
    case FLOW3_VALUE_SIGNED:  return a.as.i != 0;
    case FLOW3_VALUE_UNSIGNED: return a.as.u != 0;
    case FLOW3_VALUE_PTR:  return a.as.p != 0;
    case FLOW3_VALUE_REF:
        //assert(false);
        break;
    }
    return false;
}

static bool flow3_value_is_false(struct flow3_value a)
{
    switch (a.kind)
    {
    case FLOW3_VALUE_SIGNED:  return a.as.i == 0;
    case FLOW3_VALUE_UNSIGNED: return a.as.u == 0;
    case FLOW3_VALUE_PTR:  return a.as.p == 0;
    case FLOW3_VALUE_REF:
        //r/eturn false;
        break;
    }
    return false;
}

static unsigned int flow3_hash_key(const struct object* obj)
{
    uintptr_t p = (uintptr_t)obj;
    return (unsigned int)((p ^ (p >> 5)) % 64);
}

static void flow3_alternatives_clear(struct flow3_alternatives* vs);

static void flow3_map_remove(struct flow3_map* m, const struct object* obj)
{
    if (!m->buckets)
    {
        return;
    }

    unsigned int idx = flow3_hash_key(obj);
    struct flow3_key_alternatives** pp = &m->buckets[idx];
    while (*pp)
    {
        struct flow3_key_alternatives* e = *pp;
        if (e->p_obj_key == obj)
        {
            *pp = e->next;
            flow3_alternatives_clear(&e->alternatives);
            free(e);
            return;
        }
        pp = &e->next;
    }
}

static void flow3_map_delete(struct flow3_map* _Owner _Opt m);

void flow3_map_arena_clear(struct flow3_map_arena* a)
{
    for (int i = 0; i < a->size; i++)
    {
        flow3_map_delete(a->data[i]);
    }

    free(a->data);
    a->data = NULL;
    a->size = 0;
    a->capacity = 0;
}

struct flow3_map* _Owner _Opt flow3_map_arena_new(struct flow3_map_arena* a, struct flow3_map* parent, const char* name)
{
    try
    {
        if (a->data == NULL)
        {
            const int initial_capacity = 10;
            a->data = calloc(initial_capacity, sizeof(struct flow3_map*));
            if (a->data == NULL)
            {
                throw;
            }
            a->size = 0;
            a->capacity = initial_capacity;
        }

        if (a->size == a->capacity)
        {
            int new_capacity = a->capacity == 0 ? 4 : a->capacity * 2;
            struct flow3_map** new_data = realloc(a->data, new_capacity * sizeof(struct flow3_map*));
            if (new_data == NULL)
            {
                throw;
            }
            a->data = new_data;
            a->capacity = new_capacity;
        }

        struct flow3_map* obj = calloc(1, sizeof(struct flow3_map));
        if (obj == NULL)
        {
            throw;
        }
        obj->p_parent_map = parent;
        obj->name = name;

        a->data[a->size++] = obj;
        return obj;
    }
    catch {
        return NULL;
    }
}

void flow3_allocated_object_arena_clear(struct flow3_allocated_object_arena* a)
{
    for (int i = 0; i < a->size; i++)
    {
        object_delete(a->data[i]);
    }
    free(a->data);

    a->data = NULL;
    a->size = 0;
    a->capacity = 0;
}

struct object* flow3_allocated_object_arena_new(struct flow3_allocated_object_arena* a)
{
    try
    {
        if (a->data == NULL)
        {
            const int initial_capacity = 10;
            a->data = malloc(initial_capacity * sizeof(struct object*));
            if (a->data == NULL)
            {
                throw;
            }
            a->size = 0;
            a->capacity = initial_capacity;
        }

        if (a->size == a->capacity)
        {
            int new_capacity = a->capacity == 0 ? 4 : a->capacity * 2;
            struct object** new_data = realloc(a->data, new_capacity * sizeof(struct object*));
            if (new_data == NULL)
            {
                throw;
            }
            a->data = new_data;
            a->capacity = new_capacity;
        }

        struct object* obj = calloc(1, sizeof(struct object));
        if (obj == NULL)
        {
            throw;
        }

        a->data[a->size++] = obj;
        return obj;
    }
    catch
    {
        return NULL;
    }
}

static void flow3_alternatives_add(struct flow3_alternatives* vs, const struct flow3_alternative* p_alternative, const struct flow3_map* origin, int line)
{
    try
    {
        for (int i = 0; i < vs->size; i++)
        {
            if (flow3_value_is_same(vs->data[i].value, p_alternative->value) &&
                vs->data[i].relation == p_alternative->relation &&
                vs->data[i].imaginary == p_alternative->imaginary &&
                vs->data[i].origin == origin)
            {
                return;
            }
        }

        if (vs->size == vs->capacity)
        {
            int new_capacity = vs->capacity ? vs->capacity * 2 : 4;
            struct flow3_alternative* p = realloc(vs->data, new_capacity * sizeof(struct flow3_alternative));
            if (p == NULL)
            {
                throw;
            }
            vs->data = p;
            vs->capacity = new_capacity;
        }

        memset(&vs->data[vs->size], 0, sizeof(vs->data[vs->size]));
        vs->data[vs->size].value = p_alternative->value;
        vs->data[vs->size].relation = p_alternative->relation;
        vs->data[vs->size].imaginary = p_alternative->imaginary;
        vs->data[vs->size].origin = origin;
        vs->data[vs->size].line = line;
        vs->size++;
    }
    catch
    {
    }
}

static void flow3_alternatives_add_object_ptr(struct flow3_alternatives* vs, const struct object* p_pointed, enum flow3_relation relation, enum flow3_imaginary imaginary, const struct flow3_map* origin, int line)
{
    struct flow3_alternative a = { .value = {.kind = FLOW3_VALUE_PTR, .as = {.p = p_pointed } }, .relation = relation, .imaginary = imaginary };
    flow3_alternatives_add(vs, &a, origin, line);
}

static void flow3_alternatives_add_object_ref(struct flow3_alternatives* vs, const struct object* p_pointed, enum flow3_imaginary imaginary, const struct flow3_map* origin, int line)
{
    struct flow3_alternative a = { .value = {.kind = FLOW3_VALUE_REF, .as = {.p = p_pointed } }, .relation = FLOW3_EQUAL, .imaginary = imaginary };
    flow3_alternatives_add(vs, &a, origin, line);
}

static void flow3_alternatives_add_i(struct flow3_alternatives* vs, signed long long i, enum flow3_relation relation, enum flow3_imaginary imaginary, const struct flow3_map* origin, int line)
{
    struct flow3_alternative a = { .value = {.kind = FLOW3_VALUE_SIGNED, .as = {.i = i } }, .relation = relation, .imaginary = imaginary };
    flow3_alternatives_add(vs, &a, origin, line);
}

static void flow3_alternatives_add_does_not_exist(struct flow3_alternatives* vs, const struct flow3_map* origin, int line)
{
    try
    {
        for (int i = 0; i < vs->size; i++)
        {
            if (vs->data[i].imaginary == FLOW3_IMAGINARY_ABSENT &&
                vs->data[i].origin == origin)
            {
                return;
            }
        }

        if (vs->size == vs->capacity)
        {
            int new_capacity = vs->capacity ? vs->capacity * 2 : 4;
            struct flow3_alternative* p = realloc(vs->data, new_capacity * sizeof(struct flow3_alternative));
            if (p == NULL)
            {
                throw;
            }
            vs->data = p;
            vs->capacity = new_capacity;
        }

        /* zero-init the slot first so relation/value/flags are clean */
        memset(&vs->data[vs->size], 0, sizeof(vs->data[vs->size]));
        vs->data[vs->size].imaginary = FLOW3_IMAGINARY_ABSENT;
        vs->data[vs->size].relation = FLOW3_EQUAL;
        vs->data[vs->size].origin = origin;
        vs->data[vs->size].line = line;
        vs->size++;
    }
    catch {
    }
}

static void flow3_alternatives_append(struct flow3_alternatives* dst, const struct flow3_alternatives* src)
{
    for (int i = 0; i < src->size; i++)
    {
        flow3_alternatives_add(dst,
                               &src->data[i],
                               src->data[i].origin,
                               src->data[i].line);
    }
}

static void flow3_alternatives_clear(struct flow3_alternatives* vs)
{
    free(vs->data);
    vs->data = NULL;
    vs->size = 0;
    vs->capacity = 0;
}

static struct flow3_key_alternatives* flow3_map_find(const struct flow3_map* m, const struct object* obj)
{
    if (!m->buckets)
    {
        return NULL;
    }

    unsigned int idx = flow3_hash_key(obj);
    for (struct flow3_key_alternatives* e = m->buckets[idx]; e; e = e->next)
    {
        if (e->p_obj_key == obj)
        {
            return e;
        }
    }
    return NULL;
}

static struct flow3_key_alternatives* flow3_map_search_up(struct flow3_map* m, const struct object* obj)
{
    while (m)
    {
        struct flow3_key_alternatives* p = flow3_map_find(m, obj);
        if (p)
        {
            return p;
        }
        m = m->p_parent_map;
    }
    return NULL;
}

static struct flow3_key_alternatives* flow3_map_find_add(struct flow3_map* m, const struct object* obj)
{
    try
    {
        if (!m->buckets)
        {
            m->num_of_buckets = 64;
            m->buckets = calloc(m->num_of_buckets, sizeof(struct flow3_key_alternatives*));
            if (m->buckets == NULL)
            {
                throw;
            }
        }

        struct flow3_key_alternatives* e = flow3_map_find(m, obj);
        if (e)
        {
            return e;
        }

        unsigned int idx = flow3_hash_key(obj);
        e = calloc(1, sizeof(struct flow3_key_alternatives));
        if (e == NULL)
        {
            throw;
        }
        e->p_obj_key = obj;
        e->next = m->buckets[idx];
        m->buckets[idx] = e;
        return e;
    }
    catch
    {
        return NULL;
    }
}

static void flow3_map_replace_alternatives(struct flow3_map* m, const struct object* dest, struct flow3_value value, enum flow3_relation state, int line)
{
    struct flow3_key_alternatives* e = flow3_map_find_add(m, dest);
    flow3_alternatives_clear(&e->alternatives);
    struct flow3_alternative a = { .value = value, .relation = state, .imaginary = FLOW3_IMAGINARY_NONE };
    flow3_alternatives_add(&e->alternatives, &a, m, line);
}

static void flow3_map_replace_alternatives_i(struct flow3_map* m, const struct object* dest, signed long long i, enum flow3_relation state, int line)
{
    struct flow3_value v = { .kind = FLOW3_VALUE_SIGNED, .as = {.i = i } };
    flow3_map_replace_alternatives(m, dest, v, state, line);
}

static void flow3_map_replace_alternatives_ref(struct flow3_map* m, const struct object* dest, const struct object* ptr, enum flow3_relation state, int line)
{
    struct flow3_value v = { .kind = FLOW3_VALUE_REF, .as = {.p = ptr } };
    flow3_map_replace_alternatives(m, dest, v, state, line);
}

static void flow3_map_set_object_moved(struct flow3_map* m, const struct object* obj, int line)
{
    if (obj->members.head)
    {
        for (const struct object* member = obj->members.head; member; member = member->next)
            flow3_map_set_object_moved(m, member, line);
        return;
    }
    struct flow3_key_alternatives* e = flow3_map_find_add(m, obj);
    if (e == NULL) return;
    if (e->alternatives.size == 0)
    {
        flow3_alternatives_add_i(&e->alternatives, ANY_VALUE, FLOW3_ANY, FLOW3_IMAGINARY_MOVED, m, line);
    }
    else
    {
        for (int i = 0; i < e->alternatives.size; i++)
        {
            e->alternatives.data[i].imaginary = FLOW3_IMAGINARY_MOVED;
        }
    }
}

static void flow3_map_set_object_zero(struct flow3_map* m, const struct object* obj, int line)
{
    if (obj->members.head)
    {
        for (const struct object* member = obj->members.head; member; member = member->next)
        {
            flow3_map_set_object_zero(m, member, line);
        }
        return;
    }
    struct flow3_key_alternatives* p_flow3_key_alternatives = flow3_map_find_add(m, obj);
    if (p_flow3_key_alternatives == NULL)
    {
        return;
    }
    flow3_alternatives_clear(&p_flow3_key_alternatives->alternatives);
    flow3_alternatives_add_i(&p_flow3_key_alternatives->alternatives, 0, FLOW3_EQUAL, FLOW3_IMAGINARY_NONE, m, line);
}

static void flow3_map_set_object_any(struct flow3_map* m, const struct object* obj, int line)
{
    if (obj->members.head)
    {
        for (const struct object* member = obj->members.head; member; member = member->next)
        {
            flow3_map_set_object_any(m, member, line);
        }
        return;
    }
    struct flow3_key_alternatives* p_flow3_key_alternatives = flow3_map_find_add(m, obj);
    if (p_flow3_key_alternatives == NULL)
    {
        return;
    }
    flow3_alternatives_clear(&p_flow3_key_alternatives->alternatives);
    flow3_alternatives_add_i(&p_flow3_key_alternatives->alternatives, ANY_VALUE, FLOW3_ANY, FLOW3_IMAGINARY_NONE, m, line);
}

static void flow3_map_set_object_lifetime_ended(struct flow3_map* m, const struct object* obj, int line)
{
    if (obj->members.head)
    {
        for (const struct object* member = obj->members.head; member; member = member->next)
        {
            flow3_map_set_object_lifetime_ended(m, member, line);
        }
        return;
    }
    struct flow3_key_alternatives* e = flow3_map_find_add(m, obj);
    if (e == NULL)
    {
        return;
    }
    if (e->alternatives.size == 0)
    {
        flow3_alternatives_add_i(&e->alternatives, UNINITIALIZED_VALUE, FLOW3_UNINITIALIZED, FLOW3_IMAGINARY_NONE, m, line);
    }


    for (int i = 0; i < e->alternatives.size; i++)
    {
        e->alternatives.data[i].imaginary = FLOW3_IMAGINARY_ENDED;
    }
}

static void flow3_map_clear(struct flow3_map* m)
{
    if (m->buckets == NULL)
        return;

    for (int i = 0; i < m->num_of_buckets; i++)
    {
        struct flow3_key_alternatives* e = m->buckets[i];
        while (e)
        {
            struct flow3_key_alternatives* n = e->next;
            flow3_alternatives_clear(&e->alternatives);
            free(e);
            e = n;
        }
    }
    free(m->buckets);
    m->buckets = NULL;
}

static void flow3_map_delete(struct flow3_map* m)
{
    if (m == NULL)
        return;

    flow3_map_clear(m);
    free(m);
}

static bool flow3_is_ancestor_or_equal(const struct flow3_map* ancestor, const struct flow3_map* descendant)
{
    for (const struct flow3_map* cur = descendant; cur; cur = cur->p_parent_map)
    {
        if (cur == ancestor)
        {
            return true;
        }
    }
    return false;
}

static void flow3_map_move_entries(struct flow3_map* dest, struct flow3_map* src)
{
    if (src->buckets == NULL)
        return;

    for (int i = 0; i < src->num_of_buckets; i++)
    {
        struct flow3_key_alternatives* e = src->buckets[i];
        while (e)
        {
            struct flow3_key_alternatives* next = e->next;
            struct flow3_key_alternatives* d = flow3_map_find_add(dest, e->p_obj_key);
            if (d)
            {
                flow3_alternatives_clear(&d->alternatives);
                d->alternatives = e->alternatives;
                /* Zero out e's alternatives so clear doesn't double-free */
                memset(&e->alternatives, 0, sizeof(e->alternatives));
                /* Remove e from src by unlinking it */
                /* (Note: e is still in the list but its alternatives are gone) */
            }
            e = next;
        }
        src->buckets[i] = NULL;
    }
}

static bool flow3_map_arm_has_entries(const struct flow3_map* arm, const struct flow3_map* parent)
{
    for (const struct flow3_map* cur = arm; cur && cur != parent; cur = cur->p_parent_map)
    {
        if (cur->buckets == NULL)
        {
            continue;
        }

        for (int i = 0; i < cur->num_of_buckets; i++)
        {
            if (cur->buckets[i])
            {
                return true;
            }
        }

    }
    return false;
}

static void flow3_map_merge_a_b(struct flow3_map* parent, const struct flow3_map* a, const struct flow3_map* b)
{
    /* Write into a temporary map parented to parent so pre-branch
       reads are clean and don't see our own partial writes. */
    struct flow3_map temp = { .p_parent_map = parent, .name = "merge-temp" };

    const struct flow3_map* arms[] = { a, b };
    struct object_set objs = { 0 };

    for (int i = 0; i < 2; i++)
    {
        for (const struct flow3_map* cur = arms[i]; cur && cur != parent; cur = cur->p_parent_map)
        {
            if (cur->buckets == NULL)
            {
                continue;
            }

            for (int j = 0; j < cur->num_of_buckets; j++)
            {
                for (struct flow3_key_alternatives* e = cur->buckets[j]; e; e = e->next)
                {
                    object_set_add(&objs, e->p_obj_key);
                }
            }
        }
    }

    for (int i = 0; i < objs.size; i++)
    {
        const struct object* obj = objs.items[i];

        struct flow3_key_alternatives* p_temp_entry = flow3_map_find_add(&temp, obj);
        flow3_alternatives_clear(&p_temp_entry->alternatives);

        for (int j = 0; j < 2; j++)
        {
            struct flow3_key_alternatives* p_entry = NULL;
            for (const struct flow3_map* cur = arms[j]; cur && cur != parent; cur = cur->p_parent_map)
            {
                p_entry = flow3_map_find(cur, obj);
                if (p_entry && p_entry->alternatives.size > 0)
                {
                    break;
                }
                p_entry = NULL;
            }

            if (p_entry)
            {
                flow3_alternatives_append(&p_temp_entry->alternatives, &p_entry->alternatives);
            }
            else if (flow3_map_arm_has_entries(arms[j], parent))
            {
                /* Arm was reachable but didn't write this variable:
                   inherit the pre-branch value from parent, but re-tag each
                   alternative's origin as arms[j] (this arm's map) instead of
                   keeping the ancestor's origin. This is required so that the
                   correlation filter in flow3_map_transfer_true_false_set can
                   distinguish values that arrived via the true path from those
                   that arrived via the false path. For example:
                     if (a) { b = 1; }        // b==1 gets origin=true_branch
                     if (a) { static_debug(b); }  // must see only b==1
                   Without the re-tag, b==0 would keep origin=root, which is an
                   ancestor of true_branch, and would incorrectly survive the filter. */
                struct flow3_key_alternatives* p_pre_entry = flow3_map_search_up(parent, obj);
                if (p_pre_entry)
                {
                    for (int k = 0; k < p_pre_entry->alternatives.size; k++)
                    {
                        const struct flow3_alternative* a2 = &p_pre_entry->alternatives.data[k];
                        flow3_alternatives_add(&p_temp_entry->alternatives,
                            a2,
                            arms[j],   /* re-tag: value flowed through THIS arm */
                            a2->line);
                    }
                }
            }
            /* else: arm was never reached (e.g. empty break-join) — skip it entirely. */
        }
    }

    object_set_destroy(&objs);

    /* Move all merged results from temp into parent, then clean up temp. */
    flow3_map_move_entries(parent, &temp);
    flow3_map_clear(&temp);
}

static void print_object_ptr(struct osstream* ss, const struct object* p, bool is_parent)
{
    static const char digits[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    uint64_t value = (uint64_t)(uintptr_t)p;

    if (value == 0)
    {
        ss_fprintf(ss, "null");
        return;
    }

    char temp[16];
    int i = 0;

    while (value > 0 && i < (int)sizeof(temp))
    {
        temp[i++] = digits[value % 62];
        value /= 62;
    }

    if (!is_parent)
    {
        ss_fprintf(ss, "'");
    }
    if (p->parent)
    {
        print_object_ptr(ss, p->parent, true);
    }


    while (i > 0)
    {
        ss_fprintf(ss, "%c", temp[--i]);
    }

    ss_fprintf(ss, ":");
    if (p->member_designator)
    {
        ss_fprintf(ss, "%s", p->member_designator);
    }

    if (!is_parent)
    {
        ss_fprintf(ss, "'");
    }
}

static void flow3_print_object_ptr(const struct object* p)
{
    struct osstream ss = { 0 };
    print_object_ptr(&ss, p, false);
    fputs(ss.c_str ? ss.c_str : "", stdout);
    ss_close(&ss);
}

static void flow3_alternative_sprint(struct osstream* ss, const struct flow3_alternative* alt)
{
    switch (alt->relation)
    {
    case FLOW3_EQUAL:         ss_fprintf(ss, "== ");     break;
    case FLOW3_NOT_EQUAL:     ss_fprintf(ss, "!= ");     break;
    case FLOW3_ANY:           ss_fprintf(ss, "ANY ");    break;
    case FLOW3_UNINITIALIZED: ss_fprintf(ss, "UNINIT "); break;
    }

    switch (alt->value.kind)
    {
    case FLOW3_VALUE_SIGNED:
    {
        if (alt->value.as.i == UNINITIALIZED_VALUE ||
            alt->value.as.i == ANY_VALUE)
        {
            //ss_fprintf(ss, "%llx", alt->value.as.i);
        }
        else
        {
            ss_fprintf(ss, "%lld", alt->value.as.i);
        }
    }
    break;
    case FLOW3_VALUE_UNSIGNED:
    {
        ss_fprintf(ss, "%lluu", alt->value.as.u);
    }
    break;

    case FLOW3_VALUE_PTR:
    {
        print_object_ptr(ss, alt->value.as.p, false);
        break;
    }
    case FLOW3_VALUE_REF:
    {
        ss_fprintf(ss, "ref ");
        print_object_ptr(ss, alt->value.as.p, false);
        break;
    }
    }

    if (alt->imaginary == FLOW3_IMAGINARY_MOVED)
    {
        ss_fprintf(ss, " MOVED");
    }

    if (alt->imaginary == FLOW3_IMAGINARY_ENDED)
    {
        ss_fprintf(ss, " ENDED");
    }
    if (alt->imaginary == FLOW3_IMAGINARY_ABSENT)
    {
        ss_fprintf(ss, "ABSENT");
    }

    ss_fprintf(ss, " line %d", alt->line);

    if (alt->origin)
    {
        ss_fprintf(ss, " \"%s\"", alt->origin->name ? alt->origin->name : "\"\"");
    }
}

static void flow3_alternative_print(const struct flow3_alternative* alt)
{
    struct osstream ss = { 0 };
    flow3_alternative_sprint(&ss, alt);
    printf("%s", ss.c_str);
    ss_close(&ss);
}

static void flow3_alternatives_print(const struct flow3_alternatives* alternatives)
{
    for (int i = 0; i < alternatives->size; i++)
    {
        printf("  ");
        flow3_alternative_print(&alternatives->data[i]);
    }
}

static void flow3_map_entry_print_compact(const struct flow3_key_alternatives* entry)
{
    for (; entry; entry = entry->next)
    {
        printf("  ");
        flow3_print_object_ptr((void*)entry->p_obj_key);
        flow3_alternatives_print(&entry->alternatives);
        printf("\n");
    }
}

static void flow3_map_debug_print(const struct flow3_map* map, int indent)
{
    if (!map)
    {
        printf("(null map)\n");
        return;
    }

    const char* name = map->name ? map->name : "?";
    printf("\"%s\"", name);


    if (map->p_parent_map)
    {
        const char* parent = map->p_parent_map && map->p_parent_map->name ? map->p_parent_map->name : "null";
        printf(" ^ \"%s\"", parent);
    }

    printf("\n");

    if (!map->buckets)
    {
        printf("  (empty)\n");
        return;
    }

    for (int b = 0; b < map->num_of_buckets; b++)
    {
        if (map->buckets[b])
        {
            flow3_map_entry_print_compact(map->buckets[b]);
        }
    }
}

static bool flow3_alternative_is_zero(const struct flow3_alternative* alt)
{
    return alt->relation == FLOW3_EQUAL && flow3_value_is_false(alt->value);
}

static bool flow3_alternative_is_false(const struct flow3_alternative* alt)
{
    return flow3_alternative_is_zero(alt);
}

static bool flow3_alternative_is_true(const struct flow3_alternative* alt)
{
    return (alt->relation == FLOW3_NOT_EQUAL && flow3_value_is_false(alt->value)) ||
        (alt->relation == FLOW3_EQUAL && !flow3_alternative_is_false(alt));
}

static bool flow3_alternative_can_be_zero(const struct flow3_alternative* alt)
{
    if (flow3_alternative_is_zero(alt))
    {
        return true;
    }
    if (alt->relation == FLOW3_ANY)
    {
        return true;
    }
    /* NOT_EQUAL with a PTR value means "pointer is this non-null object" — cannot be null. */
    if (alt->relation == FLOW3_NOT_EQUAL && alt->value.kind == FLOW3_VALUE_PTR)
    {
        return false;
    }
    /* NOT_EQUAL integer/unsigned nonzero: e.g. !=2 doesn't exclude 0. */
    if (alt->relation == FLOW3_NOT_EQUAL && flow3_value_is_true(alt->value))
    {
        return true;
    }
    return false;
}

static void flow3_narrow_map_into(struct flow3_map* p_dest, struct flow3_map* p_before, const struct object* p_obj_key, bool true_branch)
{
    struct flow3_key_alternatives* p_existing = flow3_map_search_up(p_before, p_obj_key);
    if (p_existing == NULL || p_existing->alternatives.size == 0)
    {
        return;
    }

    /* Collect origin maps that survive on this branch. */
    struct object_set surviving_origins = { 0 };
    for (int k = 0; k < p_existing->alternatives.size; k++)
    {
        struct flow3_alternative* alt = &p_existing->alternatives.data[k];
        bool survives = false;

        if (alt->relation == FLOW3_ANY || alt->relation == FLOW3_NOT_EQUAL)
        {
            survives = true;
        }
        else if (alt->relation == FLOW3_UNINITIALIZED)
        {
            survives = true;
        }
        else if (alt->relation == FLOW3_EQUAL)
        {
            if (flow3_value_is_false(alt->value) && !true_branch)
            {
                survives = true;
            }
            if (flow3_value_is_true(alt->value) && true_branch)
            {
                survives = true;
            }
        }
        else
        {
            if (true_branch)
            {
                survives = true;
            }
        }

        if (survives && alt->origin)
        {
            object_set_add(&surviving_origins, (const struct object*)alt->origin);
        }
    }

    /* Filter correlated variables by surviving origins. */
    if (surviving_origins.size > 0)
    {
        for (const struct flow3_map* cur = p_before; cur; cur = cur->p_parent_map)
        {
            if (!cur->buckets)
            {
                continue;
            }
            for (int b = 0; b < cur->num_of_buckets; b++)
            {
                for (struct flow3_key_alternatives* e = cur->buckets[b]; e; e = e->next)
                {
                    if (e->p_obj_key == p_obj_key)
                    {
                        continue;
                    }
                    if (flow3_map_find(p_dest, e->p_obj_key) != NULL)
                    {
                        continue;
                    }

                    struct flow3_alternatives filtered = { 0 };
                    for (int k = 0; k < e->alternatives.size; k++)
                    {
                        struct flow3_alternative* alt = &e->alternatives.data[k];
                        bool keep = false;

                        if (alt->relation == FLOW3_UNINITIALIZED)
                        {
                            keep = true;
                        }
                        else
                        {
                            for (int s = 0; s < surviving_origins.size; s++)
                            {
                                struct flow3_map* origin_map = (struct flow3_map*)surviving_origins.items[s];
                                if (flow3_is_ancestor_or_equal(alt->origin, origin_map) ||
                                    flow3_is_ancestor_or_equal(origin_map, alt->origin))
                                {
                                    keep = true;
                                    break;
                                }
                            }
                        }

                        if (keep)
                        {
                            flow3_alternatives_add(&filtered,
                                alt, alt->origin, alt->line);
                        }
                    }

                    if (filtered.size > 0 && filtered.size < e->alternatives.size)
                    {
                        struct flow3_key_alternatives* p_dest_entry = flow3_map_find_add(p_dest, e->p_obj_key);
                        flow3_alternatives_clear(&p_dest_entry->alternatives);
                        p_dest_entry->alternatives = filtered;
                    }
                    else
                    {
                        flow3_alternatives_clear(&filtered);
                    }
                }
            }
        }
    }

    /* Write the narrowed variable itself into p_dest. */
    struct flow3_key_alternatives* p_dest_entry = flow3_map_find_add(p_dest, p_obj_key);
    flow3_alternatives_clear(&p_dest_entry->alternatives);

    for (int k = 0; k < p_existing->alternatives.size; k++)
    {
        struct flow3_alternative* alt = &p_existing->alternatives.data[k];

        if (alt->relation == FLOW3_ANY)
        {
            if (true_branch)
            {
                flow3_alternatives_add_i(&p_dest_entry->alternatives, 0, FLOW3_NOT_EQUAL, FLOW3_IMAGINARY_NONE, p_dest, alt->line);
            }
            else
            {
                flow3_alternatives_add_i(&p_dest_entry->alternatives, 0, FLOW3_EQUAL, FLOW3_IMAGINARY_NONE, p_dest, alt->line);
            }
            continue;
        }

        if (alt->relation == FLOW3_NOT_EQUAL && flow3_value_is_false(alt->value))
        {
            if (true_branch)
            {
                flow3_alternatives_add_i(&p_dest_entry->alternatives, 0, FLOW3_NOT_EQUAL, FLOW3_IMAGINARY_NONE, p_dest, alt->line);
            }
            else
            {
                flow3_alternatives_add_i(&p_dest_entry->alternatives, 0, FLOW3_EQUAL, FLOW3_IMAGINARY_NONE, p_dest, alt->line);
            }
            continue;
        }

        bool keep = false;
        if (alt->relation == FLOW3_EQUAL)
        {
            if (flow3_value_is_false(alt->value) && !true_branch)
            {
                keep = true;
            }
            if (flow3_value_is_true(alt->value) && true_branch)
            {
                keep = true;
            }
        }
        else if (alt->relation == FLOW3_UNINITIALIZED)
        {
            keep = false;
        }
        else if (alt->relation == FLOW3_NOT_EQUAL)
        {
            if (true_branch)
            {
                struct flow3_alternative a = { .value = alt->value, .relation = FLOW3_NOT_EQUAL, .imaginary = alt->imaginary };
                flow3_alternatives_add(&p_dest_entry->alternatives, &a, p_dest, alt->line);
            }
            else
            {
                struct flow3_alternative a = { .value = alt->value, .relation = FLOW3_EQUAL, .imaginary = alt->imaginary };
                flow3_alternatives_add(&p_dest_entry->alternatives, &a, p_dest, alt->line);
            }
            continue;
        }
        else
        {
            if (true_branch)
            {
                keep = true;
            }
        }

        if (keep)
        {
            flow3_alternatives_add(&p_dest_entry->alternatives, alt, alt->origin, alt->line);
        }
    }

    if (p_dest_entry->alternatives.size == 0)
    {
        for (int k = 0; k < p_existing->alternatives.size; k++)
        {
            if (p_existing->alternatives.data[k].relation == FLOW3_UNINITIALIZED)
            {
                flow3_alternatives_add(&p_dest_entry->alternatives,
                    &p_existing->alternatives.data[k],
                    p_existing->alternatives.data[k].origin,
                    p_existing->alternatives.data[k].line);
            }
        }
    }

    object_set_destroy(&surviving_origins);
}

static struct flow3_map* flow3_narrow_map(struct flow3_map_arena* arena, struct flow3_map* p_before, const struct object* p_obj_key, bool true_branch, const char* name)
{
    struct flow3_map* p_dest = flow3_map_arena_new(arena, p_before, name);
    flow3_narrow_map_into(p_dest, p_before, p_obj_key, true_branch);
    return p_dest;
}

static void flow3_exit_block_visit_defer_item(struct flow3_visit_ctx* ctx, const struct defer_list_item* p_item, const struct token* position_token)
{
    if (p_item->defer_statement)
    {
        const int error_count = ctx->ctx->p_report->error_count;
        const int warnings_count = ctx->ctx->p_report->warnings_count;
        const int info_count = ctx->ctx->p_report->info_count;

        flow3_visit_unlabeled_statement(ctx, p_item->defer_statement->unlabeled_statement);

        if (error_count != ctx->ctx->p_report->error_count ||
            warnings_count != ctx->ctx->p_report->warnings_count ||
            info_count != ctx->ctx->p_report->info_count)
        {
            diagnostic(W_LOCATION, ctx->ctx, position_token, NULL, "defer end of scope");
        }
    }
    else if (p_item->declarator)
    {
        struct declarator* p_declarator = p_item->declarator;
        struct token* p_token = NULL;
        if (p_declarator->name_opt)
            p_token = p_declarator->name_opt;
        else if (p_declarator->specifier_qualifier_list)
            p_token = p_declarator->specifier_qualifier_list->first_token;
        else if (p_declarator->declaration_specifiers)
            p_token = p_declarator->declaration_specifiers->first_token;
        else
            assert(false);
        const struct marker marker = { .p_token_begin = p_token, .p_token_end = p_token };
        flow3_check_object_at_exit(ctx,
            &p_declarator->type,
            &p_declarator->object,
            &marker,
        position_token);
    }
}

static void flow3_exit_block_visit_defer_list(struct flow3_visit_ctx* ctx,
    const struct defer_list* p_defer_list,
    const struct token* position_token)
{
    struct defer_list_item* _Opt p_item = p_defer_list->head;
    while (p_item)
    {
        flow3_exit_block_visit_defer_item(ctx, p_item, position_token);
        p_item = p_item->next;
    }
}

static void flow3_defer_item_set_end_of_lifetime(struct flow3_visit_ctx* ctx, struct defer_list_item* p_item, struct token* position_token)
{
    if (p_item->defer_statement)
    {
        /* defer statements are executable blocks, not objects — no lifetime to end. */
    }
    else if (p_item->declarator)
    {
        struct declarator* p_declarator = p_item->declarator;
        const int line = position_token ? position_token->line : 0;
        flow3_map_set_object_lifetime_ended(ctx->p_current_flow3_map,
                                            &p_declarator->object,
                                            line);
    }
}

static void flow3_defer_list_set_end_of_lifetime(struct flow3_visit_ctx* ctx,
    const struct defer_list* p_defer_list,
    struct token* position_token)
{
    struct defer_list_item* _Opt p_item = p_defer_list->head;
    while (p_item)
    {
        flow3_defer_item_set_end_of_lifetime(ctx, p_item, position_token);
        p_item = p_item->next;
    }
}

static void flow3_visit_secondary_block(struct flow3_visit_ctx* ctx, struct secondary_block* p_secondary_block)
{
    flow3_visit_statement(ctx, p_secondary_block->statement);
}

static void flow3_visit_defer_statement(struct flow3_visit_ctx* ctx, struct defer_statement* p_defer_statement)
{
    /*
      We are not going to visit the secondary block here because
      this is not the place were defer is executed.
    */
}

static void flow3_object_init(struct flow3_visit_ctx* ctx, struct object* p_object, const struct type* p_type, int line)
{
    if (p_object->members.head)
    {
        flow3_map_replace_alternatives_ref(ctx->p_current_flow3_map,
         p_object,
         p_object,
         FLOW3_EQUAL,
         line);

        p_object = p_object->members.head;
        for (; p_object; p_object = p_object->next)
        {
            flow3_object_init(ctx, p_object, &p_object->type, line);
        }

        return;
    }

    enum flow3_relation relation = FLOW3_UNINITIALIZED;

    struct flow3_value value = { 0 };
    if (type_is_pointer(&p_object->type))
    {
        value.kind = FLOW3_VALUE_PTR;
        value.as.p = (void*)(uintptr_t)p_object->value.host_u_long_long;
    }
    else if (type_is_signed(&p_object->type))
    {
        value.kind = FLOW3_VALUE_SIGNED;
        value.as.i = p_object->value.host_long_long;
    }
    else
    {
        value.kind = FLOW3_VALUE_UNSIGNED;
        value.as.u = p_object->value.host_u_long_long;
    }

    switch (p_object->state)
    {
    case CONSTANT_VALUE_STATE_UNINITIALIZED:
        relation = FLOW3_UNINITIALIZED;
        break;

    case CONSTANT_VALUE_STATE_CONSTANT:
    case CONSTANT_VALUE_EQUAL:
        relation = FLOW3_EQUAL;
        break;

    case CONSTANT_VALUE_STATE_ANY:
        relation = FLOW3_ANY;
        break;
    }

    flow3_map_replace_alternatives(ctx->p_current_flow3_map, p_object, value, relation, line);
}

static void flow3_parameter_object_init(struct flow3_visit_ctx* ctx, struct object* p_object, const struct type* p_type, int line)
{
    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    if (p_object->members.head)
    {
        flow3_map_replace_alternatives_ref(ctx->p_current_flow3_map,
         p_object,
         p_object,
         FLOW3_EQUAL,
         line);

        p_object = p_object->members.head;
        for (; p_object; p_object = p_object->next)
        {
            flow3_parameter_object_init(ctx, p_object, &p_object->type, line);
        }
        return;
    }

    enum flow3_relation relation = FLOW3_UNINITIALIZED;

    struct flow3_value value = { 0 };
    if (type_is_pointer(&p_object->type))
    {
        value.kind = FLOW3_VALUE_PTR;
        value.as.p = NULL;
    }
    else if (type_is_signed(&p_object->type))
    {
        value.kind = FLOW3_VALUE_SIGNED;
        value.as.i = p_object->value.host_long_long;
    }
    else
    {
        value.kind = FLOW3_VALUE_UNSIGNED;
        value.as.u = p_object->value.host_u_long_long;
    }

    switch (p_object->state)
    {
    case CONSTANT_VALUE_STATE_UNINITIALIZED:
        relation = FLOW3_UNINITIALIZED;
        break;

    case CONSTANT_VALUE_STATE_CONSTANT:
    case CONSTANT_VALUE_EQUAL:
        relation = FLOW3_EQUAL;
        break;

    case CONSTANT_VALUE_STATE_ANY:
        relation = FLOW3_ANY;
        break;
    }


    /*parameter are any by default*/
    relation = FLOW3_ANY;

    /*
     * Spec — pointer parameter initial state:
     *
     *   nullable_enabled + non-_Opt pointer
     *     => assumed not null (NOT_EQUAL 0) with an arena object as the
     *        concrete alias.  The pointed object starts as ANY.
     *
     *   nullable_enabled + _Opt pointer  (handled below)
     *     => two correlated alternatives: null and non-null.
     *
     *   nullable disabled
     *     => ANY (conservative; no null-check enforcement).
     */
    if (nullable_enabled &&
        p_type != NULL &&
        type_is_pointer(p_type) &&
        !type_is_opt(p_type, nullable_enabled))
    {
        /* Non-optional pointer: assume non-null.  Allocate an arena object
           to carry the concrete alias so the analyser can track the pointed
           state (e.g. _Ctor initialisation checks). */
        struct object* p_pointed = flow3_allocated_object_arena_new(&ctx->allocated_object_arena);
        if (p_pointed != NULL)
        {
            struct type pointed_type = type_remove_pointer(p_type);
            make_object(&pointed_type, p_pointed, MAKE_STATE_ANY, ctx->ctx->options.target);
            type_destroy(&pointed_type);
        }

        struct flow3_key_alternatives* ep = flow3_map_find_add(ctx->p_current_flow3_map, p_object);
        flow3_alternatives_clear(&ep->alternatives);
        /* Single EQUAL alternative carrying the concrete arena pointer.
         * The parameter is non-null on entry and points exactly to this object. */
        if (p_pointed != NULL)
        {
            flow3_alternatives_add_object_ptr(&ep->alternatives, p_pointed,
                FLOW3_EQUAL, FLOW3_IMAGINARY_NONE, ctx->p_current_flow3_map, line);
        }
        else
        {
            flow3_alternatives_add_object_ptr(&ep->alternatives, NULL,
                FLOW3_NOT_EQUAL, FLOW3_IMAGINARY_NONE, ctx->p_current_flow3_map, line);
        }

        if (p_pointed != NULL)
        {
            /*
             * _Ctor parameter: the pointed object is uninitialized on entry —
             * that is the whole purpose of the constructor.  Seed every _Owner
             * member (and scalar leaves) as UNINITIALIZED so constructor
             * writes are treated as first initialization.
             *
             * Non-_Ctor parameter: seed as ANY (unknown but valid state).
             */
            if (type_is_pointed_ctor(p_type))
            {
                /* Mark every leaf _Owner member as uninitialized. */
                if (p_pointed->members.head)
                {
                    for (struct object* m = p_pointed->members.head; m; m = m->next)
                    {
                        flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                            m, UNINITIALIZED_VALUE, FLOW3_UNINITIALIZED, line);
                    }
                    /* Parent node: NOT_APPLICABLE (aggregate, state lives in members). */
                    //flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                    //    p_pointed, NOT_APPLICABLE_VALUE, FLOW3_NOT_APPLICABLE, line);
                }
                else
                {
                    flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                        p_pointed, UNINITIALIZED_VALUE, FLOW3_UNINITIALIZED, line);
                }
            }
            else
            {
                /* Seed the pointed object as ANY. */
                flow3_parameter_object_init(ctx, p_pointed, NULL, line);
            }
        }

        return;
    }

    /*
     * For _Opt pointers with no explicit initializer the pointer
     * can be either null or non-null.  We create two correlated states:
     *
     *  Pointer alternatives:
     *   - EQUAL 0      (null)   — origin: null_map
     *   - NOT_EQUAL 0  (non-null, value.p -> arena object) — origin: nonnull_map
     *
     *  Pointed-to object alternatives (in the same map entry):
     *   - does_not_exist = true  — origin: null_map
     *     (when the pointer is null the object simply does not exist)
     *   - normal initialised state — origin: nonnull_map
     *     (when the pointer is non-null the object exists and is valid)
     *
     * By sharing the origin maps, flow narrowing (e.g. after `if (p)`)
     * automatically keeps only the matching alternative for the pointed
     * object, giving the analyser full correlation between the two.
     *
     * Note: the condition checks `relation == FLOW3_ANY` (not FLOW3_UNINITIALIZED)
     * because the unconditional assignment above sets relation to FLOW3_ANY.
     */

    if (relation == FLOW3_ANY &&
    p_type != NULL &&
    type_is_pointer(p_type) &&
    type_is_opt(p_type, nullable_enabled))
    {
        /* Two child maps so alternatives from each arm have distinct origins. */
        struct flow3_map* p_null_map =
            flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, "opt-null");
        struct flow3_map* p_nonnull_map =
            flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, "opt-nonnull");

        /* Allocate and initialise the arena object for the non-null arm. */
        struct object* p_pointed = flow3_allocated_object_arena_new(&ctx->allocated_object_arena);
        if (p_pointed != NULL)
        {
            struct type pointed_type = type_remove_pointer(p_type);
            make_object(&pointed_type, p_pointed, MAKE_STATE_ANY, ctx->ctx->options.target);
            type_destroy(&pointed_type);
        }

        /* --- pointer alternatives --- */
        struct flow3_key_alternatives* ep = flow3_map_find_add(ctx->p_current_flow3_map, p_object);
        flow3_alternatives_clear(&ep->alternatives);
        /* null arm */
        flow3_alternatives_add_object_ptr(&ep->alternatives, NULL, FLOW3_EQUAL, FLOW3_IMAGINARY_NONE, p_null_map, line);
        /* non-null arm */
        if (p_pointed != NULL)
        {
            flow3_alternatives_add_object_ptr(&ep->alternatives, p_pointed, FLOW3_EQUAL, FLOW3_IMAGINARY_NONE, p_nonnull_map, line);
        }
        else
        {
            flow3_alternatives_add_object_ptr(&ep->alternatives, NULL, FLOW3_NOT_EQUAL, FLOW3_IMAGINARY_NONE, p_nonnull_map, line);
        }

        /* --- pointed-to object alternatives --- */
        if (p_pointed != NULL)
        {
            /*
             * We need two alternatives on the same entry for p_pointed:
             *   1. normal init state (origin: p_nonnull_map) — pointer was non-null
             *   2. does_not_exist    (origin: p_null_map)    — pointer was null
             *
             * We do NOT call flow3_parameter_object_init here because:
             *   - passing NULL as p_type loses the actual pointed type, so the
             *     origin stamp in the old step 2 finds no entry and is skipped;
             *   - passing the real type triggers recursive opt-pointer expansion
             *     which overwrites p_pointed's entry before the origin can be
             *     stamped, leaving only the absent alternative behind.
             *
             * Instead, write both alternatives directly and atomically.
             */

            struct flow3_key_alternatives* eo = flow3_map_find_add(ctx->p_current_flow3_map, p_pointed);
            if (eo != NULL)
            {
                flow3_alternatives_clear(&eo->alternatives);

                /* Non-null arm: ANY state, correlated with p_nonnull_map. */
                flow3_alternatives_add_i(&eo->alternatives, ANY_VALUE, FLOW3_ANY, FLOW3_IMAGINARY_NONE, p_nonnull_map, line);

                /* Null arm: object does not exist when pointer is null. */
                flow3_alternatives_add_does_not_exist(&eo->alternatives, p_null_map, line);
            }
        }
    }
}

static void flow3_visit_init_declarator(struct flow3_visit_ctx* ctx, struct init_declarator* p_init_declarator)
{
    flow3_visit_declarator(ctx, p_init_declarator->p_declarator);

    if (!type_is_function(&p_init_declarator->p_declarator->type))
    {
        flow3_object_init(ctx,
             &p_init_declarator->p_declarator->object,
             &p_init_declarator->p_declarator->type,
             p_init_declarator->p_declarator->declaration_specifiers->first_token->line);
    }

    if (p_init_declarator->initializer)
    {
        if (p_init_declarator->initializer->assignment_expression)
        {
            struct expression* p_init_expr = p_init_declarator->initializer->assignment_expression;
            flow3_visit_full_expression(ctx, p_init_expr);
            flow3_check_object_init_assigment(ctx,
                p_init_declarator->initializer->assignment_expression,
                &p_init_declarator->p_declarator->object,
                &p_init_expr->object,
                INIT_OBJ
            );
            //  flow3_check_rhs_value(ctx, &p_init_declarator->p_declarator->type, p_init_expr);
              //flow3_assign_rhs_to_lhs(ctx, &p_init_declarator->p_declarator->object, p_init_expr);
        }
        else if (p_init_declarator->initializer->braced_initializer)
        {
            struct object* p_aggregate = &p_init_declarator->p_declarator->object;


            /* 1. Visit the whole braced initializer – evaluates all RHS expressions. */
            flow3_visit_bracket_initializer_list(ctx, p_init_declarator->initializer->braced_initializer);


            /* 3. Zero‑initialise any scalar members not explicitly set. */
            if (type_is_struct_or_union(&p_aggregate->type))
            {
                //flow3_zero_initialize_uninitialized_members(ctx, p_aggregate, line);
            }
        }
    }

    if (p_init_declarator->p_declarator->type.category != TYPE_CATEGORY_FUNCTION)
    {
        if (p_init_declarator->initializer &&
            p_init_declarator->initializer->assignment_expression)
        {
            //cast?
            if (expression_is_malloc(p_init_declarator->initializer->assignment_expression))
            {
            }
            else if (expression_is_calloc(p_init_declarator->initializer->assignment_expression))
            {
            }
        }
    }
}

static void flow3_visit_init_declarator_list(struct flow3_visit_ctx* ctx, struct init_declarator_list* p_init_declarator_list);

static void flow3_visit_declaration_specifiers(struct flow3_visit_ctx* ctx, struct declaration_specifiers* p_declaration_specifiers, struct type* _Opt p_type);

static void flow3_visit_simple_declaration(struct flow3_visit_ctx* ctx, struct simple_declaration* p_simple_declaration)
{
    flow3_visit_declaration_specifiers(ctx, p_simple_declaration->p_declaration_specifiers, NULL);
    flow3_visit_init_declarator_list(ctx, &p_simple_declaration->init_declarator_list);
}

static void flow3_visit_if_statement(struct flow3_visit_ctx* ctx, struct selection_statement* p_selection_statement)
{
    if (p_selection_statement->p_init_statement &&
        p_selection_statement->p_init_statement->p_expression_statement)
    {
        flow3_visit_expression_statement(ctx, p_selection_statement->p_init_statement->p_expression_statement);
    }

    if (p_selection_statement->p_init_statement &&
        p_selection_statement->p_init_statement->p_simple_declaration)
    {
        flow3_visit_simple_declaration(ctx, p_selection_statement->p_init_statement->p_simple_declaration);
    }

    _Opt _View struct expression hidden_expression = { 0 };

    struct flow3_branch_pair cond_pair = { 0 };

    if (p_selection_statement->condition &&
        p_selection_statement->condition->expression)
    {
        cond_pair = flow3_visit_full_expression(ctx, p_selection_statement->condition->expression);
    }

    if (p_selection_statement->condition &&
        p_selection_statement->condition->p_init_declarator)
    {
        flow3_visit_init_declarator(ctx, p_selection_statement->condition->p_init_declarator);
    }

    if (p_selection_statement->condition &&
        p_selection_statement->condition->expression == NULL &&
        p_selection_statement->condition->p_init_declarator != NULL)
    {
        hidden_expression.expression_type = EXPR_PRIMARY_DECLARATOR;
        hidden_expression.declarator = p_selection_statement->condition->p_init_declarator->p_declarator;
        assert(p_selection_statement->condition->p_init_declarator->p_declarator->first_token_opt != NULL);
        hidden_expression.first_token = p_selection_statement->condition->p_init_declarator->p_declarator->first_token_opt;
        hidden_expression.last_token = hidden_expression.first_token;
        cond_pair = flow3_visit_full_expression(ctx, &hidden_expression);
    }

    assert(p_selection_statement->first_token->type == TK_KEYWORD_IF);

    struct flow3_map* p_before = ctx->p_current_flow3_map;

    /* If cond_pair was never set (no condition), default both to p_before. */
    if (cond_pair.p_true == NULL)
    {
        cond_pair.p_true = p_before;
    }
    if (cond_pair.p_false == NULL)
    {
        cond_pair.p_false = p_before;
    }

    ctx->p_current_flow3_map = cond_pair.p_true;
    flow3_visit_secondary_block(ctx, p_selection_statement->secondary_block);

    const bool true_reached_the_end = !secondary_block_ends_with_jump(p_selection_statement->secondary_block);

    if (p_selection_statement->else_secondary_block_opt)
    {
        ctx->p_current_flow3_map = cond_pair.p_false;
        flow3_visit_secondary_block(ctx, p_selection_statement->else_secondary_block_opt);

        const bool false_reached_the_end = !secondary_block_ends_with_jump(p_selection_statement->else_secondary_block_opt);

        if (true_reached_the_end && false_reached_the_end)
        {
            flow3_map_merge_a_b(p_before, cond_pair.p_true, cond_pair.p_false);
        }
        else if (true_reached_the_end)
        {
            flow3_map_merge_a_b(p_before, cond_pair.p_true, cond_pair.p_true);
        }
        else if (false_reached_the_end)
        {
            flow3_map_merge_a_b(p_before, cond_pair.p_false, cond_pair.p_false);
        }
        /* else: both jump — p_before stays as-is */
    }
    else
    {
        /* no else: false path is already narrowed in cond_pair.p_false */
        if (true_reached_the_end)
        {
            flow3_map_merge_a_b(p_before, cond_pair.p_true, cond_pair.p_false);
        }
        else
        {
            flow3_map_merge_a_b(p_before, cond_pair.p_false, cond_pair.p_false);
        }
    }

    ctx->p_current_flow3_map = p_before;
}

static void flow3_visit_try_statement(struct flow3_visit_ctx* ctx, struct try_statement* p_try_statement)
{
    struct flow3_map* _Opt p_throw_join_map_old = ctx->p_throw_join_map;

    struct flow3_map* p_before = ctx->p_current_flow3_map;

    /* throw paths are accumulated into this map */
    struct flow3_map* p_throw_join = flow3_map_arena_new(&ctx->flow3_map_arena, p_before, "throw join");

    ctx->p_throw_join_map = p_throw_join;

    /* --- visit the try body --- */
    struct flow3_map* p_try_branch = flow3_map_arena_new(&ctx->flow3_map_arena, p_before, "try branch");
    ctx->p_current_flow3_map = p_try_branch;
    flow3_visit_secondary_block(ctx, p_try_statement->secondary_block);

    const bool try_reached_the_end = !secondary_block_ends_with_jump(p_try_statement->secondary_block);
    const bool catch_reached_the_end = !secondary_block_ends_with_jump(p_try_statement->catch_secondary_block_opt);

    if (p_try_statement->catch_secondary_block_opt)
    {
        /* --- visit the catch body, starting from the throw-join map --- */
        struct flow3_map* p_catch_branch = flow3_map_arena_new(&ctx->flow3_map_arena, p_throw_join, "catch branch");
        ctx->p_current_flow3_map = p_catch_branch;


        flow3_visit_secondary_block(ctx, p_try_statement->catch_secondary_block_opt);

        if (try_reached_the_end && catch_reached_the_end)
        {
            flow3_map_merge_a_b(p_before, p_try_branch, p_catch_branch);
        }
        else if (try_reached_the_end)
        {
            /* catch jumps away; only the try-branch outcome matters */
            flow3_map_merge_a_b(p_before, p_try_branch, p_try_branch);
        }
        else if (catch_reached_the_end)
        {
            /* try jumps away; only the catch-branch outcome matters */
            flow3_map_merge_a_b(p_before, p_catch_branch, p_catch_branch);
        }
        /* else: both jump — p_before stays as-is (unreachable continuation) */
    }
    else
    {
        /* no catch block */
        if (try_reached_the_end)
        {
            flow3_map_merge_a_b(p_before, p_try_branch, p_try_branch);
        }
    }

    ctx->p_current_flow3_map = p_before;

    /* restore */
    ctx->p_throw_join_map = p_throw_join_map_old;
}

static void flow3_visit_switch_statement(struct flow3_visit_ctx* ctx, struct selection_statement* p_selection_statement)
{
    struct flow3_map* p_before = ctx->p_current_flow3_map;
    struct flow3_map* p_body = flow3_map_arena_new(&ctx->flow3_map_arena, p_before, "switch body");
    struct flow3_map* p_break_join = flow3_map_arena_new(&ctx->flow3_map_arena, p_before, "break join");

    struct flow3_map* _Opt old_p_initial_map = ctx->p_initial_map;
    struct flow3_map* _Opt old_p_break_join_map = ctx->p_break_join_map;
    const struct object* _Opt old_p_switch_obj_key = ctx->p_switch_obj_key;

    ctx->p_initial_map = p_before;
    ctx->p_break_join_map = p_break_join;

    /* record which object is being switched on */
    ctx->p_switch_obj_key = NULL;
    if (p_selection_statement->condition &&
        p_selection_statement->condition->expression)
    {
        ctx->p_switch_obj_key = &p_selection_statement->condition->expression->object;
    }

    ctx->p_current_flow3_map = p_body;
    flow3_visit_secondary_block(ctx, p_selection_statement->secondary_block);

    bool reached_the_end = !secondary_block_ends_with_jump(p_selection_statement->secondary_block);

    if (reached_the_end)
    {
        flow3_map_merge_a_b(p_before, ctx->p_current_flow3_map, p_break_join);
    }
    else
    {
        flow3_map_merge_a_b(p_before, p_break_join, p_break_join);
    }

    ctx->p_current_flow3_map = p_before;

    /* restore */
    ctx->p_initial_map = old_p_initial_map;
    ctx->p_break_join_map = old_p_break_join_map;
    ctx->p_switch_obj_key = old_p_switch_obj_key;

    flow3_exit_block_visit_defer_list(ctx, &p_selection_statement->defer_list,
                                      p_selection_statement->secondary_block->last_token);
    flow3_defer_list_set_end_of_lifetime(ctx, &p_selection_statement->defer_list,
                                         p_selection_statement->secondary_block->last_token);
}

static void flow3_visit_selection_statement(struct flow3_visit_ctx* ctx, struct selection_statement* p_selection_statement)
{
    if (p_selection_statement->first_token->type == TK_KEYWORD_IF)
    {
        flow3_visit_if_statement(ctx, p_selection_statement);
    }
    else if (p_selection_statement->first_token->type == TK_KEYWORD_SWITCH)
    {
        flow3_visit_switch_statement(ctx, p_selection_statement);
    }
    else
    {
        assert(false);
    }

    if (p_selection_statement->lint_token)
    {
        flow3_check_dianostic_suppression(ctx, p_selection_statement->lint_token);
    }

}

static void flow3_visit_compound_statement(struct flow3_visit_ctx* ctx, struct compound_statement* p_compound_statement);

static void flow3_visit_initializer_list(struct flow3_visit_ctx* ctx, struct initializer_list* p_initializer_list);

static void flow3_visit_bracket_initializer_list(struct flow3_visit_ctx* ctx, struct braced_initializer* p_bracket_initializer_list)
{
    if (p_bracket_initializer_list->initializer_list == NULL)
    {

    }
    else
    {
        flow3_visit_initializer_list(ctx, p_bracket_initializer_list->initializer_list);
    }
}

static void flow3_visit_initializer_list_item(struct flow3_visit_ctx* ctx, struct initializer_list_item* p_initializer)
{
    assert(p_initializer->initializer != NULL);
    flow3_visit_initializer(ctx, p_initializer->initializer);
}

static void flow3_visit_initializer(struct flow3_visit_ctx* ctx, struct initializer* p_initializer)
{
    if (p_initializer->assignment_expression)
    {
        flow3_visit_full_expression(ctx, p_initializer->assignment_expression);
    }
    else if (p_initializer->braced_initializer)
    {
        flow3_visit_bracket_initializer_list(ctx, p_initializer->braced_initializer);
    }
}

static void flow3_visit_initializer_list(struct flow3_visit_ctx* ctx, struct initializer_list* p_initializer_list)
{
    struct initializer_list_item* _Opt p_initializer = p_initializer_list->head;
    while (p_initializer)
    {
        flow3_visit_initializer_list_item(ctx, p_initializer);
        p_initializer = p_initializer->next;
    }
}

static void flow3_visit_generic_selection(struct flow3_visit_ctx* ctx, struct generic_selection* p_generic_selection)
{
    if (p_generic_selection->expression)
    {
        flow3_visit_full_expression(ctx, p_generic_selection->expression);
    }
}

const char* flow3_get_only_member_name(const char* member_designator)
{
    const char* s = member_designator;
    const char* dot = s;
    while (*s)
    {
        if (*s == '.')
            dot = s;
        s++;
    }
    return dot;
}
/*
  Check the state of some reachable object,
  struct X * f(){
      return p;
  }
  p2 = p1;
*/
static void flow3_check_object_access(struct flow3_visit_ctx* ctx,
    const char* parent_expression_str,
    struct expression* p_expression,
    const struct object* p_object_src,
    bool check_unitialized)
{
    struct marker marker = expression_to_marker(p_expression);

    if (p_object_src->members.head)
    {
        struct object* member = p_object_src->members.head;
        while (member)
        {
            struct osstream ss = { 0 };
            ss_fprintf(&ss, "%s%s", parent_expression_str, flow3_get_only_member_name(member->member_designator));
            //p->
            flow3_check_object_access(ctx, ss.c_str, p_expression, member, check_unitialized);
            member = member->next;
            ss_close(&ss);
        }
    }

    const struct flow3_key_alternatives* p_source_key_alternatives = flow3_map_search_up(ctx->p_current_flow3_map, p_object_src);
    if (p_source_key_alternatives == NULL)
        return;

    for (int ri = 0; ri < p_source_key_alternatives->alternatives.size; ri++)
    {
        struct flow3_alternative* p_alternative = &p_source_key_alternatives->alternatives.data[ri];
        //flow3_alternative_print(p_alternative);
        //printf("\n");

        if (p_alternative->imaginary == FLOW3_IMAGINARY_ENDED)
        {
            diagnostic(W_FLOW_LIFETIME_ENDED,
                ctx->ctx, NULL, &marker,
                "object '%s' lifetime has ended (see line %d)",
                parent_expression_str, p_alternative->line);

            continue; //no need to print other errors
        }

        if (p_alternative->value.kind == FLOW3_VALUE_REF)
        {
            if (p_alternative->value.as.p != p_object_src)
            {
                flow3_check_object_access(ctx,
                    parent_expression_str,
                                                    p_expression,
                                                    p_alternative->value.as.p, check_unitialized);

                continue;
            }
        }

        if (type_is_pointer(&p_object_src->type) &&
            !type_is_opt(&p_object_src->type, ctx->ctx->options.null_checks_enabled) &&
            flow3_alternative_can_be_zero(p_alternative))
        {
            diagnostic(W_FLOW_NULLABLE_TO_NON_NULLABLE,
                ctx->ctx, NULL, &marker,
                "passing a possible null pointer '%s' to non-nullable pointer parameter (see line %d)",
                parent_expression_str, p_alternative->line);
        }

        if (p_alternative->imaginary != FLOW3_IMAGINARY_ENDED && p_alternative->relation == FLOW3_UNINITIALIZED)
        {
            if (check_unitialized)
            {

                diagnostic(W_FLOW_NULLABLE_TO_NON_NULLABLE,
                    ctx->ctx, NULL, &marker,
                    "passing a possible uninitialized object  '%s' (see line %d)",
                    parent_expression_str, p_alternative->line);
            }
        }

        if (p_alternative->imaginary == FLOW3_IMAGINARY_MOVED)
        {
            diagnostic(W_FLOW_LIFETIME_ENDED,
                ctx->ctx, NULL, &marker,
                "object '%s' is moved (see line %d)",
                parent_expression_str, p_alternative->line);
        }
    }
}

static void flow3_check_object_init_assigment(struct flow3_visit_ctx* ctx,
    struct expression* p_expression,
     const struct object* p_object_dest, //uninitialized alawys
     const struct object* p_object_src,
     enum init_type init_type)
{
    if (p_object_dest == NULL)
    {
        return;
    }
    struct marker marker = expression_to_marker(p_expression);

    if (p_object_src->members.head)
    {
        struct object* member_dest = p_object_dest->members.head;
        struct object* member_src = p_object_src->members.head;
        while (member_src && member_dest)
        {
            flow3_check_object_init_assigment(ctx, p_expression, member_dest, member_src, init_type);
            member_src = member_src->next;
            member_dest = member_dest->next;
        }
        return;
    }

    const struct flow3_key_alternatives* p_src_key_alternatives = flow3_map_search_up(ctx->p_current_flow3_map, p_object_src);
    if (p_src_key_alternatives == NULL)
        return;

    struct flow3_key_alternatives* p_dest_key_alternatives = NULL;

    if (init_type == INIT_OBJ)
    {
        p_dest_key_alternatives = flow3_map_find_add(ctx->p_current_flow3_map, p_object_dest);
        flow3_alternatives_clear(&p_dest_key_alternatives->alternatives);
    }

    for (int ri = 0; ri < p_src_key_alternatives->alternatives.size; ri++)
    {
        struct flow3_alternative* p_src_alternative = &p_src_key_alternatives->alternatives.data[ri];
        //flow3_alternative_print(p_src_alternative);
        //printf("\n");

        if (p_src_alternative->imaginary == FLOW3_IMAGINARY_ENDED)
        {
            struct osstream ss = { 0 };
            flow3_expression_to_string(p_expression, &ss);
            diagnostic(W_FLOW_LIFETIME_ENDED,
                ctx->ctx, NULL, &marker,
                "object '%s' lifetime has ended (see line %d)",
                ss.c_str, p_src_alternative->line);
            ss_close(&ss);
            continue; //avoid printing extra errors
        }

        if (p_src_alternative->value.kind == FLOW3_VALUE_REF)
        {
            if (p_src_alternative->value.as.p != p_object_src)
            {
                flow3_check_object_init_assigment(ctx,
                                                    p_expression,
                                                    p_object_dest, //uninitialized alawys
                                                    p_src_alternative->value.as.p,
                                                    init_type);


                if (type_is_owner(&p_object_dest->type))
                {
                    flow3_map_set_object_moved(ctx->p_current_flow3_map, p_src_alternative->value.as.p, p_expression->first_token->line);
                }
                continue;
            }
        }

        if (init_type == INIT_OBJ)
        {
            flow3_alternatives_add(&p_dest_key_alternatives->alternatives,
                                         p_src_alternative,
                                         p_src_alternative->origin,
                                         p_src_alternative->line);
        }



        if (p_src_alternative->relation == FLOW3_EQUAL &&
            p_src_alternative->value.kind == FLOW3_VALUE_PTR &&
            p_src_alternative->value.as.p)
        {
            struct osstream ss = { 0 };
            flow3_expression_to_string(p_expression, &ss);
            struct osstream ss2 = { 0 };

            ss_fprintf(&ss2, "(*%s)", ss.c_str);


            /*
            struct X * f() {
                struct X* p;
                {
                    struct X x;
                    p = &x;
                }
                return p;
            }
            */
            //we check (*p)
            //*expression
            //expression-> 
            const bool check_unitialized = !type_is_pointed_ctor(&p_object_dest->type);
            flow3_check_object_access(ctx, ss2.c_str, p_expression, p_src_alternative->value.as.p, check_unitialized);


            if (init_type == INIT_PARAMETER)
            {
                if (type_is_pointed_clear(&p_object_dest->type))
                {
                    flow3_map_set_object_zero(ctx->p_current_flow3_map,
                  p_src_alternative->value.as.p, p_expression->first_token->line);
                }

                if (type_is_pointed_dtor(&p_object_dest->type))
                {
                    flow3_map_set_object_lifetime_ended(ctx->p_current_flow3_map,
                  p_src_alternative->value.as.p, p_expression->first_token->line);
                }

                if (type_is_pointed_ctor(&p_object_dest->type))
                {
                    flow3_map_set_object_any(ctx->p_current_flow3_map,
                  p_src_alternative->value.as.p, p_expression->first_token->line);
                }

                if (type_is_owner(&p_object_dest->type))
                {
                    flow3_map_set_object_lifetime_ended(ctx->p_current_flow3_map,
                  p_src_alternative->value.as.p, p_expression->first_token->line);
                }
            }

            ss_close(&ss);
            ss_close(&ss2);
        }

        if (type_is_pointer(&p_object_dest->type) &&
            !type_is_opt(&p_object_dest->type, ctx->ctx->options.null_checks_enabled) &&
            flow3_alternative_can_be_zero(p_src_alternative))
        {
            struct osstream ss = { 0 };
            flow3_expression_to_string(p_expression, &ss);
            diagnostic(W_FLOW_NULLABLE_TO_NON_NULLABLE,
                ctx->ctx, NULL, &marker,
                "passing a possible null pointer '%s' to non-nullable pointer parameter (see line %d)",
                ss.c_str, p_src_alternative->line);
            ss_close(&ss);
        }

        if (p_src_alternative->relation == FLOW3_UNINITIALIZED)
        {
            if (type_is_pointed_ctor(&p_object_dest->type))
            {
            }
            else
            {
                diagnostic(W_FLOW_NULLABLE_TO_NON_NULLABLE,
                    ctx->ctx, NULL, &marker,
                    "passing a possible uninitialized object  '%s' (see line %d)",
                    p_object_src->member_designator, p_src_alternative->line);
            }
        }

        if (p_src_alternative->imaginary == FLOW3_IMAGINARY_MOVED)
        {
            struct osstream ss = { 0 };
            flow3_expression_to_string(p_expression, &ss);
            diagnostic(W_FLOW_LIFETIME_ENDED,
                ctx->ctx, NULL, &marker,
                "object '%s' is moved (see line %d)",
                ss.c_str, p_src_alternative->line);
            ss_close(&ss);
        }
    }
}

static void flow3_check_assigment(struct flow3_visit_ctx* ctx,
    struct expression* p_expression_dest,
    struct expression* p_expression_src)
{
    const struct flow3_key_alternatives* p_expression_dest_key_alternatives =
        flow3_map_search_up(ctx->p_current_flow3_map, &p_expression_dest->object);

    if (p_expression_dest_key_alternatives == NULL)
        return;

    for (int i = 0; i < p_expression_dest_key_alternatives->alternatives.size; i++)
    {
        const struct flow3_alternative* p_expression_dest_alternative =
            &p_expression_dest_key_alternatives->alternatives.data[i];

        if (p_expression_dest_alternative->value.kind == FLOW3_VALUE_REF)
        {
            flow3_check_object_init_assigment(ctx,
                p_expression_src,
                p_expression_dest_alternative->value.as.p,
                &p_expression_src->object,
                INIT_OBJ);
        }
    }
}

static void flow3_visit_function_arguments(struct flow3_visit_ctx* ctx,
    const struct type* p_type,
    const struct argument_expression_list* p_argument_expression_list)
{
    try
    {
        const struct param_list* _Opt p_param_list = type_get_func_or_func_ptr_params(p_type);
        if (p_param_list == NULL)
        {
            throw;
        }

        struct param* _Opt p_current_parameter_type = p_param_list->head;
        struct argument_expression* _Opt p_current_argument = p_argument_expression_list->head;

        while (p_current_argument && p_current_parameter_type)
        {
            struct expression* p_arg_expr = p_current_argument->expression;
            const struct type* p_param_type = &p_current_parameter_type->type;

            struct object param_object = { 0 };
            make_object(p_param_type, &param_object, MAKE_STATE_UNITIALIZED, ctx->ctx->options.target);

            flow3_visit_full_expression(ctx, p_arg_expr);
            flow3_check_object_init_assigment(ctx, p_arg_expr, &param_object, &p_arg_expr->object, INIT_PARAMETER);

            p_current_argument = p_current_argument->next;
            p_current_parameter_type = p_current_parameter_type->next;
            object_destroy(&param_object);
        }

        /* Variadic tail – visit remaining arguments with no paired parameter */
        while (p_current_argument)
        {
            flow3_visit_full_expression(ctx, p_current_argument->expression);
            p_current_argument = p_current_argument->next;
        }
    }
    catch
    {
    }
}

static void flow3_check_dianostic_suppression(struct flow3_visit_ctx* ctx, struct token* p_token)
{
    check_dianostic_suppression_phase(ctx->ctx, p_token, 2);
}

static const struct expression* skip_parenthesis(const struct expression* expr)
{
    while (expr && expr->expression_type == EXPR_PRIMARY_PARENTHESIS)
    {
        assert(expr->right != NULL);
        expr = expr->right;
    }
    return expr;
}

static void flow3_expression_static_debug(struct flow3_visit_ctx* ctx, const struct expression* p_expression)
{
    struct token* first_token = p_expression->first_token;
    struct token* last_token = p_expression->last_token;
    p_expression = skip_parenthesis(p_expression);
    object_static_debug(ctx, &p_expression->object, first_token, last_token);
}

static struct flow3_branch_pair flow3_visit_full_expression(struct flow3_visit_ctx* ctx, struct expression* p_expression)
{
    return flow3_visit_expression(ctx, p_expression);
}

typedef enum { KNOWN_EQUAL, KNOWN_NOT_EQUAL, KNOWN_NONE } known_rel_t;

static bool flow3_alternatives_is_single_constant(const struct flow3_alternatives* alts, long long* out_val)
{
    if (alts->size != 1) return false;
    const struct flow3_alternative* alt = &alts->data[0];
    if (alt->relation != FLOW3_EQUAL) return false;
    if (alt->value.kind == FLOW3_VALUE_SIGNED)
    {
        *out_val = alt->value.as.i;
        return true;
    }
    else if (alt->value.kind == FLOW3_VALUE_UNSIGNED)
    {
        *out_val = (long long)alt->value.as.u;
        return true;
    }
    else if (alt->value.kind == FLOW3_VALUE_PTR)
    {
        *out_val = (uintptr_t)alt->value.as.p;
        return true;
    }
    return false;
}

static void narrow_by_constant(const struct flow3_alternatives* src,
                               long long c,
                               bool is_equal,   /* true for ==, false for != */
                               struct flow3_alternatives* true_alts,
                               struct flow3_alternatives* false_alts,
                               const struct flow3_map* origin,
                               int line)
{
    for (int i = 0; i < src->size; i++)
    {
        const struct flow3_alternative* alt = &src->data[i];
        if (alt->relation == FLOW3_UNINITIALIZED)
            continue;

        /* Handle absent: treat as value 0 (null) */
        if (alt->imaginary == FLOW3_IMAGINARY_ABSENT)
        {
            bool result = is_equal ? (0 == c) : (0 != c);
            if (result)
                flow3_alternatives_add(true_alts, alt, origin, line);
            else
                flow3_alternatives_add(false_alts, alt, origin, line);
            continue;
        }

        /* Extract numeric value from the alternative if possible */
        long long val;
        bool is_ptr = false;
        if (alt->value.kind == FLOW3_VALUE_SIGNED)
            val = alt->value.as.i;
        else if (alt->value.kind == FLOW3_VALUE_UNSIGNED)
            val = (long long)alt->value.as.u;
        else if (alt->value.kind == FLOW3_VALUE_PTR)
        {
            val = (uintptr_t)alt->value.as.p;
            is_ptr = true;
        }
        else if (alt->value.kind == FLOW3_VALUE_REF)
        {
            /* REF should have been resolved before calling this function */
            continue;
        }
        else
        {
            continue;
        }

        if (alt->relation == FLOW3_EQUAL)
        {
            bool result = is_equal ? (val == c) : (val != c);
            if (result)
                flow3_alternatives_add(true_alts, alt, origin, line);
            else
                flow3_alternatives_add(false_alts, alt, origin, line);
        }
        else if (alt->relation == FLOW3_NOT_EQUAL)
        {
            if (val == c)
            {
                /* Already known to be NOT_EQUAL c */
                if (is_equal)
                {
                    /* == c is false, keep NOT_EQUAL c in false branch only */
                    flow3_alternatives_add(false_alts, alt, origin, line);
                }
                else
                {
                    /* != c is true, keep NOT_EQUAL c in true branch only */
                    flow3_alternatives_add(true_alts, alt, origin, line);
                }
            }
            else
            {
                /* val != c */
                if (is_equal)
                {
                    /* For true branch of ==, we must enforce x == c, overriding the NOT_EQUAL val */
                    struct flow3_value v;
                    if (is_ptr)
                    {
                        v.kind = FLOW3_VALUE_PTR;
                        v.as.p = (const struct object*)(uintptr_t)c;
                    }
                    else
                    {
                        v.kind = FLOW3_VALUE_SIGNED;
                        v.as.i = c;
                    }
                    struct flow3_alternative a_eq = { .value = v, .relation = FLOW3_EQUAL, .imaginary = alt->imaginary };
                    flow3_alternatives_add(true_alts, &a_eq, origin, line);
                    /* For false branch, keep the original NOT_EQUAL val (we lose the info x != c) */
                    flow3_alternatives_add(false_alts, alt, origin, line);
                }
                else
                {
                    /* For !=, the condition x != c is not determined by x != val when val != c,
                       so we keep NOT_EQUAL val in both branches */
                    flow3_alternatives_add(true_alts, alt, origin, line);
                    flow3_alternatives_add(false_alts, alt, origin, line);
                }
            }
        }
        else if (alt->relation == FLOW3_ANY)
        {
            /* Unknown value – refine to EQUAL c for true, NOT_EQUAL c for false */
            struct flow3_value v;
            if (is_ptr)
            {
                v.kind = FLOW3_VALUE_PTR;
                v.as.p = (const struct object*)(uintptr_t)c;
            }
            else
            {
                v.kind = FLOW3_VALUE_SIGNED;
                v.as.i = c;
            }
            if (is_equal)
            {
                struct flow3_alternative a_eq = { .value = v, .relation = FLOW3_EQUAL, .imaginary = alt->imaginary };
                struct flow3_alternative a_ne = { .value = v, .relation = FLOW3_NOT_EQUAL, .imaginary = alt->imaginary };
                flow3_alternatives_add(true_alts, &a_eq, origin, line);
                flow3_alternatives_add(false_alts, &a_ne, origin, line);
            }
            else
            {
                struct flow3_alternative a_ne = { .value = v, .relation = FLOW3_NOT_EQUAL, .imaginary = alt->imaginary };
                struct flow3_alternative a_eq = { .value = v, .relation = FLOW3_EQUAL, .imaginary = alt->imaginary };
                flow3_alternatives_add(true_alts, &a_ne, origin, line);
                flow3_alternatives_add(false_alts, &a_eq, origin, line);
            }
        }
    }
}

static int flow3_evaluate_equality_against_constant(const struct flow3_alternatives* alts,
                                                    long long c,
                                                    bool is_equal)
{
    bool all_true = true, all_false = true;
    for (int i = 0; i < alts->size; i++)
    {
        const struct flow3_alternative* alt = &alts->data[i];
        if (alt->imaginary == FLOW3_IMAGINARY_ABSENT)
        {
            bool is_null = true;
            bool result = is_equal ? is_null : !is_null;
            if (result) all_false = false; else all_true = false;
            continue;
        }
        bool result = false, known = false;
        if (alt->value.kind == FLOW3_VALUE_PTR)
        {
            if (c == 0)
            {
                if (alt->relation == FLOW3_EQUAL)
                {
                    result = is_equal ? (alt->value.as.p == NULL) : (alt->value.as.p != NULL);
                    known = true;
                }
                else if (alt->relation == FLOW3_NOT_EQUAL && alt->value.as.p == NULL)
                {
                    result = is_equal ? false : true;
                    known = true;
                }
            }
        }
        else if (alt->value.kind == FLOW3_VALUE_SIGNED || alt->value.kind == FLOW3_VALUE_UNSIGNED)
        {
            long long val = (alt->value.kind == FLOW3_VALUE_SIGNED) ? alt->value.as.i : (long long)alt->value.as.u;
            if (alt->relation == FLOW3_EQUAL)
            {
                result = is_equal ? (val == c) : (val != c);
                known = true;
            }
            else if (alt->relation == FLOW3_NOT_EQUAL && val == c)
            {
                result = is_equal ? false : true;
                known = true;
            }
        }
        if (!known) return -1;
        if (result) all_false = false; else all_true = false;
    }
    if (all_true) return 1;
    if (all_false) return 0;
    return -1;
}


static int flow3_evaluate_relational(const struct flow3_alternatives* left_alts,
                                     const struct flow3_alternatives* right_alts,
                                     enum expression_type op)
{
    long long left_vals[32];
    int left_count = 0;
    for (int i = 0; i < left_alts->size && left_count < 32; i++)
    {
        const struct flow3_alternative* alt = &left_alts->data[i];
        if (alt->imaginary == FLOW3_IMAGINARY_ABSENT || alt->relation != FLOW3_EQUAL) return -1;
        if (alt->value.kind != FLOW3_VALUE_SIGNED && alt->value.kind != FLOW3_VALUE_UNSIGNED) return -1;
        left_vals[left_count++] = (alt->value.kind == FLOW3_VALUE_SIGNED) ? alt->value.as.i : (long long)alt->value.as.u;
    }
    if (left_count == 0) return -1;

    long long right_vals[32];
    int right_count = 0;
    for (int i = 0; i < right_alts->size && right_count < 32; i++)
    {
        const struct flow3_alternative* alt = &right_alts->data[i];
        if (alt->imaginary == FLOW3_IMAGINARY_ABSENT || alt->relation != FLOW3_EQUAL) return -1;
        if (alt->value.kind != FLOW3_VALUE_SIGNED && alt->value.kind != FLOW3_VALUE_UNSIGNED) return -1;
        right_vals[right_count++] = (alt->value.kind == FLOW3_VALUE_SIGNED) ? alt->value.as.i : (long long)alt->value.as.u;
    }
    if (right_count == 0) return -1;

    bool all_true = true, all_false = true;
    for (int i = 0; i < left_count; i++)
    {
        for (int j = 0; j < right_count; j++)
        {
            bool result;
            switch (op)
            {
            case EXPR_RELATIONAL_BIGGER_THAN:          result = left_vals[i] > right_vals[j]; break;
            case EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN: result = left_vals[i] >= right_vals[j]; break;
            case EXPR_RELATIONAL_LESS_THAN:            result = left_vals[i] < right_vals[j]; break;
            case EXPR_RELATIONAL_LESS_OR_EQUAL_THAN:   result = left_vals[i] <= right_vals[j]; break;
            default: return -1;
            }
            if (result) all_false = false;
            else all_true = false;
        }
    }

    if (all_true) return 1;
    if (all_false) return 0;
    return -1;
}

// Helper: get a display name (uses member_designator if present)
const char* obj_display(const struct object* obj)
{
    if (obj && obj->member_designator && obj->member_designator[0])
        return obj->member_designator;
    static char buf[32];
    snprintf(buf, sizeof(buf), "0x%lx", (unsigned long)(uintptr_t)obj);
    return buf;
}

// Helper: print a single alternative’s value (ignores relation)
void print_value(const struct flow3_alternative* alt)
{
    if (alt->imaginary == FLOW3_IMAGINARY_ABSENT)
    {
        printf("ABSENT");
        return;
    }
    switch (alt->value.kind)
    {
    case FLOW3_VALUE_SIGNED:   printf("%lld", alt->value.as.i); break;
    case FLOW3_VALUE_UNSIGNED: printf("%llu", alt->value.as.u); break;
    case FLOW3_VALUE_PTR:      printf("%s", obj_display(alt->value.as.p)); break;
    case FLOW3_VALUE_REF:      printf("REF->%s", obj_display(alt->value.as.p)); break;
    default:                   printf("?");
    }
}


static void flow3_evaluate_binary_arithmetic(struct flow3_visit_ctx* ctx,
                                             const struct expression* p_left,
                                             const struct expression* p_right,
                                             const struct expression* p_result,
                                             char op)
{
    const struct flow3_key_alternatives* left_entry =
        flow3_map_search_up(ctx->p_current_flow3_map, &p_left->object);
    const struct flow3_key_alternatives* right_entry =
        flow3_map_search_up(ctx->p_current_flow3_map, &p_right->object);

    if (!left_entry || !right_entry)
    {
        flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                                         &p_result->object,
                                         ANY_VALUE, FLOW3_ANY,
                                         p_result->first_token->line);
        return;
    }

    struct flow3_alternatives result_alts = { 0 };
    struct marker marker = expression_to_marker(p_result);

    // ---- Outer loop over left top‑level alternatives ----
    for (int li = 0; li < left_entry->alternatives.size; li++)
    {
        const struct flow3_alternative* lalt = &left_entry->alternatives.data[li];
        if (lalt->imaginary == FLOW3_IMAGINARY_ABSENT) continue;

        // Resolve left REF if present
        const struct flow3_key_alternatives* left_resolved = left_entry;
        if (lalt->value.kind == FLOW3_VALUE_REF && lalt->value.as.p != NULL)
        {
            left_resolved = flow3_map_search_up(ctx->p_current_flow3_map, lalt->value.as.p);
            if (!left_resolved) continue;
        }

        // ---- Loop over resolved left values ----
        for (int lvi = 0; lvi < left_resolved->alternatives.size; lvi++)
        {
            const struct flow3_alternative* lval = &left_resolved->alternatives.data[lvi];
            if (lval->imaginary == FLOW3_IMAGINARY_ABSENT) continue;

            // ---- Middle loop over right top‑level alternatives ----
            for (int ri = 0; ri < right_entry->alternatives.size; ri++)
            {
                const struct flow3_alternative* ralt = &right_entry->alternatives.data[ri];
                if (ralt->imaginary == FLOW3_IMAGINARY_ABSENT) continue;

                // Resolve right REF if present
                const struct flow3_key_alternatives* right_resolved = right_entry;
                if (ralt->value.kind == FLOW3_VALUE_REF && ralt->value.as.p != NULL)
                {
                    right_resolved = flow3_map_search_up(ctx->p_current_flow3_map, ralt->value.as.p);
                    if (!right_resolved) continue;
                }

                // ---- Loop over resolved right values ----
                for (int rvi = 0; rvi < right_resolved->alternatives.size; rvi++)
                {
                    const struct flow3_alternative* rval = &right_resolved->alternatives.data[rvi];
                    if (rval->imaginary == FLOW3_IMAGINARY_ABSENT) continue;

                    enum flow3_relation result_rel = FLOW3_ANY;
                    long long result_val = ANY_VALUE;

                    // Only if both are EQUAL and numeric can we compute an exact result
                    if (lval->relation == FLOW3_EQUAL && rval->relation == FLOW3_EQUAL)
                    {
                        bool lnum = (lval->value.kind == FLOW3_VALUE_SIGNED || lval->value.kind == FLOW3_VALUE_UNSIGNED);
                        bool rnum = (rval->value.kind == FLOW3_VALUE_SIGNED || rval->value.kind == FLOW3_VALUE_UNSIGNED);
                        if (lnum && rnum)
                        {
                            long long l = (lval->value.kind == FLOW3_VALUE_SIGNED) ? lval->value.as.i : (long long)lval->value.as.u;
                            long long r = (rval->value.kind == FLOW3_VALUE_SIGNED) ? rval->value.as.i : (long long)rval->value.as.u;

                            switch (op)
                            {
                            case '+':
                                result_val = l + r;
                                result_rel = FLOW3_EQUAL;
                                break;
                            case '-':
                                result_val = l - r;
                                result_rel = FLOW3_EQUAL;
                                break;
                            case '*':
                                result_val = l * r;
                                result_rel = FLOW3_EQUAL;
                                break;
                            case '/':
                                if (r == 0)
                                {
                                    diagnostic(W_FLOW_DIVISION_BY_ZERO, ctx->ctx, NULL, &marker, "division by zero");
                                    // result remains ANY
                                }
                                else
                                {
                                    result_val = l / r;
                                    result_rel = FLOW3_EQUAL;
                                }
                                break;
                            case '%':
                                if (r == 0)
                                {
                                    diagnostic(W_FLOW_DIVISION_BY_ZERO, ctx->ctx, NULL, &marker, "modulo by zero");
                                    // result remains ANY
                                }
                                else
                                {
                                    result_val = l % r;
                                    result_rel = FLOW3_EQUAL;
                                }
                                break;
                            default:
                                break;
                            }
                        }
                    }

                    // Build the result alternative
                    enum flow3_value_kind result_kind = FLOW3_VALUE_SIGNED; // could be refined based on type
                    struct flow3_value v;
                    if (result_rel == FLOW3_EQUAL)
                    {
                        v.kind = result_kind;
                        v.as.i = result_val;
                    }
                    else
                    {
                        v.kind = result_kind;
                        v.as.i = ANY_VALUE;
                    }

                    struct flow3_alternative a = { .value = v, .relation = result_rel, .imaginary = FLOW3_IMAGINARY_NONE };
                    flow3_alternatives_add(&result_alts, &a,
                                           ctx->p_current_flow3_map,
                                           p_result->first_token->line);
                }
            }
        }
    }

    // ---- Write the computed alternatives into the result expression ----
    struct flow3_key_alternatives* dst =
        flow3_map_find_add(ctx->p_current_flow3_map, &p_result->object);
    if (dst)
    {
        flow3_alternatives_clear(&dst->alternatives);
        if (result_alts.size > 0)
            dst->alternatives = result_alts;   // move
        else
            flow3_alternatives_add_i(&dst->alternatives, ANY_VALUE, FLOW3_ANY, FLOW3_IMAGINARY_NONE,
                                     ctx->p_current_flow3_map,
                                     p_result->first_token->line);
    }
    else
    {
        flow3_alternatives_clear(&result_alts);
    }
}


static struct flow3_branch_pair flow3_visit_expression(struct flow3_visit_ctx* ctx, const struct expression* p_expression)
{
    struct flow3_map* p_before = ctx->p_current_flow3_map;
    (void)p_before; /* used in some cases below */

    switch (p_expression->expression_type)
    {
    case EXPR_INVALID:
        assert(false);
        break;

    case EXPR_PRIMARY__FUNC__:
        break;

    case EXPR_PRIMARY_ENUMERATOR:
        break;

    case EXPR_PRIMARY_DECLARATOR:
    {
        const struct object* p_obj = &p_expression->declarator->object;
        if (!type_is_function(&p_expression->type) &&
            p_obj->state != CONSTANT_VALUE_STATE_CONSTANT &&
            flow3_map_search_up(ctx->p_current_flow3_map, p_obj) == NULL)
        {
            /*file scope*/
            //TODO create flow_set_object_any
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map, p_obj, ANY_VALUE, FLOW3_ANY, p_expression->first_token->line);
        }

        assert(p_expression->declarator != NULL);

        flow3_map_replace_alternatives_ref(ctx->p_current_flow3_map,
            &p_expression->object,
            p_obj,
            FLOW3_EQUAL,
            p_expression->first_token->line);

        /* Build true/false branch maps narrowed on this variable. */
        const struct object* p_obj2 = &p_expression->declarator->object;
        struct flow3_map* p_true = flow3_narrow_map(&ctx->flow3_map_arena, ctx->p_current_flow3_map, p_obj2, true, "var-true");
        struct flow3_map* p_false = flow3_narrow_map(&ctx->flow3_map_arena, ctx->p_current_flow3_map, p_obj2, false, "var-false");
        return (struct flow3_branch_pair) { p_true, p_false };
    }

    case EXPR_PRIMARY_PARENTHESIS:
        assert(p_expression->right != NULL);
        return flow3_visit_expression(ctx, skip_parenthesis(p_expression->right));

    case EXPR_PRIMARY_STATEMENT_EXPRESSION:
        assert(p_expression->compound_statement != NULL);
        flow3_visit_compound_statement(ctx, p_expression->compound_statement);
        break;

    case EXPR_PRIMARY_STRING_LITERAL:
    case EXPR_PRIMARY_CHAR_LITERAL:
    case EXPR_PRIMARY_NUMBER:
    case EXPR_PRIMARY_PREDEFINED_CONSTANT:
    {
        flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
            &p_expression->object,
            object_to_signed_long_long(&p_expression->object),
            FLOW3_EQUAL,
            p_expression->first_token->line);
    }
    break;

    case EXPR_PRIMARY_GENERIC:
        assert(p_expression->generic_selection != NULL);
        flow3_visit_generic_selection(ctx, p_expression->generic_selection);
        break;

    case EXPR_POSTFIX_DOT:
    {
        const int member_index = p_expression->member_index;
        flow3_visit_expression(ctx, p_expression->left);

        struct flow3_key_alternatives* result_entry = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
        flow3_alternatives_clear(&result_entry->alternatives);

        const struct flow3_key_alternatives* p_left_alternatives = flow3_map_search_up(ctx->p_current_flow3_map, &skip_parenthesis(p_expression->left)->object);

        struct flow3_map* p_true = flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, ".true");
        struct flow3_map* p_false = flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, ".false");

        for (int i = 0; i < p_left_alternatives->alternatives.size; i++)
        {
            const struct flow3_alternative* p_left_alternative = &p_left_alternatives->alternatives.data[i];

            if (p_left_alternative->relation == FLOW3_EQUAL &&
                    p_left_alternative->value.kind == FLOW3_VALUE_REF &&
                    p_left_alternative->value.as.p != NULL)
            {
                struct object* p_member = object_get_member(p_left_alternative->value.as.p, member_index);

                flow3_alternatives_add_object_ref(&result_entry->alternatives,
                     p_member,
                     FLOW3_IMAGINARY_NONE,
                     ctx->p_current_flow3_map,
                     p_expression->first_token->line);

                flow3_narrow_map_into(p_true, ctx->p_current_flow3_map, p_member, true);
                flow3_narrow_map_into(p_false, ctx->p_current_flow3_map, p_member, false);
            }
        }


        /* Narrow on the member field used as bool. */
        return (struct flow3_branch_pair) { p_true, p_false };
    }

    case EXPR_POSTFIX_ARROW:
    {
        assert(p_expression->left != NULL);

        flow3_visit_expression(ctx, p_expression->left);

        const int member_index = p_expression->member_index;
        struct marker marker = expression_to_marker(p_expression);

        const struct flow3_key_alternatives* p_left_alternatives = flow3_map_search_up(ctx->p_current_flow3_map, &p_expression->left->object);

        struct flow3_key_alternatives* result_entry = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
        flow3_alternatives_clear(&result_entry->alternatives);

        struct flow3_map* p_true = flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, "arrow-true");
        struct flow3_map* p_false = flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, "arrow-false");

        bool any_member_resolved = false;

        if (p_left_alternatives != NULL)
        {
            for (int i = 0; i < p_left_alternatives->alternatives.size; i++)
            {
                const struct flow3_alternative* ptr_alt = &p_left_alternatives->alternatives.data[i];

                if (ptr_alt->imaginary == FLOW3_IMAGINARY_ABSENT)
                    continue;

                /* Resolve LHS to concrete pointer alternatives */
                const struct flow3_key_alternatives* p_pointer_alts = NULL;
                if (ptr_alt->relation == FLOW3_EQUAL &&
                    ptr_alt->value.kind == FLOW3_VALUE_REF &&
                    ptr_alt->value.as.p != NULL)
                {
                    p_pointer_alts = flow3_map_search_up(ctx->p_current_flow3_map, ptr_alt->value.as.p);
                }
                else if (ptr_alt->value.kind == FLOW3_VALUE_PTR)
                {
                    /* Direct pointer – treat as single-alternative */
                    p_pointer_alts = NULL;
                }
                else
                {
                    continue;
                }

                /* Process a list of pointer alternatives (resolved from REF) */
                if (p_pointer_alts != NULL)
                {
                    for (int k = 0; k < p_pointer_alts->alternatives.size; k++)
                    {
                        const struct flow3_alternative* p_pointer_alt = &p_pointer_alts->alternatives.data[k];

                        if (p_pointer_alt->imaginary == FLOW3_IMAGINARY_ABSENT)
                            continue;

                        /* Null check */
                        if (p_pointer_alt->relation == FLOW3_EQUAL &&
                            p_pointer_alt->value.kind == FLOW3_VALUE_PTR &&
                            p_pointer_alt->value.as.p == NULL)
                        {
                            diagnostic(W_FLOW_NULL_DEREFERENCE, ctx->ctx, NULL, &marker,
                                       "-> operator applied to a null pointer");
                            continue;
                        }

                        if (p_pointer_alt->value.kind != FLOW3_VALUE_PTR || p_pointer_alt->value.as.p == NULL)
                            continue;

                        const struct object* p_pointed_obj = p_pointer_alt->value.as.p;

                        /* Lifetime check for the pointed object */
                        const struct flow3_key_alternatives* p_pointed_entry = flow3_map_search_up(ctx->p_current_flow3_map, p_pointed_obj);
                        if (p_pointed_entry)
                        {
                            for (int j = 0; j < p_pointed_entry->alternatives.size; j++)
                            {
                                const struct flow3_alternative* pointed_alt = &p_pointed_entry->alternatives.data[j];
                                if (pointed_alt->imaginary == FLOW3_IMAGINARY_ENDED)
                                {
                                    diagnostic(W_FLOW_LIFETIME_ENDED, ctx->ctx, NULL, &marker,
                                               "-> operator: pointed object lifetime has ended");
                                }
                            }
                        }

                        struct object* member_obj = object_get_member(p_pointed_obj, member_index);
                        if (member_obj == NULL)
                        {
                            //assert(false);
                            continue;
                        }

                        flow3_alternatives_add_object_ref(&result_entry->alternatives,
                            member_obj,
                            FLOW3_IMAGINARY_NONE,
                            ctx->p_current_flow3_map,
                            p_expression->first_token->line);

                        flow3_narrow_map_into(p_true, ctx->p_current_flow3_map, member_obj, true);
                        flow3_narrow_map_into(p_false, ctx->p_current_flow3_map, member_obj, false);

                        any_member_resolved = true;
                    }
                }
                else
                {
                    /* Direct pointer alternative (ptr_alt is FLOW3_VALUE_PTR) */
                    if (ptr_alt->relation == FLOW3_EQUAL &&
                        ptr_alt->value.kind == FLOW3_VALUE_PTR &&
                        ptr_alt->value.as.p == NULL)
                    {
                        diagnostic(W_FLOW_NULL_DEREFERENCE, ctx->ctx, NULL, &marker,
                                   "-> operator applied to a null pointer");
                        continue;
                    }

                    if (ptr_alt->value.kind != FLOW3_VALUE_PTR || ptr_alt->value.as.p == NULL)
                        continue;

                    const struct object* p_pointed_obj = ptr_alt->value.as.p;

                    const struct flow3_key_alternatives* p_pointed_entry = flow3_map_search_up(ctx->p_current_flow3_map, p_pointed_obj);
                    if (p_pointed_entry)
                    {
                        for (int j = 0; j < p_pointed_entry->alternatives.size; j++)
                        {
                            const struct flow3_alternative* pointed_alt = &p_pointed_entry->alternatives.data[j];
                            if (pointed_alt->imaginary == FLOW3_IMAGINARY_ENDED)
                            {
                                diagnostic(W_FLOW_LIFETIME_ENDED, ctx->ctx, NULL, &marker,
                                           "-> operator: pointed object lifetime has ended");
                            }
                        }
                    }

                    struct object* member_obj = object_get_member(p_pointed_obj, member_index);
                    if (member_obj == NULL)
                    {
                        assert(false);
                        continue;
                    }

                    flow3_alternatives_add_object_ref(&result_entry->alternatives,
                        member_obj,
                        FLOW3_IMAGINARY_NONE,
                        ctx->p_current_flow3_map,
                        p_expression->first_token->line);

                    flow3_narrow_map_into(p_true, ctx->p_current_flow3_map, member_obj, true);
                    flow3_narrow_map_into(p_false, ctx->p_current_flow3_map, member_obj, false);

                    any_member_resolved = true;
                }
            }
        }

        if (!any_member_resolved)
        {
            p_true = flow3_narrow_map(&ctx->flow3_map_arena, ctx->p_current_flow3_map, &p_expression->object, true, "arrow-true");
            p_false = flow3_narrow_map(&ctx->flow3_map_arena, ctx->p_current_flow3_map, &p_expression->object, false, "arrow-false");
        }

        flow3_map_remove(ctx->p_current_flow3_map, &p_expression->left->object);
        return (struct flow3_branch_pair) { p_true, p_false };
    }
    break;

    case EXPR_POSTFIX_ARRAY:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);
        flow3_visit_expression(ctx, p_expression->left);
        flow3_visit_expression(ctx, p_expression->right);
        /* Array element used as bool: narrow on the element object. */

        const struct object* p_obj = &p_expression->object;
        struct flow3_map* p_true = flow3_narrow_map(&ctx->flow3_map_arena, ctx->p_current_flow3_map, p_obj, true, "arr-true");
        struct flow3_map* p_false = flow3_narrow_map(&ctx->flow3_map_arena, ctx->p_current_flow3_map, p_obj, false, "arr-false");
        flow3_map_remove(ctx->p_current_flow3_map, &p_expression->left->object);
        flow3_map_remove(ctx->p_current_flow3_map, &p_expression->right->object);
        return (struct flow3_branch_pair) { p_true, p_false };
    }

    case EXPR_POSTFIX_FUNCTION_CALL:
    {
        assert(p_expression->left != NULL);

        flow3_visit_expression(ctx, p_expression->left);
        flow3_visit_function_arguments(ctx, &p_expression->left->type, &p_expression->argument_expression_list);

        const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;
        const struct type* p_ret_type = &p_expression->type;
        const int call_line = p_expression->first_token->line;

        if (nullable_enabled && type_is_pointer(p_ret_type) && type_is_opt(p_ret_type, nullable_enabled))
        {
            struct flow3_key_alternatives* p_result_alternatives = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
            flow3_alternatives_clear(&p_result_alternatives->alternatives);


            struct flow3_map* p_null_map = flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, "call-opt-null");
            flow3_alternatives_add_object_ptr(&p_result_alternatives->alternatives, NULL, FLOW3_EQUAL, FLOW3_IMAGINARY_NONE, p_null_map, call_line);

            struct flow3_map* p_nonnull_map = flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, "call-opt-nonnull");

            struct object* p_pointed = flow3_allocated_object_arena_new(&ctx->allocated_object_arena);
            struct type pointed_type = type_remove_pointer(p_ret_type);
            make_object(&pointed_type, p_pointed, MAKE_STATE_ANY, ctx->ctx->options.target);
            type_destroy(&pointed_type);
            struct flow3_map* old = ctx->p_current_flow3_map;
            ctx->p_current_flow3_map = p_nonnull_map;
            flow3_object_init(ctx, p_pointed, &pointed_type, call_line);
            ctx->p_current_flow3_map = old;

            flow3_alternatives_add_object_ptr(&p_result_alternatives->alternatives, p_pointed, FLOW3_EQUAL, FLOW3_IMAGINARY_NONE, p_nonnull_map, call_line);


        }
        else if (type_is_pointer(p_ret_type))
        {
            struct flow3_value v = { .kind = FLOW3_VALUE_PTR, .as = {.p = NULL } };
            flow3_map_replace_alternatives(ctx->p_current_flow3_map, &p_expression->object, v, FLOW3_NOT_EQUAL, call_line);
        }
        flow3_map_remove(ctx->p_current_flow3_map, &p_expression->left->object);
    }
    break;

    case EXPR_POSTFIX_FUNCTION_LITERAL:
        assert(p_expression->compound_statement != NULL);
        flow3_visit_compound_statement(ctx, p_expression->compound_statement);
        break;

    case EXPR_POSTFIX_COMPOUND_LITERAL:
    {
        assert(p_expression->left == NULL);
        assert(p_expression->right == NULL);
        assert(p_expression->type_name != NULL);
        assert(p_expression->braced_initializer != NULL);

        //const struct object* p_agg = &p_expression->object;
        int line = p_expression->first_token->line;

        /* 1. Evaluate all RHS expressions. */
        flow3_visit_bracket_initializer_list(ctx, p_expression->braced_initializer);


        /* 4. Compound literal object is now fully initialised; mark as ANY. */
        flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
            &p_expression->object, ANY_VALUE, FLOW3_ANY, line);
        break;
    }

    case EXPR_UNARY_STATIC_ASSERTION:
        assert(p_expression->static_assertion);
        flow3_visit_static_assertion(ctx, p_expression->static_assertion);
        break;

    case EXPR_UNARY_ALIGNOF_EXPRESSION:
        assert(p_expression->right);
        flow3_visit_expression(ctx, p_expression->right);
        //TODO returns a constant
        break;

    case EXPR_UNARY_ALIGNOF_TYPE:
        //TODO returns a constant

        break;

    case EXPR_UNARY_ASSERT:
        /*
         * assert(expr) is equivalent to:
         *   if (!expr) exit();   // exit does not return
         *
         * So after assert, only the TRUE branch of expr is reachable.
         * We apply the true-branch refinements to the current map and
         * discard the false branch (it is a dead end, like exit()).
         */
        if (p_expression->right)
        {
            struct flow3_branch_pair assert_pair = flow3_visit_expression(ctx, p_expression->right);

            /* The false branch is dead (assert would have aborted).
               Merge only the true outcome back into p_before. */
            flow3_map_merge_a_b(p_before, assert_pair.p_true, assert_pair.p_true);
            ctx->p_current_flow3_map = p_before;
        }
        break;

    case EXPR_UNARY_SIZEOF_EXPRESSION:
        assert(p_expression->right);

        const bool t2 = ctx->expression_is_not_evaluated;
        ctx->expression_is_not_evaluated = true;
        flow3_visit_expression(ctx, p_expression->right);
        ctx->expression_is_not_evaluated = t2;
        //TODO return a constant
        break;

    case EXPR_UNARY_NEG:
    case EXPR_UNARY_PLUS:
        assert(p_expression->right != NULL);
        /*
         * Visit the child first so that any sub-expression (e.g. -(a + b))
         * is fully evaluated and its constant value — if any — is propagated
         * into p_expression->right->object before we inspect it.
         */
        flow3_visit_expression(ctx, p_expression->right);
        if (object_has_constant_value(&p_expression->right->object))
        {
            const long long rv = object_to_signed_long_long(&p_expression->right->object);
            const long long result = (p_expression->expression_type == EXPR_UNARY_NEG) ? -rv : rv;
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                &p_expression->object, result, FLOW3_EQUAL,
                p_expression->first_token->line);
        }
        else
        {
            /* Operand value unknown — result is also unknown. */
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                &p_expression->object, ANY_VALUE, FLOW3_ANY,
                p_expression->first_token->line);
        }
        flow3_map_remove(ctx->p_current_flow3_map, &p_expression->right->object);
        break;

    case EXPR_UNARY_NOT:
    {
        assert(p_expression->right != NULL);

        /*
         * Visit the child first so that any sub-expression is fully evaluated
         * and its constant value — if any — is propagated into
         * p_expression->right->object before we inspect it.
         */
        struct flow3_branch_pair child = flow3_visit_expression(ctx, p_expression->right);

        if (object_has_constant_value(&p_expression->right->object))
        {
            /* Both sub-expression and its value are now known: fold directly. */
            const long long rv = object_to_signed_long_long(&p_expression->right->object);
            const long long result = rv ? 0 : 1;
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                &p_expression->object, result, FLOW3_EQUAL,
                p_expression->first_token->line);
            flow3_map_remove(ctx->p_current_flow3_map, &p_expression->right->object);
            if (result != 0)
            {
                struct flow3_map* p_dead = flow3_map_arena_new(&ctx->flow3_map_arena, p_before, "not-dead-false");
                return (struct flow3_branch_pair) { ctx->p_current_flow3_map, p_dead };
            }
            else
            {
                struct flow3_map* p_dead = flow3_map_arena_new(&ctx->flow3_map_arena, p_before, "not-dead-true");
                return (struct flow3_branch_pair) { p_dead, ctx->p_current_flow3_map };
            }
        }

        flow3_map_remove(ctx->p_current_flow3_map, &p_expression->right->object);
        /* NOT swaps the two branches. */
        return (struct flow3_branch_pair) { child.p_false, child.p_true };
    }

    case EXPR_CHECKED:
    {
        assert(p_expression->left != NULL);
        struct flow3_branch_pair checked_pair = flow3_visit_expression(ctx, p_expression->left);
        flow3_exit_block_visit_defer_list(ctx, &p_expression->defer_list, p_expression->first_token);
        flow3_defer_list_set_end_of_lifetime(ctx, &p_expression->defer_list, p_expression->first_token);
        return checked_pair;
    }

    case EXPR_UNARY_SIZEOF_TYPE:
    case EXPR_UNARY_COUNTOF:
        //todo return constnat
        break;

    case EXPR_UNARY_INCREMENT:
    case EXPR_UNARY_DECREMENT:
    case EXPR_POSTFIX_INCREMENT:
    case EXPR_POSTFIX_DECREMENT:
    {
        struct expression* p_operand = NULL;

        if (p_expression->expression_type == EXPR_UNARY_INCREMENT ||
            p_expression->expression_type == EXPR_UNARY_DECREMENT)
        {
            assert(p_expression->right != NULL);
            p_operand = p_expression->right;
        }
        else // postfix
        {
            assert(p_expression->left != NULL);
            p_operand = p_expression->left;
        }

        // Evaluate the operand first.
        flow3_visit_expression(ctx, p_operand);

        bool is_postfix = (p_expression->expression_type == EXPR_POSTFIX_INCREMENT ||
                           p_expression->expression_type == EXPR_POSTFIX_DECREMENT);
        bool is_increment = (p_expression->expression_type == EXPR_UNARY_INCREMENT ||
                             p_expression->expression_type == EXPR_POSTFIX_INCREMENT);

        // Resolve the operand's object to its actual alternatives.
        const struct object* p_obj = &p_operand->object;
        const struct flow3_key_alternatives* p_entry =
            flow3_map_search_up(ctx->p_current_flow3_map, p_obj);
        if (p_entry == NULL)
        {
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                p_obj, ANY_VALUE, FLOW3_ANY, p_expression->first_token->line);
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                &p_expression->object, ANY_VALUE, FLOW3_ANY,
                p_expression->first_token->line);
            break;
        }

        // Follow REF indirections.
        const struct flow3_key_alternatives* p_resolved = p_entry;
        if (p_entry->alternatives.size == 1 &&
            p_entry->alternatives.data[0].relation == FLOW3_EQUAL &&
            p_entry->alternatives.data[0].value.kind == FLOW3_VALUE_REF &&
            p_entry->alternatives.data[0].value.as.p != NULL)
        {
            p_resolved = flow3_map_search_up(ctx->p_current_flow3_map,
                                             p_entry->alternatives.data[0].value.as.p);
            if (p_resolved == NULL)
            {
                flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                    p_obj, ANY_VALUE, FLOW3_ANY, p_expression->first_token->line);
                flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                    &p_expression->object, ANY_VALUE, FLOW3_ANY,
                    p_expression->first_token->line);
                break;
            }
        }

        const struct object* p_actual_obj = p_resolved->p_obj_key;
        struct flow3_alternatives new_var_alts = { 0 };
        struct flow3_alternatives new_result_alts = { 0 };

        for (int i = 0; i < p_resolved->alternatives.size; i++)
        {
            const struct flow3_alternative* alt = &p_resolved->alternatives.data[i];

            if (alt->imaginary == FLOW3_IMAGINARY_ABSENT || alt->relation == FLOW3_UNINITIALIZED)
            {
                flow3_alternatives_add_i(&new_var_alts, ANY_VALUE, FLOW3_ANY, FLOW3_IMAGINARY_NONE,
                    ctx->p_current_flow3_map, p_expression->first_token->line);
                flow3_alternatives_add_i(&new_result_alts, ANY_VALUE, FLOW3_ANY, FLOW3_IMAGINARY_NONE,
                    ctx->p_current_flow3_map, p_expression->first_token->line);
                continue;
            }

            if (alt->relation == FLOW3_EQUAL &&
                (alt->value.kind == FLOW3_VALUE_SIGNED ||
                    alt->value.kind == FLOW3_VALUE_UNSIGNED))
            {
                long long old = (alt->value.kind == FLOW3_VALUE_SIGNED)
                    ? alt->value.as.i
                    : (long long)alt->value.as.u;
                long long new_val = is_increment ? old + 1 : old - 1;

                flow3_alternatives_add_i(&new_var_alts, new_val, FLOW3_EQUAL, FLOW3_IMAGINARY_NONE,
                    ctx->p_current_flow3_map, p_expression->first_token->line);

                long long result_val = is_postfix ? old : new_val;
                flow3_alternatives_add_i(&new_result_alts, result_val, FLOW3_EQUAL, FLOW3_IMAGINARY_NONE,
                    ctx->p_current_flow3_map, p_expression->first_token->line);
            }
            else
            {
                flow3_alternatives_add_i(&new_var_alts, ANY_VALUE, FLOW3_ANY, FLOW3_IMAGINARY_NONE,
                    ctx->p_current_flow3_map, p_expression->first_token->line);
                flow3_alternatives_add_i(&new_result_alts, ANY_VALUE, FLOW3_ANY, FLOW3_IMAGINARY_NONE,
                    ctx->p_current_flow3_map, p_expression->first_token->line);
            }
        }

        // Replace the variable's alternatives.
        struct flow3_key_alternatives* p_var_entry =
            flow3_map_find_add(ctx->p_current_flow3_map, p_actual_obj);
        if (p_var_entry)
        {
            flow3_alternatives_clear(&p_var_entry->alternatives);
            p_var_entry->alternatives = new_var_alts;
        }

        // Replace the expression result's alternatives.
        struct flow3_key_alternatives* p_result_entry =
            flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
        if (p_result_entry)
        {
            flow3_alternatives_clear(&p_result_entry->alternatives);
            p_result_entry->alternatives = new_result_alts;
        }

        // Remove the temporary operand object entry.
        flow3_map_remove(ctx->p_current_flow3_map, &p_operand->object);
        break;
    }
    break;

    case EXPR_UNARY_BITNOT:
        assert(p_expression->right != NULL);
        /*
         * Visit the child first so that any sub-expression is fully evaluated
         * and its constant value — if any — is propagated into
         * p_expression->right->object before we inspect it.
         */
        flow3_visit_expression(ctx, p_expression->right);
        if (object_has_constant_value(&p_expression->right->object))
        {
            const long long rv = object_to_signed_long_long(&p_expression->right->object);
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                &p_expression->object, ~rv, FLOW3_EQUAL,
                p_expression->first_token->line);
        }
        else
        {
            /* Operand value unknown — result is also unknown. */
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                &p_expression->object, ANY_VALUE, FLOW3_ANY,
                p_expression->first_token->line);
        }
        flow3_map_remove(ctx->p_current_flow3_map, &p_expression->right->object);
        break;

    case EXPR_UNARY_ADDRESSOF:
    {
        flow3_visit_expression(ctx, p_expression->right);

        struct flow3_key_alternatives* result_entry = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
        flow3_alternatives_clear(&result_entry->alternatives);

        const struct flow3_key_alternatives* p_right_alternatives = flow3_map_search_up(ctx->p_current_flow3_map, &p_expression->right->object);

        for (int i = 0; i < p_right_alternatives->alternatives.size; i++)
        {
            const struct flow3_alternative* p_right_alternative = &p_right_alternatives->alternatives.data[i];
            if (p_right_alternative->relation == FLOW3_EQUAL &&
                p_right_alternative->value.kind == FLOW3_VALUE_REF &&
                p_right_alternative->value.as.p != NULL)
            {
                flow3_alternatives_add_object_ptr(&result_entry->alternatives,
                    p_right_alternative->value.as.p,
                    FLOW3_EQUAL,
                    FLOW3_IMAGINARY_NONE,
                    ctx->p_current_flow3_map,
                    p_expression->first_token->line);
            }
        }
        flow3_map_remove(ctx->p_current_flow3_map, &p_expression->right->object);
    }

    break;

    case EXPR_UNARY_CONTENT:
    {
        assert(p_expression->right != NULL);

        flow3_visit_expression(ctx, p_expression->right);

        struct flow3_key_alternatives* result_entry = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
        flow3_alternatives_clear(&result_entry->alternatives);
        struct marker marker = expression_to_marker(p_expression);

        const struct flow3_key_alternatives* p_right_alternatives = flow3_map_search_up(ctx->p_current_flow3_map, &p_expression->right->object);

        for (int i = 0; i < p_right_alternatives->alternatives.size; i++)
        {
            const struct flow3_alternative* p_right_alt = &p_right_alternatives->alternatives.data[i];

            if (p_right_alt->imaginary == FLOW3_IMAGINARY_ABSENT)
                continue;
            if (p_right_alt->value.kind == FLOW3_VALUE_REF)
            {
                const struct flow3_key_alternatives* p_right_alternatives2 = flow3_map_search_up(ctx->p_current_flow3_map,
                    p_right_alt->value.as.p);

                for (int j = 0; j < p_right_alternatives2->alternatives.size; j++)
                {
                    const struct flow3_alternative* p_right_alt2 = &p_right_alternatives2->alternatives.data[j];

                    if (flow3_alternative_can_be_zero(p_right_alt2))
                    {
                        diagnostic(W_FLOW_NULL_DEREFERENCE, ctx->ctx, NULL, &marker,
                                                        "possible null pointer dereference");

                    }

                    flow3_alternatives_add_object_ref(&result_entry->alternatives,
                                                    p_right_alt2->value.as.p,
                                                    FLOW3_IMAGINARY_NONE,
                                                    ctx->p_current_flow3_map,
                                                    p_expression->first_token->line);
                }
            }
            else
            {
                //*0 ?
                // *(p ++)
            }
        }

        flow3_map_remove(ctx->p_current_flow3_map, &p_expression->right->object);
    }
    break;

    case EXPR_ASSIGNMENT_ASSIGN:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        // Evaluate both sides (side effects, constant folding)
        flow3_visit_expression(ctx, p_expression->left);
        flow3_visit_expression(ctx, p_expression->right);

        flow3_check_assigment(ctx, p_expression->left, p_expression->right);
        // ---- Branch maps for boolean context ----
        struct flow3_map* p_true = flow3_narrow_map(&ctx->flow3_map_arena,
                                                    ctx->p_current_flow3_map,
                                                    &p_expression->left->object,
                                                    true,
                                                    "assign-true");
        struct flow3_map* p_false = flow3_narrow_map(&ctx->flow3_map_arena,
                                                     ctx->p_current_flow3_map,
                                                     &p_expression->left->object,
                                                      false,
                                                       "assign-false");

        // Remove temporary entries for left and right sub‑expressions
        flow3_map_remove(ctx->p_current_flow3_map, &p_expression->left->object);
        flow3_map_remove(ctx->p_current_flow3_map, &p_expression->right->object);

        return (struct flow3_branch_pair) { p_true, p_false };
    }

    case EXPR_ASSIGNMENT_PLUS_ASSIGN:
    case EXPR_ASSIGNMENT_MINUS_ASSIGN:
    case EXPR_ASSIGNMENT_MULTI_ASSIGN:
    case EXPR_ASSIGNMENT_DIV_ASSIGN:
    case EXPR_ASSIGNMENT_MOD_ASSIGN:
    case EXPR_ASSIGNMENT_SHIFT_LEFT_ASSIGN:
    case EXPR_ASSIGNMENT_SHIFT_RIGHT_ASSIGN:
    case EXPR_ASSIGNMENT_AND_ASSIGN:
    case EXPR_ASSIGNMENT_OR_ASSIGN:
    case EXPR_ASSIGNMENT_NOT_ASSIGN:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);



        struct flow3_branch_pair lhs_pair2 = flow3_visit_expression(ctx, p_expression->left);
        flow3_visit_expression(ctx, p_expression->right);

        const struct object* p_left_obj = object_get_referenced(&p_expression->left->object);
        struct flow3_key_alternatives* p_lhs_entry = flow3_map_search_up(ctx->p_current_flow3_map, p_left_obj);

        bool computed = false;
        if (p_lhs_entry &&
            p_lhs_entry->alternatives.size == 1 &&
            p_lhs_entry->alternatives.data[0].relation == FLOW3_EQUAL &&
            object_has_known_value(&p_expression->right->object))
        {
            const signed long long lv = p_lhs_entry->alternatives.data[0].value.as.i;
            const signed long long rv = object_to_signed_long_long(&p_expression->right->object);
            signed long long result = lv;

            switch (p_expression->expression_type)
            {
            case EXPR_ASSIGNMENT_PLUS_ASSIGN:       result = lv + rv;               break;
            case EXPR_ASSIGNMENT_MINUS_ASSIGN:      result = lv - rv;               break;
            case EXPR_ASSIGNMENT_MULTI_ASSIGN:      result = lv * rv;               break;
            case EXPR_ASSIGNMENT_DIV_ASSIGN:        result = rv != 0 ? lv / rv : lv; break;
            case EXPR_ASSIGNMENT_MOD_ASSIGN:        result = rv != 0 ? lv % rv : lv; break;
            case EXPR_ASSIGNMENT_SHIFT_LEFT_ASSIGN: result = lv << rv;              break;
            case EXPR_ASSIGNMENT_SHIFT_RIGHT_ASSIGN:result = lv >> rv;              break;
            case EXPR_ASSIGNMENT_AND_ASSIGN:        result = lv & rv;               break;
            case EXPR_ASSIGNMENT_OR_ASSIGN:         result = lv | rv;               break;
            case EXPR_ASSIGNMENT_NOT_ASSIGN:        result = lv ^ rv;               break;
            default: break;
            }

            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                p_left_obj, result, FLOW3_EQUAL,
                p_expression->right->first_token->line);
            computed = true;
        }

        if (!computed)
        {
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                p_left_obj, ANY_VALUE, FLOW3_ANY,
                p_expression->right->first_token->line);
        }

        flow3_map_remove(ctx->p_current_flow3_map, &p_expression->left->object);
        flow3_map_remove(ctx->p_current_flow3_map, &p_expression->right->object);
        return lhs_pair2;
    }
    case EXPR_MULTIPLICATIVE_MULT:
    case EXPR_MULTIPLICATIVE_DIV:
    case EXPR_MULTIPLICATIVE_MOD:
    {
        char op = 0;
        switch (p_expression->expression_type)
        {
        case EXPR_MULTIPLICATIVE_MULT: op = '*'; break;
        case EXPR_MULTIPLICATIVE_DIV:  op = '/'; break;
        case EXPR_MULTIPLICATIVE_MOD:  op = '%'; break;
        default: break;
        }

        flow3_visit_expression(ctx, p_expression->left);
        flow3_visit_expression(ctx, p_expression->right);

        flow3_evaluate_binary_arithmetic(ctx,
                                         p_expression->left,
                                         p_expression->right,
                                         p_expression,
                                         op);

        flow3_map_remove(ctx->p_current_flow3_map, &p_expression->left->object);
        flow3_map_remove(ctx->p_current_flow3_map, &p_expression->right->object);
        break;
    }


    case EXPR_ADDITIVE_PLUS:
    case EXPR_ADDITIVE_MINUS:
    {
        char op = 0;
        switch (p_expression->expression_type)
        {
        case EXPR_ADDITIVE_PLUS:  op = '+'; break;
        case EXPR_ADDITIVE_MINUS: op = '-'; break;
        default: break;
        }

        flow3_visit_expression(ctx, p_expression->left);
        flow3_visit_expression(ctx, p_expression->right);

        flow3_evaluate_binary_arithmetic(ctx,
                                         p_expression->left,
                                         p_expression->right,
                                         p_expression,
                                         op);

        flow3_map_remove(ctx->p_current_flow3_map, &p_expression->left->object);
        flow3_map_remove(ctx->p_current_flow3_map, &p_expression->right->object);
        break;
    }


    case EXPR_CAST:
    {
        assert(p_expression->left != NULL);

        flow3_visit_expression(ctx, p_expression->left);

        const struct type* p_target_type = &p_expression->type;
        const struct object* p_src_obj = &p_expression->left->object;
        const struct flow3_key_alternatives* p_src_entry =
            flow3_map_search_up(ctx->p_current_flow3_map, p_src_obj);

        if (p_src_entry == NULL)
        {
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                &p_expression->object, ANY_VALUE, FLOW3_ANY,
                p_expression->first_token->line);
            break;
        }

        // Resolve REF indirections
        const struct flow3_key_alternatives* p_src_resolved = p_src_entry;
        if (p_src_entry->alternatives.size == 1 &&
            p_src_entry->alternatives.data[0].relation == FLOW3_EQUAL &&
            p_src_entry->alternatives.data[0].value.kind == FLOW3_VALUE_REF &&
            p_src_entry->alternatives.data[0].value.as.p != NULL)
        {
            p_src_resolved = flow3_map_search_up(ctx->p_current_flow3_map,
                                                 p_src_entry->alternatives.data[0].value.as.p);
            if (p_src_resolved == NULL)
            {
                flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                    &p_expression->object, ANY_VALUE, FLOW3_ANY,
                    p_expression->first_token->line);
                break;
            }
        }

        struct flow3_alternatives new_alts = { 0 };
        bool all_handled = true;

        for (int i = 0; i < p_src_resolved->alternatives.size; i++)
        {
            const struct flow3_alternative* alt = &p_src_resolved->alternatives.data[i];

            if (alt->imaginary == FLOW3_IMAGINARY_ABSENT || alt->relation == FLOW3_UNINITIALIZED)
            {
                all_handled = false;
                break;
            }

            if (alt->relation == FLOW3_EQUAL)
            {
                // Integer source
                if (alt->value.kind == FLOW3_VALUE_SIGNED ||
                    alt->value.kind == FLOW3_VALUE_UNSIGNED)
                {
                    long long val = (alt->value.kind == FLOW3_VALUE_SIGNED)
                        ? alt->value.as.i
                        : (long long)alt->value.as.u;
                    if (type_is_integer(p_target_type))
                    {
                        long long casted = flow3_cast_integer_value(ctx, val, p_target_type);
                        struct flow3_value v = { .kind = FLOW3_VALUE_SIGNED, .as.i = casted };
                        struct flow3_alternative a = { .value = v, .relation = FLOW3_EQUAL, .imaginary = alt->imaginary };
                        flow3_alternatives_add(&new_alts, &a,
                                               ctx->p_current_flow3_map,
                                               p_expression->first_token->line);
                    }
                    else if (type_is_pointer(p_target_type))
                    {
                        // Integer to pointer – cannot safely compute address; set to ANY.
                        all_handled = false;
                        break;
                    }
                    else
                    {
                        all_handled = false;
                        break;
                    }
                }
                // Pointer source
                else if (alt->value.kind == FLOW3_VALUE_PTR)
                {
                    if (type_is_pointer(p_target_type))
                    {
                        // Preserve the pointer value
                        flow3_alternatives_add(&new_alts, alt,
                                               ctx->p_current_flow3_map,
                                               p_expression->first_token->line);
                    }
                    else if (type_is_integer(p_target_type))
                    {
                        // Pointer to integer – cannot safely compute integer; set to ANY.
                        all_handled = false;
                        break;
                    }
                    else
                    {
                        all_handled = false;
                        break;
                    }
                }
                else
                {
                    all_handled = false;
                    break;
                }
            }
            else
            {
                // NOT_EQUAL or ANY – cannot cast exactly
                all_handled = false;
                break;
            }
        }

        if (all_handled && new_alts.size > 0)
        {
            struct flow3_key_alternatives* dst =
                flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
            flow3_alternatives_clear(&dst->alternatives);
            dst->alternatives = new_alts; /* move */
        }
        else
        {
            flow3_alternatives_clear(&new_alts);
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                &p_expression->object, ANY_VALUE, FLOW3_ANY,
                p_expression->first_token->line);
        }

        flow3_map_remove(ctx->p_current_flow3_map, &p_expression->left->object);
        break;
    }
    break;

    case EXPR_SHIFT_RIGHT:
    case EXPR_SHIFT_LEFT:
    {
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);

        bool is_left = (p_expression->expression_type == EXPR_SHIFT_LEFT);

        flow3_visit_expression(ctx, p_expression->left);
        flow3_visit_expression(ctx, p_expression->right);

        // Resolve left and right to concrete alternatives (following REF)
        const struct flow3_key_alternatives* p_left_entry =
            flow3_map_search_up(ctx->p_current_flow3_map, &p_expression->left->object);
        const struct flow3_key_alternatives* p_right_entry =
            flow3_map_search_up(ctx->p_current_flow3_map, &p_expression->right->object);

        const struct flow3_key_alternatives* p_left_resolved = p_left_entry;
        if (p_left_entry && p_left_entry->alternatives.size == 1 &&
            p_left_entry->alternatives.data[0].relation == FLOW3_EQUAL &&
            p_left_entry->alternatives.data[0].value.kind == FLOW3_VALUE_REF &&
            p_left_entry->alternatives.data[0].value.as.p != NULL)
        {
            p_left_resolved = flow3_map_search_up(ctx->p_current_flow3_map,
                                                  p_left_entry->alternatives.data[0].value.as.p);
        }

        const struct flow3_key_alternatives* p_right_resolved = p_right_entry;
        if (p_right_entry && p_right_entry->alternatives.size == 1 &&
            p_right_entry->alternatives.data[0].relation == FLOW3_EQUAL &&
            p_right_entry->alternatives.data[0].value.kind == FLOW3_VALUE_REF &&
            p_right_entry->alternatives.data[0].value.as.p != NULL)
        {
            p_right_resolved = flow3_map_search_up(ctx->p_current_flow3_map,
                                                   p_right_entry->alternatives.data[0].value.as.p);
        }

        // Extract single constant values if available
        long long lv = 0, rv = 0;
        bool left_is_const = false, right_is_const = false;
        if (p_left_resolved)
        {
            left_is_const = flow3_alternatives_is_single_constant(&p_left_resolved->alternatives, &lv);
        }
        if (p_right_resolved)
        {
            right_is_const = flow3_alternatives_is_single_constant(&p_right_resolved->alternatives, &rv);
        }

        // Both sides known: fold if shift count is within valid range (0..63)
        if (left_is_const && right_is_const)
        {
            if (rv < 0 || rv >= (long long)(sizeof(long long) * 8))
            {
                // Shift count out of range: result is undefined – set to ANY to avoid incorrect folding.
                flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                    &p_expression->object, ANY_VALUE, FLOW3_ANY,
                    p_expression->first_token->line);
            }
            else
            {
                long long result;
                if (is_left)
                    result = lv << rv;
                else
                    result = lv >> rv;
                flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                    &p_expression->object, result, FLOW3_EQUAL,
                    p_expression->first_token->line);
            }
        }
        // x << 0  or x >> 0  -> copy left alternatives (identity)
        else if (right_is_const && rv == 0)
        {
            if (p_left_resolved && p_left_resolved->alternatives.size > 0)
            {
                struct flow3_key_alternatives* dst =
                    flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                flow3_alternatives_clear(&dst->alternatives);
                flow3_alternatives_append(&dst->alternatives, &p_left_resolved->alternatives);
            }
            else
            {
                flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                    &p_expression->object, ANY_VALUE, FLOW3_ANY,
                    p_expression->first_token->line);
            }
        }
        // 0 << x  or 0 >> x  -> 0, but only if x is a known non-negative constant (otherwise undefined)
        else if (left_is_const && lv == 0 && right_is_const && rv >= 0)
        {
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                &p_expression->object, 0, FLOW3_EQUAL,
                p_expression->first_token->line);
        }
        // Otherwise, result is unknown
        else
        {
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                &p_expression->object, ANY_VALUE, FLOW3_ANY,
                p_expression->first_token->line);
        }

        // Remove temporary operand entries
        flow3_map_remove(ctx->p_current_flow3_map, &p_expression->left->object);
        flow3_map_remove(ctx->p_current_flow3_map, &p_expression->right->object);
        break;
    }

    break;
    case EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN:
    case EXPR_RELATIONAL_LESS_OR_EQUAL_THAN:
    case EXPR_RELATIONAL_BIGGER_THAN:
    case EXPR_RELATIONAL_LESS_THAN:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        flow3_visit_expression(ctx, p_expression->left);
        flow3_visit_expression(ctx, p_expression->right);

        // Resolve REF indirections to get concrete alternatives
        const struct flow3_key_alternatives* p_left_entry =
            flow3_map_search_up(ctx->p_current_flow3_map, &p_expression->left->object);
        const struct flow3_key_alternatives* p_right_entry =
            flow3_map_search_up(ctx->p_current_flow3_map, &p_expression->right->object);

        const struct flow3_key_alternatives* p_left_resolved = p_left_entry;
        if (p_left_entry && p_left_entry->alternatives.size == 1 &&
            p_left_entry->alternatives.data[0].relation == FLOW3_EQUAL &&
            p_left_entry->alternatives.data[0].value.kind == FLOW3_VALUE_REF &&
            p_left_entry->alternatives.data[0].value.as.p != NULL)
        {
            p_left_resolved = flow3_map_search_up(ctx->p_current_flow3_map,
                                                  p_left_entry->alternatives.data[0].value.as.p);
        }
        const struct flow3_key_alternatives* p_right_resolved = p_right_entry;
        if (p_right_entry && p_right_entry->alternatives.size == 1 &&
            p_right_entry->alternatives.data[0].relation == FLOW3_EQUAL &&
            p_right_entry->alternatives.data[0].value.kind == FLOW3_VALUE_REF &&
            p_right_entry->alternatives.data[0].value.as.p != NULL)
        {
            p_right_resolved = flow3_map_search_up(ctx->p_current_flow3_map,
                                                   p_right_entry->alternatives.data[0].value.as.p);
        }

        // Attempt to fold the relational expression
        int fold_result = -1;
        if (p_left_resolved && p_left_resolved->alternatives.size > 0 &&
            p_right_resolved && p_right_resolved->alternatives.size > 0)
        {
            fold_result = flow3_evaluate_relational(&p_left_resolved->alternatives,
                                                    &p_right_resolved->alternatives,
                                                    p_expression->expression_type);
        }

        if (fold_result != -1)
        {
            // Expression always true or always false
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                &p_expression->object, fold_result ? 1 : 0, FLOW3_EQUAL,
                p_expression->first_token->line);
            struct flow3_map* p_dead = flow3_map_arena_new(&ctx->flow3_map_arena,
                                                           ctx->p_current_flow3_map,
                                                           "rel-dead");
            return (struct flow3_branch_pair)
            {
                fold_result ? ctx->p_current_flow3_map : p_dead,
                    fold_result ? p_dead : ctx->p_current_flow3_map
            };
        }

        // Not foldable: result is unknown
        flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
            &p_expression->object, ANY_VALUE, FLOW3_ANY,
            p_expression->first_token->line);
        return (struct flow3_branch_pair) { ctx->p_current_flow3_map, ctx->p_current_flow3_map };
    }
    break;
    case EXPR_EQUALITY_NOT_EQUAL:
    case EXPR_EQUALITY_EQUAL:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        flow3_visit_expression(ctx, p_expression->left);
        flow3_visit_expression(ctx, p_expression->right);

        struct flow3_key_alternatives* p_result_alternatives =
            flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
        flow3_alternatives_clear(&p_result_alternatives->alternatives);

        const struct flow3_key_alternatives* p_left_alts =
            flow3_map_search_up(ctx->p_current_flow3_map, &p_expression->left->object);
        const struct flow3_key_alternatives* p_right_alts =
            flow3_map_search_up(ctx->p_current_flow3_map, &p_expression->right->object);

        /* Resolve REF indirections to get the concrete object entries */
        const struct flow3_key_alternatives* p_left_resolved = p_left_alts;
        if (p_left_alts && p_left_alts->alternatives.size == 1 &&
            p_left_alts->alternatives.data[0].relation == FLOW3_EQUAL &&
            p_left_alts->alternatives.data[0].value.kind == FLOW3_VALUE_REF &&
            p_left_alts->alternatives.data[0].value.as.p != NULL)
        {
            p_left_resolved = flow3_map_search_up(ctx->p_current_flow3_map,
                                                  p_left_alts->alternatives.data[0].value.as.p);
        }
        const struct flow3_key_alternatives* p_right_resolved = p_right_alts;
        if (p_right_alts && p_right_alts->alternatives.size == 1 &&
            p_right_alts->alternatives.data[0].relation == FLOW3_EQUAL &&
            p_right_alts->alternatives.data[0].value.kind == FLOW3_VALUE_REF &&
            p_right_alts->alternatives.data[0].value.as.p != NULL)
        {
            p_right_resolved = flow3_map_search_up(ctx->p_current_flow3_map,
                                                   p_right_alts->alternatives.data[0].value.as.p);
        }

        struct flow3_map* p_true = flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, "eq-true");
        struct flow3_map* p_false = flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, "eq-false");

        bool is_equal_op = (p_expression->expression_type == EXPR_EQUALITY_EQUAL);

        /* -------- Try to fold the comparison (always true/false) -------- */
        long long left_const = 0, right_const = 0;
        bool left_is_const = p_left_resolved &&
            flow3_alternatives_is_single_constant(&p_left_resolved->alternatives, &left_const);
        bool right_is_const = p_right_resolved &&
            flow3_alternatives_is_single_constant(&p_right_resolved->alternatives, &right_const);

        if (left_is_const && p_right_resolved)
        {
            int fold = flow3_evaluate_equality_against_constant(&p_right_resolved->alternatives,
                                                                left_const, is_equal_op);
            if (fold != -1)
            {
                flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                    &p_expression->object, fold ? 1 : 0, FLOW3_EQUAL,
                    p_expression->first_token->line);
                struct flow3_map* p_dead = flow3_map_arena_new(&ctx->flow3_map_arena,
                                                               ctx->p_current_flow3_map,
                                                               "eq-dead");
                return (struct flow3_branch_pair)
                {
                    fold ? ctx->p_current_flow3_map : p_dead,
                        fold ? p_dead : ctx->p_current_flow3_map
                };
            }
        }
        else if (right_is_const && p_left_resolved)
        {
            int fold = flow3_evaluate_equality_against_constant(&p_left_resolved->alternatives,
                                                                right_const, is_equal_op);
            if (fold != -1)
            {
                flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                    &p_expression->object, fold ? 1 : 0, FLOW3_EQUAL,
                    p_expression->first_token->line);
                struct flow3_map* p_dead = flow3_map_arena_new(&ctx->flow3_map_arena,
                                                               ctx->p_current_flow3_map,
                                                               "eq-dead");
                return (struct flow3_branch_pair)
                {
                    fold ? ctx->p_current_flow3_map : p_dead,
                        fold ? p_dead : ctx->p_current_flow3_map
                };
            }
        }

        /* If both are constants, handle as fallback */
        if (left_is_const && right_is_const)
        {
            bool result = (left_const == right_const) ? is_equal_op : !is_equal_op;
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                &p_expression->object, result ? 1 : 0, FLOW3_EQUAL,
                p_expression->first_token->line);
            struct flow3_map* p_dead = flow3_map_arena_new(&ctx->flow3_map_arena,
                                                           ctx->p_current_flow3_map,
                                                           "eq-dead");
            return (struct flow3_branch_pair)
            {
                result ? ctx->p_current_flow3_map : p_dead,
                    result ? p_dead : ctx->p_current_flow3_map
            };
        }

        /* -------- Narrowing for if‑conditions (when one side is a constant) -------- */
        if (left_is_const && p_right_resolved)
        {
            const struct object* p_right_obj = p_right_resolved->p_obj_key;
            struct flow3_alternatives true_alts = { 0 }, false_alts = { 0 };
            narrow_by_constant(&p_right_resolved->alternatives, left_const, is_equal_op,
                               &true_alts, &false_alts, p_true, p_expression->first_token->line);
            if (true_alts.size > 0)
            {
                struct flow3_key_alternatives* e = flow3_map_find_add(p_true, p_right_obj);
                flow3_alternatives_clear(&e->alternatives);
                e->alternatives = true_alts;
            }
            if (false_alts.size > 0)
            {
                struct flow3_key_alternatives* e = flow3_map_find_add(p_false, p_right_obj);
                flow3_alternatives_clear(&e->alternatives);
                e->alternatives = false_alts;
            }
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                &p_expression->object, ANY_VALUE, FLOW3_ANY,
                p_expression->first_token->line);
            return (struct flow3_branch_pair) { p_true, p_false };
        }
        else if (right_is_const && p_left_resolved)
        {
            const struct object* p_left_obj = p_left_resolved->p_obj_key;
            struct flow3_alternatives true_alts = { 0 }, false_alts = { 0 };
            narrow_by_constant(&p_left_resolved->alternatives, right_const, is_equal_op,
                               &true_alts, &false_alts, p_true, p_expression->first_token->line);
            if (true_alts.size > 0)
            {
                struct flow3_key_alternatives* e = flow3_map_find_add(p_true, p_left_obj);
                flow3_alternatives_clear(&e->alternatives);
                e->alternatives = true_alts;
            }
            if (false_alts.size > 0)
            {
                struct flow3_key_alternatives* e = flow3_map_find_add(p_false, p_left_obj);
                flow3_alternatives_clear(&e->alternatives);
                e->alternatives = false_alts;
            }
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                &p_expression->object, ANY_VALUE, FLOW3_ANY,
                p_expression->first_token->line);
            return (struct flow3_branch_pair) { p_true, p_false };
        }

        /* -------- Fallback: unknown -------- */
        flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
            &p_expression->object, ANY_VALUE, FLOW3_ANY,
            p_expression->first_token->line);
        return (struct flow3_branch_pair) { ctx->p_current_flow3_map, ctx->p_current_flow3_map };
    }
    break;

    case EXPR_LOGICAL_OR:
    {
        /*
         * L || R
         *   true  = merge(left_true, right_true_from_left_false)
         *           (left was true, OR left was false but right was true)
         *   false = right_false_from_left_false
         *           (both were false)
         */
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);


        if (object_has_constant_value(&p_expression->left->object) &&
            object_has_constant_value(&p_expression->right->object))
        {
            const long long result = (object_to_signed_long_long(&p_expression->left->object) ||
                                      object_to_signed_long_long(&p_expression->right->object)) ? 1 : 0;
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                &p_expression->object, result, FLOW3_EQUAL,
                p_expression->first_token->line);
            if (result != 0)
            {
                struct flow3_map* p_dead = flow3_map_arena_new(&ctx->flow3_map_arena, p_before, "or-dead-false");
                return (struct flow3_branch_pair) { ctx->p_current_flow3_map, p_dead };
            }
            else
            {
                struct flow3_map* p_dead = flow3_map_arena_new(&ctx->flow3_map_arena, p_before, "or-dead-true");
                return (struct flow3_branch_pair) { p_dead, ctx->p_current_flow3_map };
            }
        }

        struct flow3_branch_pair left_pair = flow3_visit_expression(ctx, p_expression->left);

        /* Visit right on the false map of left (right only runs when left is false). */
        ctx->p_current_flow3_map = left_pair.p_false;
        struct flow3_branch_pair right_pair = flow3_visit_expression(ctx, p_expression->right);
        ctx->p_current_flow3_map = p_before;

        /*
         * true  = merge(left_true, right_true)
         * false = right_false
         */
        struct flow3_map* p_or_true = flow3_map_arena_new(&ctx->flow3_map_arena, p_before, "or-true");
        flow3_map_merge_a_b(p_or_true, left_pair.p_true, right_pair.p_true);



        return (struct flow3_branch_pair) { p_or_true, right_pair.p_false };
    }

    case EXPR_LOGICAL_AND:
    {
        /*
         * L && R
         *   true  = right_true_from_left_true
         *           (both were true)
         *   false = merge(left_false, right_false_from_left_true)
         *           (left was false, OR left was true but right was false)
         */
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);


        if (object_has_constant_value(&p_expression->left->object) &&
            object_has_constant_value(&p_expression->right->object))
        {
            const long long result = (object_to_signed_long_long(&p_expression->left->object) &&
                                      object_to_signed_long_long(&p_expression->right->object)) ? 1 : 0;
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                &p_expression->object, result, FLOW3_EQUAL,
                p_expression->first_token->line);
            if (result != 0)
            {
                struct flow3_map* p_dead = flow3_map_arena_new(&ctx->flow3_map_arena, p_before, "and-dead-false");
                return (struct flow3_branch_pair) { ctx->p_current_flow3_map, p_dead };
            }
            else
            {
                struct flow3_map* p_dead = flow3_map_arena_new(&ctx->flow3_map_arena, p_before, "and-dead-true");
                return (struct flow3_branch_pair) { p_dead, ctx->p_current_flow3_map };
            }
        }

        struct flow3_branch_pair left_pair = flow3_visit_expression(ctx, p_expression->left);

        if (object_has_constant_value(&p_expression->left->object) &&
            object_is_true(&p_expression->left->object) == false)
        {
            /* Left is always false: short-circuit, right never evaluated. */
            return left_pair;
        }

        /* Visit right on the true map of left (right only runs when left is true). */
        ctx->p_current_flow3_map = left_pair.p_true;
        struct flow3_branch_pair right_pair = flow3_visit_expression(ctx, p_expression->right);
        ctx->p_current_flow3_map = p_before;

        /*
         * false = merge(left_false, right_false)
         */
        struct flow3_map* p_and_false = flow3_map_arena_new(&ctx->flow3_map_arena, p_before, "and-false");
        flow3_map_merge_a_b(p_and_false, left_pair.p_false, right_pair.p_false);



        return (struct flow3_branch_pair) { right_pair.p_true, p_and_false };
    }

    case EXPR_INCLUSIVE_OR:
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);
        if (object_has_constant_value(&p_expression->left->object) &&
            object_has_constant_value(&p_expression->right->object))
        {
            const long long lv = object_to_signed_long_long(&p_expression->left->object);
            const long long rv = object_to_signed_long_long(&p_expression->right->object);
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                &p_expression->object, lv | rv, FLOW3_EQUAL,
                p_expression->first_token->line);
            break;
        }
        flow3_visit_expression(ctx, p_expression->left);
        flow3_visit_expression(ctx, p_expression->right);
        flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
            &p_expression->object, ANY_VALUE, FLOW3_ANY,
            p_expression->first_token->line);
        break;

    case EXPR_AND:
    case EXPR_EXCLUSIVE_OR:
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);
        if (object_has_constant_value(&p_expression->left->object) &&
            object_has_constant_value(&p_expression->right->object))
        {
            const long long lv = object_to_signed_long_long(&p_expression->left->object);
            const long long rv = object_to_signed_long_long(&p_expression->right->object);
            const long long result = (p_expression->expression_type == EXPR_AND)
                ? (lv & rv)
                : (lv ^ rv);
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                &p_expression->object, result, FLOW3_EQUAL,
                p_expression->first_token->line);
            break;
        }
        flow3_visit_expression(ctx, p_expression->left);
        flow3_visit_expression(ctx, p_expression->right);
        flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
            &p_expression->object, ANY_VALUE, FLOW3_ANY,
            p_expression->first_token->line);
        break;

    case EXPR_UNARY_TRAITS:
        break;

    case EXPR_UNARY_IS_SAME:
        break;

    case EXPR_UNARY_DECLARATOR_ATTRIBUTE:
        break;

    case EXPR_EXPRESSION:
    {
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);
        flow3_visit_expression(ctx, p_expression->left);
        /* Comma: the value (and branch state) of the right operand is what matters. */
        return flow3_visit_expression(ctx, p_expression->right);
    }

    case EXPR_CONDITIONAL:
    {
        assert(p_expression->condition_expr != NULL);
        assert(p_expression->right != NULL);

        struct flow3_branch_pair cond_pair = flow3_visit_expression(ctx, p_expression->condition_expr);

        /* true branch */
        ctx->p_current_flow3_map = cond_pair.p_true;
        /* Elvis: left==NULL means use condition_expr value on true branch */
        flow3_visit_expression(ctx,
            p_expression->left ? p_expression->left
                               : p_expression->condition_expr);

        /* false branch */
        ctx->p_current_flow3_map = cond_pair.p_false;
        flow3_visit_expression(ctx, p_expression->right);

        /* merge both arms back into p_before */
        flow3_map_merge_a_b(p_before, cond_pair.p_true, cond_pair.p_false);
        ctx->p_current_flow3_map = p_before;

        /*
         * Propagate the result value of the conditional expression.
         * The true arm carries the value from left (or condition_expr),
         * the false arm from right.  Merge both sides' alternatives for
         * this expression's object into the current map so that downstream
         * consumers (static_debug, assert_state, etc.) can see it.
         *
         * We collect from cond_pair.p_true (the true-arm expression object)
         * and cond_pair.p_false (the false-arm expression object) and append
         * both.  If neither arm has a known value record ANY.
         */
        {
            struct expression* p_true_expr = p_expression->left
                ? p_expression->left
                : p_expression->condition_expr;
            struct expression* p_false_expr = p_expression->right;

            const struct flow3_key_alternatives* p_true_entry = flow3_map_search_up(cond_pair.p_true, &p_true_expr->object);
            const struct flow3_key_alternatives* p_false_entry = flow3_map_search_up(cond_pair.p_false, &p_false_expr->object);

            struct flow3_key_alternatives* p_result_entry = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);

            if (p_result_entry)
            {
                flow3_alternatives_clear(&p_result_entry->alternatives);
                if (p_true_entry && p_true_entry->alternatives.size > 0)
                {
                    flow3_alternatives_append(&p_result_entry->alternatives, &p_true_entry->alternatives);
                }
                if (p_false_entry && p_false_entry->alternatives.size > 0)
                {
                    flow3_alternatives_append(&p_result_entry->alternatives, &p_false_entry->alternatives);
                }
                if (p_result_entry->alternatives.size == 0)
                {
                    flow3_alternatives_add_i(&p_result_entry->alternatives, ANY_VALUE, FLOW3_ANY, FLOW3_IMAGINARY_NONE, ctx->p_current_flow3_map,
                        p_expression->first_token->line);
                }
            }
        }
    }
    break;

    case EXPR_UNARY_GCC__BUILTIN_VA_START:
    case EXPR_UNARY_GCC__BUILTIN_VA_END:
    case EXPR_UNARY_GCC__BUILTIN_VA_COPY:
    case EXPR_UNARY_GCC__BUILTIN_VA_ARG:
        break;
    case EXPR_UNARY_GCC__BUILTIN_OFFSETOF:
    case EXPR_UNARY_CONSTEVAL:
        break;

    }

    struct flow3_branch_pair identity_pair = { ctx->p_current_flow3_map, ctx->p_current_flow3_map };

    /* Non-boolean or unhandled expression: both branches are identical (no narrowing). */
    return identity_pair;
}

static void flow3_visit_expression_statement(struct flow3_visit_ctx* ctx, struct expression_statement* p_expression_statement)
{
    if (p_expression_statement->expression_opt)
    {
        flow3_visit_full_expression(ctx, p_expression_statement->expression_opt);
    }

    if (p_expression_statement->p_lint_token)
    {
        flow3_check_dianostic_suppression(ctx, p_expression_statement->p_lint_token);
    }
}

static void flow3_visit_block_item_list(struct flow3_visit_ctx* ctx, struct block_item_list* p_block_item_list);

static void flow3_visit_compound_statement(struct flow3_visit_ctx* ctx, struct compound_statement* p_compound_statement)
{
    flow3_visit_block_item_list(ctx, &p_compound_statement->block_item_list);
    flow3_exit_block_visit_defer_list(ctx, &p_compound_statement->defer_list, p_compound_statement->last_token);
    flow3_defer_list_set_end_of_lifetime(ctx, &p_compound_statement->defer_list, p_compound_statement->last_token);

    if (p_compound_statement->lint_token)
    {
        flow3_check_dianostic_suppression(ctx, p_compound_statement->lint_token);
    }
}

static void flow3_visit_do_while_statement(struct flow3_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    assert(p_iteration_statement->first_token->type == TK_KEYWORD_DO);

    struct flow3_map* _Opt old_p_initial_map = ctx->p_initial_map;
    struct flow3_map* _Opt old_p_break_join_map = ctx->p_break_join_map;

    struct flow3_map* p_before = ctx->p_current_flow3_map;
    struct flow3_map* p_break_join = flow3_map_arena_new(&ctx->flow3_map_arena, p_before, "break join");

    ctx->p_initial_map = p_before;
    ctx->p_break_join_map = p_break_join;

    struct flow3_branch_pair do_cond_pair1 = { 0 };

    /*
     * do-while: the body executes first, then the condition is checked.
     * First pass — suppress warnings (warm up state for second pass).
     */
    diagnostic_stack_push_empty(&ctx->ctx->options.diagnostic_stack);

    flow3_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

    if (p_iteration_statement->expression1)
    {
        do_cond_pair1 = flow3_visit_full_expression(ctx, p_iteration_statement->expression1);
        /* Narrow to true branch before running second-pass body, mirroring while. */
        ctx->p_current_flow3_map = do_cond_pair1.p_true;
    }

    /* Second pass — warnings on */
    diagnostic_stack_pop(&ctx->ctx->options.diagnostic_stack);

    flow3_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

    struct flow3_map* p_after_body = ctx->p_current_flow3_map;
    struct flow3_branch_pair do_cond_pair2 = { 0 };
    if (p_iteration_statement->expression1)
    {
        do_cond_pair2 = flow3_visit_full_expression(ctx, p_iteration_statement->expression1);
    }

    flow3_exit_block_visit_defer_list(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);

    const bool was_last_statement_inside_true_branch_return =
        secondary_block_ends_with_jump(p_iteration_statement->secondary_block);

    struct flow3_map* p_false_branch_dw = do_cond_pair2.p_false ? do_cond_pair2.p_false
        : flow3_map_arena_new(&ctx->flow3_map_arena, p_after_body, "do-while false branch");

    if (was_last_statement_inside_true_branch_return)
    {
        ctx->p_current_flow3_map = p_before;
        flow3_map_merge_a_b(p_before, p_break_join, p_break_join);
    }
    else
    {
        flow3_map_merge_a_b(p_before, p_false_branch_dw, p_break_join);
    }

    ctx->p_current_flow3_map = p_before;

    flow3_defer_list_set_end_of_lifetime(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);

    ctx->p_initial_map = old_p_initial_map;
    ctx->p_break_join_map = old_p_break_join_map;
}

static void flow3_visit_while_statement(struct flow3_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    assert(p_iteration_statement->first_token->type == TK_KEYWORD_WHILE);

    if (p_iteration_statement->expression1 == NULL)
    {
        return;
    }

    //const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    struct flow3_map* _Opt old_p_initial_map = ctx->p_initial_map;
    struct flow3_map* _Opt old_p_break_join_map = ctx->p_break_join_map;

    struct flow3_map* p_before = ctx->p_current_flow3_map;
    struct flow3_map* p_break_join = flow3_map_arena_new(&ctx->flow3_map_arena, p_before, "break join");

    ctx->p_initial_map = p_before;
    ctx->p_break_join_map = p_break_join;

    /* First pass — suppress warnings */
    diagnostic_stack_push_empty(&ctx->ctx->options.diagnostic_stack);
    {
        struct flow3_branch_pair w_pair1 = flow3_visit_full_expression(ctx, p_iteration_statement->expression1);
        ctx->p_current_flow3_map = w_pair1.p_true;
    }

    flow3_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

    /* Second pass — warnings on */
    diagnostic_stack_pop(&ctx->ctx->options.diagnostic_stack);

    //struct flow3_map* p_after_body = ctx->p_current_flow3_map;
    struct flow3_branch_pair w_pair2 = flow3_visit_full_expression(ctx, p_iteration_statement->expression1);

    ctx->p_current_flow3_map = w_pair2.p_true;

    flow3_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

    flow3_exit_block_visit_defer_list(ctx, &p_iteration_statement->defer_list,
                                      p_iteration_statement->secondary_block->last_token);

    const bool was_last_statement_inside_true_branch_return =
        secondary_block_ends_with_jump(p_iteration_statement->secondary_block);

    if (was_last_statement_inside_true_branch_return)
    {
        ctx->p_current_flow3_map = p_before;
        flow3_map_merge_a_b(p_before, w_pair2.p_false, w_pair2.p_false);
    }
    else
    {
        flow3_map_merge_a_b(p_before, w_pair2.p_false, p_break_join);
    }

    ctx->p_current_flow3_map = p_before;

    flow3_defer_list_set_end_of_lifetime(ctx, &p_iteration_statement->defer_list,
                                         p_iteration_statement->secondary_block->last_token);

    ctx->p_initial_map = old_p_initial_map;
    ctx->p_break_join_map = old_p_break_join_map;
}

static void flow3_visit_for_statement(struct flow3_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    assert(p_iteration_statement->first_token->type == TK_KEYWORD_FOR);
    //const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    struct expression* _Opt p_next = p_iteration_statement->expression2;
    struct expression* _Opt p_condition = p_iteration_statement->expression1;

    if (p_iteration_statement->declaration &&
        p_iteration_statement->declaration->init_declarator_list.head)
    {
        flow3_visit_init_declarator_list(ctx, &p_iteration_statement->declaration->init_declarator_list);
    }
    else if (p_iteration_statement->expression0)
    {
        flow3_visit_full_expression(ctx, p_iteration_statement->expression0);
    }

    struct flow3_map* _Opt old_p_initial_map = ctx->p_initial_map;
    struct flow3_map* _Opt old_p_break_join_map = ctx->p_break_join_map;

    struct flow3_map* p_before = ctx->p_current_flow3_map;
    struct flow3_map* p_break_join = flow3_map_arena_new(&ctx->flow3_map_arena, p_before, "break join");

    ctx->p_initial_map = p_before;
    ctx->p_break_join_map = p_break_join;

    if (p_condition)
    {
        bool inside_loop = ctx->inside_loop;
        ctx->inside_loop = false;
        ctx->inside_loop = inside_loop;
    }

    /* First pass — suppress warnings */
    diagnostic_stack_push_empty(&ctx->ctx->options.diagnostic_stack);

    if (p_condition)
    {
        struct flow3_branch_pair for_pair1 = flow3_visit_full_expression(ctx, p_condition);
        ctx->p_current_flow3_map = for_pair1.p_true;
    }

    flow3_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

    if (p_next)
    {
        flow3_visit_full_expression(ctx, p_next);
    }

    /* Second pass — warnings on */
    diagnostic_stack_pop(&ctx->ctx->options.diagnostic_stack);

    //struct flow3_map* p_after_body = ctx->p_current_flow3_map;
    struct flow3_branch_pair for_pair2 = { 0 };
    if (p_condition)
    {
        for_pair2 = flow3_visit_full_expression(ctx, p_condition);
        ctx->p_current_flow3_map = for_pair2.p_true;
    }

    flow3_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

    if (p_next)
    {
        flow3_visit_full_expression(ctx, p_next);
    }

    flow3_exit_block_visit_defer_list(ctx, &p_iteration_statement->defer_list,
                                      p_iteration_statement->secondary_block->last_token);

    const bool was_last_statement_inside_true_branch_return =
        secondary_block_ends_with_jump(p_iteration_statement->secondary_block);

    struct flow3_map* p_false_branch_for = for_pair2.p_false ? for_pair2.p_false
        : flow3_map_arena_new(&ctx->flow3_map_arena, p_before, "for false branch");

    if (was_last_statement_inside_true_branch_return)
    {
        ctx->p_current_flow3_map = p_before;
        flow3_map_merge_a_b(p_before, p_false_branch_for, p_false_branch_for);
    }
    else
    {
        flow3_map_merge_a_b(p_before, p_false_branch_for, p_break_join);
    }

    ctx->p_current_flow3_map = p_before;

    flow3_defer_list_set_end_of_lifetime(ctx, &p_iteration_statement->defer_list,
                                         p_iteration_statement->secondary_block->last_token);

    ctx->p_initial_map = old_p_initial_map;
    ctx->p_break_join_map = old_p_break_join_map;
}

static void flow3_visit_iteration_statement(struct flow3_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    bool inside_loop = ctx->inside_loop;
    ctx->inside_loop = true;

    switch (p_iteration_statement->first_token->type)
    {
    case  TK_KEYWORD_WHILE:
        flow3_visit_while_statement(ctx, p_iteration_statement);
        break;
    case TK_KEYWORD_DO:
        flow3_visit_do_while_statement(ctx, p_iteration_statement);
        break;
    case TK_KEYWORD_FOR:
        flow3_visit_for_statement(ctx, p_iteration_statement);
        break;
    default:
        assert(false);
        break;
    }
    ctx->inside_loop = inside_loop; //restore

    if (p_iteration_statement->p_lint_token)
    {
        flow3_check_dianostic_suppression(ctx, p_iteration_statement->p_lint_token);
    }
}

static void flow3_check_arena_objects_at_function_exit(struct flow3_visit_ctx* ctx)
{
    for (int i = 0; i < ctx->allocated_object_arena.size; i++)
    {
        struct object* p_obj = ctx->allocated_object_arena.data[i];
        if (p_obj == NULL)
        {
            continue;
        }

        /*
         * TODO: check origin tag to distinguish external objects
         * (parameter-seeded, caller-owned) from internal ones
         * (function-created, fully checkable).
         */
    }
}
static void flow3_check_file_scope_objects_at_function_exit(struct flow3_visit_ctx* ctx, const struct marker* marker)
{
    /* Build a fast-lookup set of arena object pointers so we can skip them. */
    struct object_set arena_set = { 0 };
    for (int i = 0; i < ctx->allocated_object_arena.size; i++)
    {
        if (ctx->allocated_object_arena.data[i] != NULL)
        {
            object_set_add(&arena_set, ctx->allocated_object_arena.data[i]);
        }
    }

    /* Collect every unique object key present anywhere in the map chain. */
    struct object_set all_keys = { 0 };
    for (const struct flow3_map* cur = ctx->p_current_flow3_map; cur != NULL; cur = cur->p_parent_map)
    {
        if (!cur->buckets)
        {
            continue;
        }
        for (int i = 0; i < cur->num_of_buckets; i++)
        {
            for (const struct flow3_key_alternatives* e = cur->buckets[i]; e; e = e->next)
            {
                /* Skip sentinel keys that are not real objects */
                uintptr_t val = (uintptr_t)e->p_obj_key;
                if (val == UNINITIALIZED_VALUE || val == ANY_VALUE)
                {
                    continue;
                }
                object_set_add(&all_keys, e->p_obj_key);
            }
        }
    }

    for (int i = 0; i < all_keys.size; i++)
    {
        const struct object* p_obj = all_keys.items[i];

        /* Skip arena (synthetic) objects. */
        bool in_arena = false;
        for (int j = 0; j < arena_set.size; j++)
        {
            if (arena_set.items[j] == p_obj) { in_arena = true; break; }
        }
        if (in_arena)
        {
            continue;
        }

        /* Skip non-file-scope objects (parameters, block-scope locals). */
        if (!object_is_file_scope(p_obj))
        {
            continue;
        }
    }

    object_set_destroy(&all_keys);
    object_set_destroy(&arena_set);
}

static void flow3_check_function_exit(struct flow3_visit_ctx* ctx, struct jump_statement* p_jump_statement)
{
    flow3_exit_block_visit_defer_list(ctx,
                                     &p_jump_statement->defer_list,
                                     p_jump_statement->first_token);

    flow3_defer_list_set_end_of_lifetime(ctx,
                                         &p_jump_statement->defer_list,
                                         p_jump_statement->first_token);


    flow3_check_arena_objects_at_function_exit(ctx);//TODO do file scope together

    const struct marker marker = {
        .p_token_begin = p_jump_statement->first_token,
        .p_token_end = p_jump_statement->first_token
    };
    flow3_check_file_scope_objects_at_function_exit(ctx, &marker);
}

static void flow3_visit_jump_statement(struct flow3_visit_ctx* ctx, struct jump_statement* p_jump_statement)
{
    try
    {
        if (p_jump_statement->first_token->type == TK_KEYWORD_CAKE_THROW)
        {

            if (ctx->p_throw_join_map != NULL)
            {
                flow3_map_merge_a_b(ctx->p_throw_join_map,
                                    ctx->p_current_flow3_map,
                                    ctx->p_throw_join_map);
            }

            flow3_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list,
                                              p_jump_statement->first_token);
            flow3_defer_list_set_end_of_lifetime(ctx, &p_jump_statement->defer_list,
                                                 p_jump_statement->first_token);
        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_RETURN)
        {
            if (ctx->p_return_type == NULL)
            {
                //we must be inside a function and we need this return set.
                throw;
            }

            //const bool ownership_enabled = ctx->ctx->options.ownership_enabled;

            if (p_jump_statement->expression_opt)
            {
                flow3_visit_full_expression(ctx, p_jump_statement->expression_opt);

                struct object param_object = { 0 };
                make_object(ctx->p_return_type, &param_object, MAKE_STATE_UNITIALIZED, ctx->ctx->options.target);
                flow3_check_object_init_assigment(ctx, p_jump_statement->expression_opt, &param_object, &p_jump_statement->expression_opt->object, INIT_RETURN);
            }

            /*
             * On every explicit return: run deferred cleanup, verify
             * _Ctor/_Dtor/_Owner parameter exit conditions, and check
             * arena (synthetic pointed-to) objects.
             */
            flow3_check_function_exit(ctx, p_jump_statement);

        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_CONTINUE)
        {
            flow3_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
            flow3_defer_list_set_end_of_lifetime(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_BREAK)
        {
            if (ctx->p_break_join_map != NULL)
            {
                flow3_map_merge_a_b(ctx->p_break_join_map, ctx->p_current_flow3_map, ctx->p_break_join_map);
            }

            flow3_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
            flow3_defer_list_set_end_of_lifetime(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_GOTO)
        {
            assert(p_jump_statement->label != NULL);

            bool found = false;
            for (int i = 0; i < ctx->labels_size; i++)
            {
                if (strcmp(ctx->labels[i].label_name, p_jump_statement->label->lexeme) == 0)
                {
                    flow3_map_merge_a_b(ctx->labels[i].p_flow3_map,
                                        ctx->p_current_flow3_map,
                                        ctx->labels[i].p_flow3_map);
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                struct flow3_map* p_label_map =
                    flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, p_jump_statement->label->lexeme);
                ctx->labels[ctx->labels_size].label_name = p_jump_statement->label->lexeme;
                ctx->labels[ctx->labels_size].p_flow3_map = p_label_map;
                ctx->labels_size++;
            }

            flow3_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
            flow3_defer_list_set_end_of_lifetime(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
        }
        else
        {
            assert(false);
        }
    }
    catch
    {
    }
    if (p_jump_statement->p_lint_token)
    {
        flow3_check_dianostic_suppression(ctx, p_jump_statement->p_lint_token);
    }
}

static void flow3_visit_labeled_statement(struct flow3_visit_ctx* ctx, struct labeled_statement* p_labeled_statement)
{
    flow3_visit_label(ctx, p_labeled_statement->label);
    flow3_visit_statement(ctx, p_labeled_statement->statement);
}

static void flow3_visit_primary_block(struct flow3_visit_ctx* ctx, struct primary_block* p_primary_block)
{
    if (p_primary_block->compound_statement)
    {
        flow3_visit_compound_statement(ctx, p_primary_block->compound_statement);
    }
    else if (p_primary_block->iteration_statement)
    {
        flow3_visit_iteration_statement(ctx, p_primary_block->iteration_statement);
    }
    else if (p_primary_block->selection_statement)
    {
        flow3_visit_selection_statement(ctx, p_primary_block->selection_statement);
    }
    else if (p_primary_block->try_statement)
    {
        flow3_visit_try_statement(ctx, p_primary_block->try_statement);
    }
}

static void flow3_visit_unlabeled_statement(struct flow3_visit_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement)
{
    if (p_unlabeled_statement->primary_block)
    {
        flow3_visit_primary_block(ctx, p_unlabeled_statement->primary_block);
    }
    else if (p_unlabeled_statement->expression_statement)
    {
        flow3_visit_expression_statement(ctx, p_unlabeled_statement->expression_statement);
    }
    else if (p_unlabeled_statement->defer_statement)
    {
        flow3_visit_defer_statement(ctx, p_unlabeled_statement->defer_statement);
    }
    else if (p_unlabeled_statement->jump_statement)
    {
        flow3_visit_jump_statement(ctx, p_unlabeled_statement->jump_statement);
    }
    else
    {
        assert(false);
    }
}

static void flow3_visit_statement(struct flow3_visit_ctx* ctx, struct statement* p_statement)
{
    if (p_statement->labeled_statement)
    {
        flow3_visit_labeled_statement(ctx, p_statement->labeled_statement);
    }
    else if (p_statement->unlabeled_statement)
    {
        flow3_visit_unlabeled_statement(ctx, p_statement->unlabeled_statement);
    }
}

static void flow3_visit_label(struct flow3_visit_ctx* ctx, struct label* p_label)
{
    if (p_label->p_identifier_opt)
    {
        for (int i = 0; i < ctx->labels_size; i++)
        {
            if (strcmp(ctx->labels[i].label_name, p_label->p_identifier_opt->lexeme) == 0)
            {
                ctx->p_current_flow3_map = ctx->labels[i].p_flow3_map;
                break;
            }
        }
    }
    else
    {
        /* case/default — always start from the pre-switch state */
        if (ctx->p_initial_map == NULL)
        {
            return;
        }

        if (p_label->constant_expression == NULL)
        {
            /* default: label — no refinement */
            ctx->p_current_flow3_map = ctx->p_initial_map;
            return;
        }

        /* case N: — create a child map that records the switch variable == N */
        struct flow3_map* p_case_map =
            flow3_map_arena_new(&ctx->flow3_map_arena,
                                ctx->p_initial_map,
                                "case");

        /* ctx->p_switch_object_key is the object being switched on,
           set when entering the switch statement */
        if (ctx->p_switch_obj_key != NULL &&
            object_has_constant_value(&p_label->constant_expression->object))
        {
            signed long long case_value =
                object_to_signed_long_long(&p_label->constant_expression->object);

            flow3_map_replace_alternatives_i(p_case_map,
                                           ctx->p_switch_obj_key,
                                           case_value,
                                           FLOW3_EQUAL,
                                           p_label->constant_expression->first_token->line);
        }

        ctx->p_current_flow3_map = p_case_map;
    }
}

static void flow3_visit_block_item(struct flow3_visit_ctx* ctx, struct block_item* p_block_item)
{
    if (p_block_item->declaration)
    {
        flow3_visit_declaration(ctx, p_block_item->declaration);
    }
    else if (p_block_item->unlabeled_statement)
    {
        flow3_visit_unlabeled_statement(ctx, p_block_item->unlabeled_statement);
    }
    else if (p_block_item->label)
    {
        flow3_visit_label(ctx, p_block_item->label);
    }
}

static void flow3_visit_block_item_list(struct flow3_visit_ctx* ctx, struct block_item_list* p_block_item_list)
{
    struct block_item* _Opt p_block_item = p_block_item_list->head;
    while (p_block_item)
    {
        flow3_visit_block_item(ctx, p_block_item);
        p_block_item = p_block_item->next;
    }
}

static void flow3_visit_pragma_declaration(struct flow3_visit_ctx* ctx, struct pragma_declaration* p_pragma_declaration)
{
    execute_pragma_declaration(ctx->ctx, p_pragma_declaration, true);
}

static void object_static_debug(struct flow3_visit_ctx* ctx, const struct object* p_object, struct token* first_token, struct token* last_token)
{
    const struct object* member = p_object->members.head;
    if (member)
    {
        while (member)
        {
            object_static_debug(ctx, member, first_token, last_token);
            member = member->next;
        }
        return;
    }

    struct flow3_key_alternatives* p_entry = flow3_map_search_up(ctx->p_current_flow3_map, p_object);
    if (p_entry == NULL)
    {
        const struct marker m =
        {
            .p_token_begin = first_token,
            .p_token_end = last_token
        };

        struct osstream ss = { 0 };
        print_object_ptr(&ss, p_object, false);
        diagnostic(W_INFO, ctx->ctx, NULL, &m, "%s", ss.c_str);
        ss_close(&ss);
        return;
    }

    for (int i = 0; i < p_entry->alternatives.size; i++)
    {
        struct flow3_alternative* p_flow3_alternative = &p_entry->alternatives.data[i];

        if (p_flow3_alternative->imaginary == FLOW3_IMAGINARY_ABSENT)
        {
            continue;
        }
        if (p_flow3_alternative->value.kind == FLOW3_VALUE_REF)
        {
            object_static_debug(ctx, p_flow3_alternative->value.as.p, first_token, last_token);
            continue;
        }

        const struct marker m =
        {
            .p_token_begin = first_token,
            .p_token_end = last_token
        };

        struct osstream key_ss = { 0 };

        if (p_entry->p_obj_key->member_designator)
        {
            print_object_ptr(&key_ss, p_entry->p_obj_key, false);
        }

        struct osstream ss = { 0 };
        flow3_alternative_sprint(&ss, p_flow3_alternative);

        diagnostic(W_INFO, ctx->ctx, NULL, &m, "%s %s",
            key_ss.c_str ? key_ss.c_str : "",
            ss.c_str);
        ss_close(&key_ss);
        ss_close(&ss);
    }
}

// Explains, in human terms, why an alternative fails flow3_alternative_is_true.
static void flow3_explain_alternative_not_true(struct osstream* ss, const struct flow3_alternative* alt)
{
    if (alt->relation == FLOW3_UNINITIALIZED)
    {
        ss_fprintf(ss, "value is uninitialized");
    }
    else if (alt->relation == FLOW3_ANY)
    {
        ss_fprintf(ss, "value could be any value, including zero");
    }
    else if (alt->relation == FLOW3_EQUAL && flow3_value_is_false(alt->value))
    {
        ss_fprintf(ss, "value equals zero");
    }
    else if (alt->relation == FLOW3_NOT_EQUAL)
    {
        ss_fprintf(ss, "value is only known to differ from a nonzero value, so it could still be zero");
    }
    else
    {
        ss_fprintf(ss, "value cannot be proven nonzero");
    }

    switch (alt->imaginary)
    {
    case FLOW3_IMAGINARY_MOVED:  ss_fprintf(ss, " (object was moved)");           break;
    case FLOW3_IMAGINARY_ENDED:  ss_fprintf(ss, " (object's lifetime has ended)"); break;
    case FLOW3_IMAGINARY_ABSENT: ss_fprintf(ss, " (object is absent)");           break;
    default: break;
    }

    ss_fprintf(ss, ", set at line %d", alt->line);

    if (alt->origin)
    {
        ss_fprintf(ss, " in \"%s\"", alt->origin->name ? alt->origin->name : "");
    }
}

static void check_object_true(struct flow3_visit_ctx* ctx, const struct object* p_object, const struct token* p_position_token)
{
    struct flow3_key_alternatives* p_entry = flow3_map_search_up(ctx->p_current_flow3_map, p_object);
    if (p_entry == NULL)
        return;

    for (int i = 0; i < p_entry->alternatives.size; i++)
    {
        const struct flow3_alternative* alt = &p_entry->alternatives.data[i];
        if (alt->value.kind == FLOW3_VALUE_REF)
        {
            check_object_true(ctx, alt->value.as.p, p_position_token);
            continue;
        }

        if (!flow3_alternative_is_true(alt))
        {
            struct osstream ss = { 0 };
            flow3_explain_alternative_not_true(&ss, alt);
            diagnostic(C_FLOW_ANALIZER_ERROR_STATIC_STATE_FAILED, ctx->ctx,
                       p_position_token, NULL, "compile_assert failed: %s", ss.c_str);
            ss_close(&ss);
        }
    }
}

static void flow3_visit_compile_assert(struct flow3_visit_ctx* ctx, struct static_assertion* p_static_assertion)
{
    check_object_true(ctx,
        &p_static_assertion->constant_expression->object,
        p_static_assertion->first_token);
}

static void flow3_visit_static_assertion(struct flow3_visit_ctx* ctx, struct static_assertion* p_static_assertion)
{
    const bool t2 = ctx->expression_is_not_evaluated;
    ctx->expression_is_not_evaluated = true;

    flow3_visit_full_expression(ctx, p_static_assertion->constant_expression);

    ctx->expression_is_not_evaluated = t2; //restore

    if (p_static_assertion->first_token->type == TK_KEYWORD_CAKE_STATIC_DEBUG ||
        p_static_assertion->first_token->type == TK_KEYWORD_CAKE_STATIC_DEBUG_EX)
    {
        if (object_is_zero(&p_static_assertion->constant_expression->object))
        {
            flow3_map_debug_print(ctx->p_current_flow3_map, 0);
        }
        else
        {
            flow3_expression_static_debug(ctx, p_static_assertion->constant_expression);
        }
    }
    else if (p_static_assertion->first_token->type == TK_KEYWORD_STATIC_STATE)
    {
    }
    else if (p_static_assertion->first_token->type == TK_KEYWORD__COMPILE_ASSERT)
    {
        flow3_visit_compile_assert(ctx, p_static_assertion);
    }
    else if (p_static_assertion->first_token->type == TK_KEYWORD_STATIC_SET)
    {
    }
}

static void flow3_visit_direct_declarator(struct flow3_visit_ctx* ctx, struct direct_declarator* p_direct_declarator)
{
    if (p_direct_declarator->function_declarator)
    {
        struct parameter_declaration* _Opt parameter = NULL;

        if (p_direct_declarator->function_declarator->parameter_type_list_opt &&
            p_direct_declarator->function_declarator->parameter_type_list_opt->parameter_list)
        {
            parameter = p_direct_declarator->function_declarator->parameter_type_list_opt->parameter_list->head;
        }

        ctx->parameter_list++;

        while (parameter)
        {
            flow3_visit_declaration_specifiers(ctx, parameter->declaration_specifiers, &parameter->declarator->type);

            if (parameter->declarator)
            {
                flow3_visit_declarator(ctx, parameter->declarator);
            }

            flow3_parameter_object_init(ctx, &parameter->declarator->object, &parameter->declarator->type, parameter->declaration_specifiers->first_token->line);

            parameter = parameter->next;
        }

        ctx->parameter_list--;
    }
    else if (p_direct_declarator->array_declarator)
    {
        if (p_direct_declarator->array_declarator->assignment_expression)
        {
            flow3_visit_full_expression(ctx, p_direct_declarator->array_declarator->assignment_expression);
        }
    }
}

static void flow3_visit_declarator(struct flow3_visit_ctx* ctx, struct declarator* p_declarator)
{
    if (p_declarator->type.category != TYPE_CATEGORY_FUNCTION)
    {
        if (ctx->parameter_list > 1)
        {
            /*
             The objective here is to avoid including the arguments
             of function pointers inside the scope.
             Sample
             void x_destroy(void (*f)(void * _Owner p))
             We add f but not p.
            */
            return;
        }
    }

    if (p_declarator->direct_declarator)
    {
        flow3_visit_direct_declarator(ctx, p_declarator->direct_declarator);
    }
}

static void flow3_visit_init_declarator_list(struct flow3_visit_ctx* ctx, struct init_declarator_list* p_init_declarator_list)
{
    struct init_declarator* _Opt p_init_declarator = p_init_declarator_list->head;
    while (p_init_declarator)
    {
        flow3_visit_init_declarator(ctx, p_init_declarator);
        p_init_declarator = p_init_declarator->next;
    }
}

static void flow3_visit_member_declarator(struct flow3_visit_ctx* ctx, struct member_declarator* p_member_declarator)
{
    if (p_member_declarator->declarator)
    {
        flow3_visit_declarator(ctx, p_member_declarator->declarator);
    }
}

static void flow3_visit_member_declarator_list(struct flow3_visit_ctx* ctx, struct member_declarator_list* p_member_declarator_list)
{
    struct member_declarator* _Opt p_member_declarator = p_member_declarator_list->head;
    while (p_member_declarator)
    {
        flow3_visit_member_declarator(ctx, p_member_declarator);
        p_member_declarator = p_member_declarator->next;
    }
}

static void flow3_visit_member_declaration(struct flow3_visit_ctx* ctx, struct member_declaration* p_member_declaration)
{
    if (p_member_declaration->member_declarator_list_opt)
    {
        flow3_visit_member_declarator_list(ctx, p_member_declaration->member_declarator_list_opt);
    }
}

static void flow3_visit_member_declaration_list(struct flow3_visit_ctx* ctx, struct member_declaration_list* p_member_declaration_list)
{
    struct member_declaration* _Opt p_member_declaration = p_member_declaration_list->head;
    while (p_member_declaration)
    {
        flow3_visit_member_declaration(ctx, p_member_declaration);
        p_member_declaration = p_member_declaration->next;
    }
}

static void flow3_visit_struct_or_union_specifier(struct flow3_visit_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier)
{
    flow3_visit_member_declaration_list(ctx, &p_struct_or_union_specifier->member_declaration_list);
}

static void flow3_visit_enumerator(struct flow3_visit_ctx* ctx, struct enumerator* p_enumerator)
{
    if (p_enumerator->constant_expression_opt)
    {
        flow3_visit_full_expression(ctx, p_enumerator->constant_expression_opt);
    }
}

static void flow3_visit_enumerator_list(struct flow3_visit_ctx* ctx, struct enumerator_list* p_enumerator_list)
{
    struct enumerator* _Opt current = p_enumerator_list->head;
    while (current)
    {
        flow3_visit_enumerator(ctx, current);
        current = current->next;
    }
}

static void flow3_visit_enum_specifier(struct flow3_visit_ctx* ctx, struct enum_specifier* p_enum_specifier)
{
    flow3_visit_enumerator_list(ctx, &p_enum_specifier->enumerator_list);
}

static void flow3_visit_type_specifier(struct flow3_visit_ctx* ctx, struct type_specifier* p_type_specifier)
{
    if (p_type_specifier->struct_or_union_specifier)
    {
        flow3_visit_struct_or_union_specifier(ctx, p_type_specifier->struct_or_union_specifier);
    }

    if (p_type_specifier->enum_specifier)
    {
        flow3_visit_enum_specifier(ctx, p_type_specifier->enum_specifier);
    }
}

static void flow3_visit_type_specifier_qualifier(struct flow3_visit_ctx* ctx, struct type_specifier_qualifier* p_type_specifier_qualifier)
{
    if (p_type_specifier_qualifier->type_specifier)
    {
        flow3_visit_type_specifier(ctx, p_type_specifier_qualifier->type_specifier);
    }
}

static void flow3_visit_declaration_specifier(struct flow3_visit_ctx* ctx, struct declaration_specifier* p_declaration_specifier)
{
    if (p_declaration_specifier->type_specifier_qualifier)
    {
        flow3_visit_type_specifier_qualifier(ctx, p_declaration_specifier->type_specifier_qualifier);
    }
}

static void flow3_visit_declaration_specifiers(struct flow3_visit_ctx* ctx, struct declaration_specifiers* p_declaration_specifiers, struct type* _Opt p_type_opt)
{
    struct declaration_specifier* _Opt p_declaration_specifier = p_declaration_specifiers->head;
    while (p_declaration_specifier)
    {
        flow3_visit_declaration_specifier(ctx, p_declaration_specifier);
        p_declaration_specifier = p_declaration_specifier->next;
    }
}

static void flow3_check_object_at_exit(struct flow3_visit_ctx* ctx,
                                             const struct type* p_type,
                                             const struct object* p_obj,
                                             const struct marker* marker,
                                             const struct token* p_exit_token)
{
    if (p_obj->members.head)
    {
        for (const struct object* member = p_obj->members.head; member; member = member->next)
        {
            flow3_check_object_at_exit(ctx,
                                       &member->type,
                                       member,
                                       marker,
                                       p_exit_token);
        }
        return;
    }

    const bool is_owner = type_is_owner(&p_obj->type);

    const struct flow3_key_alternatives* e = flow3_map_search_up(ctx->p_current_flow3_map, p_obj);
    if (e == NULL) return;

    for (int i = 0; i < e->alternatives.size; i++)
    {
        const struct flow3_alternative* p_alternative = &e->alternatives.data[i];

        if (p_alternative->value.kind == FLOW3_VALUE_REF)
        {
            //IS THIS POSSIBLE?
            if (p_alternative->value.as.p != p_obj)
            {
                flow3_check_object_at_exit(ctx,
                                            p_type,
                                            p_alternative->value.as.p,
                                            marker,
                                            p_exit_token);
                continue;
            }
        }

        if (p_alternative->imaginary == FLOW3_IMAGINARY_ABSENT)
            continue;

        if (is_owner && (p_alternative->imaginary != FLOW3_IMAGINARY_MOVED))
        {
            if (flow3_alternative_is_zero(p_alternative))
            {

            }
            else
            {
                const char* object_name = p_obj->member_designator;
                if (diagnostic(W_FLOW_LIFETIME_ENDED,
                    ctx->ctx,
                    NULL,
                    marker,
                    "owner object (%s) not moved (%s see line %d)",
                    object_name,
                    p_alternative->origin->name,
                    p_alternative->line))
                {
                    diagnostic(W_INFO,
                     ctx->ctx,
                     p_exit_token,
                     NULL,
                     "exit point");
                }
            }
        }


    }
}

static bool flow3_is_last_item_return(struct compound_statement* p_compound_statement)
{
    if (p_compound_statement &&
        p_compound_statement->block_item_list.tail &&
        p_compound_statement->block_item_list.tail->unlabeled_statement &&
        p_compound_statement->block_item_list.tail->unlabeled_statement->jump_statement &&
        p_compound_statement->block_item_list.tail->unlabeled_statement->jump_statement->first_token &&
        p_compound_statement->block_item_list.tail->unlabeled_statement->jump_statement->first_token->type == TK_KEYWORD_RETURN) //lint 28 28
    {
        return true;
    }
    return false;
}

void flow3_visit_declaration(struct flow3_visit_ctx* ctx, struct declaration* p_declaration)
{
    if (p_declaration->static_assertion)
    {
        flow3_visit_static_assertion(ctx, p_declaration->static_assertion);
    }

    if (p_declaration->pragma_declaration)
    {
        flow3_visit_pragma_declaration(ctx, p_declaration->pragma_declaration);
    }

    if (p_declaration->declaration_specifiers)
    {
        if (p_declaration->init_declarator_list.head)
        {
            flow3_visit_declaration_specifiers(ctx, p_declaration->declaration_specifiers,
                &p_declaration->init_declarator_list.head->p_declarator->type);
        }
        else
        {
            flow3_visit_declaration_specifiers(ctx, p_declaration->declaration_specifiers, NULL);

        }
    }

    if (p_declaration->init_declarator_list.head)
    {
        flow3_visit_init_declarator_list(ctx, &p_declaration->init_declarator_list);
    }

    if (p_declaration->function_body)
    {
        assert(ctx->p_return_type == NULL);

        struct type type = get_function_return_type(&p_declaration->init_declarator_list.head->p_declarator->type);
        ctx->p_return_type = &type;

        ctx->p_current_function_declaration = p_declaration;
        flow3_visit_compound_statement(ctx, p_declaration->function_body);
        ctx->p_current_function_declaration = NULL;
        type_destroy(&type);
        ctx->p_return_type = NULL;

        if (!flow3_is_last_item_return(p_declaration->function_body))
        {
            flow3_exit_block_visit_defer_list(ctx, &p_declaration->defer_list, p_declaration->function_body->last_token);
            flow3_defer_list_set_end_of_lifetime(ctx, &p_declaration->defer_list, p_declaration->function_body->last_token);
            //flow3_check_params_at_function_exit(ctx, p_declaration);
            flow3_check_arena_objects_at_function_exit(ctx);
            const struct marker marker = {
                .p_token_begin = p_declaration->function_body->last_token,
                .p_token_end = p_declaration->function_body->last_token
            };
            flow3_check_file_scope_objects_at_function_exit(ctx, &marker);
        }
    }

    if (p_declaration->lint_token)
    {
        flow3_check_dianostic_suppression(ctx, p_declaration->lint_token);
    }
}

void flow3_start_visit_declaration(struct flow3_visit_ctx* ctx, struct declaration* p_declaration)
{
    ctx->labels_size = 0;

    flow3_allocated_object_arena_clear(&ctx->allocated_object_arena);
    flow3_map_arena_clear(&ctx->flow3_map_arena);
    ctx->p_current_flow3_map = flow3_map_arena_new(&ctx->flow3_map_arena, NULL, "root");

    flow3_visit_declaration(ctx, p_declaration);

    flow3_allocated_object_arena_clear(&ctx->allocated_object_arena);
    flow3_map_arena_clear(&ctx->flow3_map_arena);
    ctx->p_current_flow3_map = NULL;
}

void flow3_visit_ctx_destroy(_Dtor struct flow3_visit_ctx* ctx)
{
    flow3_allocated_object_arena_clear(&ctx->allocated_object_arena);
    flow3_map_arena_clear(&ctx->flow3_map_arena);
}