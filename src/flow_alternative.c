#pragma safety enable

#include "flow_alternative.h"
#include "error.h"
#include "cake_compat.h"
#include "osstream.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <stdint.h>

#include "flow_branch.h"

#define FLOW_ALT_POOL_BLOCK_NODES 4096

union flow_alt_pool_node
{
    struct flow_alternative alt;
    union flow_alt_pool_node* _Opt next;
};

struct flow_alt_pool
{
    union flow_alt_pool_node* _Opt free_list;
    union flow_alt_pool_node* _Owner _Opt* _Owner _Opt blocks;
    int blocks_size;
    int blocks_capacity;
};

static struct flow_alt_pool g_flow_alt_pool = { 0 };

static struct flow_alternative* _Opt _Owner flow_alt_pool_alloc(struct flow_alt_pool* pool)
{
    if (pool->free_list == NULL)
    {
        union flow_alt_pool_node* _Owner _Opt block =
            calloc(FLOW_ALT_POOL_BLOCK_NODES, sizeof(union flow_alt_pool_node));
        if (block == NULL)
            return NULL;

        if (pool->blocks_size == pool->blocks_capacity)
        {
            int new_capacity = pool->blocks_capacity == 0 ? 8 : pool->blocks_capacity * 2;
            union flow_alt_pool_node* _Owner _Opt* _Owner _Opt new_blocks =
                realloc(pool->blocks, new_capacity * sizeof(union flow_alt_pool_node*));
            if (new_blocks == NULL)
            {
                free(block);
                return NULL;
            }
            pool->blocks = new_blocks;
            pool->blocks_capacity = new_capacity;
        }
        pool->blocks[pool->blocks_size++] = block;

        /* Thread every node in this block onto the free list. */
        for (int i = 0; i < FLOW_ALT_POOL_BLOCK_NODES - 1; i++)
        {
            block[i].next = &block[i + 1];
        }
        block[FLOW_ALT_POOL_BLOCK_NODES - 1].next = pool->free_list;
        pool->free_list = block; //lint 26 (pool block, freed by flow_alt_pool_free_all)
    }

    union flow_alt_pool_node* node = pool->free_list;
    pool->free_list = node->next;
    /* Recycled nodes carry stale bytes from their previous life (only the
       leading `next` field was written while parked on the free list).
       Callers rely on calloc-style zero defaults for fields they don't set
       explicitly (e.g. value_kind/value), so zero the node every time it is
       handed out, not just on first carve from a fresh block. */
    memset(&node->alt, 0, sizeof(node->alt));
    /* Not actually heap-owned -- recycled from the pool above and returned
       to it by flow_alt_pool_release, never freed individually. The _Owner
       cast is the established idiom (see free-opt-owner-cast.c) for opting
       a pointer into the ownership contract at the use site. */
    return (struct flow_alternative* _Opt _Owner) & node->alt;
}

static void flow_alt_pool_free_all(_Clear struct flow_alt_pool* pool)
{
    for (int i = 0; i < pool->blocks_size; i++)
    {
        free(pool->blocks[i]);
    }
    free(pool->blocks); //lint 29

    *pool = (struct flow_alt_pool){ 0 };
}

void flow_alternatives_pool_shutdown(void)
{
    flow_alt_pool_free_all(&g_flow_alt_pool);
}

static void flow_alt_pool_free(struct flow_alt_pool* pool, struct flow_alternative* _Owner _Opt p)
{
    if (p == NULL)
        return;
    union flow_alt_pool_node* node = (union flow_alt_pool_node*)p;
    node->next = pool->free_list;
    pool->free_list = node;
} //lint 29

