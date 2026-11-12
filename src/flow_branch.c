#pragma safety enable


#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "flow_branch.h"
#include "object.h"
#include "expressions.h"
#include "cake_compat.h"
#include "osstream.h"
#include "error.h"
#include "console.h"

enum
{ 
   FLOW_BRANCH_INITIAL_BUCKETS = 8,
   FLOW_ALLOCATED_OBJECT_ARENA_MAX_SIZE = 5000
};


static unsigned int flow_hash_key(const struct object* obj, int num_of_buckets)
{
    assert(num_of_buckets > 0);

    /*
        Narrow the unsigned value itself: the flow analysis does not carry a
        value relation through the cast, so testing num_of_buckets before
        converting would not rule out a zero divisor.
    */
    const unsigned int buckets = (unsigned int)num_of_buckets;
    if (buckets == 0)
    {
        return 0; /* unreachable: every live map has at least one bucket */
    }

    uintptr_t p = (uintptr_t)obj;
    return (unsigned int)((p ^ (p >> 5)) % buckets);
}

static bool flow_alternatives_same_values(const struct flow_alternatives* a, const struct flow_alternatives* b)
{
    for (int i = 0; i < a->size; i++)
    {
        bool found = false;
        for (int j = 0; j < b->size; j++)
        {
            if (flow_value_is_same(a->data[i], b->data[j]) &&
                a->data[i]->value_relation == b->data[j]->value_relation &&
                a->data[i]->imaginary == b->data[j]->imaginary) //lint 33 33 33 33 33 33
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            return false;
        }
    }
    for (int j = 0; j < b->size; j++)
    {
        bool found = false;
        for (int i = 0; i < a->size; i++)
        {
            if (flow_value_is_same(a->data[i], b->data[j]) &&
                a->data[i]->value_relation == b->data[j]->value_relation &&
                a->data[i]->imaginary == b->data[j]->imaginary) //lint 33 33 33 33 33 33
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            return false;
        }
    }
    return true;
}

void object_set_add(struct object_set* l, const struct object* obj);
void object_set_destroy(_Dtor struct object_set* l);
static bool object_set_table_insert(struct object_set* l, const struct object* obj)
{
    if (l->table == NULL || l->size * 2 >= l->table_capacity)
    {
        int new_capacity = l->table_capacity ? l->table_capacity * 2 : 16;
        const struct object* _Nullable* _Owner new_table = calloc((size_t)new_capacity, sizeof(struct object*));
        if (new_table == NULL)
            return false; /* fall back to inserting into items only; a rare rescan is safe */

        if (l->table != NULL)
        {
            const size_t new_mask = (size_t)new_capacity - 1;
            for (int i = 0; i < l->table_capacity; i++)
            {
                const struct object* existing = l->table[i];
                if (existing == NULL)
                    continue;
                size_t h = ((size_t)(uintptr_t)existing >> 4) & new_mask;
                while (new_table[h] != NULL)
                    h = (h + 1) & new_mask;
                new_table[h] = existing;
            }
            free(l->table);
        }
        l->table = new_table;
        l->table_capacity = new_capacity;
    }

    const size_t mask = (size_t)l->table_capacity - 1;
    size_t h = ((size_t)(uintptr_t)obj >> 4) & mask;
    while (l->table[h] != NULL)
    {
        if (l->table[h] == obj)
            return true; /* already present */
        h = (h + 1) & mask;
    }
    l->table[h] = obj;
    return false;
}

void object_set_add(struct object_set* l, const struct object* obj)
{
    try
    {
        if (object_set_table_insert(l, obj))
        {
            return; /* already present */
        }

        if (l->size == l->capacity)
        {
            int new_capacity = l->capacity ? l->capacity * 2 : 8;
            const struct object** _Owner p = realloc(l->items, new_capacity * sizeof(struct object*));
            if (p == NULL)
            {
                throw;
            }
            l->items = p; //lint 26  (realloc semantics)
            l->capacity = new_capacity;
        }
        l->items[l->size++] = obj; //lint 33
    }
    catch
    {
    }
}

void object_set_destroy(_Dtor struct object_set* l)
{
    free(l->items);
    free(l->table);
}
                              
static void flow_branch_rehash(struct flow_branch* m, int new_num_of_buckets)
{
    assert(new_num_of_buckets > 0);

    struct flow_key_alternatives* _Owner _Nullable* _Owner new_buckets =
        calloc(new_num_of_buckets, sizeof(struct flow_key_alternatives*));

    if (new_buckets == NULL)
    {
        return; /* keep the current table; growing is only an optimization */
    }

    if (m->buckets != NULL)
    {
        for (int i = 0; i < m->num_of_buckets; i++)
        {
            struct flow_key_alternatives* _Owner e = m->buckets[i];
            m->buckets[i] = NULL;

            while (e)
            {
                struct flow_key_alternatives* _Owner next = e->next; /*MOVED*/
                unsigned int idx = flow_hash_key(e->p_obj_key, new_num_of_buckets);
                e->next = new_buckets[idx]; /*MOVED*/
                new_buckets[idx] = e;
                e = next;
            }
        }
        free(m->buckets); //lint 29
    }

    m->buckets = new_buckets;
    m->num_of_buckets = new_num_of_buckets;
}

static void flow_branch_maybe_grow(struct flow_branch* m)
{
    if (m->num_of_entries * 100 > m->num_of_buckets * FLOW_BRANCH_LOAD_FACTOR_PERCENT)
    {
        /*
            Doubling forever briefly holds the old and new bucket arrays live
            together (rehash walks the old table while filling the new one
            before freeing it). Past a threshold, grow by half instead of
            doubling, same reasoning as flow_alternatives_grow.
        */
        enum { FLOW_BRANCH_GROW_DOUBLE_LIMIT = 4096 };
        int new_num_of_buckets = m->num_of_buckets < FLOW_BRANCH_GROW_DOUBLE_LIMIT
            ? m->num_of_buckets * 2
            : m->num_of_buckets + m->num_of_buckets / 2;
                    
        flow_branch_rehash(m, new_num_of_buckets);
    }
}

void flow_branch_remove(struct flow_branch* _Nullable m, const struct object* obj)
{
    if (m == NULL)
        return; /* no map, nothing to remove */

    if (!m->buckets)
    {
        return;
    }

    unsigned int idx = flow_hash_key(obj, m->num_of_buckets);
    struct flow_key_alternatives* _Owner _Nullable* pp = &m->buckets[idx];
    while (*pp)
    {
        struct flow_key_alternatives* _Owner e = *pp;
        if (e->p_obj_key == obj)
        {
            *pp = e->next;
            m->num_of_entries--;
            flow_alternatives_clear(&e->alternatives);
            free(e);
            return;
        }
        pp = &e->next;
    }
}

static void flow_branch_delete(struct flow_branch* _Owner _Nullable m);

void flow_branch_arena_clear(_Clear struct flow_branch_arena* a)
{
    if (a->data != NULL)
    {
        for (int i = 0; i < a->size; i++)
        {
            flow_branch_delete(a->data[i]);
            a->data[i] = NULL;
        }

        free(a->data); //lint 29
    }

    a->data = NULL;
    a->size = 0;
    a->capacity = 0;
}

