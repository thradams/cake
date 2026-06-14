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

enum flow3_relation
{
    FLOW3_UNINITIALIZED = 0, // value == trash
    FLOW3_EQUAL,             // == value
    FLOW3_NOT_EQUAL,         // != value
    FLOW3_ANY,               // value >= TYPE_MIN and value <= TYPE_MAX

    FLOW3_NOT_APPLICABLE,    // cannot be used
};

enum
{
    ANY_VALUE = 0xCAFE,
    UNINITIALIZED_VALUE = 0xBAD,
    NOT_APPLICABLE_VALUE = 0xFACE
};

struct object_set
{
    const struct object** items;
    int             size;
    int             capacity;
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

struct flow3_alternative
{
    struct flow3_value value;
    enum flow3_relation relation;

    bool moved;    // target was moved elsewhere, target is alive
    bool expired;  // target may  no longer be alive
    bool ended;    // this object's lifetime ended
    bool absent;   // no object is present

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

static void flow3_check_pointer_used_as_bool(struct flow3_visit_ctx* ctx, struct expression* p_expression);

static void flow3_check_params_at_function_exit(struct flow3_visit_ctx* ctx, struct declaration* p_declaration);
static void flow3_check_file_scope_objects_at_function_exit(struct flow3_visit_ctx* ctx, const struct marker* marker);

static struct flow3_branch_pair flow3_visit_expression(struct flow3_visit_ctx* ctx, struct expression* p_expression);
static void object_static_debug(struct flow3_visit_ctx* ctx, const struct object* p_object, struct token* first_token, struct token* last_token);

static void flow3_check_owner_object_at_exit(struct flow3_visit_ctx* ctx, const struct type* p_type, struct object* p_obj, const struct marker* marker);
static void flow3_check_arena_objects_at_function_exit(struct flow3_visit_ctx* ctx);
static void flow3_check_rhs_value(struct flow3_visit_ctx* ctx, const struct type* p_dest_type, struct expression* p_rhs_expr);
static void flow3_check_rhs_owner_validity(struct flow3_visit_ctx* ctx, const struct object* p_right_obj, struct expression* p_right_expr);

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

static void flow3_alternatives_add(struct flow3_alternatives* vs, struct flow3_value value, enum flow3_relation state, const struct flow3_map* origin, int line)
{
    try
    {
        for (int i = 0; i < vs->size; i++)
        {
            if (flow3_value_is_same(vs->data[i].value, value) &&
                vs->data[i].relation == state &&
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
        vs->data[vs->size].value = value;
        vs->data[vs->size].relation = state;
        vs->data[vs->size].origin = origin;
        vs->data[vs->size].line = line;
        vs->size++;
    }
    catch {
    }
}

static void flow3_alternatives_add_object_ptr(struct flow3_alternatives* vs, const struct object* p_pointed, enum flow3_relation relation, const struct flow3_map* origin, int line)
{
    struct flow3_value v = { .kind = FLOW3_VALUE_PTR, .as = {.p = p_pointed } };
    flow3_alternatives_add(vs, v, relation, origin, line);
}

static void flow3_alternatives_add_object_ref(struct flow3_alternatives* vs, const struct object* p_pointed, const struct flow3_map* origin, int line)
{
    struct flow3_value v = { .kind = FLOW3_VALUE_REF, .as = {.p = p_pointed } };
    flow3_alternatives_add(vs, v, FLOW3_EQUAL, origin, line);
}

static void flow3_alternatives_add_i(struct flow3_alternatives* vs, signed long long i, enum flow3_relation relation, const struct flow3_map* origin, int line)
{
    struct flow3_value v = { .kind = FLOW3_VALUE_SIGNED, .as = {.i = i } };
    flow3_alternatives_add(vs, v, relation, origin, line);
}

static void flow3_alternatives_add_does_not_exist(struct flow3_alternatives* vs, const struct flow3_map* origin, int line)
{
    try
    {
        for (int i = 0; i < vs->size; i++)
        {
            if (vs->data[i].absent &&
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
        vs->data[vs->size].absent = true;
        vs->data[vs->size].relation = FLOW3_NOT_APPLICABLE;
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
                               src->data[i].value,
                               src->data[i].relation,
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
    flow3_alternatives_add(&e->alternatives, value, state, m, line);
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

static void flow3_map_set_object_expired(struct flow3_map* m, const struct object* obj, int line)
{
    if (obj->members.head)
    {
        for (const struct object* member = obj->members.head; member; member = member->next)
            flow3_map_set_object_expired(m, member, line);
        return;
    }
    struct flow3_key_alternatives* e = flow3_map_find_add(m, obj);
    if (e == NULL)
    {
        return;
    }
    if (e->alternatives.size == 0)
    {
        /* No existing value to preserve - add a neutral carrier. */
        flow3_alternatives_add_i(&e->alternatives, ANY_VALUE, FLOW3_ANY, m, line);
    }
    for (int i = 0; i < e->alternatives.size; i++)
    {
        e->alternatives.data[i].expired = true;
    }
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
    flow3_alternatives_clear(&e->alternatives);
    flow3_alternatives_add_i(&e->alternatives, UNINITIALIZED_VALUE, FLOW3_UNINITIALIZED, m, line);
    e->alternatives.data[0].ended = true;
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
    {
        return;
    }

    for (int i = 0; i < src->num_of_buckets; i++)
    {
        struct flow3_key_alternatives* e = src->buckets[i];
        while (e)
        {
            struct flow3_key_alternatives* next = e->next;

            /* Find or create the slot in dest. */
            struct flow3_key_alternatives* d = flow3_map_find_add(dest, e->p_obj_key);
            flow3_alternatives_clear(&d->alternatives);
            /* Steal the alternatives from src entry. */
            d->alternatives = e->alternatives;
            e->alternatives.data = NULL;
            e->alternatives.size = 0;
            e->alternatives.capacity = 0;

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
                            a2->value, a2->relation,
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
    case FLOW3_NOT_APPLICABLE: ss_fprintf(ss, "N/A ");   break;
    }

    switch (alt->value.kind)
    {
    case FLOW3_VALUE_SIGNED:
    {
        if (alt->value.as.i == UNINITIALIZED_VALUE ||
            alt->value.as.i == ANY_VALUE ||
            alt->value.as.i == NOT_APPLICABLE_VALUE)
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
        ss_fprintf(ss, "&");
        print_object_ptr(ss, alt->value.as.p, false);
        break;
    }
    }

    if (alt->moved)
    {
        ss_fprintf(ss, " MOVED");
    }
    if (alt->expired)
    {
        ss_fprintf(ss, " EXPIRED");
    }
    if (alt->ended)
    {
        ss_fprintf(ss, " ENDED");
    }
    if (alt->absent)
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

static bool flow3_object_is_not_null(struct flow3_map* flow3_map, const struct object* p)
{
    //p = object_get_referenced(p);


    if (p->state == CONSTANT_VALUE_STATE_CONSTANT)
    {
        return !object_is_zero(p);
    }


    /* null-ness is a property of pointer leaves, not of struct objects with members */
    if (p->members.head)
    {
        return false;
    }

    /* true only if every alternative is definitely non-null */
    const struct flow3_key_alternatives* e = flow3_map_search_up(flow3_map, p);
    if (e == NULL || e->alternatives.size == 0)
    {
        return false;
    }
    for (int i = 0; i < e->alternatives.size; i++)
    {
        if (!flow3_alternative_is_true(&e->alternatives.data[i]))
        {
            return false;
        }
    }
    return true;
}

static bool flow3_object_is_zero(struct flow3_map* flow3_map, const struct object* p)
{
    //p = object_get_referenced(p);

    if (p->state == CONSTANT_VALUE_STATE_CONSTANT)
    {
        return object_is_zero(p);
    }


    /* zero-ness is a scalar leaf property; not applicable to struct objects with members */
    if (p->members.head)
    {
        return false;
    }

    /* true only if every alternative is zero */
    const struct flow3_key_alternatives* e = flow3_map_search_up(flow3_map, p);
    if (e == NULL || e->alternatives.size == 0)
    {
        return false;
    }
    for (int i = 0; i < e->alternatives.size; i++)
    {
        if (!flow3_alternative_is_zero(&e->alternatives.data[i]))
        {
            return false;
        }
    }
    return true;
}

static bool flow3_object_is_null(struct flow3_map* flow3_map, const struct object* p)
{
    return flow3_object_is_zero(flow3_map, p);
}

static bool flow3_object_is_not_zero(struct flow3_map* flow3_map, const struct object* p)
{
    //p = object_get_referenced(p);

    if (p->state == CONSTANT_VALUE_STATE_CONSTANT)
    {
        return !object_is_zero(p);
    }


    /* zero-ness is a scalar leaf property; not applicable to struct objects with members */
    if (p->members.head)
    {
        return false;
    }

    /* true only if every alternative is definitely non-zero */
    const struct flow3_key_alternatives* e = flow3_map_search_up(flow3_map, p);
    if (e == NULL || e->alternatives.size == 0)
    {
        return false;
    }
    for (int i = 0; i < e->alternatives.size; i++)
    {
        if (!flow3_alternative_is_true(&e->alternatives.data[i]))
        {
            return false;
        }
    }
    return true;
}

static bool flow3_object_get_known_equal_int(struct flow3_map* flow3_map, const struct object* p, long long* out_value)
{
    const struct flow3_key_alternatives* e = flow3_map_search_up(flow3_map, p);
    if (e == NULL || e->alternatives.size == 0)
    {
        return false;
    }

    bool found = false;
    long long known = 0;

    for (int i = 0; i < e->alternatives.size; i++)
    {
        const struct flow3_alternative* alt = &e->alternatives.data[i];
        if (alt->relation != FLOW3_EQUAL)
        {
            return false;
        }

        long long v;
        if (alt->value.kind == FLOW3_VALUE_SIGNED)
        {
            v = alt->value.as.i;
        }
        else if (alt->value.kind == FLOW3_VALUE_UNSIGNED)
        {
            v = (long long)alt->value.as.u;
        }
        else
        {
            return false;
        } /* PTR — not an integer comparison */

        if (!found)
        {
            known = v;
            found = true;
        }
        else if (v != known)
        {
            return false; /* alternatives disagree */
        }
    }

    if (found)
    {
        *out_value = known;
    }
    return found;
}

static bool flow3_get_known_int(struct flow3_map* flow3_map, const struct object* p, long long* out_value)
{
    if (flow3_object_get_known_equal_int(flow3_map, p, out_value))
    {
        return true;
    }
    /* Fallback: compile-time constant in the AST (e.g. literal "1"). */
    if (p->state == CONSTANT_VALUE_STATE_CONSTANT)
    {
        *out_value = object_to_signed_long_long(p);
        return true;
    }
    return false;
}

static bool flow3_object_can_be_zero(struct flow3_map* flow3_map, const struct object* p)
{
    p = object_get_referenced(p);

    if (p->state == CONSTANT_VALUE_STATE_CONSTANT)
    {
        return object_is_zero(p);
    }

    /* zero-ness is a scalar leaf property; not applicable to struct objects with members */
    if (p->members.head)
    {
        return false;
    }

    /* true if at least one alternative can be zero */
    const struct flow3_key_alternatives* e = flow3_map_search_up(flow3_map, p);
    if (e == NULL)
    {
        return false;
    }
    for (int i = 0; i < e->alternatives.size; i++)
    {
        if (flow3_alternative_can_be_zero(&e->alternatives.data[i]))
        {
            return true;
        }
    }
    return false;
}

static bool flow3_object_can_be_moved(struct flow3_map* flow3_map, const struct object* p)
{
    p = object_get_referenced(p);

    if (p->state == CONSTANT_VALUE_STATE_CONSTANT)
    {
        return false;
    }

    if (p->members.head)
    {
        /* true if any member can be moved */
        for (const struct object* m = p->members.head; m; m = m->next)
        {
            if (flow3_object_can_be_moved(flow3_map, m))
            {
                return true;
            }
        }
        return false;
    }

    /* true if at least one alternative is moved.
     * does_not_exist alternatives are skipped — a non-existent object
     * (null-pointer arm) has no state to report. */
    const struct flow3_key_alternatives* e = flow3_map_search_up(flow3_map, p);
    if (e == NULL)
    {
        return false;
    }
    for (int i = 0; i < e->alternatives.size; i++)
    {
        if (e->alternatives.data[i].absent)
        {
            continue;
        }
        if (e->alternatives.data[i].moved)
        {
            return true;
        }
    }
    return false;
}

static bool flow3_object_can_be_null(struct flow3_map* flow3_map, const struct object* p)
{
    return flow3_object_can_be_zero(flow3_map, p);
}

static bool flow3_object_is_uninitialized(struct flow3_map* flow3_map, const struct object* p)
{
    p = object_get_referenced(p);
    if (p->state == CONSTANT_VALUE_STATE_CONSTANT)
    {
        return false;
    }

    if (p->members.head)
    {
        /* true only if every member is uninitialized */
        for (const struct object* m = p->members.head; m; m = m->next)
        {
            if (!flow3_object_is_uninitialized(flow3_map, m))
            {
                return false;
            }
        }
        return true;
    }

    /* true only if every non-does_not_exist alternative is uninitialized.
     * does_not_exist alternatives are skipped — a non-existent object
     * (null-pointer arm) has no state to report. */
    const struct flow3_key_alternatives* e = flow3_map_search_up(flow3_map, p);
    if (e == NULL || e->alternatives.size == 0)
    {
        return false;
    }
    bool has_real = false;
    for (int i = 0; i < e->alternatives.size; i++)
    {
        if (e->alternatives.data[i].absent)
        {
            continue;
        }
        has_real = true;
        if (e->alternatives.data[i].relation != FLOW3_UNINITIALIZED)
        {
            return false;
        }
    }
    return has_real;
}

static bool flow3_object_can_be_uninitialized(struct flow3_map* flow3_map, const struct object* p)
{
    p = object_get_referenced(p);
    if (p->state == CONSTANT_VALUE_STATE_CONSTANT)
    {
        return false;
    }

    if (p->members.head)
    {
        /* true if any member can be uninitialized */
        for (const struct object* m = p->members.head; m; m = m->next)
        {
            if (flow3_object_can_be_uninitialized(flow3_map, m))
            {
                return true;
            }
        }
        return false;
    }

    /* true if at least one alternative is uninitialized.
     * does_not_exist alternatives are skipped — a non-existent object
     * (null-pointer arm) has no state to report. */
    const struct flow3_key_alternatives* e = flow3_map_search_up(flow3_map, p);
    if (e == NULL)
    {
        return false;
    }
    for (int i = 0; i < e->alternatives.size; i++)
    {
        if (e->alternatives.data[i].absent)
        {
            continue;
        }
        if (e->alternatives.data[i].relation == FLOW3_UNINITIALIZED)
        {
            return true;
        }
    }
    return false;
}

static bool flow3_object_can_have_its_lifetime_ended(struct flow3_map* flow3_map, const struct object* p)
{
    p = object_get_referenced(p);
    if (p->state == CONSTANT_VALUE_STATE_CONSTANT)
    {
        return false;
    }

    if (p->members.head)
    {
        /* true if any member can have its lifetime ended */
        for (const struct object* m = p->members.head; m; m = m->next)
        {
            if (flow3_object_can_have_its_lifetime_ended(flow3_map, m))
            {
                return true;
            }
        }
        return false;
    }

    /* true if at least one alternative is expired or lifetime_ended.
     * does_not_exist alternatives are skipped — a non-existent object
     * (null-pointer arm) has no state to report. */
    const struct flow3_key_alternatives* e = flow3_map_search_up(flow3_map, p);
    if (e == NULL)
    {
        return false;
    }
    for (int i = 0; i < e->alternatives.size; i++)
    {
        if (e->alternatives.data[i].absent)
        {
            continue;
        }
        if (e->alternatives.data[i].expired || e->alternatives.data[i].ended)
        {
            return true;
        }
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
        for (struct flow3_map* cur = p_before; cur; cur = cur->p_parent_map)
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
                                alt->value, alt->relation, alt->origin, alt->line);
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
                flow3_alternatives_add_i(&p_dest_entry->alternatives, 0, FLOW3_NOT_EQUAL, p_dest, alt->line);
            }
            else
            {
                flow3_alternatives_add_i(&p_dest_entry->alternatives, 0, FLOW3_EQUAL, p_dest, alt->line);
            }
            continue;
        }

        if (alt->relation == FLOW3_NOT_EQUAL && flow3_value_is_false(alt->value))
        {
            if (true_branch)
            {
                flow3_alternatives_add_i(&p_dest_entry->alternatives, 0, FLOW3_NOT_EQUAL, p_dest, alt->line);
            }
            else
            {
                flow3_alternatives_add_i(&p_dest_entry->alternatives, 0, FLOW3_EQUAL, p_dest, alt->line);
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
                flow3_alternatives_add(&p_dest_entry->alternatives, alt->value, FLOW3_NOT_EQUAL, p_dest, alt->line);
            }
            else
            {
                flow3_alternatives_add(&p_dest_entry->alternatives, alt->value, FLOW3_EQUAL, p_dest, alt->line);
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
            flow3_alternatives_add(&p_dest_entry->alternatives, alt->value, alt->relation, alt->origin, alt->line);
        }
    }

    if (p_dest_entry->alternatives.size == 0)
    {
        for (int k = 0; k < p_existing->alternatives.size; k++)
        {
            if (p_existing->alternatives.data[k].relation == FLOW3_UNINITIALIZED)
            {
                flow3_alternatives_add(&p_dest_entry->alternatives,
                    p_existing->alternatives.data[k].value,
                    p_existing->alternatives.data[k].relation,
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

static void flow3_exit_block_visit_defer_item(struct flow3_visit_ctx* ctx, struct defer_list_item* p_item, struct token* position_token)
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
        struct token* p_token = p_declarator->name_opt ? p_declarator->name_opt : p_declarator->specifier_qualifier_list->first_token;
        const struct marker marker = { .p_token_begin = p_token, .p_token_end = p_token };
        flow3_check_owner_object_at_exit(ctx,
            &p_declarator->type,
            &p_declarator->object,
            &marker);
    }
}

static void flow3_exit_block_visit_defer_list(struct flow3_visit_ctx* ctx, struct defer_list* p_defer_list, struct token* position_token)
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

static void flow3_defer_list_set_end_of_lifetime(struct flow3_visit_ctx* ctx, struct defer_list* p_defer_list, struct token* position_token)
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
        flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
         p_object,
         NOT_APPLICABLE_VALUE,
         FLOW3_NOT_APPLICABLE,
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
        flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
         p_object,
         NOT_APPLICABLE_VALUE,
         FLOW3_NOT_APPLICABLE,
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
                FLOW3_EQUAL, ctx->p_current_flow3_map, line);
        }
        else
        {
            flow3_alternatives_add_object_ptr(&ep->alternatives, NULL,
                FLOW3_NOT_EQUAL, ctx->p_current_flow3_map, line);
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
                    flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                        p_pointed, NOT_APPLICABLE_VALUE, FLOW3_NOT_APPLICABLE, line);
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
        flow3_alternatives_add_object_ptr(&ep->alternatives, NULL, FLOW3_EQUAL, p_null_map, line);
        /* non-null arm */
        if (p_pointed != NULL)
        {
            flow3_alternatives_add_object_ptr(&ep->alternatives, p_pointed, FLOW3_EQUAL, p_nonnull_map, line);
        }
        else
        {
            flow3_alternatives_add_object_ptr(&ep->alternatives, NULL, FLOW3_NOT_EQUAL, p_nonnull_map, line);
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
                flow3_alternatives_add_i(&eo->alternatives, ANY_VALUE, FLOW3_ANY, p_nonnull_map, line);

                /* Null arm: object does not exist when pointer is null. */
                flow3_alternatives_add_does_not_exist(&eo->alternatives, p_null_map, line);
            }
        }
    }
}

static void flow3_assign_rhs_to_lhs(struct flow3_visit_ctx* ctx, const struct object* p_left_obj, struct expression* p_rhs_expr)
{
    const struct object* p_right_obj = &p_rhs_expr->object;
    const int            line = p_rhs_expr->first_token->line;

    if (object_has_known_value(&p_rhs_expr->object)
        && p_rhs_expr->expression_type != EXPR_UNARY_INCREMENT
        && p_rhs_expr->expression_type != EXPR_UNARY_DECREMENT)
    {
        /* Compile-time constant: record exact value. */
        flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
            p_left_obj,
            object_to_signed_long_long(&p_rhs_expr->object),
            FLOW3_EQUAL,
            line);
    }
    else if (p_rhs_expr->declarator != NULL
             && p_rhs_expr->expression_type != EXPR_UNARY_INCREMENT
             && p_rhs_expr->expression_type != EXPR_UNARY_DECREMENT)
    {
        /* Variable reference: copy its current alternatives. */
        struct flow3_key_alternatives* p_rhs_entry =
            flow3_map_search_up(ctx->p_current_flow3_map, p_right_obj);
        if (p_rhs_entry && p_rhs_entry->alternatives.size > 0)
        {
            struct flow3_key_alternatives* p_lhs_entry =
                flow3_map_find_add(ctx->p_current_flow3_map, p_left_obj);
            if (p_lhs_entry)
            {
                flow3_alternatives_clear(&p_lhs_entry->alternatives);
                flow3_alternatives_append(&p_lhs_entry->alternatives,
                                          &p_rhs_entry->alternatives);
            }
        }
        else
        {
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                                             p_left_obj, 0, FLOW3_ANY, line);
        }
    }
    else
    {
        /*
         * Arbitrary expression (--i, ++i, i--, i++, arithmetic, call, cast …).
         * Look up the expression object directly — not via p_right_obj — because
         * pre/post inc/dec produce a result object that differs from the
         * underlying variable.
         */
        const struct flow3_key_alternatives* p_rhs_entry = flow3_map_search_up(ctx->p_current_flow3_map, &p_rhs_expr->object);
        if (p_rhs_entry && p_rhs_entry->alternatives.size > 0)
        {
            struct flow3_key_alternatives* p_lhs_entry = flow3_map_find_add(ctx->p_current_flow3_map, p_left_obj);
            if (p_lhs_entry)
            {
                flow3_alternatives_clear(&p_lhs_entry->alternatives);
                flow3_alternatives_append(&p_lhs_entry->alternatives, &p_rhs_entry->alternatives);
            }
        }
        else
        {
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map, p_left_obj, ANY_VALUE, FLOW3_ANY, line);
        }
    }
}

static void flow3_check_rhs_value(struct flow3_visit_ctx* ctx, const struct type* p_dest_type, struct expression* p_rhs_expr)
{
    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;
    const bool ownership_enabled = ctx->ctx->options.ownership_enabled;

    const struct object* p_rhs_obj = object_get_referenced(&p_rhs_expr->object);

    /* Uninitialized source — always invalid (§2.3/§3.3/§4.4). */
    if (flow3_object_is_uninitialized(ctx->p_current_flow3_map, p_rhs_obj))
    {
        struct marker marker = expression_to_marker(p_rhs_expr);
        diagnostic(W_FLOW_UNINITIALIZED, ctx->ctx, NULL, &marker, "object is uninitialized");
    }
    else if (flow3_object_can_be_uninitialized(ctx->p_current_flow3_map, p_rhs_obj))
    {
        struct marker marker = expression_to_marker(p_rhs_expr);
        diagnostic(W_FLOW_UNINITIALIZED, ctx->ctx, NULL, &marker, "object may be uninitialized");
    }

    /* Expired source (§2.3/§3.3/§4.4) */
    if (flow3_object_can_have_its_lifetime_ended(ctx->p_current_flow3_map, p_rhs_obj))
    {
        struct marker marker = expression_to_marker(p_rhs_expr);
        diagnostic(W_FLOW_LIFETIME_ENDED, ctx->ctx, NULL, &marker, "object lifetime ended");
    }

    /* Moved source — invalid in all contexts (§3.3/§4.4).
     * For _Owner types this is caught by the ownership block below;
     * check here covers non-owner moved objects. */
    if (!type_is_owner(&p_rhs_expr->type) &&
        flow3_object_can_be_moved(ctx->p_current_flow3_map, p_rhs_obj))
    {
        struct marker marker = expression_to_marker(p_rhs_expr);
        diagnostic(W_FLOW_LIFETIME_ENDED, ctx->ctx, NULL, &marker, "object is moved");
    }

    /* Nullable: non-_Opt pointer destination must not receive a possibly-null
     * value (§2.1/§3.1/§4.2). */
    if (nullable_enabled && type_is_pointer(p_dest_type))
    {
        if (!type_is_opt(p_dest_type, nullable_enabled))
        {
            if (flow3_object_can_be_null(ctx->p_current_flow3_map, p_rhs_obj))
            {
                struct marker marker = expression_to_marker(p_rhs_expr);
                diagnostic(W_FLOW_NULL_DEREFERENCE, ctx->ctx, NULL, &marker, "assigning possibly null pointer to non-nullable destination");
            }
        }
        else
        {
            /* _Opt destination: null is allowed but the pointer value (when
             * non-null) must still be valid — not moved or expired (§2.1). */
            if (flow3_object_can_be_moved(ctx->p_current_flow3_map, p_rhs_obj))
            {
                struct marker marker = expression_to_marker(p_rhs_expr);
                diagnostic(W_FLOW_LIFETIME_ENDED, ctx->ctx, NULL, &marker, "assigning a moved pointer to an _Opt destination");
            }
        }
    }

    if (ownership_enabled)
    {
        /* Returning/assigning an _Owner source into a non-_Owner destination
         * silently drops the ownership and leaks the resource (§2.2/§3.2). */
        if (type_is_owner(&p_rhs_expr->type) && !type_is_owner(p_dest_type))
        {
            struct marker marker = expression_to_marker(p_rhs_expr);
            diagnostic(W_FLOW_LIFETIME_ENDED, ctx->ctx, NULL, &marker, "assigning an _Owner object to a non-_Owner destination (resource leak)");
        }

        /* _Owner destination must not receive a moved object (§3.2/§4.3). */
        if (type_is_owner(p_dest_type))
        {
            if (flow3_object_can_be_moved(ctx->p_current_flow3_map, p_rhs_obj))
            {
                struct marker marker = expression_to_marker(p_rhs_expr);
                diagnostic(W_FLOW_LIFETIME_ENDED, ctx->ctx, NULL, &marker, "assigning a moved object to an _Owner destination");
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

            /*
             * Shared diagnostic checks (uninitialized, expired, nullable, ownership).
             * The source object must never be uninitialized — the spec is unconditional
             * across initialization, assignment, and return.
             */
            flow3_check_rhs_value(ctx,
                                  &p_init_declarator->p_declarator->type,
                                  p_init_expr);

            /* Shared alternative-copy: write RHS flow state into LHS map entry. */
            flow3_assign_rhs_to_lhs(ctx, &p_init_declarator->p_declarator->object, p_init_expr);
        }
        else
        {
            if (p_init_declarator->initializer->braced_initializer)
            {
                flow3_visit_bracket_initializer_list(ctx, p_init_declarator->initializer->braced_initializer);
            }
            else
            {
                assert(false);
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
        flow3_check_pointer_used_as_bool(ctx, p_selection_statement->condition->expression);
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

static const struct object* get_lvalue_object_from_expr(const struct expression* p_arg_expr)
{
    if (p_arg_expr->expression_type == EXPR_POSTFIX_ARROW)
    {
        /* p->member: the pointer variable is the left operand. */
        if (p_arg_expr->left != NULL)
        {
            return object_get_referenced(&p_arg_expr->left->object);
        }
    }
    else if (p_arg_expr->expression_type == EXPR_UNARY_CONTENT)
    {
        /* *p: the pointer variable is the right operand. */
        if (p_arg_expr->right != NULL)
        {
            return object_get_referenced(&p_arg_expr->right->object);
        }
    }
    else if (p_arg_expr->expression_type == EXPR_PRIMARY_PARENTHESIS)
    {
        while (p_arg_expr->expression_type == EXPR_PRIMARY_PARENTHESIS)
        {
            assert(p_arg_expr->right != NULL);
            p_arg_expr = p_arg_expr->right;
        }
    }

    return object_get_referenced(&p_arg_expr->object);
}

static void flow3_visit_function_arguments(struct flow3_visit_ctx* ctx, struct type* p_type, struct argument_expression_list* p_argument_expression_list)
{
    try
    {
        const bool ownership_enabled = ctx->ctx->options.ownership_enabled;

        const struct param_list* _Opt p_param_list = type_get_func_or_func_ptr_params(p_type);
        if (p_param_list == NULL)
        {
            throw;
        }

        struct param* _Opt p_current_parameter_type = p_param_list->head;
        struct argument_expression* _Opt p_current_argument = p_argument_expression_list->head;

        //////////////////////////// FIRST PASS ////////////////////////////
        /*
         * Visit each argument expression for its side-effects (updating the
         * flow map, resolving sub-expressions, etc.).  W_FLOW_UNINITIALIZED is
         * suppressed here because the uninitialized check is performed manually
         * in the second pass, where the _Ctor distinction is applied.
         *
         * Motivation: in a call like f(pX, pX->p) the pointed member pX->p
         * must be evaluated with the map state that exists *before* the call
         * rewrites it; deferring the uninitialized diagnostic to the second
         * pass preserves that ordering.
         */
        while (p_current_argument && p_current_parameter_type)
        {
            struct diagnostic temp = ctx->ctx->options.diagnostic_stack.stack[ctx->ctx->options.diagnostic_stack.top_index];

            /* Suppress; will be re-checked per-parameter in the second pass. */
            diagnostic_remove(&ctx->ctx->options.diagnostic_stack.stack[ctx->ctx->options.diagnostic_stack.top_index], W_FLOW_UNINITIALIZED);

            flow3_visit_full_expression(ctx, p_current_argument->expression);

            ctx->ctx->options.diagnostic_stack.stack[ctx->ctx->options.diagnostic_stack.top_index] = temp;

            p_current_argument = p_current_argument->next;
            p_current_parameter_type = p_current_parameter_type->next;
        }

        /* Variadic tail — visit remaining arguments with no paired parameter. */
        while (p_current_argument)
        {
            flow3_visit_full_expression(ctx, p_current_argument->expression);
            p_current_argument = p_current_argument->next;
        }

        //////////////////////////// SECOND PASS ////////////////////////////

        p_current_parameter_type = p_param_list->head;
        p_current_argument = p_argument_expression_list->head;

        while (p_current_argument && p_current_parameter_type)
        {
            const struct type* p_param_type = &p_current_parameter_type->type;
            struct expression* p_arg_expr = p_current_argument->expression;
            const int           line = p_arg_expr->first_token->line;


            /* ------------------------------------------------------------
             * Spec — RHS value checks (uninitialized, expired, moved, nullable,
             *         ownership) — same rules as return and initialization.
             *
             * _Ctor exception: when the parameter is a pointer-to-_Ctor the
             * callee is the constructor; the pointed object is intentionally
             * uninitialized on entry, so we skip flow3_check_rhs_value for
             * that case.  The "becomes ANY" update below still applies.
             *
             * For _Owner arguments flow3_check_rhs_owner_validity runs the
             * member-recursive uninitialized/expired/moved checks, mirroring
             * EXPR_ASSIGNMENT_ASSIGN.
             * ------------------------------------------------------------
             */
            {
                const bool param_is_ctor = type_is_pointer(p_param_type) &&
                    type_is_pointed_ctor(p_param_type);

                if (!param_is_ctor)
                {
                    flow3_check_rhs_value(ctx, p_param_type, p_arg_expr);
                }

                if (ownership_enabled && type_is_owner(&p_arg_expr->type))
                {
                    const struct object* p_arg_obj = object_get_referenced(&p_arg_expr->object);
                    flow3_check_rhs_owner_validity(ctx, p_arg_obj, p_arg_expr);
                }
            }

            if (type_is_pointer(p_param_type))
            {
                /*
                 * Both _Ctor and non-_Ctor: after the call the pointed object's
                 * state is unknown — mark every concrete alias as ANY.
                 *
                 * Use get_pointer_object_from_expr so that p->member arguments
                 * are keyed on the pointer variable (left operand), not on the
                 * transient result node.
                 */
                {
                    const struct object* p_ptr_obj = get_lvalue_object_from_expr(p_arg_expr);
                    const struct flow3_key_alternatives* e = flow3_map_search_up(ctx->p_current_flow3_map, p_ptr_obj);
                    if (e != NULL)
                    {
                        for (int i = 0; i < e->alternatives.size; i++)
                        {
                            const struct flow3_alternative* alt = &e->alternatives.data[i];
                            if (alt->value.kind == FLOW3_VALUE_PTR &&
                                alt->value.as.p != NULL)
                            {
                                flow3_map_replace_alternatives_i(
                                    ctx->p_current_flow3_map,
                                    alt->value.as.p,
                                    ANY_VALUE,
                                    FLOW3_ANY,
                                    line);
                            }
                        }
                    }
                }
            }

            /* ------------------------------------------------------------
             * Spec — Ownership transfer
             *
             *   If `ownership_enabled` is enabled, and the parameter is
             *   marked `_Owner`, then ownership is transferred to the callee
             *   and the argument object becomes expired.
             *   If the parameter is an `_Owner` pointer, then the pointed
             *   object also becomes expired.
             *
             * Spec — Destructor ownership propagation
             *
             *   If `ownership_enabled` is enabled, and the pointed object
             *   type of the parameter is marked `_Dtor`, then both the
             *   argument object and the pointed object become expired.
             *
             *   If the pointed object type is marked `_Clear`, the same
             *   call-site effect applies: the pointed object becomes expired
             *   after the call, and the pointer variable remains live.
             * ------------------------------------------------------------
             */
            if (ownership_enabled)
            {
                const bool param_is_owner = type_is_owner(p_param_type);
                const bool param_is_dtor = type_is_pointed_dtor(p_param_type);
                const bool param_is_clear = type_is_pointed_clear(p_param_type);

                if (param_is_owner || param_is_dtor || param_is_clear)
                {
                    if (type_is_pointer(p_param_type))
                    {
                        /*
                         * void * _Owner exception:
                         * When the parameter is void * _Owner the analyser cannot follow
                         * the pointer to the concrete pointed object.  The callee will
                         * take raw ownership and may free whatever the pointer aliases.
                         * We therefore require that the pointed object is already in a
                         * safe state (zero, uninitialized, moved, or expired) before the
                         * call — the caller must have cleaned it up first.
                         *
                         * For all other _Owner pointer / _Dtor / _Clear parameters:
                         * the pointed object's lifetime ends — the callee may actually
                         * free the memory, so the value is gone (not just moved).
                         */
                         /* _Dtor, _Clear, and _Ctor cannot be applied to void* —
                          * the void * _Owner exception is only for _Owner. */
                        const bool param_points_to_void = param_is_owner && type_is_void((p_param_type));
                        const struct object* p_ptr_obj2 = get_lvalue_object_from_expr(p_arg_expr);
                        const struct flow3_key_alternatives* e = flow3_map_search_up(ctx->p_current_flow3_map, p_ptr_obj2);
                        if (e != NULL)
                        {
                            for (int i = 0; i < e->alternatives.size; i++)
                            {
                                const struct flow3_alternative* alt = &e->alternatives.data[i];
                                if (alt->value.kind == FLOW3_VALUE_PTR &&
                                    alt->value.as.p != NULL)
                                {
                                    if (param_points_to_void)
                                    {
                                        /*
                                         * void * _Owner: check the pointed object is already
                                         * safe — zero, uninitialized, moved, or expired.
                                         * We cannot end its lifetime because we don't know
                                         * its type or layout.
                                         */
                                        const struct object* p_pointed = alt->value.as.p;
                                        const bool is_zero = flow3_object_is_zero(ctx->p_current_flow3_map, p_pointed);
                                        const bool is_uninit = flow3_object_is_uninitialized(ctx->p_current_flow3_map, p_pointed);
                                        const bool is_moved = flow3_object_can_be_moved(ctx->p_current_flow3_map, p_pointed);
                                        const bool is_expired = flow3_object_can_have_its_lifetime_ended(ctx->p_current_flow3_map, p_pointed);
                                        if (!is_zero && !is_uninit && !is_moved && !is_expired)
                                        {
                                            struct marker marker = expression_to_marker(p_arg_expr);
                                            diagnostic(W_FLOW_UNINITIALIZED, ctx->ctx, NULL, &marker,
                                                "pointed object must be zero, uninitialized, moved, or expired "
                                                "before passing as void * _Owner argument");
                                        }
                                    }
                                    else
                                    {
                                        /* Pointed object's lifetime ends — memory may be freed. */
                                        flow3_map_set_object_lifetime_ended(ctx->p_current_flow3_map, alt->value.as.p, line);
                                    }
                                }
                            }
                        }
                    }

                    /*
                     * For _Owner parameters ownership of the argument itself is
                     * transferred to the callee — the pointer is moved (ownership
                     * gone from the caller's side).
                     *
                     * For _Dtor and _Clear parameters only the *contents* of the
                     * pointed object are consumed (lifetime ended above).  The pointer
                     * variable remains live — the caller is expected to free it
                     * afterwards (e.g. my_free(p) after x_destroy(p)).
                     */
                    if (param_is_owner)
                    {
                        flow3_map_set_object_expired(
                            ctx->p_current_flow3_map,
                            object_get_referenced(&p_arg_expr->object),
                            line);
                    }
                }
            }

            /*
             * const pointer: no state change — callee cannot modify the
             * pointed object, and the "becomes ANY" block above already
             * skips const-pointed cases via FLOW3_RELATION_ANY write being
             * irrelevant when ownership_enabled rules don't fire.
             * (The pointed-object ANY update was applied unconditionally for
             * non-owner/non-dtor pointers in the _Ctor block above.)
             */

            p_current_argument = p_current_argument->next;
            p_current_parameter_type = p_current_parameter_type->next;
        }

        /* Variadic tail — no parameter rules to apply. */
        while (p_current_argument)
        {
            p_current_argument = p_current_argument->next;
        }
    }
    catch
    {
    }
}

static void flow3_check_pointer_used_as_bool(struct flow3_visit_ctx* ctx, struct expression* p_expression)
{
    if (ctx->inside_loop || ctx->expression_is_not_evaluated)
    {
        return;
    }

    struct marker marker = expression_to_marker(p_expression);

    if (type_is_pointer(&p_expression->type))
    {
        if (flow3_object_is_null(ctx->p_current_flow3_map, &p_expression->object))
        {
            diagnostic(W_FLOW_NON_NULL,
                    ctx->ctx,
                    NULL,
                    &marker,
                    "pointer is always null");
        }
        else if (flow3_object_is_not_null(ctx->p_current_flow3_map, &p_expression->object))
        {
            diagnostic(W_FLOW_NON_NULL,
                    ctx->ctx,
                    NULL,
                    &marker,
                    "pointer is always not-null");
        }
    }
    else if (type_is_integer(&p_expression->type))
    {
        /*
         * if (a) {} when a has a statically known value, e.g.  a == 5 => always true,
         *                                                        a == 0 => always false.
         */
        long long known = 0;
        if (flow3_object_get_known_equal_int(ctx->p_current_flow3_map, &p_expression->object, &known))
        {
            if (known != 0)
            {
                diagnostic(W_FLOW_NON_NULL, ctx->ctx, NULL, &marker, "condition is always true (value is known == %lld)", known);
            }
            else
            {
                diagnostic(W_FLOW_NON_NULL, ctx->ctx, NULL, &marker, "condition is always false (value is known == 0)");
            }
        }
    }
}

static void flow3_check_dianostic_suppression(struct flow3_visit_ctx* ctx, struct token* p_token)
{
    check_dianostic_suppression_phase(ctx->ctx, p_token, 2);
}

static struct expression* skip_parenthesis(struct expression* expr)
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

    while (p_expression && p_expression->expression_type == EXPR_PRIMARY_PARENTHESIS)
    {
        assert(p_expression->right != NULL);
        p_expression = p_expression->right;
    }

    object_static_debug(ctx, &p_expression->object, first_token, last_token);
}

static struct flow3_branch_pair flow3_visit_full_expression(struct flow3_visit_ctx* ctx, struct expression* p_expression)
{
    return flow3_visit_expression(ctx, p_expression);
}

static void flow3_check_rhs_owner_validity(struct flow3_visit_ctx* ctx, const struct object* p_right_obj, struct expression* p_right_expr)
{
    if (p_right_obj->members.head)
    {
        for (const struct object* m = p_right_obj->members.head; m; m = m->next)
        {
            flow3_check_rhs_owner_validity(ctx, m, p_right_expr);
        }
        return;
    }

    if (flow3_object_is_uninitialized(ctx->p_current_flow3_map, p_right_obj))
    {
        struct marker marker = expression_to_marker(p_right_expr);
        diagnostic(W_FLOW_UNINITIALIZED, ctx->ctx, NULL, &marker, "assigning an uninitialized _Owner object");
    }
    else if (flow3_object_can_be_uninitialized(ctx->p_current_flow3_map, p_right_obj))
    {
        struct marker marker = expression_to_marker(p_right_expr);
        diagnostic(W_FLOW_UNINITIALIZED, ctx->ctx, NULL, &marker, "assigning a possibly uninitialized _Owner object");
    }

    if (flow3_object_can_have_its_lifetime_ended(ctx->p_current_flow3_map, p_right_obj))
    {
        struct marker marker = expression_to_marker(p_right_expr);
        diagnostic(W_FLOW_LIFETIME_ENDED, ctx->ctx, NULL, &marker, "assigning an _Owner object whose lifetime has ended");
    }

    if (flow3_object_can_be_moved(ctx->p_current_flow3_map, p_right_obj))
    {
        struct marker marker = expression_to_marker(p_right_expr);
        diagnostic(W_FLOW_LIFETIME_ENDED, ctx->ctx, NULL, &marker, "assigning a moved _Owner object");
    }
}

static struct flow3_branch_pair flow3_visit_expression(struct flow3_visit_ctx* ctx, struct expression* p_expression)
{
    struct flow3_map* p_before = ctx->p_current_flow3_map;
    (void)p_before; /* used in some cases below */
    struct object* p_expresssion_object = &p_expression->object;
    p_expresssion_object;

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


        if (ctx->ctx->options.do_static_debug)
        {
            flow3_expression_static_debug(ctx, p_expression);
        }
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
        /*
           pointer->member
           (*pointer).member
        */

        assert(p_expression->left != NULL);

        flow3_visit_expression(ctx, p_expression->left);

        const int member_index = p_expression->member_index;

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

                /* left->object holds PTR alternatives (concrete arena objects). */
                //if (ptr_alt->relation != FLOW3_EQUAL ||
                  //  ptr_alt->value.kind != FLOW3_VALUE_PTR ||
//                    ptr_alt->value.as.p == NULL)
  //                  continue;

                /* Step 1: dereference — get the REF alternatives of *p. */
                const struct flow3_key_alternatives* p_pointed_alternatives = flow3_map_search_up(ctx->p_current_flow3_map, ptr_alt->value.as.p);
                if (p_pointed_alternatives == NULL)
                {
                    continue;
                }

                for (int j = 0; j < p_pointed_alternatives->alternatives.size; j++)
                {
                    const struct flow3_alternative* ref_alt = &p_pointed_alternatives->alternatives.data[j];

                    // if (ref_alt->relation != FLOW3_EQUAL ||
                      //   ref_alt->value.kind != FLOW3_VALUE_REF ||
                        // ref_alt->value.as.p == NULL)
                         //continue;

                     /* Step 2: index the member on the struct object. */
                    struct object* member_obj = object_get_member(ref_alt->value.as.p, member_index);
                    if (member_obj == NULL)
                    {
                        assert(false);
                        continue;
                    }

                    flow3_alternatives_add_object_ref(&result_entry->alternatives,
                        member_obj,
                        ctx->p_current_flow3_map,
                        p_expression->first_token->line);

                    /* Narrow both branch maps on this member. */
                    flow3_narrow_map_into(p_true, ctx->p_current_flow3_map, member_obj, true);
                    flow3_narrow_map_into(p_false, ctx->p_current_flow3_map, member_obj, false);

                    any_member_resolved = true;
                }
            }
        }

        if (!any_member_resolved)
        {
            /* No concrete member resolved — fall back to narrowing the
             * transient expression object, preserving pre-existing behaviour. */
            p_true = flow3_narrow_map(&ctx->flow3_map_arena, ctx->p_current_flow3_map, &p_expression->object, true, "arrow-true");
            p_false = flow3_narrow_map(&ctx->flow3_map_arena, ctx->p_current_flow3_map, &p_expression->object, false, "arrow-false");
        }

        flow3_map_remove(ctx->p_current_flow3_map, &p_expression->left->object);
        return (struct flow3_branch_pair) { p_true, p_false };
    }
    break;

    case EXPR_POSTFIX_INCREMENT:
    case EXPR_POSTFIX_DECREMENT:
    {
        assert(p_expression->left != NULL);
        flow3_visit_expression(ctx, p_expression->left);
        {
            long long old_val = 0;
            if (flow3_object_get_known_equal_int(ctx->p_current_flow3_map, &p_expression->left->object, &old_val))
            {
                /*
                 * Operand has a known EQUAL value in the map: compute both the
                 * expression result (the original value, post semantics) and the
                 * new value of the variable itself.
                 */
                const long long new_val = (p_expression->expression_type == EXPR_POSTFIX_INCREMENT)
                    ? old_val + 1 : old_val - 1;

                /* The expression yields the OLD value. */
                flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                    &p_expression->object, old_val, FLOW3_EQUAL,
                    p_expression->first_token->line);

                /* The operand variable is updated to the NEW value. */
                flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                    &p_expression->left->object, new_val, FLOW3_EQUAL,
                    p_expression->first_token->line);
            }
            else
            {
                /*
                 * Operand value unknown — result and updated variable are both
                 * unknown.  Mark the expression object as ANY so downstream
                 * consumers don't read a stale zero.
                 */
                flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                    &p_expression->object, ANY_VALUE, FLOW3_ANY,
                    p_expression->first_token->line);
            }
        }
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
            flow3_alternatives_add_object_ptr(&p_result_alternatives->alternatives, NULL, FLOW3_EQUAL, p_null_map, call_line);

            struct flow3_map* p_nonnull_map = flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, "call-opt-nonnull");