static bool flow_alternatives_grow(struct flow_alternatives* vs)
{
    if (vs->size < vs->capacity)
        return true;

    /* Doubling forever means a list that has grown large doubles again --
       e.g. 100k -> 200k entries in one realloc. That is fine for the pointer
       array itself (8 bytes/entry) but risky because it briefly holds both
       the old and new blocks live at once. Past a threshold, grow by half
       instead of doubling, so big lists creep up rather than jump. */
    enum { FLOW_ALT_GROW_DOUBLE_LIMIT = 1024 };
    try
    {
        int new_capacity = 0;
        if (vs->capacity == 0)
            new_capacity = 4;
        else if (vs->capacity < FLOW_ALT_GROW_DOUBLE_LIMIT)
            new_capacity = vs->capacity * 2;
        else
            new_capacity = vs->capacity + vs->capacity / 2; /* +50% */

        struct flow_alternative* _Owner _Opt* _Owner _Opt p =
            realloc(vs->data, (size_t)new_capacity * sizeof(struct flow_alternative*));

        if (p == NULL) throw;

        vs->data = p; //lint 26 (realloc)
        vs->capacity = new_capacity;
        return true;
    }
    catch
    {
    }
    return false;
}

void flow_alternatives_add(struct flow_alternatives* vs, const struct flow_alternative* p_alternative)
{
    try
    {
        /* O(vs->size) dedup scan. To add many alternatives use
           flow_alternatives_push and one flow_alternatives_remove_duplicates. */
        for (int i = 0; i < vs->size; i++)
        {
            if (flow_value_is_same(vs->data[i], p_alternative) &&
                    vs->data[i]->value_relation == p_alternative->value_relation &&
                    vs->data[i]->imaginary == p_alternative->imaginary &&
                    vs->data[i]->p_origin_map == p_alternative->p_origin_map &&
                    vs->data[i]->p_narrowed_from == p_alternative->p_narrowed_from &&
                    vs->data[i]->contradicted == p_alternative->contradicted)
            {
                return;
            }
        }

        if (!flow_alternatives_grow(vs))
        {
            throw;
        }

        struct flow_alternative* _Opt _Owner p_new = flow_alt_pool_alloc(&g_flow_alt_pool);
        if (p_new == NULL)
        {
            throw;
        }
        *p_new = *p_alternative;
        vs->data[vs->size] = p_new; /*MOVED*/
        vs->size++;
    }
    catch
    {
    }
}

void flow_alternatives_push(struct flow_alternatives* vs, const struct flow_alternative* p_alternative)
{
    try
    {
        if (!flow_alternatives_grow(vs))
        {
            throw;
        }

        struct flow_alternative* _Opt _Owner p_new = flow_alt_pool_alloc(&g_flow_alt_pool);
        if (p_new == NULL)
        {
            throw;
        }
        *p_new = *p_alternative;
        vs->data[vs->size] = p_new; /*MOVED*/
        vs->size++;
    }
    catch
    {
    }
}

struct flow_alt_sort_item
{
    const struct flow_alternative* p_alternative;
    int index;
};

static int flow_alt_compare_ptr(const void* _Opt p_left, const void* _Opt p_right)
{
    int result = 0;
    if ((uintptr_t)p_left < (uintptr_t)p_right)
    {
        result = -1;
    }
    else if ((uintptr_t)p_left > (uintptr_t)p_right)
    {
        result = 1;
    }
    return result;
}

/* Orders by the same fields flow_alternatives_add compares, then by index,
   so equal alternatives are adjacent with the first-added one leading. */