struct flow_branch* flow_branch_arena_new(struct flow_branch_arena* a, struct flow_branch* _Nullable parent, enum flow_branch_kind kind)
{
    try
    {
        if (a->data == NULL)
        {
            const int initial_capacity = 10;
            a->data = calloc(initial_capacity, sizeof(struct flow_branch*));
            if (a->data == NULL) throw;
            a->size = 0;
            a->capacity = initial_capacity;
        }

        if (a->size == a->capacity)
        {
            enum { FLOW_BRANCH_ARENA_GROW_DOUBLE_LIMIT = 4096 };
            int new_capacity = a->capacity == 0 ? 4
                : a->capacity < FLOW_BRANCH_ARENA_GROW_DOUBLE_LIMIT ? a->capacity * 2
                : a->capacity + a->capacity / 2;
            struct flow_branch* _Owner _Nullable* _Owner new_data = realloc(a->data, new_capacity * sizeof(struct flow_branch*));
            if (new_data == NULL) throw;
            a->data = new_data; //lint 26
            a->capacity = new_capacity;
        }

        struct flow_branch* _Owner obj = calloc(1, sizeof(struct flow_branch));
        if (obj == NULL) throw;
        obj->p_parent_map = parent;
        obj->kind = kind;

        if (parent != NULL)
        {
            parent->child_count++;
        }

        a->data[a->size++] = obj;
        return obj;
    }
    catch
    {
    }
    return NULL;
}

struct flow_branch* flow_branch_arena_new_dead(struct flow_branch_arena* a, struct flow_branch* parent, enum flow_branch_kind kind)
{
    struct flow_branch* m = flow_branch_arena_new(a, parent, kind);
    if (m)
    {
        m->is_unreachable = true;
    }
    return m;
}

void flow_allocated_object_arena_clear(_Clear struct flow_allocated_object_arena* a)
{
    for (int i = 0; i < a->size; i++)
    {
        object_delete(a->data[i]); //lint 33
    }
    free(a->data); //lint 29

    a->data = NULL;
    a->size = 0;
    a->capacity = 0;
}

struct object* flow_allocated_object_arena_new(struct flow_allocated_object_arena* a)
{
    try
    {
        if (a->size >= FLOW_ALLOCATED_OBJECT_ARENA_MAX_SIZE)
        {
            throw;
        }

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
            struct object* _Owner _Nullable* _Owner new_data = realloc(a->data, new_capacity * sizeof(struct object*));
            if (new_data == NULL)
            {
                throw;
            }
            a->data = new_data; //lint 26 (realloc semantics)
            a->capacity = new_capacity;
        }

        struct object* _Owner obj = calloc(1, sizeof(struct object));
        if (obj == NULL)
        {
            throw;
        }

        a->data[a->size++] = obj;
        return obj;
    }
    catch
    {
    }

    return NULL;
}

static const struct flow_key_alternatives* flow_branch_find_const(const struct flow_branch* m, const struct object* obj)
{
    if (!m->buckets)
    {
        return NULL;
    }

    unsigned int idx = flow_hash_key(obj, m->num_of_buckets);
    for (const struct flow_key_alternatives* e = m->buckets[idx]; e; e = e->next)
    {
        if (e->p_obj_key == obj)
        {
            return e;
        }
    }
    return NULL;
}

static struct flow_key_alternatives* flow_branch_find(const struct flow_branch* m, const struct object* obj)
{
    return (struct flow_key_alternatives*)flow_branch_find_const(m, obj);
}

struct flow_key_alternatives* flow_branch_search_up(struct flow_branch* _Nullable m, const struct object* obj)
{
    struct flow_branch* pm = m;
    while (pm)
    {
        struct flow_key_alternatives* p = flow_branch_find(pm, obj);
        if (p)
        {
            return p;
        }
        pm = pm->p_parent_map;
    }
    return NULL;
}

struct flow_branch_entries flow_branch_entries_begin(const struct flow_branch* _Nullable first, const struct flow_branch* _Nullable stop)
{
    struct flow_branch_entries it = { .map = first, .stop = stop, .bucket = -1, .entry = NULL };
    return it;
}

const struct flow_key_alternatives* flow_branch_entries_next(struct flow_branch_entries* it)
{
    if (it->entry != NULL)
    {
        it->entry = it->entry->next;
    }

    while (it->entry == NULL)
    {
        if (it->map == NULL || it->map == it->stop)
        {
            return NULL;
        }

        it->bucket++;
        if (it->map->buckets == NULL || it->bucket >= it->map->num_of_buckets)
        {
            it->map = it->map->p_parent_map;
            it->bucket = -1;
            continue;
        }
        it->entry = it->map->buckets[it->bucket];
    }
    return it->entry;
}

struct flow_key_alternatives* flow_branch_find_add(struct flow_branch* _Nullable m, const struct object* obj)
{
    if (m == NULL)
        return NULL;

    try
    {
        if (!m->buckets)
        {
            m->num_of_buckets = FLOW_BRANCH_INITIAL_BUCKETS;
            m->buckets = calloc(m->num_of_buckets, sizeof(struct flow_key_alternatives*));
            if (m->buckets == NULL)
            {
                throw;
            }
        }

        struct flow_key_alternatives* e = flow_branch_find(m, obj);
        if (e)
        {
            return e;
        }

        /*
            Grow BEFORE inserting: simpler than growing after and having to
            re-derive the new index for the entry we just added. One extra
            rehash near a threshold crossing is not worth the extra code path.
        */
        flow_branch_maybe_grow(m);

        unsigned int idx = flow_hash_key(obj, m->num_of_buckets);
        struct flow_key_alternatives* _Owner e2 = calloc(1, sizeof(struct flow_key_alternatives));
        if (e2 == NULL)
        {
            throw;
        }
        e2->p_obj_key = obj;
        e2->next = m->buckets[idx]; //lint 33
        m->buckets[idx] = e2; //lint 33
        m->num_of_entries++;
        return e2;
    }
    catch
    {
        return NULL;
    }
}

/* a side of an entry pair: made at function entry or at a call (_Nullable, same object), it holds in the whole body and never closes in a join */
bool flow_branch_kind_is_entry_side(enum flow_branch_kind kind)
{
    return kind == FLOW_BRANCH_OPT_NULL || kind == FLOW_BRANCH_OPT_NONNULL ||
           kind == FLOW_BRANCH_SAME_OBJECT || kind == FLOW_BRANCH_NOT_SAME_OBJECT;
}

/* p_map, or an ancestor, is a side of a same-object if (FLOW_BRANCH_SAME_OBJECT / NOT_SAME_OBJECT), or implies one */
bool flow_branch_is_under_same_object(const struct flow_branch* _Nullable p_map)
{
    bool under_same_object = false;
    for (const struct flow_branch* p_branch = p_map; p_branch != NULL && !under_same_object; p_branch = p_branch->p_parent_map)
    {
        for (int fact_index = -1; fact_index < p_branch->implied_facts.count; fact_index++)
        {
            const enum flow_branch_kind kind = fact_index < 0 ? p_branch->kind : p_branch->implied_facts.data[fact_index].kind;
            if (kind == FLOW_BRANCH_SAME_OBJECT || kind == FLOW_BRANCH_NOT_SAME_OBJECT)
            {
                under_same_object = true;
            }
        }
    }
    return under_same_object;
}