            struct object* p_pointed = flow3_allocated_object_arena_new(&ctx->allocated_object_arena);
            struct type pointed_type = type_remove_pointer(p_ret_type);
            make_object(&pointed_type, p_pointed, MAKE_STATE_ANY, ctx->ctx->options.target);
            type_destroy(&pointed_type);
            struct flow3_map* old = ctx->p_current_flow3_map;
            ctx->p_current_flow3_map = p_nonnull_map;
            flow3_object_init(ctx, p_pointed, &pointed_type, call_line);
            ctx->p_current_flow3_map = old;

            flow3_alternatives_add_object_ptr(&p_result_alternatives->alternatives, p_pointed, FLOW3_EQUAL, p_nonnull_map, call_line);


        }
        else if (type_is_pointer(p_ret_type))
        {
            struct flow3_value v = { .kind = FLOW3_VALUE_PTR, .as = {.p = NULL } };
            flow3_map_replace_alternatives(ctx->p_current_flow3_map, &p_expression->object, v, FLOW3_NOT_EQUAL, call_line);
        }
    }
    break;

    case EXPR_POSTFIX_FUNCTION_LITERAL:
        assert(p_expression->compound_statement != NULL);
        flow3_visit_compound_statement(ctx, p_expression->compound_statement);
        break;

    case EXPR_POSTFIX_COMPOUND_LITERAL:

        assert(p_expression->left == NULL);
        assert(p_expression->right == NULL);
        assert(p_expression->type_name != NULL);
        assert(p_expression->braced_initializer != NULL);
        flow3_visit_bracket_initializer_list(ctx, p_expression->braced_initializer);
        /*
         * A compound literal produces a value that is not tracked as a named
         * variable.  Record ANY so the expression object is not left
         * uninitialized in the map.
         */
        flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
            &p_expression->object, ANY_VALUE, FLOW3_ANY,
            p_expression->first_token->line);
        break;

    case EXPR_UNARY_STATIC_ASSERTION:

        assert(p_expression->static_assertion);
        flow3_visit_static_assertion(ctx, p_expression->static_assertion);

        break;

    case EXPR_UNARY_ALIGNOF_EXPRESSION:
        if (p_expression->right)
        {
            flow3_visit_expression(ctx, p_expression->right);
        }
        break;

    case EXPR_UNARY_ALIGNOF_TYPE:
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
        if (p_expression->right)
        {
            const bool t2 = ctx->expression_is_not_evaluated;
            ctx->expression_is_not_evaluated = true;
            flow3_visit_expression(ctx, p_expression->right);
            ctx->expression_is_not_evaluated = t2;
        }
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
        break;

    case EXPR_UNARY_NOT:
    {
        assert(p_expression->right != NULL);
        flow3_check_pointer_used_as_bool(ctx, p_expression->right);

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

        /* Propagate the boolean result into this expression's object. */
        {
            long long result_value;
            enum flow3_relation result_rel;

            if (flow3_object_is_zero(ctx->p_current_flow3_map,
                &p_expression->right->object))
            {
                /* Operand is definitively zero -> !x is 1. */
                result_value = 1; result_rel = FLOW3_EQUAL;
            }
            else if (flow3_object_is_not_zero(ctx->p_current_flow3_map,
                &p_expression->right->object))
            {
                /* Operand is definitively nonzero -> !x is 0. */
                result_value = 0; result_rel = FLOW3_EQUAL;
            }
            else
            {
                result_value = 0; result_rel = FLOW3_ANY;
            }

            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                &p_expression->object,
                result_value, result_rel,
                p_expression->first_token->line);
        }

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
        break;

    case EXPR_UNARY_INCREMENT:
    case EXPR_UNARY_DECREMENT:
        assert(p_expression->right != NULL);
        flow3_visit_expression(ctx, p_expression->right);
        {
            long long old_val = 0;
            if (flow3_object_get_known_equal_int(ctx->p_current_flow3_map,
                &p_expression->right->object, &old_val))
            {
                /*
                 * Operand has a known EQUAL value in the map: compute the new
                 * value.  Pre-increment/decrement yields the NEW value (unlike
                 * postfix).
                 */
                const long long new_val = (p_expression->expression_type == EXPR_UNARY_INCREMENT)
                    ? old_val + 1 : old_val - 1;

                /* The expression yields the NEW value. */
                flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                    &p_expression->object, new_val, FLOW3_EQUAL,
                    p_expression->first_token->line);

                /* The operand variable is updated to the NEW value. */
                flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                    &p_expression->right->object, new_val, FLOW3_EQUAL,
                    p_expression->first_token->line);
            }
            else
            {
                /*
                 * Operand value unknown — result and updated variable are both
                 * unknown.  Mark the expression object as ANY so downstream
                 * consumers don't read a stale zero.
                 */
                flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                    &p_expression->object, ANY_VALUE, FLOW3_ANY,
                    p_expression->first_token->line);
            }
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
                    ctx->p_current_flow3_map,
                    p_expression->first_token->line);
            }
        }
    }

    break;

    case EXPR_UNARY_CONTENT:
    {
        assert(p_expression->right != NULL);

        flow3_visit_expression(ctx, p_expression->right);

        struct flow3_key_alternatives* result_entry = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
        flow3_alternatives_clear(&result_entry->alternatives);

        const struct flow3_key_alternatives* p_right_alternatives = flow3_map_search_up(ctx->p_current_flow3_map, &p_expression->right->object);

        for (int i = 0; i < p_right_alternatives->alternatives.size; i++)
        {
            const struct flow3_alternative* p_right_alternative = &p_right_alternatives->alternatives.data[i];

            const struct flow3_key_alternatives* p_pointed_alternatives = flow3_map_search_up(ctx->p_current_flow3_map, p_right_alternative->value.as.p);
            for (int j = 0; j < p_pointed_alternatives->alternatives.size; j++)
            {
                const struct flow3_alternative* pointed_alternative = &p_pointed_alternatives->alternatives.data[j];

                flow3_alternatives_add_object_ref(&result_entry->alternatives,
                        pointed_alternative->value.as.p,
                        ctx->p_current_flow3_map,
                        p_expression->first_token->line);
            }
        }
    }
    break;

    case EXPR_ASSIGNMENT_ASSIGN:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);
        struct object* p_right_object = &p_expression->right->object;
        struct object* p_left_object = &p_expression->left->object;
        p_left_object; p_right_object;
        flow3_visit_expression(ctx, p_expression->left);
        flow3_visit_expression(ctx, p_expression->right);

        struct flow3_key_alternatives* p_result_alternatives = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
        flow3_alternatives_clear(&p_result_alternatives->alternatives);

        const struct flow3_key_alternatives* p_left_alternatives = flow3_map_search_up(ctx->p_current_flow3_map, &p_expression->left->object);
        const struct flow3_key_alternatives* p_right_alternatives = flow3_map_search_up(ctx->p_current_flow3_map, &p_expression->right->object);

        struct flow3_map* p_true = flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, ".true");
        struct flow3_map* p_false = flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, ".false");

        for (int i = 0; i < p_left_alternatives->alternatives.size; i++)
        {
            const struct flow3_alternative* p_left_alternative = &p_left_alternatives->alternatives.data[i];
            if (p_left_alternative->relation == FLOW3_EQUAL &&
                p_left_alternative->value.kind == FLOW3_VALUE_REF &&
                p_left_alternative->value.as.p != NULL)
            {
                struct flow3_key_alternatives* p_left_object_alternatives = flow3_map_find_add(ctx->p_current_flow3_map, p_left_alternative->value.as.p);
                flow3_alternatives_clear(&p_left_object_alternatives->alternatives);

                for (int j = 0; j < p_right_alternatives->alternatives.size; j++)
                {

                    const struct flow3_alternative* p_right_alternative = &p_right_alternatives->alternatives.data[j];
                    if (p_right_alternative->relation == FLOW3_EQUAL)
                    {
                        flow3_alternatives_add(&p_left_object_alternatives->alternatives, p_right_alternative->value, FLOW3_EQUAL, ctx->p_current_flow3_map, p_expression->first_token->line);
                    }
                }

                flow3_alternatives_add_object_ref(&p_result_alternatives->alternatives, &p_expression->left->object, ctx->p_current_flow3_map, p_expression->first_token->line);

            }
        }
        flow3_map_remove(ctx->p_current_flow3_map, &p_expression->left->object);
        flow3_map_remove(ctx->p_current_flow3_map, &p_expression->right->object);

        /* Narrow on the member field used as bool. */
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

        /*
         * Uninitialized check must happen BEFORE the map is updated so
         * that we diagnose reading an uninitialised LHS (e.g. b += 1
         * when b was never written).
         */
        if (flow3_object_can_be_uninitialized(ctx->p_current_flow3_map, &p_expression->left->object))
        {
            if (flow3_object_is_uninitialized(ctx->p_current_flow3_map, &p_expression->left->object))
            {
                diagnostic(W_UNINITIALZED, ctx->ctx, p_expression->left->first_token, NULL, "left object is uninitialized");
            }
            else
            {
                diagnostic(W_UNINITIALZED, ctx->ctx, p_expression->left->first_token, NULL, "left object can be uninitialized");
            }
        }

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

        return lhs_pair2;
    }
    case EXPR_MULTIPLICATIVE_DIV:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        if (object_has_constant_value(&p_expression->left->object) &&
            object_has_constant_value(&p_expression->right->object))
        {
            const long long lv = object_to_signed_long_long(&p_expression->left->object);
            const long long rv = object_to_signed_long_long(&p_expression->right->object);
            if (rv == 0)
            {
                diagnostic(W_FLOW_DIVISION_BY_ZERO, ctx->ctx, p_expression->right->first_token, NULL, "division by zero");
            }
            else
            {
                flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                    &p_expression->object, lv / rv, FLOW3_EQUAL,
                    p_expression->first_token->line);
            }
            break;
        }

        flow3_visit_expression(ctx, p_expression->left);

        if (flow3_object_can_be_zero(ctx->p_current_flow3_map, &p_expression->right->object))
        {
            diagnostic(W_FLOW_DIVISION_BY_ZERO, ctx->ctx, p_expression->right->first_token, NULL, "possible division by zero");
        }

        flow3_visit_expression(ctx, p_expression->right);
    }
    break;

    case EXPR_ADDITIVE_PLUS:
    case EXPR_ADDITIVE_MINUS:
    {
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);

        flow3_visit_expression(ctx, p_expression->left);
        flow3_visit_expression(ctx, p_expression->right);

        long long lv = 0, rv = 0;
        const bool left_is_const = flow3_get_known_int(ctx->p_current_flow3_map,
                                        &p_expression->left->object, &lv);
        const bool right_is_const = flow3_get_known_int(ctx->p_current_flow3_map,
                                        &p_expression->right->object, &rv);

        if (left_is_const && right_is_const)
        {
            /* Both sides known: fold to a single constant. */
            const long long result = (p_expression->expression_type == EXPR_ADDITIVE_PLUS)
                ? lv + rv
                : lv - rv;
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                &p_expression->object, result, FLOW3_EQUAL,
                p_expression->first_token->line);
        }
        else if (p_expression->expression_type == EXPR_ADDITIVE_PLUS && right_is_const && rv == 0)
        {
            /*
             * x + 0 == x: copy all of left's current alternatives into this
             * expression's object so downstream consumers see the same value set.
             */
            const struct flow3_key_alternatives* src = flow3_map_search_up(ctx->p_current_flow3_map, &p_expression->left->object);
            struct flow3_key_alternatives* dst = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
            if (dst)
            {
                flow3_alternatives_clear(&dst->alternatives);
                if (src)
                {
                    flow3_alternatives_append(&dst->alternatives, &src->alternatives);
                }
            }
        }
        else if (p_expression->expression_type == EXPR_ADDITIVE_PLUS && left_is_const && lv == 0)
        {
            /*
             * 0 + x == x: copy all of right's current alternatives into this
             * expression's object.
             */
            const struct flow3_key_alternatives* src = flow3_map_search_up(ctx->p_current_flow3_map, &p_expression->right->object);
            struct flow3_key_alternatives* dst = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
            if (dst)
            {
                flow3_alternatives_clear(&dst->alternatives);
                if (src)
                {
                    flow3_alternatives_append(&dst->alternatives, &src->alternatives);
                }
            }
        }
        else if (p_expression->expression_type == EXPR_ADDITIVE_MINUS && right_is_const && rv == 0)
        {
            /*
             * x - 0 == x: copy all of left's current alternatives into this
             * expression's object.
             */
            const struct flow3_key_alternatives* src = flow3_map_search_up(ctx->p_current_flow3_map, &p_expression->left->object);
            struct flow3_key_alternatives* dst = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
            if (dst)
            {
                flow3_alternatives_clear(&dst->alternatives);
                if (src)
                {
                    flow3_alternatives_append(&dst->alternatives, &src->alternatives);
                }
            }
        }
        else if (p_expression->expression_type == EXPR_ADDITIVE_MINUS &&
                 object_get_referenced(&p_expression->left->object) ==
                 object_get_referenced(&p_expression->right->object))
        {
            /*
             * x - x == 0: both operands are the same object, so the result is
             * always zero regardless of the runtime value of x.
             */
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                &p_expression->object, 0, FLOW3_EQUAL,
                p_expression->first_token->line);
        }
        else
        {
            /* Result not statically known. */
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                &p_expression->object, ANY_VALUE, FLOW3_ANY,
                p_expression->first_token->line);
        }
        break;
    }

    case EXPR_MULTIPLICATIVE_MULT:
    {
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);
        flow3_visit_expression(ctx, p_expression->left);
        flow3_visit_expression(ctx, p_expression->right);

        long long lv = 0, rv = 0;
        const bool left_is_const = flow3_get_known_int(ctx->p_current_flow3_map, &p_expression->left->object, &lv);
        const bool right_is_const = flow3_get_known_int(ctx->p_current_flow3_map, &p_expression->right->object, &rv);

        if (left_is_const && right_is_const)
        {
            /* Both sides known: fold directly. */
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                &p_expression->object, lv * rv, FLOW3_EQUAL,
                p_expression->first_token->line);
        }
        else if (right_is_const && rv == 1)
        {
            /*
             * x * 1 == x: propagate left's alternatives unchanged.
             */
            const struct flow3_key_alternatives* src = flow3_map_search_up(ctx->p_current_flow3_map, &p_expression->left->object);
            struct flow3_key_alternatives* dst = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
            if (dst)
            {
                flow3_alternatives_clear(&dst->alternatives);
                if (src)
                {
                    flow3_alternatives_append(&dst->alternatives, &src->alternatives);
                }
            }
        }
        else if (left_is_const && lv == 1)
        {
            /*
             * 1 * x == x: propagate right's alternatives unchanged.
             */
            const struct flow3_key_alternatives* src = flow3_map_search_up(ctx->p_current_flow3_map, &p_expression->right->object);
            struct flow3_key_alternatives* dst = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
            if (dst)
            {
                flow3_alternatives_clear(&dst->alternatives);
                if (src)
                {
                    flow3_alternatives_append(&dst->alternatives, &src->alternatives);
                }
            }
        }
        else if ((right_is_const && rv == 0) || (left_is_const && lv == 0))
        {
            /*
             * x * 0 == 0 or 0 * x == 0: result is always zero.
             */
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                &p_expression->object, 0, FLOW3_EQUAL,
                p_expression->first_token->line);
        }
        else
        {
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                &p_expression->object, ANY_VALUE, FLOW3_ANY,
                p_expression->first_token->line);
        }
        break;
    }

    case EXPR_CAST:
    case EXPR_MULTIPLICATIVE_MOD:
    case EXPR_SHIFT_RIGHT:
    case EXPR_SHIFT_LEFT:
        if (object_has_constant_value(&p_expression->left->object) &&
            object_has_constant_value(&p_expression->right->object))
        {
            const long long lv = object_to_signed_long_long(&p_expression->left->object);
            const long long rv = object_to_signed_long_long(&p_expression->right->object);
            long long result = 0;
            switch (p_expression->expression_type)
            {
            case EXPR_MULTIPLICATIVE_MOD:  result = rv != 0 ? lv % rv : 0; break;
            case EXPR_ADDITIVE_PLUS:       result = lv + rv;               break;
            case EXPR_ADDITIVE_MINUS:      result = lv - rv;               break;
            case EXPR_SHIFT_RIGHT:         result = lv >> rv;              break;
            case EXPR_SHIFT_LEFT:          result = lv << rv;              break;
            default:                       result = 0;                     break;
            }
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                &p_expression->object, result, FLOW3_EQUAL,
                p_expression->first_token->line);
            break;
        }
        if (p_expression->left)
        {
            flow3_visit_expression(ctx, p_expression->left);
        }
        if (p_expression->right)
        {
            flow3_visit_expression(ctx, p_expression->right);
        }
        flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
            &p_expression->object, ANY_VALUE, FLOW3_ANY,
            p_expression->first_token->line);
        break;

    case EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN:
    case EXPR_RELATIONAL_LESS_OR_EQUAL_THAN:
    case EXPR_RELATIONAL_BIGGER_THAN:
    case EXPR_RELATIONAL_LESS_THAN:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        const bool left_is_constant = object_has_constant_value(&p_expression->left->object);
        const bool right_is_constant = object_has_constant_value(&p_expression->right->object);

        if (left_is_constant && right_is_constant)
        {
            const long long lv = object_to_signed_long_long(&p_expression->left->object);
            const long long rv = object_to_signed_long_long(&p_expression->right->object);
            long long result;
            switch (p_expression->expression_type)
            {
            case EXPR_RELATIONAL_BIGGER_THAN:          result = lv > rv ? 1 : 0; break;
            case EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN: result = lv >= rv ? 1 : 0; break;
            case EXPR_RELATIONAL_LESS_THAN:            result = lv < rv ? 1 : 0; break;
            case EXPR_RELATIONAL_LESS_OR_EQUAL_THAN:   result = lv <= rv ? 1 : 0; break;
            default:                                   result = 0;                break;
            }
            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                &p_expression->object, result, FLOW3_EQUAL,
                p_expression->first_token->line);
            if (result != 0)
            {
                struct flow3_map* p_dead = flow3_map_arena_new(&ctx->flow3_map_arena, p_before, "rel-dead-false");
                return (struct flow3_branch_pair) { ctx->p_current_flow3_map, p_dead };
            }
            else
            {
                struct flow3_map* p_dead = flow3_map_arena_new(&ctx->flow3_map_arena, p_before, "rel-dead-true");
                return (struct flow3_branch_pair) { p_dead, ctx->p_current_flow3_map };
            }
        }

        if (left_is_constant)
        {
            const long long left_value = object_to_signed_long_long(&p_expression->left->object);
            struct flow3_branch_pair right_pair = flow3_visit_expression(ctx, p_expression->right);

            /*
             * Diagnose when the right operand has a statically known value:
             *   e.g.  0 < a  when a is known == 5  =>  always true
             */
            if (!ctx->inside_loop && !ctx->expression_is_not_evaluated)
            {
                long long right_known = 0;
                if (flow3_object_get_known_equal_int(ctx->p_current_flow3_map,
                    &p_expression->right->object,
                    &right_known))
                {
                    bool result;
                    switch (p_expression->expression_type)
                    {
                    case EXPR_RELATIONAL_BIGGER_THAN:          result = left_value > right_known; break;
                    case EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN: result = left_value >= right_known; break;
                    case EXPR_RELATIONAL_LESS_THAN:            result = left_value < right_known; break;
                    case EXPR_RELATIONAL_LESS_OR_EQUAL_THAN:   result = left_value <= right_known; break;
                    default: result = false; break;
                    }
                    struct marker marker = expression_to_marker(p_expression->right);
                    diagnostic(W_FLOW_NON_NULL, ctx->ctx, NULL, &marker,
                               result ? "comparison is always true (value is known == %lld)"
                                      : "comparison is always false (value is known == %lld)",
                               right_known);
                }
            }

            {
                long long right_known2 = 0;
                if (flow3_object_get_known_equal_int(ctx->p_current_flow3_map,
                    &p_expression->right->object, &right_known2))
                {
                    bool result;
                    switch (p_expression->expression_type)
                    {
                    case EXPR_RELATIONAL_BIGGER_THAN:          result = left_value > right_known2;  break;
                    case EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN: result = left_value >= right_known2; break;
                    case EXPR_RELATIONAL_LESS_THAN:            result = left_value < right_known2;  break;
                    case EXPR_RELATIONAL_LESS_OR_EQUAL_THAN:   result = left_value <= right_known2; break;
                    default: result = false; break;
                    }
                    flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                        &p_expression->object,
                        result ? 1 : 0,
                        FLOW3_EQUAL,
                        p_expression->first_token->line);
                }
                else
                {
                    flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                        &p_expression->object,
                        ANY_VALUE, FLOW3_ANY,
                        p_expression->first_token->line);
                }
            }

            if (left_value == 0)
            {
                /*
                 * 0 < x  => x is non-zero on the true branch  (map as NOT_EQUAL 0)
                 * 0 > x  => x is non-zero on the false branch (impossible for unsigned; keep pair)
                 * 0 >= x => x is zero on the true branch      (swap pair)
                 * 0 <= x => x is non-zero on the false branch  (keep pair)
                 */
                bool invert = (p_expression->expression_type == EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN ||
                               p_expression->expression_type == EXPR_RELATIONAL_LESS_OR_EQUAL_THAN);
                if (invert)
                {
                    return (struct flow3_branch_pair) { right_pair.p_false, right_pair.p_true };
                }
                return right_pair;
            }
            else
            {
                /*
                 * N < x  (N != 0):
                 *   true  branch => x > N  — not representable exactly; keep as NOT_EQUAL N
                 *                             (conservative: rules out only x==N on true arm)
                 *   false branch => x <= N — on strict < false means x==N is possible;
                 *                             keep as EQUAL N (best single-value approximation)
                 * Mirror logic applies to the other three operators via the swap below.
                 *
                 * This gives the analyser basic knowledge after comparisons like
                 *   if (n < MAX)  or  if (x > threshold)
                 * without implementing full range intervals.
                 */
                const struct object* p_right_obj = &p_expression->right->object;

                bool strict = (p_expression->expression_type == EXPR_RELATIONAL_LESS_THAN ||
                               p_expression->expression_type == EXPR_RELATIONAL_BIGGER_THAN);

                struct flow3_map* p_true_map =
                    flow3_map_arena_new(&ctx->flow3_map_arena, p_before, "rel-true");
                struct flow3_map* p_false_map =
                    flow3_map_arena_new(&ctx->flow3_map_arena, p_before, "rel-false");

                if (strict)
                {
                    /* true arm: variable != N  (conservative under-approximation) */
                    flow3_map_replace_alternatives_i(p_true_map, p_right_obj,
                        left_value, FLOW3_NOT_EQUAL,
                        p_expression->left->first_token->line);
                    /* false arm: variable == N  (false of strict < means >=N; EQUAL is safe) */
                    flow3_map_replace_alternatives_i(p_false_map, p_right_obj,
                        left_value, FLOW3_EQUAL,
                        p_expression->left->first_token->line);
                }
                /* For <= / >= with non-zero N both arms are too imprecise to narrow usefully;
                   leave the maps empty (inherits parent state). */

                   /*
                    * 0 < x  is already handled above (left_value == 0 branch).
                    * For N > x (left_value > 0) the roles swap: true arm means x < N.
                    * Re-use the same maps — the pair orientation matches the operator.
                    */
                bool swap_arms =
                    (p_expression->expression_type == EXPR_RELATIONAL_BIGGER_THAN ||
                     p_expression->expression_type == EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN);

                if (swap_arms)
                {
                    return (struct flow3_branch_pair) { p_false_map, p_true_map };
                }
                return (struct flow3_branch_pair) { p_true_map, p_false_map };
            }
        }
        else if (right_is_constant)
        {
            const long long right_value = object_to_signed_long_long(&p_expression->right->object);
            struct flow3_branch_pair left_pair = flow3_visit_expression(ctx, p_expression->left);

            /*
             * Diagnose when the left operand has a statically known value:
             *   e.g.  a > 0  when a is known == 5  =>  always true
             */
            if (!ctx->inside_loop && !ctx->expression_is_not_evaluated)
            {
                long long left_known = 0;
                if (flow3_object_get_known_equal_int(ctx->p_current_flow3_map,
                    &p_expression->left->object,
                    &left_known))
                {
                    bool result;
                    switch (p_expression->expression_type)
                    {
                    case EXPR_RELATIONAL_BIGGER_THAN:          result = left_known > right_value; break;
                    case EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN: result = left_known >= right_value; break;
                    case EXPR_RELATIONAL_LESS_THAN:            result = left_known < right_value; break;
                    case EXPR_RELATIONAL_LESS_OR_EQUAL_THAN:   result = left_known <= right_value; break;
                    default: result = false; break;
                    }
                    struct marker marker = expression_to_marker(p_expression->left);
                    diagnostic(W_FLOW_NON_NULL, ctx->ctx, NULL, &marker,
                               result ? "comparison is always true (value is known == %lld)"
                                      : "comparison is always false (value is known == %lld)",
                               left_known);
                }
            }

            /*
             * Store the boolean result (0 or 1) of this comparison into
             * p_expression->object so downstream consumers such as
             * assert_state, static_debug, and "int b = (a > 0)" can read it.
             * We fold to a concrete value only when the left operand's value
             * is also statically known; otherwise we record FLOW3_ANY.
             */
            {
                long long left_known2 = 0;
                if (flow3_object_get_known_equal_int(ctx->p_current_flow3_map,
                    &p_expression->left->object, &left_known2))
                {
                    bool result;
                    switch (p_expression->expression_type)
                    {
                    case EXPR_RELATIONAL_BIGGER_THAN:          result = left_known2 > right_value;  break;
                    case EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN: result = left_known2 >= right_value; break;
                    case EXPR_RELATIONAL_LESS_THAN:            result = left_known2 < right_value;  break;
                    case EXPR_RELATIONAL_LESS_OR_EQUAL_THAN:   result = left_known2 <= right_value; break;
                    default: result = false; break;
                    }
                    flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                        &p_expression->object,
                        result ? 1 : 0,
                        FLOW3_EQUAL,
                        p_expression->first_token->line);
                }
                else
                {
                    flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                        &p_expression->object,
                        ANY_VALUE, FLOW3_ANY,
                        p_expression->first_token->line);
                }
            }

            if (right_value == 0)
            {
                bool invert = (p_expression->expression_type == EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN ||
                               p_expression->expression_type == EXPR_RELATIONAL_LESS_OR_EQUAL_THAN);
                if (invert)
                {
                    return (struct flow3_branch_pair) { left_pair.p_false, left_pair.p_true };
                }
                return left_pair;
            }
            else
            {
                /*
                 * x > N  (N != 0):
                 *   true  branch => x != N  (conservative)
                 *   false branch => x == N  (safe single-value approximation for strict >)
                 *
                 * x < N  (N != 0):
                 *   true  branch => x != N
                 *   false branch => x == N
                 */
                const struct object* p_left_obj = &p_expression->left->object;

                bool strict = (p_expression->expression_type == EXPR_RELATIONAL_LESS_THAN ||
                               p_expression->expression_type == EXPR_RELATIONAL_BIGGER_THAN);

                struct flow3_map* p_true_map2 = flow3_map_arena_new(&ctx->flow3_map_arena, p_before, "rel-true");
                struct flow3_map* p_false_map2 = flow3_map_arena_new(&ctx->flow3_map_arena, p_before, "rel-false");

                if (strict)
                {
                    flow3_map_replace_alternatives_i(p_true_map2, p_left_obj,
                        right_value, FLOW3_NOT_EQUAL,
                        p_expression->right->first_token->line);

                    flow3_map_replace_alternatives_i(p_false_map2, p_left_obj,
                        right_value, FLOW3_EQUAL,
                        p_expression->right->first_token->line);
                }

                /*
                 * x < N (true = x!=N, false = x==N) — already correct.
                 * x > N (true = x!=N, false = x==N) — same orientation.
                 * x <= N (no useful narrowing) — maps inherit parent.
                 * x >= N (no useful narrowing) — maps inherit parent.
                 */
                return (struct flow3_branch_pair) { p_true_map2, p_false_map2 };
            }
        }
        else
        {
            flow3_visit_expression(ctx, p_expression->left);
            flow3_visit_expression(ctx, p_expression->right);

            /*
             * Neither operand is a compile-time constant.  Before giving up,
             * try to fold by looking up the known EQUAL value for each side.
             * If both resolve to a single statically-known integer we can
             * evaluate the comparison exactly.
             *
             * Example: inside  if (x == 3) { if (y == 7) { compile_assert(x < y); } }
             * the map knows x==3 and y==7, so  x < y  folds to  3 < 7  → 1 (true).
             */
            long long left_known = 0;
            long long right_known = 0;
            bool left_ok = flow3_object_get_known_equal_int(ctx->p_current_flow3_map, &p_expression->left->object, &left_known);
            bool right_ok = flow3_object_get_known_equal_int(ctx->p_current_flow3_map, &p_expression->right->object, &right_known);

            if (left_ok && right_ok)
            {
                long long result;
                switch (p_expression->expression_type)
                {
                case EXPR_RELATIONAL_BIGGER_THAN:          result = left_known > right_known ? 1 : 0; break;
                case EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN: result = left_known >= right_known ? 1 : 0; break;
                case EXPR_RELATIONAL_LESS_THAN:            result = left_known < right_known ? 1 : 0; break;
                case EXPR_RELATIONAL_LESS_OR_EQUAL_THAN:   result = left_known <= right_known ? 1 : 0; break;
                default:                                   result = 0;                                  break;
                }
                flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                    &p_expression->object, result, FLOW3_EQUAL,
                    p_expression->first_token->line);
            }
            else
            {
                /* Truly unknown — record ANY so downstream consumers don't read a stale zero. */
                flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                    &p_expression->object, ANY_VALUE, FLOW3_ANY,
                    p_expression->first_token->line);
            }
        }
    }
    break;

    case EXPR_EQUALITY_NOT_EQUAL:
    case EXPR_EQUALITY_EQUAL:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        flow3_visit_expression(ctx, p_expression->left);
        flow3_visit_expression(ctx, p_expression->right);

        struct flow3_key_alternatives* p_result_alternatives = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
        flow3_alternatives_clear(&p_result_alternatives->alternatives);

        const struct flow3_key_alternatives* p_left_alternatives = flow3_map_search_up(ctx->p_current_flow3_map, &p_expression->left->object);
        const struct flow3_key_alternatives* p_right_alternatives = flow3_map_search_up(ctx->p_current_flow3_map, &p_expression->right->object);

        struct flow3_map* p_true = flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, "eq-true");
        struct flow3_map* p_false = flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, "eq-false");

        for (int i = 0; i < p_left_alternatives->alternatives.size; i++)
        {
            const struct flow3_alternative* p_left_alt = &p_left_alternatives->alternatives.data[i];

            const struct flow3_key_alternatives* p_left_resolved = p_left_alternatives;
            if (p_left_alt->relation == FLOW3_EQUAL &&
                p_left_alt->value.kind == FLOW3_VALUE_REF &&
                p_left_alt->value.as.p != NULL)
            {
                p_left_resolved = flow3_map_search_up(ctx->p_current_flow3_map, p_left_alt->value.as.p);
            }

            for (int j = 0; j < p_right_alternatives->alternatives.size; j++)
            {
                const struct flow3_alternative* p_right_alt = &p_right_alternatives->alternatives.data[j];

                const struct flow3_key_alternatives* p_right_resolved = p_right_alternatives;
                if (p_right_alt->relation == FLOW3_EQUAL &&
                    p_right_alt->value.kind == FLOW3_VALUE_REF &&
                    p_right_alt->value.as.p != NULL)
                {
                    p_right_resolved = flow3_map_search_up(ctx->p_current_flow3_map, p_right_alt->value.as.p);
                }

                for (int li = 0; li < p_left_resolved->alternatives.size; li++)
                {
                    const struct flow3_alternative* lv = &p_left_resolved->alternatives.data[li];
                    for (int rj = 0; rj < p_right_resolved->alternatives.size; rj++)
                    {
                        const struct flow3_alternative* rv = &p_right_resolved->alternatives.data[rj];

                        if (lv->relation == FLOW3_EQUAL && rv->relation == FLOW3_EQUAL)
                        {
                            bool values_are_same = flow3_value_is_same(lv->value, rv->value);
                            bool result_is_true =
                                (p_expression->expression_type == EXPR_EQUALITY_EQUAL)
                                ? values_are_same
                                : !values_are_same;

                            flow3_alternatives_add_i(&p_result_alternatives->alternatives,
                                result_is_true ? 1 : 0, FLOW3_EQUAL,
                                ctx->p_current_flow3_map,
                                p_expression->first_token->line);

                            /* Narrow branch maps on the left object */
                            if (p_expression->expression_type == EXPR_EQUALITY_EQUAL)
                            {
                                flow3_map_replace_alternatives(p_true,
                                    &p_expression->left->object, rv->value, FLOW3_EQUAL,
                                    p_expression->first_token->line);
                                flow3_map_replace_alternatives(p_false,
                                    &p_expression->left->object, rv->value, FLOW3_NOT_EQUAL,
                                    p_expression->first_token->line);
                            }
                            else
                            {
                                flow3_map_replace_alternatives(p_true,
                                    &p_expression->left->object, rv->value, FLOW3_NOT_EQUAL,
                                    p_expression->first_token->line);
                                flow3_map_replace_alternatives(p_false,
                                    &p_expression->left->object, rv->value, FLOW3_EQUAL,
                                    p_expression->first_token->line);
                            }
                        }
                        else
                        {
                            flow3_alternatives_add_i(&p_result_alternatives->alternatives,
                                ANY_VALUE, FLOW3_ANY,
                                ctx->p_current_flow3_map,
                                p_expression->first_token->line);
                        }
                    }
                }
            }
        }

        return (struct flow3_branch_pair) { p_true, p_false };
    }

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

        flow3_check_pointer_used_as_bool(ctx, p_expression->left);
        flow3_check_pointer_used_as_bool(ctx, p_expression->right);

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

        /* Propagate the boolean result into this expression's object. */
        {
            /*
             * left->object was written into p_before (= ctx->p_current_flow3_map).
             * right->object was written into left_pair.p_false.
             * Check each result in the map where it was actually evaluated.
             */
            bool left_is_zero = flow3_object_is_zero(p_before, &p_expression->left->object);
            bool left_is_true = flow3_object_is_not_zero(p_before, &p_expression->left->object);
            bool right_is_zero = flow3_object_is_zero(left_pair.p_false, &p_expression->right->object);
            bool right_is_true = flow3_object_is_not_zero(left_pair.p_false, &p_expression->right->object);

            long long result_value;
            enum flow3_relation result_rel;

            if (left_is_true || right_is_true)
            {
                /* At least one side is definitively nonzero -> always true. */
                result_value = 1; result_rel = FLOW3_EQUAL;
            }
            else if (left_is_zero && right_is_zero)
            {
                /* Both sides are definitively zero -> always false. */
                result_value = 0; result_rel = FLOW3_EQUAL;
            }
            else
            {
                result_value = 0; result_rel = FLOW3_ANY;
            }

            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                &p_expression->object,
                result_value, result_rel,
                p_expression->first_token->line);
        }

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

        flow3_check_pointer_used_as_bool(ctx, p_expression->left);
        flow3_check_pointer_used_as_bool(ctx, p_expression->right);

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

        /* Propagate the boolean result into this expression's object. */
        {
            /*
             * left->object was written into p_before.
             * right->object was written into left_pair.p_true.
             * Check each result in the map where it was actually evaluated.
             */
            bool left_is_zero = flow3_object_is_zero(p_before, &p_expression->left->object);
            bool left_is_true = flow3_object_is_not_zero(p_before, &p_expression->left->object);
            bool right_is_zero = flow3_object_is_zero(left_pair.p_true, &p_expression->right->object);
            bool right_is_true = flow3_object_is_not_zero(left_pair.p_true, &p_expression->right->object);

            long long result_value;
            enum flow3_relation result_rel;

            if (left_is_zero || right_is_zero)
            {
                /* At least one side is definitively zero -> always false. */
                result_value = 0; result_rel = FLOW3_EQUAL;
            }
            else if (left_is_true && right_is_true)
            {
                /* Both sides are definitively nonzero -> always true. */
                result_value = 1; result_rel = FLOW3_EQUAL;
            }
            else
            {
                result_value = 0; result_rel = FLOW3_ANY;
            }

            flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                &p_expression->object,
                result_value, result_rel,
                p_expression->first_token->line);
        }

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

        flow3_check_pointer_used_as_bool(ctx, p_expression->condition_expr);
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
                    flow3_alternatives_add_i(&p_result_entry->alternatives, ANY_VALUE, FLOW3_ANY, ctx->p_current_flow3_map,
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
    //const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

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
        flow3_check_pointer_used_as_bool(ctx, p_condition);
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
                object_set_add(&all_keys, e->p_obj_key);
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

        /* Check leaves: for struct objects recurse into members. */
        if (p_obj->members.head)
        {
            for (const struct object* m = p_obj->members.head; m; m = m->next)
            {
                const bool m_uninit = flow3_object_is_uninitialized(ctx->p_current_flow3_map, m);
                const bool m_moved = flow3_object_can_be_moved(ctx->p_current_flow3_map, m);
                const bool m_expired = flow3_object_can_have_its_lifetime_ended(ctx->p_current_flow3_map, m);

                if (m_uninit || m_moved || m_expired)
                {
                    const bool can_uninit = flow3_object_can_be_uninitialized(ctx->p_current_flow3_map, m);
                    if (can_uninit || m_moved || m_expired)
                    {
                        diagnostic(W_FLOW_UNINITIALIZED, ctx->ctx, NULL, marker,
                            "file-scope object member may be uninitialized, "
                            "expired, or moved at function exit");
                    }
                    else
                    {
                        diagnostic(W_FLOW_UNINITIALIZED, ctx->ctx, NULL, marker,
                            "file-scope object member is uninitialized, "
                            "expired, or moved at function exit");
                    }
                }
            }
        }
        else
        {
            const bool uninit = flow3_object_is_uninitialized(ctx->p_current_flow3_map, p_obj);
            const bool moved = flow3_object_can_be_moved(ctx->p_current_flow3_map, p_obj);
            const bool expired = flow3_object_can_have_its_lifetime_ended(ctx->p_current_flow3_map, p_obj);

            if (uninit || moved || expired)
            {
                const bool can_uninit = flow3_object_can_be_uninitialized(ctx->p_current_flow3_map, p_obj);
                if (can_uninit || moved || expired)
                {
                    diagnostic(W_FLOW_UNINITIALIZED, ctx->ctx, NULL, marker,
                        "file-scope object may be uninitialized, "
                        "expired, or moved at function exit");
                }
                else
                {
                    diagnostic(W_FLOW_UNINITIALIZED, ctx->ctx, NULL, marker,
                        "file-scope object is uninitialized, "
                        "expired, or moved at function exit");
                }
            }
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

    if (ctx->p_current_function_declaration != NULL)
    {
        flow3_check_params_at_function_exit(ctx, ctx->p_current_function_declaration);
    }

    flow3_check_arena_objects_at_function_exit(ctx);

    const struct marker marker = {
        .p_token_begin = p_jump_statement->first_token,
        .p_token_end = p_jump_statement->first_token
    };
    flow3_check_file_scope_objects_at_function_exit(ctx, &marker);
}

static void flow3_visit_jump_statement(struct flow3_visit_ctx* ctx, struct jump_statement* p_jump_statement)
{
    //const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;
    try
    {
        if (p_jump_statement->first_token->type == TK_KEYWORD_CAKE_THROW)
        {

            /* accumulate into map-based throw join */
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

            const bool ownership_enabled = ctx->ctx->options.ownership_enabled;

            if (p_jump_statement->expression_opt)
            {
                flow3_visit_full_expression(ctx, p_jump_statement->expression_opt);
            }

            if (p_jump_statement->expression_opt)
            {
                struct expression* p_ret_expr = p_jump_statement->expression_opt;
                const int          line = p_ret_expr->first_token->line;
                const struct object* p_ret_obj =
                    object_get_referenced(&p_ret_expr->object);

                /*
                 * Shared checks: uninitialized, expired, nullable, ownership-moved.
                 */
                flow3_check_rhs_value(ctx, ctx->p_return_type, p_ret_expr);

                /* ------------------------------------------------------------
                 * Spec — Ownership transfer
                 *
                 *   If `ownership_enabled` is enabled, and the return type is
                 *   marked `_Owner`, then ownership is transferred to the
                 *   caller and the source object becomes expired.
                 *   If the return type is an `_Owner` pointer, then the
                 *   pointed object also becomes expired.
                 * ------------------------------------------------------------
                 */
                if (ownership_enabled && type_is_owner(ctx->p_return_type))
                {
                    if (type_is_pointer(ctx->p_return_type))
                    {
                        /*
                         * _Owner pointer: expire every concrete object the
                         * returned pointer may currently alias.
                         */
                        const struct flow3_key_alternatives* e =
                            flow3_map_search_up(ctx->p_current_flow3_map, p_ret_obj);
                        if (e != NULL)
                        {
                            for (int i = 0; i < e->alternatives.size; i++)
                            {
                                const struct flow3_alternative* alt = &e->alternatives.data[i];
                                if (alt->value.kind == FLOW3_VALUE_PTR &&
                                    alt->value.as.p != NULL)
                                {
                                    /* Pointed object's lifetime ends — ownership is transferred
                                     * to the caller who may free it. */
                                    flow3_map_set_object_lifetime_ended(
                                        ctx->p_current_flow3_map, alt->value.as.p, line);
                                }
                            }
                        }
                    }

                    /*
                     * The source object itself is expired — ownership transferred
                     * to the caller, whether the return type is a pointer or
                     * a non-pointer _Owner.
                     */
                    flow3_map_set_object_expired(
                        ctx->p_current_flow3_map,
                        p_ret_obj,
                        line);
                }
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

static bool flow3_alternative_satisfies_token(const struct flow3_alternative* alt, const char* token_start, int token_len)
{
    if (strncmp(token_start, "null", token_len) == 0 ||
        strncmp(token_start, "zero", token_len) == 0)
    {
        return alt->relation == FLOW3_EQUAL && flow3_value_is_false(alt->value);
    }

    if (strncmp(token_start, "not-null", token_len) == 0 ||
        strncmp(token_start, "not-zero", token_len) == 0)
    {
        /* NOT_EQUAL 0  -or-  EQUAL N where N != 0 */
        return (alt->relation == FLOW3_NOT_EQUAL && flow3_value_is_false(alt->value)) ||
            (alt->relation == FLOW3_EQUAL && flow3_value_is_true(alt->value));
    }

    if (strncmp(token_start, "any", token_len) == 0)
    {
        return alt->relation == FLOW3_ANY;
    }

    if (strncmp(token_start, "moved", token_len) == 0)
    {
        return alt->moved;
    }

    if (strncmp(token_start, "absent", token_len) == 0)
    {
        return alt->absent;
    }

    if (strncmp(token_start, "uninitialized", token_len) == 0)
    {
        return alt->relation == FLOW3_UNINITIALIZED;
    }

    return false; /* unknown token */
}

static bool flow3_assert_state_check(struct flow3_visit_ctx* ctx, const struct object* p_obj_key, const char* state_str)
{
    struct flow3_key_alternatives* p_entry =
        flow3_map_search_up(ctx->p_current_flow3_map, p_obj_key);

    if (p_entry == NULL)
    {
        return false;
    }

    const char* p = state_str;
    if (*p == '"')
    {
        p++;
    }

    int expected_count = 0;

    while (*p && *p != '"')
    {
        while (*p == ' ' || *p == '|' || *p == ',') p++;
        if (*p == '\0' || *p == '"')
        {
            break;
        }

        const char* start = p;
        while (*p && *p != ' ' && *p != '|' && *p != ',' && *p != '"') p++;
        int len = (int)(p - start);

        /* verify the token is known */
        bool known = strncmp(start, "null", len) == 0 ||
            strncmp(start, "zero", len) == 0 ||
            strncmp(start, "not-null", len) == 0 ||
            strncmp(start, "not-zero", len) == 0 ||
            strncmp(start, "any", len) == 0 ||
            strncmp(start, "moved", len) == 0 ||
            strncmp(start, "does-not-exist", len) == 0 ||
            strncmp(start, "uninitialized", len) == 0;
        if (!known)
        {
            return false;
        }

        /* find exactly one alternative that satisfies this token,
           and make sure it isn't claimed by another token */
        bool found = false;
        for (int k = 0; k < p_entry->alternatives.size; k++)
        {
            if (flow3_alternative_satisfies_token(&p_entry->alternatives.data[k], start, len))
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            return false;
        }

        expected_count++;
    }

    /* The number of expected tokens must equal the number of actual alternatives.
       Without this check, assert_state("null") would pass even when the object
       has two alternatives (null + not-null), because each token found its match
       and each alternative was covered — but the counts don't agree. */
    if (expected_count != p_entry->alternatives.size)
    {
        return false;
    }

    /* every alternative in the entry must be explained by some token */
    for (int k = 0; k < p_entry->alternatives.size; k++)
    {
        const struct flow3_alternative* alt = &p_entry->alternatives.data[k];
        bool covered = false;

        const char* q = state_str;
        if (*q == '"')
        {
            q++;
        }
        while (*q && *q != '"')
        {
            while (*q == ' ' || *q == '|' || *q == ',') q++;
            if (*q == '\0' || *q == '"')
            {
                break;
            }
            const char* start = q;
            while (*q && *q != ' ' && *q != '|' && *q != ',' && *q != '"') q++;
            int len = (int)(q - start);

            if (flow3_alternative_satisfies_token(alt, start, len))
            {
                covered = true;
                break;
            }
        }

        if (!covered)
        {
            return false;
        } /* unexpected alternative present */
    }

    return true;
}

static void object_static_debug_concrete(struct flow3_visit_ctx* ctx, const struct object* p_object, struct token* first_token, struct token* last_token)
{
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

    const struct object* member = p_object->members.head;
    if (member)
    {
        while (member)
        {
            struct flow3_key_alternatives* p_entry_member = flow3_map_search_up(ctx->p_current_flow3_map, member);
            if (p_entry_member)
            {
                for (int i = 0; i < p_entry_member->alternatives.size; i++)
                {
                    struct flow3_alternative* p_flow3_alternative = &p_entry_member->alternatives.data[i];

                    /* does_not_exist marks the null-pointer arm of an _Opt
                     * pointer.  After narrowing (e.g. inside `if (p)`) the
                     * live map only retains alternatives correlated with the
                     * non-null arm, but the does_not_exist slot on the arena
                     * object is stored at the root map level and therefore
                     * still visible via flow3_map_search_up.  Skip it here
                     * so static_debug never reports "!EXIST" for an object
                     * that is provably reachable. */
                    if (p_flow3_alternative->absent)
                    {
                        continue;
                    }

                    const struct marker m =
                    {
                        .p_token_begin = first_token,
                        .p_token_end = last_token
                    };

                    struct osstream ss = { 0 };
                    flow3_alternative_sprint(&ss, p_flow3_alternative);

                    const char* member_designator =
                        member->member_designator ?
                        member->member_designator : "";

                    diagnostic(W_INFO, ctx->ctx, NULL, &m, "%s %s",
                        member_designator,
                        ss.c_str);
                    ss_close(&ss);
                }
            }
            member = member->next;
        }
    }
    else
    {
        for (int i = 0; i < p_entry->alternatives.size; i++)
        {
            struct flow3_alternative* p_flow3_alternative = &p_entry->alternatives.data[i];

            if (p_flow3_alternative->absent)
            {
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
}

static void object_static_debug(struct flow3_visit_ctx* ctx, const struct object* p_object, struct token* first_token, struct token* last_token)
{
    const struct flow3_key_alternatives* p_entry = flow3_map_search_up(ctx->p_current_flow3_map, p_object);
    if (p_entry == NULL)
    {
        return;
    }

    bool any_ref = false;
    for (int i = 0; i < p_entry->alternatives.size; i++)
    {
        const struct flow3_alternative* alt = &p_entry->alternatives.data[i];
        if (alt->absent)
        {
            continue;
        }
        if (alt->value.kind == FLOW3_VALUE_REF && alt->value.as.p != NULL)
        {
            any_ref = true;
            object_static_debug_concrete(ctx, alt->value.as.p, first_token, last_token);
        }
    }

    if (!any_ref)
    {
        object_static_debug_concrete(ctx, p_object, first_token, last_token);
    }
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
        if (p_static_assertion->string_literal_opt == NULL)
        {
            diagnostic(C_FLOW_ANALIZER_ERROR_STATIC_STATE_FAILED,
                       ctx->ctx,
                       p_static_assertion->first_token, NULL,
                       "assert_state requires a string argument");
        }
        else
        {
            const struct object* p_obj_key =
                object_get_referenced(&p_static_assertion->constant_expression->object);

            if (!flow3_assert_state_check(ctx,
                p_obj_key,
                p_static_assertion->string_literal_opt->lexeme))
            {
                struct flow3_key_alternatives* p_entry = flow3_map_search_up(ctx->p_current_flow3_map, p_obj_key);

                diagnostic(C_FLOW_ANALIZER_ERROR_STATIC_STATE_FAILED,
                           ctx->ctx,
                           p_static_assertion->first_token, NULL,
                           "assert_state failed: expected '%s', got %d alternative(s)",
                           p_static_assertion->string_literal_opt->lexeme,
                           p_entry ? p_entry->alternatives.size : 0);
            }
        }
    }
    else if (p_static_assertion->first_token->type == TK_KEYWORD__COMPILE_ASSERT)
    {

        if (flow3_object_is_not_zero(ctx->p_current_flow3_map, &p_static_assertion->constant_expression->object))
        {
            /* Provably true — assertion holds. */
        }
        else if (flow3_object_is_zero(ctx->p_current_flow3_map, &p_static_assertion->constant_expression->object))
        {
            diagnostic(C_FLOW_ANALIZER_ERROR_STATIC_STATE_FAILED,
                       ctx->ctx,
                       p_static_assertion->first_token, NULL,
                       "compile_assert failed");
        }
        else
        {
            /* Result is ANY or unknown — cannot prove the assertion. */
            diagnostic(C_FLOW_ANALIZER_ERROR_STATIC_STATE_FAILED,
                       ctx->ctx,
                       p_static_assertion->first_token, NULL,
                       "compile_assert: assertion could not be proven");
        }
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
    //const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

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

static void flow3_check_owner_object_at_exit(struct flow3_visit_ctx* ctx, const struct type* p_type, struct object* p_obj, const struct marker* marker)
{
    /* ----------------------------------------------------------------
     * _Ctor pointer: pointed object must be initialized at exit.
     * ---------------------------------------------------------------- */
    if (type_is_pointer(p_type) && type_is_pointed_ctor(p_type))
    {
        const struct object* p_ptr_obj = object_get_referenced(p_obj);
        const struct flow3_key_alternatives* e =
            flow3_map_search_up(ctx->p_current_flow3_map, p_ptr_obj);
        if (e != NULL)
        {
            for (int i = 0; i < e->alternatives.size; i++)
            {
                const struct flow3_alternative* alt = &e->alternatives.data[i];
                if (alt->absent)
                {
                    continue;
                } /* null arm — no object */
                if (alt->value.kind == FLOW3_VALUE_PTR && alt->value.as.p != NULL)
                {
                    const struct object* p_pointed = alt->value.as.p;
                    if (flow3_object_is_uninitialized(ctx->p_current_flow3_map, p_pointed))
                    {
                        diagnostic(W_FLOW_UNINITIALIZED, ctx->ctx, NULL, marker,
                                   "_Ctor: pointed object is uninitialized at end of scope");
                    }
                    else if (flow3_object_can_be_uninitialized(ctx->p_current_flow3_map, p_pointed))
                    {
                        diagnostic(W_FLOW_UNINITIALIZED, ctx->ctx, NULL, marker,
                                   "_Ctor: pointed object may be uninitialized at end of scope");
                    }
                }
            }
        }
    }

    /* ----------------------------------------------------------------
     * _Dtor pointer: pointed object must be uninitialized, moved, or
     * expired at exit.  All three states are valid: the destructor may
     * have zeroed/released the object (uninitialized), transferred its
     * ownership elsewhere (moved), or ended its lifetime via some other
     * path (expired).  Any other state means the destructor failed to
     * consume the object and a resource leak or use-after-destroy is
     * possible.
     * ---------------------------------------------------------------- */
    if (type_is_pointer(p_type) && type_is_pointed_dtor(p_type))
    {
        const struct object* p_ptr_obj = object_get_referenced(p_obj);
        const struct flow3_key_alternatives* e = flow3_map_search_up(ctx->p_current_flow3_map, p_ptr_obj);
        if (e != NULL)
        {
            for (int i = 0; i < e->alternatives.size; i++)
            {
                const struct flow3_alternative* alt = &e->alternatives.data[i];
                if (alt->absent)
                {
                    continue;
                }
                if (alt->value.kind == FLOW3_VALUE_PTR && alt->value.as.p != NULL)
                {
                    const struct object* p_pointed = alt->value.as.p;
                    const bool is_uninit = flow3_object_is_uninitialized(ctx->p_current_flow3_map, p_pointed);
                    const bool is_moved = flow3_object_can_be_moved(ctx->p_current_flow3_map, p_pointed);
                    const bool is_expired = flow3_object_can_have_its_lifetime_ended(ctx->p_current_flow3_map, p_pointed);

                    if (!is_uninit && !is_moved && !is_expired)
                    {
                        const bool can_uninit = flow3_object_can_be_uninitialized(ctx->p_current_flow3_map, p_pointed);
                        if (can_uninit)
                        {
                            diagnostic(W_FLOW_UNINITIALIZED, ctx->ctx, NULL, marker,
                                       "_Dtor: pointed object may not be uninitialized, moved, or expired at end of scope");
                        }
                        else
                        {
                            diagnostic(W_FLOW_UNINITIALIZED, ctx->ctx, NULL, marker,
                                       "_Dtor: pointed object is not uninitialized, moved, or expired at end of scope");
                        }
                    }
                }
            }
        }
    }

    /* ----------------------------------------------------------------
     * _Clear pointer: every leaf of the pointed object must be zero
     * (EQUAL 0) at exit.  The callee is expected to zero-clear the
     * object (e.g. assign members to 0/NULL, memset to 0).
     * For struct objects the check is applied to every leaf member.
     * ---------------------------------------------------------------- */
    if (type_is_pointer(p_type) && type_is_pointed_clear(p_type))
    {
        const struct object* p_ptr_obj = object_get_referenced(p_obj);
        const struct flow3_key_alternatives* e =
            flow3_map_search_up(ctx->p_current_flow3_map, p_ptr_obj);
        if (e != NULL)
        {
            for (int i = 0; i < e->alternatives.size; i++)
            {
                const struct flow3_alternative* alt = &e->alternatives.data[i];
                if (alt->absent)
                {
                    continue;
                }
                if (alt->value.kind == FLOW3_VALUE_PTR && alt->value.as.p != NULL)
                {
                    const struct object* p_pointed = alt->value.as.p;

                    /* For struct objects check every leaf member individually. */
                    if (p_pointed->members.head)
                    {
                        for (const struct object* m = p_pointed->members.head; m; m = m->next)
                        {
                            if (!flow3_object_is_zero(ctx->p_current_flow3_map, m))
                            {
                                if (flow3_object_can_be_zero(ctx->p_current_flow3_map, m))
                                {
                                    diagnostic(W_FLOW_UNINITIALIZED, ctx->ctx, NULL, marker,
                                               "_Clear: pointed object member may not be zero at end of scope");
                                }
                                else
                                {
                                    diagnostic(W_FLOW_UNINITIALIZED, ctx->ctx, NULL, marker,
                                               "_Clear: pointed object member is not zero at end of scope");
                                }
                            }
                        }
                    }
                    else
                    {
                        if (!flow3_object_is_zero(ctx->p_current_flow3_map, p_pointed))
                        {
                            if (flow3_object_can_be_zero(ctx->p_current_flow3_map, p_pointed))
                            {
                                diagnostic(W_FLOW_UNINITIALIZED, ctx->ctx, NULL, marker,
                                           "_Clear: pointed object may not be zero at end of scope");
                            }
                            else
                            {
                                diagnostic(W_FLOW_UNINITIALIZED, ctx->ctx, NULL, marker,
                                           "_Clear: pointed object is not zero at end of scope");
                            }
                        }
                    }
                }
            }
        }
    }

    if (!ctx->ctx->options.ownership_enabled)
    {
        return;
    }

    /* ----------------------------------------------------------------
     * _Owner pointer: pointer itself and every alias must be
     * moved, expired, or uninitialized at exit.
     * ---------------------------------------------------------------- */
    if (type_is_pointer(p_type) && type_is_owner(p_type))
    {
        const struct object* p_ptr_obj = object_get_referenced(p_obj);

        const bool ptr_uninit = flow3_object_is_uninitialized(ctx->p_current_flow3_map, p_ptr_obj);
        const bool ptr_moved = flow3_object_can_be_moved(ctx->p_current_flow3_map, p_ptr_obj);
        const bool ptr_expired = flow3_object_can_have_its_lifetime_ended(ctx->p_current_flow3_map, p_ptr_obj);

        if (!ptr_uninit && !ptr_moved && !ptr_expired)
        {
            const struct flow3_key_alternatives* e =
                flow3_map_search_up(ctx->p_current_flow3_map, p_ptr_obj);
            if (e != NULL)
            {
                for (int i = 0; i < e->alternatives.size; i++)
                {
                    const struct flow3_alternative* alt = &e->alternatives.data[i];
                    if (alt->absent)
                    {
                        continue;
                    } /* null — nothing to own */
                    if (alt->moved || alt->expired || alt->ended)
                    {
                        continue;
                    }
                    /* UNINITIALIZED relation means the pointer slot has been freed/consumed:
                       treat as safe regardless of the value kind. */
                    if (alt->relation == FLOW3_UNINITIALIZED)
                    {
                        continue;
                    }
                    if (alt->value.kind == FLOW3_VALUE_PTR && alt->value.as.p != NULL)
                    {
                        const struct object* p_pointed = alt->value.as.p;
                        const bool pointed_uninit = flow3_object_is_uninitialized(ctx->p_current_flow3_map, p_pointed);
                        const bool pointed_moved = flow3_object_can_be_moved(ctx->p_current_flow3_map, p_pointed);
                        const bool pointed_expired = flow3_object_can_have_its_lifetime_ended(ctx->p_current_flow3_map, p_pointed);

                        if (!pointed_uninit && !pointed_moved && !pointed_expired)
                        {
                            diagnostic(W_FLOW_LIFETIME_ENDED, ctx->ctx, NULL, marker,
                                "_Owner pointer: pointed object is not moved, "
                                "expired, or uninitialized at end of scope (possible resource leak)");
                        }
                    }
                    else if (alt->value.kind == FLOW3_VALUE_PTR && alt->value.as.p == NULL)
                    {
                        /* EQUAL null — pointer is null, nothing leaked */
                    }
                    else
                    {
                        /* Pointer value unknown (ANY relation) — conservative leak warning.
                         * Only warn for truly unknown state; UNINITIALIZED is already
                         * handled by the continue above. */
                        if (alt->relation == FLOW3_ANY)
                        {
                            diagnostic(W_FLOW_LIFETIME_ENDED, ctx->ctx, NULL, marker,
                                "_Owner pointer: pointed object may not be moved, "
                                "expired, or uninitialized at end of scope (possible resource leak)");
                        }
                    }
                }
            }
        }
    }

    /* ----------------------------------------------------------------
     * Non-pointer _Owner: the object (or its owner members for structs)
     * must be expired, uninitialized, or moved at exit.
     * _View objects are excluded: they carry no ownership.
     * ---------------------------------------------------------------- */
    if (!type_is_pointer(p_type) && type_is_owner(p_type))
    {
        const struct object* p_obj_ref = object_get_referenced(p_obj);

        if (p_obj_ref->members.head)
        {
            for (const struct object* m = p_obj_ref->members.head; m; m = m->next)
            {
                if (!type_is_owner(&m->type))
                {
                    continue;
                } /* non-owner member — skip */

                const bool m_uninit = flow3_object_is_uninitialized(ctx->p_current_flow3_map, m);
                const bool m_moved = flow3_object_can_be_moved(ctx->p_current_flow3_map, m);
                const bool m_expired = flow3_object_can_have_its_lifetime_ended(ctx->p_current_flow3_map, m);
                const bool m_can_uninit = flow3_object_can_be_uninitialized(ctx->p_current_flow3_map, m);

                if (!m_uninit && !m_moved && !m_expired)
                {
                    if (m_can_uninit)
                    {
                        diagnostic(W_FLOW_LIFETIME_ENDED, ctx->ctx, NULL, marker,
                            "_Owner member may not be expired, uninitialized, "
                            "or moved at end of scope (possible resource leak)");
                    }
                    else
                    {
                        diagnostic(W_FLOW_LIFETIME_ENDED, ctx->ctx, NULL, marker,
                            "_Owner member is not expired, uninitialized, "
                            "or moved at end of scope (possible resource leak)");
                    }
                }
            }
        }
        else
        {
            const bool uninit = flow3_object_is_uninitialized(ctx->p_current_flow3_map, p_obj_ref);
            const bool moved = flow3_object_can_be_moved(ctx->p_current_flow3_map, p_obj_ref);
            const bool expired = flow3_object_can_have_its_lifetime_ended(ctx->p_current_flow3_map, p_obj_ref);
            const bool can_uninit = flow3_object_can_be_uninitialized(ctx->p_current_flow3_map, p_obj_ref);

            if (!uninit && !moved && !expired)
            {
                if (can_uninit)
                {
                    diagnostic(W_FLOW_LIFETIME_ENDED, ctx->ctx, NULL, marker,
                        "_Owner object may not be expired, uninitialized, "
                        "or moved at end of scope (possible resource leak)");
                }
                else
                {
                    diagnostic(W_FLOW_LIFETIME_ENDED, ctx->ctx, NULL, marker,
                        "_Owner object is not expired, uninitialized, "
                        "or moved at end of scope (possible resource leak)");
                }
            }
        }
    }
}

static void flow3_check_plain_ptr_param_at_exit(struct flow3_visit_ctx* ctx, const struct type* p_type, struct object* p_obj, const struct marker* marker)
{
    /* Only plain pointers: must be a pointer but NOT any special annotation. */
    if (!type_is_pointer(p_type))
    {
        return;
    }
    if (type_is_owner(p_type))
    {
        return;
    }
    if (type_is_pointed_dtor(p_type))
    {
        return;
    }
    if (type_is_pointed_ctor(p_type))
    {
        return;
    }
    if (type_is_pointed_clear(p_type))
    {
        return;
    }

    const struct object* p_ptr_obj = object_get_referenced(p_obj);

    const struct flow3_key_alternatives* e =
        flow3_map_search_up(ctx->p_current_flow3_map, p_ptr_obj);
    if (e == NULL)
    {
        return;
    }

    for (int i = 0; i < e->alternatives.size; i++)
    {
        const struct flow3_alternative* alt = &e->alternatives.data[i];

        /* Null arm — nothing pointed to, nothing to check. */
        if (alt->absent)
        {
            continue;
        }

        /* Pointer is null (EQUAL 0) — nothing pointed to. */
        if (alt->relation == FLOW3_EQUAL &&
            alt->value.kind == FLOW3_VALUE_PTR &&
            alt->value.as.p == NULL)
        {
            continue;
        }

        /* Uninitialized pointer slot — the pointer itself is garbage,
           treat as no reachable object. */
        if (alt->relation == FLOW3_UNINITIALIZED)
        {
            continue;
        }

        if (alt->value.kind == FLOW3_VALUE_PTR && alt->value.as.p != NULL)
        {
            const struct object* p_pointed = alt->value.as.p;

            /* Check leaf members individually for struct objects. */
            if (p_pointed->members.head)
            {
                for (const struct object* m = p_pointed->members.head; m; m = m->next)
                {
                    const bool m_uninit = flow3_object_is_uninitialized(ctx->p_current_flow3_map, m);
                    const bool m_moved = flow3_object_can_be_moved(ctx->p_current_flow3_map, m);
                    const bool m_expired = flow3_object_can_have_its_lifetime_ended(ctx->p_current_flow3_map, m);

                    if (m_uninit || m_moved || m_expired)
                    {
                        const bool can_uninit = flow3_object_can_be_uninitialized(ctx->p_current_flow3_map, m);
                        if (can_uninit || m_moved || m_expired)
                        {
                            diagnostic(W_FLOW_UNINITIALIZED, ctx->ctx, NULL, marker,
                                "pointed object member may be uninitialized, "
                                "expired, or moved at end of scope");
                        }
                        else
                        {
                            diagnostic(W_FLOW_UNINITIALIZED, ctx->ctx, NULL, marker,
                                "pointed object member is uninitialized, "
                                "expired, or moved at end of scope");
                        }
                    }
                }
            }
            else
            {
                const bool uninit = flow3_object_is_uninitialized(ctx->p_current_flow3_map, p_pointed);
                const bool moved = flow3_object_can_be_moved(ctx->p_current_flow3_map, p_pointed);
                const bool expired = flow3_object_can_have_its_lifetime_ended(ctx->p_current_flow3_map, p_pointed);

                if (uninit || moved || expired)
                {
                    const bool can_uninit = flow3_object_can_be_uninitialized(ctx->p_current_flow3_map, p_pointed);
                    if (can_uninit || moved || expired)
                    {
                        diagnostic(W_FLOW_UNINITIALIZED, ctx->ctx, NULL, marker,
                            "pointed object may be uninitialized, "
                            "expired, or moved at end of scope");
                    }
                    else
                    {
                        diagnostic(W_FLOW_UNINITIALIZED, ctx->ctx, NULL, marker,
                            "pointed object is uninitialized, "
                            "expired, or moved at end of scope");
                    }
                }
            }
        }
        /*
         * alt->relation == FLOW3_ANY: pointer value is completely unknown —
         * no concrete arena object to inspect, skip conservatively.
         */
    }
}

static void flow3_check_params_at_function_exit(struct flow3_visit_ctx* ctx, struct declaration* p_declaration)
{
    if (p_declaration->init_declarator_list.head == NULL)
    {
        return;
    }

    struct declarator* p_func_declarator = p_declaration->init_declarator_list.head->p_declarator;

    if (p_func_declarator == NULL ||
        p_func_declarator->direct_declarator == NULL ||
        p_func_declarator->direct_declarator->function_declarator == NULL)
    {
        return;
    }

    struct function_declarator* p_func_decl = p_func_declarator->direct_declarator->function_declarator;

    if (p_func_decl->parameter_type_list_opt == NULL ||
        p_func_decl->parameter_type_list_opt->parameter_list == NULL)
    {
        return;
    }

    struct parameter_declaration* _Opt parameter = p_func_decl->parameter_type_list_opt->parameter_list->head;

    while (parameter)
    {
        if (parameter->declarator != NULL)
        {
            struct token* p_token = parameter->declaration_specifiers->first_token;
            const struct marker marker = { .p_token_begin = p_token, .p_token_end = p_token };
            flow3_check_owner_object_at_exit(ctx,
                &parameter->declarator->type,
                &parameter->declarator->object,
                &marker);
            flow3_check_plain_ptr_param_at_exit(ctx,
                &parameter->declarator->type,
                &parameter->declarator->object,
                &marker);
        }
        parameter = parameter->next;
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

        /*
         * Spec — fall-through exit checks (§1.3–§1.6):
         * When the last item in the body is NOT a return, control can
         * fall off the end of the function.  Run the full exit audit here.
         * When the last item IS a return, flow3_visit_jump_statement has
         * already called all helpers for every explicit return path, so
         * there is nothing left to do and we must not call them again.
         */
        if (!flow3_is_last_item_return(p_declaration->function_body))
        {
            flow3_exit_block_visit_defer_list(ctx, &p_declaration->defer_list, p_declaration->function_body->last_token);
            flow3_defer_list_set_end_of_lifetime(ctx, &p_declaration->defer_list, p_declaration->function_body->last_token);
            flow3_check_params_at_function_exit(ctx, p_declaration);
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