static int flow_alt_sort_item_compare(const void* p_left_void, const void* p_right_void)
{
    const struct flow_alt_sort_item* p_left_item = p_left_void;
    const struct flow_alt_sort_item* p_right_item = p_right_void;
    const struct flow_alternative* left_alt = p_left_item->p_alternative;
    const struct flow_alternative* right_alt = p_right_item->p_alternative;

    int result = 0;
    if (left_alt->value_kind != right_alt->value_kind)
    {
        result = left_alt->value_kind < right_alt->value_kind ? -1 : 1;
    }
    else if ((left_alt->value_kind == FLOW_VALUE_KIND_PTR || left_alt->value_kind == FLOW_VALUE_KIND_REF) &&
             left_alt->value.p != right_alt->value.p)
    {
        result = flow_alt_compare_ptr(left_alt->value.p, right_alt->value.p);
    }
    else if (left_alt->value_kind == FLOW_VALUE_KIND_SIGNED && left_alt->value.i != right_alt->value.i)
    {
        result = left_alt->value.i < right_alt->value.i ? -1 : 1;
    }
    else if (left_alt->value_kind == FLOW_VALUE_KIND_UNSIGNED && left_alt->value.u != right_alt->value.u)
    {
        result = left_alt->value.u < right_alt->value.u ? -1 : 1;
    }
    else if (left_alt->value_relation != right_alt->value_relation)
    {
        result = left_alt->value_relation < right_alt->value_relation ? -1 : 1;
    }
    else if (left_alt->imaginary != right_alt->imaginary)
    {
        result = left_alt->imaginary < right_alt->imaginary ? -1 : 1;
    }
    else if (left_alt->p_origin_map != right_alt->p_origin_map)
    {
        result = flow_alt_compare_ptr(left_alt->p_origin_map, right_alt->p_origin_map);
    }
    else if (left_alt->p_narrowed_from != right_alt->p_narrowed_from)
    {
        result = flow_alt_compare_ptr(left_alt->p_narrowed_from, right_alt->p_narrowed_from);
    }
    else if (left_alt->contradicted != right_alt->contradicted)
    {
        result = left_alt->contradicted ? 1 : -1;
    }
    else
    {
        result = p_left_item->index < p_right_item->index ? -1 : (p_left_item->index > p_right_item->index ? 1 : 0);
    }
    return result;
}

void flow_alternatives_remove_duplicates(struct flow_alternatives* vs)
{
    /* O(n log n): sort (alternative, index) pairs, mark every entry that equals
       its sorted predecessor, then compact keeping the original order. */
    struct flow_alt_sort_item* _Owner _Opt items = NULL;
    bool* _Owner _Opt is_duplicate = NULL;
    try
    {
        if (vs->size < 2)
        {
            throw;
        }

        items = malloc((size_t)vs->size * sizeof(struct flow_alt_sort_item));
        is_duplicate = calloc((size_t)vs->size, sizeof(bool));
        if (items == NULL || is_duplicate == NULL)
        {
            throw;
        }

        for (int i = 0; i < vs->size; i++)
        {
            items[i].p_alternative = vs->data[i];
            items[i].index = i;
        }
        qsort(items, (size_t)vs->size, sizeof(struct flow_alt_sort_item), flow_alt_sort_item_compare);

        for (int i = 1; i < vs->size; i++)
        {
            const struct flow_alternative* previous_alt = items[i - 1].p_alternative;
            const struct flow_alternative* current_alt = items[i].p_alternative;
            if (flow_value_is_same(previous_alt, current_alt) &&
                previous_alt->value_relation == current_alt->value_relation &&
                previous_alt->imaginary == current_alt->imaginary &&
                previous_alt->p_origin_map == current_alt->p_origin_map &&
                previous_alt->p_narrowed_from == current_alt->p_narrowed_from &&
                previous_alt->contradicted == current_alt->contradicted)
            {
                is_duplicate[items[i].index] = true;
            }
        }

        int new_size = 0;
        for (int i = 0; i < vs->size; i++)
        {
            if (is_duplicate[i])
            {
                flow_alt_pool_free(&g_flow_alt_pool, vs->data[i]);
            }
            else
            {
                vs->data[new_size] = vs->data[i]; /*MOVED*/
                new_size++;
            }
        }
        vs->size = new_size;
    }
    catch
    {
    }
    free(items);
    free(is_duplicate);
}