void flow_branch_set_object_moved(struct flow_branch* _Nullable m, const struct object* obj, const struct token* _Nullable p_token)
{
    if (m == NULL)
        return;

    if (obj->members.head)
    {
        for (const struct object* member = obj->members.head; member; member = member->next)
        {
            flow_branch_set_object_moved(m, member, p_token);
        }
        return;
    }

    struct flow_key_alternatives* e = flow_branch_find_add(m, obj);
    if (e == NULL) return;
    bool copied_from_above = false;
    if (e->alternatives.size == 0)
    {
        /*
            The values are in a map above (set before the if, moved inside it):
            the move happens to them on this path. Without them the pointer
            would lose its target and read as "may be null".
        */
        const struct flow_key_alternatives* p_above = flow_branch_search_up(m->p_parent_map, obj);
        if (p_above != NULL)
        {
            flow_alternatives_append(&e->alternatives, &p_above->alternatives);
            copied_from_above = true;
        }
    }
    if (e->alternatives.size == 0)
    {
        struct flow_alternative a =
        {
            .value_kind = FLOW_VALUE_KIND_SIGNED,
            .value = {.i = ANY_VALUE},
            .value_relation = FLOW_RELATION_ANY,
            .imaginary = FLOW_IMAGINARY_MOVED,
            .p_origin_map = m,
            .p_origin_token = p_token
        };
        flow_alternatives_add(&e->alternatives, &a);
    }
    else
    {
        for (int i = 0; i < e->alternatives.size; i++)
        {
            /*
                Update .line too, not just .imaginary: .line is what the
                eventual "object 'p' is moved (see line N)" diagnostic
                reports, and it must point at WHERE THE MOVE HAPPENED, not
                wherever this alternative was last seeded (e.g. the
                declaration/initializer line). Leaving the old .line behind
                pointed a double-free/use-after-move report at the variable's
                declaration instead of the first free()/move -- confirmed via
                `p = malloc(1); free(p); free(p);` reporting "(see line
                <declaration>)" instead of "(see line <first free>)".
                User-reported.
            */
            e->alternatives.data[i]->imaginary = FLOW_IMAGINARY_MOVED; //lint 33
            e->alternatives.data[i]->p_origin_token = p_token; //lint 33

            /*
                A variable's value copied from above is moved on this path (what it
                points to ended here too); a same-object side is kept. Not for a
                member: with a pointer carried by a loop the same member is reached
                as several objects, and the move of one would be read on the others.
            */
            if (copied_from_above && obj->parent == NULL &&
                !flow_branch_is_under_same_object(e->alternatives.data[i]->p_origin_map)) //lint 33
            {
                e->alternatives.data[i]->p_origin_map = m; //lint 33
            }
        }
    }
}

void flow_branch_set_object_zero(struct flow_branch* _Nullable m, const struct object* obj, const struct token* _Nullable p_token)
{
    if (m == NULL)
        return;

    try
    {
        if (obj->members.head)
        {
            for (const struct object* member = obj->members.head; member; member = member->next)
            {
                flow_branch_set_object_zero(m, member, p_token);
            }
            return;
        }
        struct flow_key_alternatives* p_flow_key_alternatives = flow_branch_find_add(m, obj);
        if (p_flow_key_alternatives == NULL) throw;

        flow_alternatives_clear(&p_flow_key_alternatives->alternatives);

        struct flow_alternative a =
        {
            .value_kind = FLOW_VALUE_KIND_SIGNED,
            .value = {.i = 0},
            .value_relation = FLOW_RELATION_EQUAL,
            .imaginary = FLOW_IMAGINARY_NONE,
            .p_origin_map = m,
            .p_origin_token = p_token
        };
        flow_alternatives_add(&p_flow_key_alternatives->alternatives, &a);
    }
    catch
    {
    }
}

void flow_branch_set_object_uninitialized(struct flow_branch* _Nullable m, const struct object* obj, const struct token* _Nullable p_token)
{
    if (m == NULL)
        return;

    try
    {
        if (obj->members.head)
        {
            for (const struct object* member = obj->members.head; member; member = member->next)
            {
                flow_branch_set_object_uninitialized(m, member, p_token);
            }
            return;
        }
        struct flow_key_alternatives* p_flow_key_alternatives = flow_branch_find_add(m, obj);
        if (p_flow_key_alternatives == NULL) throw;

        flow_alternatives_clear(&p_flow_key_alternatives->alternatives);

        struct flow_alternative a =
        {
            .value_kind = FLOW_VALUE_KIND_SIGNED,
            .value = {.i = UNINITIALIZED_VALUE},
            .value_relation = FLOW_RELATION_UNINITIALIZED,
            .imaginary = FLOW_IMAGINARY_NONE,
            .p_origin_map = m,
            .p_origin_token = p_token
        };
        flow_alternatives_add(&p_flow_key_alternatives->alternatives, &a);
    }
    catch
    {
    }
}

static void flow_branch_set_object_any_core(struct flow_branch* _Nullable m, const struct object* obj, const struct token* _Nullable p_token, bool nullable_enabled, bool release_owned_pointees, bool reach_pointees);

/*
    A call may modify what the pointer members of its argument point to, one
    level deep (flow.md 6.3): those objects become ANY. This is the only walk
    that follows a pointer member's alternatives to its pointees. The pointees
    are copied first, since setting them adds entries to the branch.

    release_owned_pointees (an experiment from 2026-10-13, on today):
    worst case for a call taking a non-const pointer: the callee may have
    freed what the owner pointers reachable from the argument pointed to, so
    aliases of those objects are dangling:

        struct X x = f();
        char* p = x.text;
        g(&x);           // void g(struct X* p) { free(p->text); p->text = 0; }
        if (p) *p;       // warning 31 only with release_owned_pointees

    Result of the test: cake's own source went from 8 to 13 warnings
    (hashmap_set(&map, k, &item) moving ownership in parser.c:7015 and
    codegen.c:6014, an ss_swap loop in codegen.c:6500, and a misleading
    "may be null" in parser.c:15648) and flow3/do-while-nested-if-unrelated-narrow.c
    started warning on a plain list walk (ptk = ptk->next after f(ptk)).

    reach_pointees: any other pointee may have been modified (the deep model).
*/
static void flow_branch_set_pointees_any(struct flow_branch* m, const struct object* obj, const struct token* _Nullable p_token, bool nullable_enabled, bool release_owned_pointees, bool reach_pointees)
{
    if (!type_is_pointer(&obj->type))
        return;

    const bool release = release_owned_pointees && type_is_owner(&obj->type);
    if (!release && !reach_pointees)
        return;

    struct flow_key_alternatives* e = flow_branch_search_up(m, obj);
    if (e == NULL || e->alternatives.size == 0)
        return;

    const struct object** _Owner pointees = calloc(e->alternatives.size, sizeof pointees[0]);
    if (pointees == NULL)
        return;

    int count = 0;
    for (int i = 0; i < e->alternatives.size; i++)
    {
        struct flow_alternative* a = e->alternatives.data[i]; //lint 33
        if (a &&
                a->value_kind == FLOW_VALUE_KIND_PTR &&
                a->value.p != NULL &&
                a->value.p != obj)
        {
            pointees[count++] = a->value.p;
        }
    }

    for (int i = 0; i < count; i++)
    {
        /* the worst case: whoever set it to "any" may have freed the old pointee */
        if (release)
            flow_branch_set_object_lifetime_ended(m, pointees[i], p_token);
        else
            flow_branch_set_object_any_core(m, pointees[i], p_token, nullable_enabled, false, false);
    }

    free(pointees);
}