void flow_alternatives_append(struct flow_alternatives* dst, const struct flow_alternatives* src)
{
    /* Small lists: the linear dedup scan in flow_alternatives_add is cheaper
       than sorting. Big ones: push everything and dedup once, O(n log n). */
    enum { FLOW_ALT_APPEND_LINEAR_LIMIT = 256 };
    if ((long long)dst->size * src->size <= FLOW_ALT_APPEND_LINEAR_LIMIT)
    {
        for (int i = 0; i < src->size; i++)
        {
            flow_alternatives_add(dst, src->data[i]);
        }
    }
    else
    {
        for (int i = 0; i < src->size; i++)
        {
            flow_alternatives_push(dst, src->data[i]);
        }
        flow_alternatives_remove_duplicates(dst);
    }
}

void flow_alternatives_clear(_Clear struct flow_alternatives* vs)
{
    for (int i = 0; i < vs->size; i++)
    {
        flow_alt_pool_free(&g_flow_alt_pool, vs->data[i]);
    }
    free(vs->data); //lint 29
    vs->data = NULL;
    vs->size = 0;
    vs->capacity = 0;
}

bool flow_alternative_is_zero(const struct flow_alternative* alt)
{
    return alt->value_relation == FLOW_RELATION_EQUAL && flow_value_is_false(alt);
}

bool flow_alternative_is_false(const struct flow_alternative* alt)
{
    return flow_alternative_is_zero(alt);
}

bool flow_alternative_is_true(const struct flow_alternative* alt)
{
    return (alt->value_relation == FLOW_RELATION_NOT_EQUAL && flow_value_is_false(alt)) ||
        (alt->value_relation == FLOW_RELATION_EQUAL && !flow_alternative_is_false(alt));
}

bool flow_alternative_is_dead(const struct flow_alternative* alt)
{
    /* The whole parent chain, not just the origin itself: the value is
       usually recorded in a map nested inside the dead arm (the assignment's
       own map), which is not flagged -- only the arm the fold killed is. */
    for (const struct flow_branch* _Opt m = alt->p_origin_map; m != NULL; m = m->p_parent_map)
    {
        if (m->is_unreachable)
        {
            return true;
        }
    }
    return false;
}

int flow_alternative_line(const struct flow_alternative* a)
{
    return a->p_origin_token ? a->p_origin_token->line : 0;
}

bool flow_value_is_same(const struct flow_alternative* a, const struct flow_alternative* b)
{
    if (a->value_kind != b->value_kind)
    {
        return false;
    }
    switch (a->value_kind)
    {
        case FLOW_VALUE_KIND_SIGNED:
            return a->value.i == b->value.i;
        case FLOW_VALUE_KIND_UNSIGNED:
            return a->value.u == b->value.u;
        case FLOW_VALUE_KIND_PTR:
            return a->value.p == b->value.p;
        case FLOW_VALUE_KIND_REF:
            return a->value.p == b->value.p;
    }
    return false;
}

bool flow_value_is_true(const struct flow_alternative* a)
{
    switch (a->value_kind)
    {
        case FLOW_VALUE_KIND_SIGNED:
            return a->value.i != 0;
        case FLOW_VALUE_KIND_UNSIGNED:
            return a->value.u != 0;
        case FLOW_VALUE_KIND_PTR:
            return a->value.p != 0;
        case FLOW_VALUE_KIND_REF:
        break;
    }
    return false;
}

bool flow_value_is_false(const struct flow_alternative* a)
{
    switch (a->value_kind)
    {
        case FLOW_VALUE_KIND_SIGNED:
            return a->value.i == 0;
        case FLOW_VALUE_KIND_UNSIGNED:
            return a->value.u == 0;
        case FLOW_VALUE_KIND_PTR:
            return a->value.p == 0;
        case FLOW_VALUE_KIND_REF:
        break;
    }
    return false;
}