/* the pointee keeps its state; what its pointer members point to becomes ANY */
void flow_branch_set_reachable_any_n(struct flow_branch* _Nullable m, const struct object* obj, const struct token* _Nullable p_token, bool nullable_enabled)
{
    if (m == NULL)
        return;

    if (obj->members.head)
    {
        for (const struct object* member = obj->members.head; member; member = member->next)
        {
            flow_branch_set_reachable_any_n(m, member, p_token, nullable_enabled);
        }
        return;
    }

    /* through const the owned objects are not released, only possibly modified */
    flow_branch_set_pointees_any(m, obj, p_token, nullable_enabled, false, true);
}

/*
    release_owned_pointees: what obj's owner pointer members point to may have been freed.
    reach_pointees: what obj's pointer members point to (one level deep) becomes ANY.
*/
void flow_branch_set_object_any_n(struct flow_branch* _Nullable m, const struct object* obj, const struct token* _Nullable p_token, bool nullable_enabled, bool release_owned_pointees, bool reach_pointees)
{
    flow_branch_set_object_any_core(m, obj, p_token, nullable_enabled, release_owned_pointees, reach_pointees);
}

static void flow_branch_set_object_any_core(struct flow_branch* _Nullable m, const struct object* obj, const struct token* _Nullable p_token, bool nullable_enabled, bool release_owned_pointees, bool reach_pointees)
{
    if (m == NULL)
        return;

    try
    {
        if (obj->members.head)
        {
            for (const struct object* member = obj->members.head; member; member = member->next)
            {
                flow_branch_set_object_any_core(m, member, p_token, nullable_enabled, release_owned_pointees, reach_pointees);
            }
            return;
        }

        /* one level deep only: the pointees' own pointer members are not followed */
        flow_branch_set_pointees_any(m, obj, p_token, nullable_enabled, release_owned_pointees, reach_pointees);

        struct flow_key_alternatives* p_flow_key_alternatives = flow_branch_find_add(m, obj);
        if (p_flow_key_alternatives == NULL) throw;
        flow_alternatives_clear(&p_flow_key_alternatives->alternatives);

        /*
            A plain FLOW_VALUE_KIND_SIGNED/ANY alternative is read by
            flow_alternative_can_be_zero as "could be zero" regardless of
            the object's actual type -- fine for scalars, but wrong for a
            non-optional pointer member: its own type already guarantees it
            can never be null, "unknown value" or not. Without this,
            invalidating a struct's members (e.g. after passing &x to a
            plain mutable-pointer parameter, or through a _Out call) made
            every non-_Nullable pointer member of that struct look
            possibly-null afterward, even though nothing could have made it
            null -- found via a user-reported false positive:

             struct X { int* p_non_null; };
             struct X x = make();
             f(&x);                  // f(struct X*), plain mutable pointer
             int y = *x.p_non_null;  // "possible null pointer dereference"

            Seed a NOT_EQUAL-0 PTR alternative instead for that case, so the
            member is still correctly treated as "unknown identity, but
            definitely non-null" -- the same guarantee flow3 already gives a
            non-_Nullable pointer PARAMETER on entry.
        */
        if (nullable_enabled &&
                type_is_pointer(&obj->type) &&
                !type_is_nullable(&obj->type, nullable_enabled))
        {
            struct flow_alternative a =
            {
                .value_kind = FLOW_VALUE_KIND_PTR,
                .value = {.p = NULL},
                .value_relation = FLOW_RELATION_NOT_EQUAL,
                .imaginary = FLOW_IMAGINARY_NONE,
                .p_origin_map = m,
                .p_origin_token = p_token
            };
            flow_alternatives_add(&p_flow_key_alternatives->alternatives, &a);
            return;
        }

        struct flow_alternative a =
        {
            .value_kind = FLOW_VALUE_KIND_SIGNED,
            .value = {.i = ANY_VALUE},
            .value_relation = FLOW_RELATION_ANY,
            .imaginary = FLOW_IMAGINARY_NONE,
            .p_origin_map = m,
            .p_origin_token = p_token
        };
        flow_alternatives_add(&p_flow_key_alternatives->alternatives, &a);
    }
    catch
    {
    }
}

void flow_branch_set_object_lifetime_ended(struct flow_branch* _Nullable m, const struct object* obj, const struct token* _Nullable p_token)
{
    if (m == NULL)
        return;

    if (obj->members.head)
    {
        for (const struct object* member = obj->members.head; member; member = member->next)
        {
            flow_branch_set_object_lifetime_ended(m, member, p_token);
        }
        return;
    }
    struct flow_key_alternatives* e = flow_branch_find_add(m, obj);
    if (e == NULL)
    {
        return;
    }
    if (e->alternatives.size == 0)
    {

        struct flow_alternative a =
        {
            .value_kind = FLOW_VALUE_KIND_SIGNED,
            .value = {.i = UNINITIALIZED_VALUE},
            .value_relation = FLOW_RELATION_UNINITIALIZED,
            .imaginary = FLOW_IMAGINARY_NONE,
            .p_origin_map = m,
            .p_origin_token = p_token
        };
        flow_alternatives_add(&e->alternatives, &a);

    }

    for (int i = 0; i < e->alternatives.size; i++)
    {
        /*
           Preserve MOVED: a moved owner reaching the end of its scope is
           fine, and overwriting the flag would make a later exit pass
           (e.g. the enclosing compound's defer list after a return already
           checked the object) report a false "owner not moved".
        */
        if (e->alternatives.data[i]->imaginary != FLOW_IMAGINARY_MOVED) //lint 33
        {
            e->alternatives.data[i]->imaginary = FLOW_IMAGINARY_ENDED; //lint 33
            /*
                The "(see line N)" in the eventual diagnostic must point at
                where the lifetime actually ended, not wherever this
                alternative happened to be seeded (e.g. an earlier read of
                the same member) -- otherwise it misdirects the reader to
                an unrelated line.
            */
            e->alternatives.data[i]->p_origin_token = p_token; //lint 33
            /* the lifetime ends on this path, not on the arm that wrote the value; a same-object side is kept */
            if (!flow_branch_is_under_same_object(e->alternatives.data[i]->p_origin_map)) //lint 33
            {
                e->alternatives.data[i]->p_origin_map = m; //lint 33
            }
        }
    }
}

void flow_branch_apply_dtor_or_clear_effect(struct flow_branch* _Nullable m, const struct object* obj, bool is_clear, const struct token* _Nullable p_token)
{
    if (m == NULL)
        return;

    if (obj->members.head)
    {
        for (const struct object* member = obj->members.head; member; member = member->next)
        {
            flow_branch_apply_dtor_or_clear_effect(m, member, is_clear, p_token);
        }
        return;
    }

    if (type_is_owner(&obj->type) && type_is_pointer(&obj->type))
    {
        struct flow_key_alternatives* e = flow_branch_search_up(m, obj);
        if (e)
        {
            for (int i = 0; i < e->alternatives.size; i++)
            {
                struct flow_alternative* a = e->alternatives.data[i]; //lint 33
                if (a &&
                        a->value_kind == FLOW_VALUE_KIND_PTR &&
                        a->value.p != NULL)
                {
                    flow_branch_set_object_lifetime_ended(m, a->value.p, p_token);
                }
            }
        }
    }

    if (is_clear)
        flow_branch_set_object_zero(m, obj, p_token);
    else
        flow_branch_set_object_uninitialized(m, obj, p_token);
}

static void flow_branch_free_entries(struct flow_branch* m);

static void flow_branch_clear(_Clear struct flow_branch* m)
{
    /*
        The bucket table and its entries go the same way here as in
        flow_branch_free_entries -- that is the whole of the freeing work, so
        share it rather than keeping a second copy of the walk.
    */
    flow_branch_free_entries(m);

    /*
        _Clear promises every member is zero on exit, on EVERY path --
        including the one where there were no buckets to free at all, which
        the old early return skipped. Written out in full rather than
        relying on what free_entries happens to reset.
    */
    m->buckets = NULL; //lint 26  flow_branch_free_entries makes null
    m->num_of_buckets = 0;
    m->num_of_entries = 0;
    m->p_parent_map = NULL;
    m->kind = FLOW_BRANCH_ROOT;
    m->p_branch_expr = NULL;
    m->is_unreachable = false;
    m->branch_id = 0;
    m->child_count = 0;
    m->implied_facts = (struct flow_branch_implied_facts){ 0 };
    free(m->label);
    m->label = NULL;
    m->same_object_of_parameters = false;
}

static void flow_branch_delete(struct flow_branch* _Owner _Nullable m)
{
    if (m == NULL)
        return;

    flow_branch_clear(m);
    free(m); /* kind/branch_expr are plain, non-owned values -- nothing else to free */
}

static void flow_branch_free_entries(struct flow_branch* m)
{
    if (m->buckets == NULL)
        return;

    for (int i = 0; i < m->num_of_buckets; i++)
    {
        struct flow_key_alternatives* _Owner e = m->buckets[i];
        while (e)
        {
            struct flow_key_alternatives* _Owner n = e->next;
            flow_alternatives_clear(&e->alternatives);
            free(e);
            e = n;
        }
        m->buckets[i] = NULL;
    }
    free(m->buckets); //lint 29
    m->buckets = NULL;
    m->num_of_buckets = 0;
    m->num_of_entries = 0;
}

static void flow_branch_move_entries(struct flow_branch* restrict dest, struct flow_branch* restrict src)
{
    if (src->buckets == NULL)
        return;

    for (int i = 0; i < src->num_of_buckets; i++)
    {
        struct flow_key_alternatives* _Owner e = src->buckets[i];
        while (e)
        {
            struct flow_key_alternatives* _Owner next = e->next;
            struct flow_key_alternatives* d = flow_branch_find_add(dest, e->p_obj_key);
            if (d)
            {
                flow_alternatives_clear(&d->alternatives);
                d->alternatives = e->alternatives;
                /* Zero out e's alternatives: ownership moved into d */
                memset(&e->alternatives, 0, sizeof(e->alternatives));
            }
            else
            {
                flow_alternatives_clear(&e->alternatives);
            }
            free(e);
            e = next;
        }
        src->buckets[i] = NULL;
    }
}

static struct flow_key_alternatives* flow_branch_arm_find_entry(const struct flow_branch* arm, const struct flow_branch* parent, const struct object* obj)
{
    for (const struct flow_branch* cur = arm; cur && cur != parent; cur = cur->p_parent_map)
    {
        struct flow_key_alternatives* p_entry = flow_branch_find(cur, obj);
        if (p_entry && p_entry->alternatives.size > 0)
        {
            return p_entry;
        }
    }
    return NULL;
}