void flow_alternative_sprint(struct osstream* ss, const struct flow_alternative* alt)
{
    switch (alt->value_relation)
    {
        case FLOW_RELATION_EQUAL:
            ss_fprintf(ss, "== ");
        break;
        case FLOW_RELATION_NOT_EQUAL:
            ss_fprintf(ss, "!= ");
        break;
        case FLOW_RELATION_ANY:
            ss_fprintf(ss, "ANY ");
        break;
        case FLOW_RELATION_UNINITIALIZED:
            ss_fprintf(ss, "UNINIT ");
        break;
        case FLOW_RELATION_GREATER:
            ss_fprintf(ss, "> ");
        break;
        case FLOW_RELATION_GREATER_EQUAL:
            ss_fprintf(ss, ">= ");
        break;
        case FLOW_RELATION_LESS:
            ss_fprintf(ss, "< ");
        break;
        case FLOW_RELATION_LESS_EQUAL:
            ss_fprintf(ss, "<= ");
        break;
    }

    switch (alt->value_kind)
    {
        case FLOW_VALUE_KIND_SIGNED:
        {
            if (alt->value.i == UNINITIALIZED_VALUE ||
            alt->value.i == ANY_VALUE)
            {
                //ss_fprintf(ss, "%llx", alt->value.i);
            }
            else
            {
                ss_fprintf(ss, "%lld", alt->value.i);
            }
        }
        break;
        case FLOW_VALUE_KIND_UNSIGNED:
        {
            ss_fprintf(ss, "%lluu", alt->value.u);
        }
        break;

        case FLOW_VALUE_KIND_PTR:
        {
            print_object_ptr(ss, alt->value.p, false);
            break;
        }
        case FLOW_VALUE_KIND_REF:
        {
            ss_fprintf(ss, "ref ");
            print_object_ptr(ss, alt->value.p, false);
            break;
        }
    }

    if (alt->imaginary == FLOW_IMAGINARY_MOVED)
    {
        ss_fprintf(ss, " MOVED");
    }

    if (alt->imaginary == FLOW_IMAGINARY_ENDED)
    {
        ss_fprintf(ss, " ENDED");
    }

    ss_fprintf(ss, " line %d", flow_alternative_line(alt));

    if (alt->p_origin_map)
    {
        struct osstream name_ss = { 0 };
        flow_branch_name_to_string(alt->p_origin_map, &name_ss);
        ss_fprintf(ss, " \"%s\"", name_ss.c_str ? name_ss.c_str : "");
        ss_close(&name_ss);
    }
}

void flow_alternative_print(const struct flow_alternative* alt)
{
    struct osstream ss = { 0 };
    flow_alternative_sprint(&ss, alt);
    printf("%s", ss.c_str);
    ss_close(&ss);
}

void flow_alternatives_print(const struct flow_alternatives* alternatives)
{
    for (int i = 0; i < alternatives->size; i++)
    {
        printf("  ");
        flow_alternative_print(alternatives->data[i]);
    }
}

bool flow_alt_to_interval(const struct flow_alternative* alt, long long* lo, long long* hi)
{
    long long v = 0;
    if (alt->value_kind == FLOW_VALUE_KIND_SIGNED)
    {
        v = alt->value.i;
    }
    else if (alt->value_kind == FLOW_VALUE_KIND_UNSIGNED)
    {
        v = (long long)alt->value.u;
    }
    else
    {
        return false;
    }

    switch (alt->value_relation)
    {
        case FLOW_RELATION_EQUAL:
            *lo = v;
            *hi = v;
            return true;
        case FLOW_RELATION_GREATER:
            if (v == LLONG_MAX) return false;
            *lo = v + 1;
            *hi = LLONG_MAX;
            return true;
        case FLOW_RELATION_GREATER_EQUAL:
            *lo = v;
            *hi = LLONG_MAX;
            return true;
        case FLOW_RELATION_LESS:
            if (v == LLONG_MIN) return false;
            *lo = LLONG_MIN;
            *hi = v - 1;
            return true;
        case FLOW_RELATION_LESS_EQUAL:
            *lo = LLONG_MIN;
            *hi = v;
            return true;
        default:
            return false;
    }
}

bool flow_alt_set_interval(struct flow_alternative* alt, long long lo, long long hi)
{
    if (alt->value_kind != FLOW_VALUE_KIND_SIGNED || lo > hi)
        return false;

    if (lo == hi)
    {
        alt->value.i = lo;
        alt->value_relation = FLOW_RELATION_EQUAL;
        return true;
    }

    if (lo == LLONG_MIN && hi == LLONG_MAX)
    {
        alt->value.i = ANY_VALUE;
        alt->value_relation = FLOW_RELATION_ANY;
        return true;
    }

    if (lo == LLONG_MIN)
    {
        alt->value.i = hi;
        alt->value_relation = FLOW_RELATION_LESS_EQUAL;
        return true;
    }

    if (hi == LLONG_MAX)
    {
        alt->value.i = lo;
        alt->value_relation = FLOW_RELATION_GREATER_EQUAL;
        return true;
    }

    return false; /* bounded range: no single relation describes it */
}

bool flow_alt_negate(const struct flow_alternative* in,
                            struct flow_alternative* out)
{
    long long v = 0;
    if (in->value_kind == FLOW_VALUE_KIND_SIGNED)
        v = in->value.i;
    else
        return false;

    if (v == LLONG_MIN) /* -LLONG_MIN is not representable */
        return false;

    enum flow_relation r = FLOW_RELATION_UNINITIALIZED;
    switch (in->value_relation)
    {
        /* Deliberately NOT mapping FLOW_RELATION_EQUAL. A genuinely constant
    operand is already folded by the object_has_constant_value branch above;
    an EQUAL here comes from flow state, which may be a stale seed that no
    write-effect has invalidated yet -- e.g. `int ids[32] = {0};` passed to a
    function that fills it in. Propagating that through `-ids[i]` folded
    `ids[i] < 0` to always-false and reported live code as unreachable
    (parser.c:2064). The half-line relations below are what this fix needs. */
        case FLOW_RELATION_NOT_EQUAL:
            r = FLOW_RELATION_NOT_EQUAL;
        break;
        case FLOW_RELATION_GREATER:
            r = FLOW_RELATION_LESS;
        break;
        case FLOW_RELATION_GREATER_EQUAL:
            r = FLOW_RELATION_LESS_EQUAL;
        break;
        case FLOW_RELATION_LESS:
            r = FLOW_RELATION_GREATER;
        break;
        case FLOW_RELATION_LESS_EQUAL:
            r = FLOW_RELATION_GREATER_EQUAL;
        break;
        default:
            return false;
    }

    *out = *in;
    out->value.i = -v;
    out->value_relation = r;
    return true;
}

bool flow_alternative_can_be_zero(const struct flow_alternative* alt)
{
    if (flow_alternative_is_zero(alt))
    {
        return true;
    }
    if (alt->value_relation == FLOW_RELATION_ANY)
    {
        return true;
    }
    /* NOT_EQUAL with a PTR value means "pointer is this non-null object" — cannot be null. */
    if (alt->value_relation == FLOW_RELATION_NOT_EQUAL && alt->value_kind == FLOW_VALUE_KIND_PTR)
    {
        return false;
    }
    /* NOT_EQUAL integer/unsigned nonzero: e.g. !=2 doesn't exclude 0. */
    if (alt->value_relation == FLOW_RELATION_NOT_EQUAL && flow_value_is_true(alt))
    {
        return true;
    }
    /* Relational (half-line) constraints: zero is reachable iff it lies
       inside the permitted interval. e.g. `> 0` excludes zero, `>= 0` and
       `< 5` include it. */
    {
        long long lo = 0;
        long long hi = 0;
        if (flow_alt_to_interval(alt, &lo, &hi) &&
                alt->value_relation != FLOW_RELATION_EQUAL)
        {
            return lo <= 0 && 0 <= hi;
        }
    }
    return false;
}