void flow_branch_merge_arms(struct flow_branch* parent, const struct flow_branch* const arms[], int num_arms)
{
    /*
        Write into a temporary map parented to parent so pre-branch
        reads are clean and don't see our own partial writes.
    */
    struct flow_branch temp = { .p_parent_map = parent, .kind = FLOW_BRANCH_MERGE_TEMP };

    struct object_set objs = { 0 };

    for (int i = 0; i < num_arms; i++)
    {
        if (arms[i] == NULL || arms[i]->is_unreachable)
        {
            continue;
        }
        for (const struct flow_branch* cur = arms[i]; cur && cur != parent; cur = cur->p_parent_map)
        {
            /* above the fork of parent and every arm nothing was written */
            bool cur_is_fork_ancestor = true;
            for (int k = -1; k < num_arms && cur_is_fork_ancestor; k++)
            {
                const struct flow_branch* p_other = k < 0 ? parent->p_parent_map : arms[k];
                if (k >= 0 && (p_other == NULL || p_other->is_unreachable))
                {
                    continue;
                }
                bool found = false;
                for (const struct flow_branch* p_ancestor = p_other; p_ancestor; p_ancestor = p_ancestor->p_parent_map)
                {
                    if (p_ancestor == cur)
                    {
                        found = true;
                        break;
                    }
                }
                cur_is_fork_ancestor = found;
            }
            if (cur_is_fork_ancestor)
            {
                break;
            }

            if (cur->buckets == NULL)
            {
                continue;
            }

            for (int j = 0; j < cur->num_of_buckets; j++)
            {
                for (const struct flow_key_alternatives* e = cur->buckets[j]; e; e = e->next)
                {
                    object_set_add(&objs, e->p_obj_key);
                }
            }
        }
    }

    for (int i = 0; i < objs.size; i++)
    {
        const struct object* obj = objs.items[i]; //lint 33

        struct flow_key_alternatives* p_temp_entry = flow_branch_find_add(&temp, obj);
        if (p_temp_entry == NULL)
        {
            continue;
        }
        flow_alternatives_clear(&p_temp_entry->alternatives);

        /*
            When every reachable arm contributes the identical set of values,
            the branch did not discriminate this object, so the per-arm origin
            tag carries no correlation information by construction: emit ONE
            copy, keeping each alternative's own pre-merge origin, instead of
            one re-tagged copy per arm. (Re-tagging the collapsed copy with
            `parent` instead loses the origin the "state comes from here"
            notes read, and produced 28 false "lifetime has ended" warnings
            analysing this very file -- keep the original origin.) Without this an
            object all arms agree on costs an alternative per arm at every
            merge, and those multiply along the path (measured: 93% of all
            alternatives stored were duplicates of an existing value that
            differed only in origin).
        */
        enum { COLLAPSE_MAX_ARMS = 8 };
        const struct flow_alternatives* agreed = NULL;
        if (num_arms <= COLLAPSE_MAX_ARMS)
        {
            bool all_agree = true;
            for (int j = 0; j < num_arms && all_agree; j++)
            {
                if (arms[j] == NULL || arms[j]->is_unreachable)
                {
                    continue;
                }
                const struct flow_key_alternatives* p_entry = flow_branch_arm_find_entry(arms[j], parent, obj);
                if (p_entry == NULL)
                {
                    p_entry = flow_branch_search_up(parent, obj);
                }
                if (p_entry == NULL || p_entry->alternatives.size == 0)
                {
                    all_agree = false;
                    break;
                }
                if (agreed == NULL)
                {
                    agreed = &p_entry->alternatives;
                }
                else if (!flow_alternatives_same_values(agreed, &p_entry->alternatives))
                {
                    all_agree = false;
                }
            }
            if (!all_agree)
            {
                agreed = NULL;
            }
        }

        if (agreed)
        {
            for (int k = 0; k < agreed->size; k++)
            {
                struct flow_alternative tagged = *agreed->data[k]; //lint 33
                /* keep the alternative's own pre-merge origin */
                flow_alternatives_push(&p_temp_entry->alternatives, &tagged);
            }
            flow_alternatives_remove_duplicates(&p_temp_entry->alternatives);
            continue;
        }

        for (int j = 0; j < num_arms; j++)
        {
            if (arms[j] == NULL || arms[j]->is_unreachable)
            {
                continue;
            }

            struct flow_key_alternatives* p_entry = flow_branch_arm_find_entry(arms[j], parent, obj);

            if (p_entry)
            {
                for (int k = 0; k < p_entry->alternatives.size; k++)
                {
                    flow_alternatives_push(&p_temp_entry->alternatives, p_entry->alternatives.data[k]); //lint 33
                }
            }
            else
            {
                /*
                    Arm is reachable but didn't write this variable:
                    inherit the pre-branch value from parent, but re-tag each
                    alternative's origin as arms[j] (this arm's map) instead of
                    keeping the ancestor's origin. This lets later logic (e.g.
                    flow_branch_transfer_true_false_set) tell values that arrived
                    via this arm's path apart from an ancestor's original
                    value, without mutating the ancestor's own entry.
                */
                struct flow_key_alternatives* p_pre_entry = flow_branch_search_up(parent, obj);
                if (p_pre_entry)
                {
                    for (int k = 0; k < p_pre_entry->alternatives.size; k++)
                    {
                        const struct flow_alternative* a2 = p_pre_entry->alternatives.data[k]; //lint 33
                        struct flow_alternative tagged = *a2;
                        tagged.p_origin_map = arms[j];

                        /*
                            A value from an arm merged before this fork (a catch, a same-object
                            side) keeps that arm as its narrowed-from map: it still exists only there.
                        */
                        if ((tagged.p_narrowed_from == NULL ||
                             (flow_branch_is_under_same_object(a2->p_origin_map) && !flow_branch_is_under_same_object(tagged.p_narrowed_from))) &&
                            a2->p_origin_map != NULL)
                        {
                            bool origin_on_chain = false;
                            for (const struct flow_branch* m = parent; m != NULL && !origin_on_chain; m = m->p_parent_map)
                            {
                                origin_on_chain = m == a2->p_origin_map;
                            }
                            if (!origin_on_chain)
                            {
                                tagged.p_narrowed_from = a2->p_origin_map;
                            }
                        }
                        tagged.p_origin_token = a2->p_origin_token;
                        flow_alternatives_push(&p_temp_entry->alternatives, &tagged);
                    }
                }
                else if (!type_is_pointer(&obj->type))
                {
                    struct flow_alternative unknown =
                    {
                        .value_kind = FLOW_VALUE_KIND_SIGNED,
                        .value = {.i = ANY_VALUE},
                        .value_relation = FLOW_RELATION_ANY,
                        .imaginary = FLOW_IMAGINARY_NONE,
                        .p_origin_map = arms[j],
                        .p_origin_token = NULL
                    };
                    flow_alternatives_push(&p_temp_entry->alternatives, &unknown);
                }
            }
        }
        flow_alternatives_remove_duplicates(&p_temp_entry->alternatives);
    }

    object_set_destroy(&objs);

    /* Move all merged results from temp into parent, then clean up temp. */
    flow_branch_move_entries(parent, &temp);
    flow_branch_clear(&temp);

    /*
        Every arm's data has now been copied out (as VALUE copies) into
        `parent`. Walk each arm's ancestor chain (up to, but not including,
        `parent`) and free a map's own bucket entries once nothing else can
        still read them.

        A map is safe to free only when it has exactly ONE child ever
        created off it (child_count == 1): that child is necessarily the
        one arm chain being cleaned up right now, so nothing else can reach
        this map through a different, still-live path. A map with MORE than
        one child is a fork point shared by more than one branch and must
        be left alone, no matter which single arm's cleanup walk happens to
        pass through it.

        This matters because a chain interposed by
        flow_narrow_map_branch/flow_branch_arena_new_branch can be shared by
        more than one arm across DIFFERENT merges. The clearest case is
        `L && R`: R is evaluated once, starting from L's own true-branch
        map, producing right_pair.p_true and right_pair.p_false as TWO
        SIBLING branches off that SAME map (child_count == 2 there). The
        `&&`'s own internal merge for its false result
        (merge_a_b(p_and_false, left_pair.p_false, right_pair.p_false))
        walks up from right_pair.p_false and, without this check, would
        free L's true-branch map's entries as "private scratch state" --
        wiping out the narrowing of L (e.g. "p is non-null") that
        right_pair.p_true's own chain still depends on, and that the `&&`'s
        TRUE result (right_pair.p_true) -- returned to and used by the
        caller AFTER this false-branch merge already ran -- still needs.
        (Confirmed via `assert(p && p->next); p->i = 1;`, equally
        reproducible via `if (!(p && p->next)) return; p->i = 1;`.)

        child_count is only ever incremented (see flow_branch_arena_new), and
        every child a map will ever have is created before that map stops
        being `ctx->p_current_flow_branch` for the branch that owns it, so by
        the time ANY cleanup walk reaches a map, its child_count is already
        final -- safe to check without true reference counting. Once a
        shared fork point (child_count > 1) is found, stop walking further
        up that arm entirely: everything above a shared node is reachable
        through the OTHER branch(es) sharing it too, so this arm's walk has
        no more exclusive ground left to reclaim.

        Separately: an arm that never actually branched (e.g. a plain
        function call as one operand of || / &&, whose fallback branch pair
        is {p_before, p_before}) IS p_before itself -- an ancestor `parent`
        was just built on top of, not a descendant created for this merge.
        `parent` itself is guaranteed to keep being read from after this
        merge returns (every caller sets ctx->p_current_flow_branch to it),
        so `parent` and everything on ITS OWN ancestor chain must never be
        freed here even if child_count happens to read as 1 (e.g. `parent`
        is p_before's only child so far, with siblings still to be created
        later by code that hasn't run yet). Check this before the
        child_count check, since it must hold regardless of child_count.
    */
    for (int i = 0; i < num_arms; i++)
    {
        if (arms[i] == NULL || arms[i] == parent)
        {
            continue;
        }
        for (struct flow_branch* cur = (struct flow_branch*)arms[i]; cur && cur != parent; cur = cur->p_parent_map)
        {
            bool cur_is_parent_or_its_ancestor = false;
            for (const struct flow_branch* panc = parent; panc; panc = panc->p_parent_map)
            {
                if (panc == cur)
                {
                    cur_is_parent_or_its_ancestor = true;
                    break;
                }
            }
            if (cur_is_parent_or_its_ancestor)
            {
                break;
            }
            if (cur->child_count > 1)
            {
                break;
            }
            flow_branch_free_entries(cur);
        }
    }
}

void flow_branch_merge_a_b(struct flow_branch* parent, const struct flow_branch* a, const struct flow_branch* b)
{
    const struct flow_branch* arms[2] = { a, b };
    flow_branch_merge_arms(parent, arms, 2);
}

void flow_branch_accumulate_into_join(struct flow_branch* p_join, struct flow_branch* _Nullable p_src,
                                          const struct flow_branch* _Nullable p_retag_origin,
                                          bool full_state, bool join_reached_before)
{
    struct object_set objs = { 0 };

    for (const struct flow_branch* cur = p_src; cur; cur = cur->p_parent_map)
    {
        /* above the common ancestor with p_join, p_join already sees the same values */
        bool cur_is_join_ancestor = false;
        for (const struct flow_branch* p_ancestor = p_join; !full_state && p_ancestor; p_ancestor = p_ancestor->p_parent_map)
        {
            if (p_ancestor == cur)
            {
                cur_is_join_ancestor = true;
                break;
            }
        }
        if (cur_is_join_ancestor)
        {
            break;
        }

        if (cur->buckets == NULL)
        {
            continue;
        }
        for (int i = 0; i < cur->num_of_buckets; i++)
        {
            for (const struct flow_key_alternatives* e = cur->buckets[i]; e; e = e->next)
            {
                object_set_add(&objs, e->p_obj_key);
            }
        }
    }

    /* an object an earlier jump put in p_join needs this jump's value too */
    for (int i = 0; !full_state && p_join->buckets != NULL && i < p_join->num_of_buckets; i++)
    {
        for (struct flow_key_alternatives* e = p_join->buckets[i]; e; e = e->next)
        {
            object_set_add(&objs, e->p_obj_key);
        }
    }

    for (int i = 0; i < objs.size; i++)
    {
        const struct object* obj = objs.items[i]; //lint 33
        const struct flow_key_alternatives* p_src_entry = flow_branch_search_up(p_src, obj);
        if (p_src_entry == NULL || p_src_entry->alternatives.size == 0)
        {
            continue;
        }
        const bool is_new_in_join = flow_branch_find(p_join, obj) == NULL;
        struct flow_key_alternatives* p_join_entry = flow_branch_find_add(p_join, obj);
        if (p_join_entry == NULL)
        {
            continue;
        }
        if (!full_state && join_reached_before && is_new_in_join && p_join->p_parent_map != NULL)
        {
            /* earlier jumps did not write it: they arrived with the value above p_join */
            const struct flow_key_alternatives* p_earlier_entry = flow_branch_search_up(p_join->p_parent_map, obj);
            if (p_earlier_entry != NULL)
            {
                flow_alternatives_append(&p_join_entry->alternatives, &p_earlier_entry->alternatives);
            }
        }
        if (p_retag_origin != NULL)
        {
            for (int k = 0; k < p_src_entry->alternatives.size; k++)
            {
                const struct flow_alternative* a = p_src_entry->alternatives.data[k]; //lint 33

                /*
                    Skip a fact the join already carries. Several jumps usually
                    agree about most objects, and adding one copy per jump
                    multiplies alternatives -- and with them the reports that
                    walk alternatives, e.g. the same "parameter was consumed"
                    said twelve times instead of twice. Two facts count as the
                    same only when value, relation, imaginary state AND the
                    token that established them all match, so the differing
                    facts this retagging exists to keep apart still land as
                    separate alternatives.
                */
                bool already_there = false;
                for (int m = 0; m < p_join_entry->alternatives.size; m++)
                {
                    const struct flow_alternative* b = p_join_entry->alternatives.data[m]; //lint 33
                    if (b->value_relation == a->value_relation &&
                            b->imaginary == a->imaginary &&
                            b->p_origin_token == a->p_origin_token &&
                            flow_value_is_same(b, a))
                    {
                        already_there = true;
                        break;
                    }
                }

                if (already_there)
                {
                    continue;
                }

                /* a same-object side holds in the whole function: the value keeps it */
                struct flow_alternative tagged = *a;
                if (!flow_branch_is_under_same_object(a->p_origin_map))
                {
                    tagged.p_origin_map = p_retag_origin;
                }
                flow_alternatives_add(&p_join_entry->alternatives, &tagged);
            }
        }
        else
        {
            flow_alternatives_append(&p_join_entry->alternatives, &p_src_entry->alternatives);
        }
    }

    object_set_destroy(&objs);
}

struct flow_branch* flow_branch_arena_new_branch(struct flow_branch_arena* a,
                                                       struct flow_branch* _Nullable parent, bool is_true, const struct expression* _Nullable p_expr)
{
    struct flow_branch* m = flow_branch_arena_new(a, parent, is_true ? FLOW_BRANCH_TRUE_BRANCH : FLOW_BRANCH_FALSE_BRANCH);
    if (m)
    {
        m->p_branch_expr = p_expr;
        /* a condition in dead code (an `if` after a return) has dead arms */
        m->is_unreachable = parent != NULL && parent->is_unreachable;
    }
    return m;
}

/*
    The p_narrowed_from of a value narrowed from `alt` into the map new_origin:
    alt's own origin, unless that is on new_origin's chain already -- then
    alt's own p_narrowed_from (a condition narrowed twice, `c > 2 && c < 9`)
    still says where the value came from.
*/
const struct flow_branch* flow_alternative_narrowed_provenance(const struct flow_alternative* alt, const struct flow_branch* _Nullable new_origin)
{
    const struct flow_branch* provenance = alt->p_origin_map;
    bool origin_on_chain = false;
    for (const struct flow_branch* m = new_origin; m != NULL && !origin_on_chain; m = m->p_parent_map)
    {
        if (m == alt->p_origin_map)
        {
            origin_on_chain = true;
        }
    }
    if (origin_on_chain && alt->p_narrowed_from != NULL)
    {
        provenance = alt->p_narrowed_from;
    }
    return provenance;
}

/* a copy of label, a ready string printed as it is */
void flow_branch_set_label(struct flow_branch* _Nullable m, const char* _Nullable label)
{
    if (m != NULL && label != NULL)
    {
        free(m->label);
        m->label = strdup(label);
    }
}

static const char* _Nonnull flow_branch_kind_name(enum flow_branch_kind kind)
{
    switch (kind)
    {
        case FLOW_BRANCH_ROOT: return "ROOT";
        case FLOW_BRANCH_TRUE_BRANCH: return "TRUE_BRANCH";
        case FLOW_BRANCH_FALSE_BRANCH: return "FALSE_BRANCH";
        case FLOW_BRANCH_BREAK_JOIN: return "BREAK_JOIN";
        case FLOW_BRANCH_CONTINUE_JOIN: return "CONTINUE_JOIN";
        case FLOW_BRANCH_THROW_JOIN: return "THROW_JOIN";
        case FLOW_BRANCH_TRY_BRANCH: return "TRY_BRANCH";
        case FLOW_BRANCH_CATCH_BRANCH: return "CATCH_BRANCH";
        case FLOW_BRANCH_SWITCH_BODY: return "SWITCH_BODY";
        case FLOW_BRANCH_OPT_NULL: return "OPT_NULL";
        case FLOW_BRANCH_OPT_NONNULL: return "OPT_NONNULL";
        case FLOW_BRANCH_FOR_BODY_DIAG: return "FOR_BODY_DIAG";
        case FLOW_BRANCH_FOR_BODY_PASS1: return "FOR_BODY_PASS1";
        case FLOW_BRANCH_DEFAULT: return "DEFAULT";
        case FLOW_BRANCH_CASE: return "CASE";
        case FLOW_BRANCH_GOTO_LABEL: return "GOTO_LABEL";
        case FLOW_BRANCH_REL_DEAD: return "REL_DEAD";
        case FLOW_BRANCH_EQ_DEAD: return "EQ_DEAD";
        case FLOW_BRANCH_NOT_DEAD_TRUE: return "NOT_DEAD_TRUE";
        case FLOW_BRANCH_NOT_DEAD_FALSE: return "NOT_DEAD_FALSE";
        case FLOW_BRANCH_OR_DEAD_TRUE: return "OR_DEAD_TRUE";
        case FLOW_BRANCH_OR_DEAD_FALSE: return "OR_DEAD_FALSE";
        case FLOW_BRANCH_AND_DEAD_TRUE: return "AND_DEAD_TRUE";
        case FLOW_BRANCH_AND_DEAD_FALSE: return "AND_DEAD_FALSE";
        case FLOW_BRANCH_MERGE_TEMP: return "MERGE_TEMP";
        case FLOW_BRANCH_POINTER_TARGET: return "POINTER_TARGET";
        case FLOW_BRANCH_SAME_OBJECT: return "SAME_OBJECT";
        case FLOW_BRANCH_NOT_SAME_OBJECT: return "NOT_SAME_OBJECT";
    }
    return "?";
}

/* #n: the map's index in the arena, stable for the function */
static void flow_raw_dump_map_ref(const struct flow_branch_arena* arena, const struct flow_branch* _Nullable m)
{
    if (m == NULL)
    {
        printf("NULL");
        return;
    }
    for (int i = 0; arena->data != NULL && i < arena->size; i++)
    {
        if (arena->data[i] == m)
        {
            printf("#%d", i);
            return;
        }
    }
    printf("#?(%p)", (const void*)m);
}

static bool flow_raw_dump_is_shown(const struct flow_debug_filter* _Nullable p_filter, const struct object* p)
{
    if (p_filter == NULL)
        return true;
    for (const struct object* o = p; o != NULL; o = o->parent)
    {
        for (int i = 0; i < p_filter->count; i++)
        {
            if (p_filter->data[i] == o)
                return true;
        }
    }
    return false;
}

/*
    The maps as they are in memory, field by field, from `map` up to the root
    (the chain flow_branch_search_up reads). Maps are #n (arena index), objects
    their address and designator.
*/
void flow_branch_raw_dump(const struct flow_branch* _Nullable map, const struct flow_branch_arena* arena, bool colors, const struct flow_debug_filter* _Nullable p_filter)
{
    static const char* const kinds[] = { "SIGNED", "UNSIGNED", "PTR", "REF" };
    static const char* const relations[] = { "UNINITIALIZED", "EQUAL", "NOT_EQUAL", "ANY", "GREATER", "GREATER_EQUAL", "LESS", "LESS_EQUAL" };
    static const char* const imaginaries[] = { "NONE", "MOVED", "ENDED" };

    const char* c_map = colors ? "\x1b[96m" : "";
    const char* c_key = colors ? "\x1b[93m" : "";
    const char* c_dim = colors ? "\x1b[90m" : "";
    const char* c_warn = colors ? "\x1b[91m" : "";
    const char* c_off = colors ? "\x1b[0m" : "";

    for (const struct flow_branch* m = map; m != NULL; m = m->p_parent_map)
    {
        printf("%sflow_branch ", c_map);
        flow_raw_dump_map_ref(arena, m);
        printf(" {%s%s\n", c_off, m == map ? "   <- current" : "");
        printf("  kind           = %s\n", flow_branch_kind_name(m->kind));
        printf("  branch_id      = %d\n", m->branch_id);
        printf("  p_parent_map   = ");
        flow_raw_dump_map_ref(arena, m->p_parent_map);
        printf("\n");
        printf("  is_unreachable = %s\n", m->is_unreachable ? "true" : "false");
        if (m->label != NULL)
            printf("  label          = \"%s\"\n", m->label);
        printf("  implied_facts  = [");
        for (int f = 0; f < m->implied_facts.count; f++)
            printf("%s {branch_id=%d, kind=%s}", f > 0 ? "," : "", m->implied_facts.data[f].branch_id, flow_branch_kind_name(m->implied_facts.data[f].kind));
        printf(" ]\n");
        printf("  entries (%d) {\n", m->num_of_entries);
        for (int b = 0; m->buckets != NULL && b < m->num_of_buckets; b++)
        {
            for (const struct flow_key_alternatives* e = m->buckets[b]; e != NULL; e = e->next)
            {
                if (!flow_raw_dump_is_shown(p_filter, e->p_obj_key))
                    continue;
                printf("    %sp_obj_key%s = %s%p%s \"%s\"", c_key, c_off, c_dim, (const void*)e->p_obj_key, c_off,
                       e->p_obj_key->member_designator ? e->p_obj_key->member_designator : "");
                if (e->p_obj_key->parent != NULL)
                    printf(" %s(parent %p)%s", c_dim, (const void*)e->p_obj_key->parent, c_off);
                printf("\n    alternatives (%d) {\n", e->alternatives.size);
                for (int i = 0; i < e->alternatives.size; i++)
                {
                    const struct flow_alternative* a = e->alternatives.data[i]; //lint 33
                    printf("      [%d] { value_kind=%s ", i, kinds[a->value_kind]);
                    if (a->value_kind == FLOW_VALUE_KIND_PTR || a->value_kind == FLOW_VALUE_KIND_REF)
                    {
                        printf("value.p=%s%p%s", c_dim, (const void*)a->value.p, c_off);
                        size_t element_index = 0;
                        const struct object* p_array = a->value.p != NULL ? object_get_array_of_element(a->value.p, &element_index) : NULL;
                        if (p_array != NULL)
                        {
                            printf(" %s(element %zu of \"%s\"[%zu])%s", c_dim, element_index,
                                   p_array->member_designator ? p_array->member_designator : "",
                                   p_array->type.array_num_elements, c_off);
                        }
                    }
                    else if (a->value_kind == FLOW_VALUE_KIND_UNSIGNED)
                        printf("value.u=%llu", a->value.u);
                    else
                        printf("value.i=%lld", a->value.i);
                    printf(" value_relation=%s", relations[a->value_relation]);
                    printf(" imaginary=%s%s%s", a->imaginary != FLOW_IMAGINARY_NONE ? c_warn : "", imaginaries[a->imaginary], c_off);
                    printf(" contradicted=%s\n", a->contradicted ? "true" : "false");
                    printf("            p_origin_map=");
                    flow_raw_dump_map_ref(arena, a->p_origin_map);
                    printf(" p_narrowed_from=");
                    flow_raw_dump_map_ref(arena, a->p_narrowed_from);
                    printf(" p_origin_token=line %d }\n", a->p_origin_token ? a->p_origin_token->line : 0);
                }
                printf("    }\n");
            }
        }
        printf("  }\n}\n");
    }
}
