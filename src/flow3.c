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

/* Caps how many levels of synthetic "pointee arena" flow3 will manufacture
   for a non-optional pointer parameter (see the recursive call below). A
   self-referential type (struct token { struct token* next; ...}, common
   for linked lists/trees) would otherwise make flow3_parameter_object_init
   recurse forever: each new arena object has the same type as its parent,
   including the very pointer member that triggered its creation, so the
   pointer branch and the members-list branch call each other without
   bound and blow the stack. Past this depth we still create the arena
   (so the pointer itself stays trackable / non-null) but stop seeding its
   *own* pointee chain -- it's simply treated as opaque ANY data, which is
   still sound, just less precise a few levels down. */
#define FLOW3_PARAMETER_OBJECT_INIT_MAX_DEPTH 6

/* Hard ceiling on how many synthetic "pointee arena" objects a single
      top-level declaration may manufacture (the arena is cleared between
      declarations in flow3_start_visit_declaration, so this budget is
      per-declaration, not per-file).

      Found via dogfooding flow3 on cake's own sources: a single function
      prototype whose parameter is a non-optional pointer to a struct with
      several pointer members (each itself pointing to another such struct)
      causes flow3_parameter_object_init to manufacture a new arena object
      per pointer member at every depth level -- branching factor ~ number
      of pointer members, depth bounded by FLOW3_PARAMETER_OBJECT_INIT_MAX_DEPTH.
      For cake's own parser types that branching factor is high enough that
      depth-bounding alone (which prevents the infinite-recursion crash) still
      produces tens of thousands of arena objects for ONE declaration --
      confirmed empirically (instrumented counter hit 40000+ per declaration,
      repeating, hanging indefinitely on parser.h's declarations alone, no
      function bodies involved).

      Once the budget is spent, flow3_allocated_object_arena_new returns NULL
      the same way it already does on real allocation failure -- every call
      site already handles p_pointed == NULL by simply not synthesizing that
      pointee further, so this is a safe (if less precise) degradation, not
      a new failure mode. */
#define FLOW3_ALLOCATED_OBJECT_ARENA_MAX_SIZE 5000

enum
{
    ANY_VALUE = 0xCAFE,
    UNINITIALIZED_VALUE = 0xBAD,

    /*
       Starting bucket count for a flow3_map's hash table, and the load factor
       (as a percentage) past which it rehashes into a bigger table. See
       flow3_map_maybe_grow.

       This used to be one fixed size for every map (tried 64, then 1024). Both
       were wrong for the same reason from opposite ends: a map for one of
       cake's larger functions holds thousands of objects, so a small fixed size
       let chains run to hundreds of entries and made flow3_map_find (called at
       every level of the parent chain by flow3_map_search_up, the single
       hottest function in the analysis) degenerate into a linear scan. A large
       fixed size fixed that but paid for it on EVERY map, including the huge
       number of short-lived branch maps (two new maps per branch: true/false,
       opt-null/opt-nonnull, ...) that hold only a handful of keys directly --
       16x the bucket memory for maps that never needed more than the original
       64. On a function with enough branches, that is what turned "slow" into
       real memory pressure.

       Starting small and growing on demand gives every map the bucket count its
       own key count actually needs.
    */
    FLOW3_MAP_INITIAL_BUCKETS = 8,
    FLOW3_MAP_LOAD_FACTOR_PERCENT = 75,
};

enum flow3_relation
{
    FLOW3_RELATION_UNINITIALIZED = 0, // value == trash
    FLOW3_RELATION_EQUAL, // == value
    FLOW3_RELATION_NOT_EQUAL, // != value
    FLOW3_RELATION_ANY, // value >= TYPE_MIN and value <= TYPE_MAX
    /* Relational (half-line) constraints against a constant. The variable
       is known to satisfy  (variable  OP  value)  for the OP below. These
       let a comparison such as `a > 0` narrow `a` in its true branch, so a
       later assert/compile_assert can prove the same fact. Consumers that
       don't understand these relations must treat them conservatively as
       "unknown" (never as a proof), which keeps the analysis sound. */
    FLOW3_RELATION_GREATER, // >  value
    FLOW3_RELATION_GREATER_EQUAL, // >= value
    FLOW3_RELATION_LESS, // <  value
    FLOW3_RELATION_LESS_EQUAL, // <= value
};

enum flow3_value_kind
{
    FLOW3_VALUE_KIND_SIGNED = 0, /* signed long long .i  (default, zero-init safe) */
    FLOW3_VALUE_KIND_UNSIGNED, /* unsigned long long .u                           */
    FLOW3_VALUE_KIND_PTR, /* struct object*   .p  (concrete arena pointer)  */
    FLOW3_VALUE_KIND_REF, /* struct object*   .p  (reference — always non-null) */
};

enum flow3_imaginary
{
    FLOW3_IMAGINARY_NONE = 0, // no imaginary state (default, zero-init safe)
    FLOW3_IMAGINARY_MOVED, // target was moved elsewhere, target is alive
    FLOW3_IMAGINARY_ENDED, // this object's lifetime ended
    FLOW3_IMAGINARY_ABSENT, // no object is present
};

struct flow3_alternative
{
    union
    {
        unsigned long long u;
        signed long long i;
        const struct object* _Opt p;
    } value;
    enum flow3_value_kind value_kind;
    enum flow3_relation value_relation;
    enum flow3_imaginary imaginary; // MOVED, ENDED, or ABSENT (or NONE)

    const struct flow3_map* _Opt origin; /* which map arm set this value; null when the arena had no map to give */
    int line; /* source line for reference */
};

struct flow3_alternatives
{
    struct flow3_alternative* _Owner _Opt* _Owner _Opt data;
    int size;
    int capacity;
};

struct flow3_key_alternatives
{
    const struct object* p_obj_key; /*key*/
    struct flow3_alternatives alternatives;
    struct flow3_key_alternatives* _Opt _Owner next;
};

/* What a map's debug/diagnostic name renders as (see flow3_map_name_to_string).
   Every map is created carrying one of these instead of a free-form string,
   so there is exactly one place (that function) that knows the literal text
   for each kind -- no per-call-site typos, no risk of two call sites drifting
   to slightly different spellings of the same tag. TRUE_BRANCH/FALSE_BRANCH
   additionally use the map's branch_expr; every other kind renders as a
   fixed string. */
enum flow3_map_kind
{
    FLOW3_MAP_ROOT,
    FLOW3_MAP_TRUE_BRANCH,
    FLOW3_MAP_FALSE_BRANCH,
    FLOW3_MAP_BREAK_JOIN,
    FLOW3_MAP_THROW_JOIN,
    FLOW3_MAP_TRY_BRANCH,
    FLOW3_MAP_CATCH_BRANCH,
    FLOW3_MAP_SWITCH_BODY,
    FLOW3_MAP_OPT_NULL,
    FLOW3_MAP_OPT_NONNULL,
    FLOW3_MAP_OPT_MEM_NULL,
    FLOW3_MAP_OPT_MEM_NONNULL,
    FLOW3_MAP_CALL_OPT_NULL,
    FLOW3_MAP_CALL_OPT_NONNULL,
    FLOW3_MAP_DO_WHILE_BODY_DIAG,
    FLOW3_MAP_DO_WHILE_FALSE,
    FLOW3_MAP_FOR_BODY_DIAG,
    FLOW3_MAP_DEFAULT,
    FLOW3_MAP_CASE,
    FLOW3_MAP_GOTO_LABEL,
    FLOW3_MAP_REL_DEAD,
    FLOW3_MAP_EQ_DEAD,
    FLOW3_MAP_NOT_DEAD_TRUE,
    FLOW3_MAP_NOT_DEAD_FALSE,
    FLOW3_MAP_OR_DEAD_TRUE,
    FLOW3_MAP_OR_DEAD_FALSE,
    FLOW3_MAP_AND_DEAD_TRUE,
    FLOW3_MAP_AND_DEAD_FALSE,
    FLOW3_MAP_MERGE_TEMP, /* short-lived local scratch map used only inside flow3_map_merge_arms; never printed */
};

struct flow3_map
{
    struct flow3_key_alternatives* _Owner _Opt* _Owner _Opt buckets; /* NULL until first write (lazily allocated) */
    int num_of_buckets;
    int num_of_entries; /* live keys, tracked to drive rehashing -- see flow3_map_maybe_grow */
    struct flow3_map* _Opt p_parent_map;

    enum flow3_map_kind kind;

    /* Only meaningful for kind == FLOW3_MAP_TRUE_BRANCH/FALSE_BRANCH: the
       condition expression, rendered as "(expr)" after "true/false branch".
       May itself be NULL (some branch pairs aren't conditioned on any single
       printable expression). Set once at creation (see
       flow3_map_arena_new_branch/flow3_narrow_map_branch); a plain, non-owned
       pointer -- creating a branch map costs nothing beyond this one write. */
    const struct expression* _Opt branch_expr;

    bool is_unreachable; /* branch proven unreachable by constant folding */

    /* Branch identity for join correlation. A true/false map pair created for
       one condition share a stable branch_id (>0); which side each map is on
       is already recorded by `kind` (FLOW3_MAP_TRUE_BRANCH/FALSE_BRANCH), so
       there is no separate branch_side to keep in sync with it. A value's
       origin chain thus records the branch decisions taken to reach it; two
       values conflict iff some branch_id appears on both chains with kinds
       disagreeing on which side. 0 = not a branch map. Survives joins because
       ids don't depend on map lifetime. */
    int branch_id;

    /* Number of child maps ever created with this map as p_parent_map (see
       flow3_map_arena_new/flow3_map_arena_new_branch/flow3_map_arena_new_dead).
       Used by flow3_map_merge_arms's cleanup pass to tell "this map belongs
       exclusively to the one branch being cleaned up right now" (count == 1)
       from "this map is a fork point shared by more than one branch" (count
       > 1, e.g. right_pair.p_true/right_pair.p_false in `L && R`, both
       children of L's own true-branch map) -- freeing a shared fork point's
       entries while cleaning up one of its children would silently corrupt
       state the other child's chain still needs to read via search_up. Only
       ever incremented, never decremented: a map's total child count is
       fixed once all its children have been created (branch maps aren't
       created lazily after the fact), so this is safe to check at any later
       point without needing full reference counting. */
    int child_count;
};

struct flow3_branch_pair
{
    struct flow3_map* _Opt p_true;
    struct flow3_map* _Opt p_false;
};

static void flow3_map_name_to_string(const struct flow3_map* _Opt map, struct osstream* ss);
static struct osstream flow3_explain_origin(const struct flow3_map* _Opt map);

struct object_set
{
    const struct object** _Owner _Opt items;
    int size;
    int capacity;

    /* Open-addressing hash index (pointer-keyed, NULL = empty slot) used only
       to make membership checks O(1) average -- `items` above stays the dense,
       insertion-ordered array every caller already iterates over; this table
       exists purely so object_set_add doesn't have to linearly rescan it.
       See the comment on object_set_add for why this exists. */
    const struct object** _Owner _Opt table;
    int table_capacity;
};

static void object_set_add(struct object_set* l, const struct object* obj);
static void object_set_destroy(_Dtor struct object_set* l);

static void flow3_check_dianostic_suppression(struct flow3_visit_ctx* ctx, struct token* p_token);

static void flow3_visit_unlabeled_statement(struct flow3_visit_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement);
static void flow3_visit_static_assertion(struct flow3_visit_ctx* ctx, struct static_assertion* p_static_assertion);
static void flow3_visit_declaration(struct flow3_visit_ctx* ctx, struct declaration* p_declaration);
static void flow3_visit_secondary_block(struct flow3_visit_ctx* ctx, struct secondary_block* _Opt p_secondary_block);
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

static struct flow3_branch_pair flow3_visit_expression(struct flow3_visit_ctx* ctx, const struct expression* _Opt p_expression);
static void object_static_debug(struct flow3_visit_ctx* ctx, const struct object* p_object, struct token* first_token, struct token* last_token);

static void flow3_check_object_at_exit(struct flow3_visit_ctx* ctx, const struct type* p_type, const struct object* p_obj, const struct marker* marker, const struct token* p_exit_token, bool in_view, const char* _Opt p_root_name_opt);
static void flow3_check_arena_objects_at_function_exit(struct flow3_visit_ctx* ctx);
static void flow3_seed_member_default(struct flow3_visit_ctx* ctx, struct object* _Opt member_obj, int line);

enum init_type
{
    INIT_PARAMETER,
    INIT_RETURN,
    INIT_OBJ
};

static void flow3_check_object_init_assigment(struct flow3_visit_ctx* ctx,
    struct expression* p_expression,
    const struct object* _Opt p_object_dest, //uninitialized alawys
    const struct object* _Opt p_object_src,
    enum init_type init_type,
    bool dest_is_dtor,
    bool dest_is_view);

static void flow3_check_static_ownership_assignment(struct flow3_visit_ctx* ctx,
    const struct type* _Opt p_dest_type,
    const struct expression* _Opt p_src_expression);

static void flow3_apply_alloc_contract_to_dest(struct flow3_visit_ctx* ctx,
    const struct type* _Opt p_dest_type,
    const struct object* _Opt p_object_dest,
    const struct expression* _Opt p_src_expression);

/* Insert obj's pointer identity into the hash index, growing/rehashing as
   needed. Returns true if obj was already present (nothing inserted), false
   if this call added it. Linear probing; NULL marks an empty slot, which is
   safe because a real `struct object*` is never NULL. */
static bool object_set_table_insert(struct object_set* l, const struct object* obj)
{
    if (l->table == NULL || l->size * 2 >= l->table_capacity)
    {
        int new_capacity = l->table_capacity ? l->table_capacity * 2 : 16;
        const struct object** _Owner _Opt new_table = calloc((size_t)new_capacity, sizeof(struct object*));
        if (new_table == NULL)
            return false; /* fall back to inserting into items only; a rare rescan is safe */

        if (l->table != NULL)
        {
            const size_t new_mask = (size_t)new_capacity - 1;
            for (int i = 0; i < l->table_capacity; i++)
            {
                const struct object* _Opt existing = l->table[i];
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

static void object_set_add(struct object_set* l, const struct object* obj)
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
            const struct object** _Owner _Opt p = realloc(l->items, new_capacity * sizeof(struct object*));
            if (p == NULL)
            {
                throw;
            }
            l->items = p; //lint 26  (realloc semantics)
            l->capacity = new_capacity;
        }
        l->items[l->size++] = obj;
    }
    catch
    {
    }
}

static void object_set_destroy(_Dtor struct object_set* l)
{
    free(l->items);
    l->items = NULL;
    l->size = 0;
    l->capacity = 0;

    free(l->table);
    l->table = NULL;
    l->table_capacity = 0;
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

static long long flow3_cast_integer_value(struct flow3_visit_ctx* ctx, long long value, const struct type* _Opt target_type)
{
    try
    {
        if (target_type == NULL) return value;
        if (!type_is_integer(target_type)) return value;

        size_t width = 1;
        enum sizeof_result r = type_get_sizeof(target_type, &width, ctx->ctx->options.target);
        if (r != SIZEOF_RESULT_OK)
            throw;

        width *= 8; /* type_get_sizeof returns bytes; we need bits */

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
    catch
    {
    }
    return value;
}

static inline bool flow3_value_is_same(const struct flow3_alternative* a, const struct flow3_alternative* b)
{
    if (a->value_kind != b->value_kind)
    {
        return false;
    }
    switch (a->value_kind)
    {
    case FLOW3_VALUE_KIND_SIGNED: return a->value.i == b->value.i;
    case FLOW3_VALUE_KIND_UNSIGNED: return a->value.u == b->value.u;
    case FLOW3_VALUE_KIND_PTR: return a->value.p == b->value.p;
    case FLOW3_VALUE_KIND_REF: return a->value.p == b->value.p;
    }
    return false;
}

static bool flow3_value_is_true(const struct flow3_alternative* a)
{
    switch (a->value_kind)
    {
    case FLOW3_VALUE_KIND_SIGNED: return a->value.i != 0;
    case FLOW3_VALUE_KIND_UNSIGNED: return a->value.u != 0;
    case FLOW3_VALUE_KIND_PTR: return a->value.p != 0;
    case FLOW3_VALUE_KIND_REF:
        break;
    }
    return false;
}

static bool flow3_value_is_false(const struct flow3_alternative* a)
{
    switch (a->value_kind)
    {
    case FLOW3_VALUE_KIND_SIGNED: return a->value.i == 0;
    case FLOW3_VALUE_KIND_UNSIGNED: return a->value.u == 0;
    case FLOW3_VALUE_KIND_PTR: return a->value.p == 0;
    case FLOW3_VALUE_KIND_REF:
        break;
    }
    return false;
}

static unsigned int flow3_hash_key(const struct object* obj, int num_of_buckets)
{
    _Assert(num_of_buckets > 0);
    uintptr_t p = (uintptr_t)obj;
    return (unsigned int)((p ^ (p >> 5)) % (unsigned int)num_of_buckets);
}
static void flow3_map_rehash(struct flow3_map* m, int new_num_of_buckets)
{
    struct flow3_key_alternatives* _Owner _Opt* _Owner _Opt new_buckets = NULL;

    try
    {
        new_buckets = calloc((size_t)new_num_of_buckets, sizeof(struct flow3_key_alternatives*));

        for (int i = 0; i < m->num_of_buckets; i++)
        {
            struct flow3_key_alternatives* _Owner _Opt e = m->buckets[i];
            if (new_buckets == NULL)
                throw;

            while (e)
            {
                struct flow3_key_alternatives* _Owner _Opt next = e->next; /*MOVED*/
                unsigned int idx = flow3_hash_key(e->p_obj_key, new_num_of_buckets);
                e->next = new_buckets[idx]; /*MOVED*/
                new_buckets[idx] = e;
                e = next;
            }
        }

        free(m->buckets);
        m->buckets = new_buckets;
        m->num_of_buckets = new_num_of_buckets;
    }
    catch
    {
    }
}

/*
   Doubling keeps the amortized cost of growth O(1) per insert regardless of
   how large the map gets, which matters here: some maps (flow3_evaluate_
   binary_arithmetic's worst objects) accumulate hundreds of keys, so this
   function alone is called by every one of the tens of millions of inserts
   flow3 does analysing cake's own sources.
*/
static void flow3_map_maybe_grow(struct flow3_map* m)
{
    if (m->num_of_entries * 100 > m->num_of_buckets * FLOW3_MAP_LOAD_FACTOR_PERCENT)
    {
        /* Doubling forever briefly holds the old and new bucket arrays live
           together (rehash walks the old table while filling the new one
           before freeing it). Past a threshold, grow by half instead of
           doubling, same reasoning as flow3_alternatives_grow. */
        enum { FLOW3_MAP_GROW_DOUBLE_LIMIT = 4096 };
        int new_num_of_buckets = m->num_of_buckets < FLOW3_MAP_GROW_DOUBLE_LIMIT
        ? m->num_of_buckets * 2
        : m->num_of_buckets + m->num_of_buckets / 2;
        flow3_map_rehash(m, new_num_of_buckets);
    }
}

static void flow3_alternatives_clear(_Clear struct flow3_alternatives* vs);

static void flow3_map_remove(struct flow3_map* _Opt m, const struct object* obj)
{
    if (m == NULL)
        return; /* no map, nothing to remove */

    if (!m->buckets)
    {
        return;
    }

    unsigned int idx = flow3_hash_key(obj, m->num_of_buckets);
    struct flow3_key_alternatives* _Owner _Opt* pp = &m->buckets[idx];
    while (*pp)
    {
        struct flow3_key_alternatives* _Owner e = *pp;
        if (e->p_obj_key == obj)
        {
            *pp = e->next;
            m->num_of_entries--;
            flow3_alternatives_clear(&e->alternatives);
            free(e);
            return;
        }
        pp = &e->next;
    }
}

static void flow3_map_delete(struct flow3_map* _Owner _Opt m);

void flow3_map_arena_clear(_Clear struct flow3_map_arena* a)
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

struct flow3_map* _Opt flow3_map_arena_new(struct flow3_map_arena* a, struct flow3_map* _Opt parent, enum flow3_map_kind kind)
{
    try
    {
        if (a->data == NULL)
        {
            const int initial_capacity = 10;
            a->data = calloc(initial_capacity, sizeof(struct flow3_map*));
            if (a->data == NULL) throw;
            a->size = 0;
            a->capacity = initial_capacity;
        }

        if (a->size == a->capacity)
        {
            enum { FLOW3_MAP_ARENA_GROW_DOUBLE_LIMIT = 4096 };
            int new_capacity = a->capacity == 0 ? 4
            : a->capacity < FLOW3_MAP_ARENA_GROW_DOUBLE_LIMIT ? a->capacity * 2
            : a->capacity + a->capacity / 2;
            struct flow3_map** _Owner _Opt new_data = realloc(a->data, new_capacity * sizeof(struct flow3_map*));
            if (new_data == NULL) throw;
            a->data = new_data; //lint 26
            a->capacity = new_capacity;
        }

        struct flow3_map* _Owner _Opt obj = calloc(1, sizeof(struct flow3_map));
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

static struct flow3_map* _Opt flow3_map_arena_new_dead(struct flow3_map_arena* a, struct flow3_map* parent, enum flow3_map_kind kind)
{
    struct flow3_map* _Opt m = flow3_map_arena_new(a, parent, kind);
    if (m)
    {
        m->is_unreachable = true;
    }
    return m;
}

void flow3_allocated_object_arena_clear(_Clear struct flow3_allocated_object_arena* a)
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

struct object* _Opt flow3_allocated_object_arena_new(struct flow3_allocated_object_arena* a)
{
    try
    {
        if (a->size >= FLOW3_ALLOCATED_OBJECT_ARENA_MAX_SIZE)
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
            struct object** _Owner _Opt new_data = realloc(a->data, new_capacity * sizeof(struct object*));
            if (new_data == NULL)
            {
                throw;
            }
            a->data = new_data; //lint 26 (realloc semantics)
            a->capacity = new_capacity;
        }

        struct object* _Owner _Opt obj = calloc(1, sizeof(struct object));
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

/*
   Free-list pool for flow3_alternative nodes: recycles freed slots instead
   of calloc/free per alternative, since this runs tens of millions of times
   per declaration and per-call CRT overhead (esp. under a debug allocator)
   was turning "slow" into "out of memory".
*/
#define FLOW3_ALT_POOL_BLOCK_NODES 4096

union flow3_alt_pool_node
{
    struct flow3_alternative alt;
    union flow3_alt_pool_node* _Opt next;
};

struct flow3_alt_pool
{
    union flow3_alt_pool_node* _Opt free_list;
    /* Block start addresses, tracked only so flow3_alt_pool_free_all can
       free() them -- once threaded onto free_list, individual nodes are no
       longer grouped by block. */
    union flow3_alt_pool_node* _Opt* _Owner _Opt blocks;
    int blocks_size;
    int blocks_capacity;
};

static struct flow3_alt_pool g_flow3_alt_pool = { 0 };

static struct flow3_alternative* _Opt _Owner flow3_alt_pool_alloc(struct flow3_alt_pool* pool)
{
    if (pool->free_list == NULL)
    {
        union flow3_alt_pool_node* _Owner _Opt block =
        calloc(FLOW3_ALT_POOL_BLOCK_NODES, sizeof(union flow3_alt_pool_node));
        if (block == NULL)
            return NULL;

        if (pool->blocks_size == pool->blocks_capacity)
        {
            int new_capacity = pool->blocks_capacity == 0 ? 8 : pool->blocks_capacity * 2;
            union flow3_alt_pool_node* _Opt* _Owner _Opt new_blocks =
            realloc(pool->blocks, new_capacity * sizeof(union flow3_alt_pool_node*));
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
        for (int i = 0; i < FLOW3_ALT_POOL_BLOCK_NODES - 1; i++)
            block[i].next = &block[i + 1];
        block[FLOW3_ALT_POOL_BLOCK_NODES - 1].next = pool->free_list;
        pool->free_list = block; //lint 26 (pool block, freed by flow3_alt_pool_free_all)
    }

    union flow3_alt_pool_node* node = pool->free_list;
    pool->free_list = node->next;
    /* Recycled nodes carry stale bytes from their previous life (only the
       leading `next` field was written while parked on the free list).
       Callers rely on calloc-style zero defaults for fields they don't set
       explicitly (e.g. value_kind/value), so zero the node every time it is
       handed out, not just on first carve from a fresh block. */
    memset(&node->alt, 0, sizeof(node->alt));
    return &node->alt;
}

static void flow3_alt_pool_free_all(_Clear struct flow3_alt_pool* pool)
{
    for (int i = 0; i < pool->blocks_size; i++)
    {
        free(pool->blocks[i]);
    }
    free(pool->blocks);

    *pool = (struct flow3_alt_pool){ 0 };
}

static void flow3_alt_pool_free(struct flow3_alt_pool* pool, struct flow3_alternative* _Owner _Opt p)
{
    if (p == NULL)
        return;
    union flow3_alt_pool_node* node = (union flow3_alt_pool_node*)p;
    node->next = pool->free_list;
    pool->free_list = node;
}

/*
   Grow vs->data to hold at least one more element. Doubling keeps the
   amortized cost O(1) per push; the realloc here only moves POINTERS
   (sizeof(struct flow3_alternative*), 8 bytes), never the alternative structs
   themselves -- see the struct's comment for why that is the point of storing
   pointers at all.
*/
static bool flow3_alternatives_grow(struct flow3_alternatives* vs)
{
    if (vs->size < vs->capacity)
        return true;

    /* Doubling forever means a list that has grown large doubles again --
       e.g. 100k -> 200k entries in one realloc. That is fine for the pointer
       array itself (8 bytes/entry) but risky because it briefly holds both
       the old and new blocks live at once. Past a threshold, grow by half
       instead of doubling, so big lists creep up rather than jump. */
    enum { FLOW3_ALT_GROW_DOUBLE_LIMIT = 1024 };
    try
    {
        int new_capacity = 0;
        if (vs->capacity == 0)
            new_capacity = 4;
        else if (vs->capacity < FLOW3_ALT_GROW_DOUBLE_LIMIT)
            new_capacity = vs->capacity * 2;
        else
            new_capacity = vs->capacity + vs->capacity / 2; /* +50% */

        struct flow3_alternative* _Owner _Opt* _Owner _Opt p =
        realloc(vs->data, (size_t)new_capacity * sizeof(struct flow3_alternative*));

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

static void flow3_alternatives_add(struct flow3_alternatives* vs, const struct flow3_alternative* p_alternative)
{
    try
    {
        for (int i = 0; i < vs->size; i++)
        {
            if (flow3_value_is_same(vs->data[i], p_alternative) &&
                vs->data[i]->value_relation == p_alternative->value_relation &&
                vs->data[i]->imaginary == p_alternative->imaginary &&
                vs->data[i]->origin == p_alternative->origin)
            {
                return;
            }
        }

        if (!flow3_alternatives_grow(vs))
        {
            throw;
        }

        struct flow3_alternative* _Opt _Owner p_new = flow3_alt_pool_alloc(&g_flow3_alt_pool);
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

static void flow3_alternatives_add_does_not_exist(struct flow3_alternatives* vs, const struct flow3_map* origin, int line)
{
    try
    {
        for (int i = 0; i < vs->size; i++)
        {
            if (vs->data[i]->imaginary == FLOW3_IMAGINARY_ABSENT &&
                vs->data[i]->origin == origin)
            {
                return;
            }
        }

        if (!flow3_alternatives_grow(vs))
        {
            throw;
        }

        struct flow3_alternative* _Opt _Owner p_new = flow3_alt_pool_alloc(&g_flow3_alt_pool);
        if (p_new == NULL)
        {
            throw;
        }
        p_new->imaginary = FLOW3_IMAGINARY_ABSENT;
        p_new->value_relation = FLOW3_RELATION_EQUAL;
        p_new->origin = origin;
        p_new->line = line;
        vs->data[vs->size] = p_new; /*MOVED*/
        vs->size++;
    }
    catch
    {
    }
}

static void flow3_alternatives_append(struct flow3_alternatives* dst, const struct flow3_alternatives* src)
{
    for (int i = 0; i < src->size; i++)
    {
        flow3_alternatives_add(dst, src->data[i]);
    }
}

static void flow3_alternatives_clear(_Clear struct flow3_alternatives* vs)
{
    for (int i = 0; i < vs->size; i++)
    {
        flow3_alt_pool_free(&g_flow3_alt_pool, vs->data[i]);
    }
    free(vs->data);
    vs->data = NULL;
    vs->size = 0;
    vs->capacity = 0;
}

static struct flow3_key_alternatives* _Opt flow3_map_find(const struct flow3_map* m, const struct object* obj)
{
    if (!m->buckets)
    {
        return NULL;
    }

    unsigned int idx = flow3_hash_key(obj, m->num_of_buckets);
    for (struct flow3_key_alternatives* _Opt e = m->buckets[idx]; e; e = e->next)
    {
        if (e->p_obj_key == obj)
        {
            return e;
        }
    }
    return NULL;
}

static struct flow3_key_alternatives* _Opt flow3_map_search_up(struct flow3_map* _Opt m, const struct object* obj)
{
    struct flow3_map* _Opt pm = m;
    while (pm)
    {
        struct flow3_key_alternatives* _Opt p = flow3_map_find(pm, obj);
        if (p)
        {
            return p;
        }
        pm = pm->p_parent_map;
    }
    return NULL;
}

static struct flow3_key_alternatives* _Opt flow3_map_find_add(struct flow3_map* _Opt m, const struct object* obj)
{
    try
    {
        if (m == NULL)
            throw; /* no map to add to -- reported to the caller as "not found" */

        if (!m->buckets)
        {
            m->num_of_buckets = FLOW3_MAP_INITIAL_BUCKETS;
            m->buckets = calloc(m->num_of_buckets, sizeof(struct flow3_key_alternatives*));
            if (m->buckets == NULL)
            {
                throw;
            }
        }

        struct flow3_key_alternatives* _Opt e = flow3_map_find(m, obj);
        if (e)
        {
            return e;
        }

        /* Grow BEFORE inserting: simpler than growing after and having to
           re-derive the new index for the entry we just added. One extra
           rehash near a threshold crossing is not worth the extra code path. */
        flow3_map_maybe_grow(m);

        unsigned int idx = flow3_hash_key(obj, m->num_of_buckets);
        struct flow3_key_alternatives* _Opt _Owner e2 = calloc(1, sizeof(struct flow3_key_alternatives));
        if (e2 == NULL)
        {
            throw;
        }
        e2->p_obj_key = obj;
        e2->next = m->buckets[idx];
        m->buckets[idx] = e2;
        m->num_of_entries++;
        return e2;
    }
    catch
    {
        return NULL;
    }
}

static void flow3_map_set_object_moved(struct flow3_map* _Opt m, const struct object* obj, int line)
{
    if (m == NULL)
        return; /* no map to record into */

    if (obj->members.head)
    {
        for (const struct object* _Opt member = obj->members.head; member; member = member->next)
        {
            flow3_map_set_object_moved(m, member, line);
        }
        return;
    }

    struct flow3_key_alternatives* _Opt e = flow3_map_find_add(m, obj);
    if (e == NULL) return;
    if (e->alternatives.size == 0)
    {
        struct flow3_alternative a = {
            .value_kind = FLOW3_VALUE_KIND_SIGNED,
            .value = {.i = ANY_VALUE},
            .value_relation = FLOW3_RELATION_ANY,
            .imaginary = FLOW3_IMAGINARY_MOVED,
            .origin = m,
            .line = line
        };
        flow3_alternatives_add(&e->alternatives, &a);
    }
    else
    {
        for (int i = 0; i < e->alternatives.size; i++)
        {
            /* Update .line too, not just .imaginary: .line is what the
               eventual "object 'p' is moved (see line N)" diagnostic
               reports, and it must point at WHERE THE MOVE HAPPENED, not
               wherever this alternative was last seeded (e.g. the
               declaration/initializer line). Leaving the old .line behind
               pointed a double-free/use-after-move report at the variable's
               declaration instead of the first free()/move -- confirmed via
               `p = malloc(1); free(p); free(p);` reporting "(see line
               <declaration>)" instead of "(see line <first free>)".
               User-reported. */
            e->alternatives.data[i]->imaginary = FLOW3_IMAGINARY_MOVED;
            e->alternatives.data[i]->line = line;
        }
    }
}

static void flow3_map_set_object_zero(struct flow3_map* _Opt m, const struct object* obj, int line)
{
    if (m == NULL)
        return; /* no map to record into */

    try
    {
        if (obj->members.head)
        {
            for (const struct object* _Opt member = obj->members.head; member; member = member->next)
            {
                flow3_map_set_object_zero(m, member, line);
            }
            return;
        }
        struct flow3_key_alternatives* _Opt p_flow3_key_alternatives = flow3_map_find_add(m, obj);
        if (p_flow3_key_alternatives == NULL) throw;

        flow3_alternatives_clear(&p_flow3_key_alternatives->alternatives);

        struct flow3_alternative a = {
            .value_kind = FLOW3_VALUE_KIND_SIGNED,
            .value = {.i = 0},
            .value_relation = FLOW3_RELATION_EQUAL,
            .imaginary = FLOW3_IMAGINARY_NONE,
            .origin = m,
            .line = line
        };
        flow3_alternatives_add(&p_flow3_key_alternatives->alternatives, &a);
    }
    catch
    {
    }
}

/* Mirror of flow3_map_set_object_zero, but seeds each leaf member as
   UNINITIALIZED (used for a _Uninitialized return, e.g. malloc: the returned
   region exists but its contents are indeterminate -- reading a member before
   writing it should warn). */
static void flow3_map_set_object_uninitialized(struct flow3_map* _Opt m, const struct object* obj, int line)
{
    if (m == NULL)
        return; /* no map to record into */

    try
    {
        if (obj->members.head)
        {
            for (const struct object* _Opt member = obj->members.head; member; member = member->next)
            {
                flow3_map_set_object_uninitialized(m, member, line);
            }
            return;
        }
        struct flow3_key_alternatives* _Opt p_flow3_key_alternatives = flow3_map_find_add(m, obj);
        if (p_flow3_key_alternatives == NULL) throw;

        flow3_alternatives_clear(&p_flow3_key_alternatives->alternatives);

        struct flow3_alternative a = {
            .value_kind = FLOW3_VALUE_KIND_SIGNED,
            .value = {.i = UNINITIALIZED_VALUE},
            .value_relation = FLOW3_RELATION_UNINITIALIZED,
            .imaginary = FLOW3_IMAGINARY_NONE,
            .origin = m,
            .line = line
        };
        flow3_alternatives_add(&p_flow3_key_alternatives->alternatives, &a);
    }
    catch
    {
    }
}

static void flow3_map_set_object_any_n(struct flow3_map* _Opt m, const struct object* obj, int line, bool nullable_enabled)
{
    if (m == NULL)
        return; /* no map to record into */

    try
    {
        if (obj->members.head)
        {
            for (const struct object* _Opt member = obj->members.head; member; member = member->next)
            {
                flow3_map_set_object_any_n(m, member, line, nullable_enabled);
            }
            return;
        }
        struct flow3_key_alternatives* _Opt p_flow3_key_alternatives = flow3_map_find_add(m, obj);
        if (p_flow3_key_alternatives == NULL) throw;
        flow3_alternatives_clear(&p_flow3_key_alternatives->alternatives);

        /*
       A plain FLOW3_VALUE_KIND_SIGNED/ANY alternative is read by
       flow3_alternative_can_be_zero as "could be zero" regardless of
       the object's actual type -- fine for scalars, but wrong for a
       non-optional pointer member: its own type already guarantees it
       can never be null, "unknown value" or not. Without this,
       invalidating a struct's members (e.g. after passing &x to a
       plain mutable-pointer parameter, or through a _Ctor call) made
       every non-_Opt pointer member of that struct look
       possibly-null afterward, even though nothing could have made it
       null -- found via a user-reported false positive:

         struct X { int* p_non_null; };
         struct X x = make();
         f(&x);                  // f(struct X*), plain mutable pointer
         int y = *x.p_non_null;  // "possible null pointer dereference"

       Seed a NOT_EQUAL-0 PTR alternative instead for that case, so the
       member is still correctly treated as "unknown identity, but
       definitely non-null" -- the same guarantee flow3 already gives a
       non-_Opt pointer PARAMETER on entry. */
        if (nullable_enabled &&
            type_is_pointer(&obj->type) &&
            !type_is_opt(&obj->type, nullable_enabled))
        {
            struct flow3_alternative a = {
                .value_kind = FLOW3_VALUE_KIND_PTR,
                .value = {.p = NULL},
                .value_relation = FLOW3_RELATION_NOT_EQUAL,
                .imaginary = FLOW3_IMAGINARY_NONE,
                .origin = m,
                .line = line
            };
            flow3_alternatives_add(&p_flow3_key_alternatives->alternatives, &a);
            return;
        }

        struct flow3_alternative a = {
            .value_kind = FLOW3_VALUE_KIND_SIGNED,
            .value = {.i = ANY_VALUE},
            .value_relation = FLOW3_RELATION_ANY,
            .imaginary = FLOW3_IMAGINARY_NONE,
            .origin = m,
            .line = line
        };
        flow3_alternatives_add(&p_flow3_key_alternatives->alternatives, &a);
    }
    catch
    {
    }
}

static void flow3_map_set_object_lifetime_ended(struct flow3_map* _Opt m, const struct object* obj, int line)
{
    if (m == NULL)
        return; /* no map to record into */

    if (obj->members.head)
    {
        for (const struct object* _Opt member = obj->members.head; member; member = member->next)
        {
            flow3_map_set_object_lifetime_ended(m, member, line);
        }
        return;
    }
    struct flow3_key_alternatives* _Opt e = flow3_map_find_add(m, obj);
    if (e == NULL)
    {
        return;
    }
    if (e->alternatives.size == 0)
    {

        struct flow3_alternative a = {
            .value_kind = FLOW3_VALUE_KIND_SIGNED,
            .value = {.i = UNINITIALIZED_VALUE},
            .value_relation = FLOW3_RELATION_UNINITIALIZED,
            .imaginary = FLOW3_IMAGINARY_NONE,
            .origin = m,
            .line = line
        };
        flow3_alternatives_add(&e->alternatives, &a);

    }

    for (int i = 0; i < e->alternatives.size; i++)
    {
        /*
           Preserve MOVED: a moved owner reaching the end of its scope is
           fine, and overwriting the flag would make a later exit pass
           (e.g. the enclosing compound's defer list after a return already
           checked the object) report a false "owner not moved".
        */
        if (e->alternatives.data[i]->imaginary != FLOW3_IMAGINARY_MOVED)
        {
            e->alternatives.data[i]->imaginary = FLOW3_IMAGINARY_ENDED;
            /* The "(see line N)" in the eventual diagnostic must point at
               where the lifetime actually ended, not wherever this
               alternative happened to be seeded (e.g. an earlier read of
               the same member) -- otherwise it misdirects the reader to
               an unrelated line. */
            e->alternatives.data[i]->line = line;
        }
    }
}

/* Shared post-call effect for _Dtor and _Clear parameters.

   struct X { const char* text; int i; };
   void x_destroy(_Dtor struct X *p);
   void x_clear(_Clear struct X *p);

   For _Dtor (is_clear == false): every leaf member of *p becomes
   UNINITIALIZED, and any leaf member that is itself an _Owner pointer has
   ITS pointee marked lifetime-ended first (the destructor released
   whatever the member pointed to before returning).

   For _Clear (is_clear == true): every leaf member of *p becomes ZERO
   (matching the well-defined all-zero state _Clear guarantees), with the
   same "end reachable owner pointees" step first.

   Both cases share the "end reachable owner pointees" walk; only the
   leaf's own post-effect (zero vs. uninitialized) differs, which is why
   this is a single function parameterized on is_clear rather than two
   near-duplicates. */
static void flow3_map_apply_dtor_or_clear_effect(struct flow3_map* _Opt m, const struct object* obj, bool is_clear, int line)
{
    if (m == NULL)
        return; /* no map to record into */

    if (obj->members.head)
    {
        for (const struct object* _Opt member = obj->members.head; member; member = member->next)
        {
            flow3_map_apply_dtor_or_clear_effect(m, member, is_clear, line);
        }
        return;
    }

    if (type_is_owner(&obj->type) && type_is_pointer(&obj->type))
    {
        struct flow3_key_alternatives* _Opt e = flow3_map_search_up(m, obj);
        if (e)
        {
            for (int i = 0; i < e->alternatives.size; i++)
            {
                struct flow3_alternative* _Opt a = e->alternatives.data[i];
                if (a &&
                    a->value_kind == FLOW3_VALUE_KIND_PTR &&
                    a->value.p != NULL)
                {
                    flow3_map_set_object_lifetime_ended(m, a->value.p, line);
                }
            }
        }
    }

    if (is_clear)
        flow3_map_set_object_zero(m, obj, line);
    else
        flow3_map_set_object_uninitialized(m, obj, line);
}

static void flow3_map_free_entries(struct flow3_map* m);

static void flow3_map_clear(_Clear struct flow3_map* m)
{
    /* The bucket table and its entries go the same way here as in
       flow3_map_free_entries -- that is the whole of the freeing work, so
       share it rather than keeping a second copy of the walk. */
    flow3_map_free_entries(m);

    /* _Clear promises every member is zero on exit, on EVERY path --
       including the one where there were no buckets to free at all, which
       the old early return skipped. Written out in full rather than
       relying on what free_entries happens to reset. */
    m->buckets = NULL;
    m->num_of_buckets = 0;
    m->num_of_entries = 0;
    m->p_parent_map = NULL;
    m->kind = FLOW3_MAP_ROOT;
    m->branch_expr = NULL;
    m->is_unreachable = false;
    m->branch_id = 0;
    m->child_count = 0;
}

static void flow3_map_delete(struct flow3_map* _Owner _Opt m)
{
    if (m == NULL)
        return;

    flow3_map_clear(m);
    free(m); /* kind/branch_expr are plain, non-owned values -- nothing else to free */
}

/*
   Frees `m`'s bucket table and every (key, alternatives) entry it holds,
   but -- unlike flow3_map_clear -- leaves the struct flow3_map itself alive
   with its identity intact: kind, branch_expr, p_parent_map, is_unreachable,
   branch_id are all left untouched.

   That identity has to survive because `origin` fields on already-copied
   flow3_alternative values keep pointing at `m` (for flow3_map_name_to_string
   in diagnostics/static_debug, and for the ancestor-chain walk in
   flow3_map_is_ancestor_or_self) for as long as the enclosing declaration is
   being analyzed. But `m`'s own bucket contents are safe to drop the moment
   nothing will look up a live value FROM `m` again: flow3_alternatives_add
   always stores a full VALUE copy of the alternative it's given (`*p_new =
   *p_alternative`), never a pointer into the source entry, so a value that
   has been merged/accumulated elsewhere no longer depends on `m` still
   holding it.

   Idempotent (like flow3_map_clear) -- calling it twice, or on a map with
   no buckets yet, is a no-op the second time.
*/
static void flow3_map_free_entries(struct flow3_map* m)
{
    if (m->buckets == NULL)
        return;

    for (int i = 0; i < m->num_of_buckets; i++)
    {
        struct flow3_key_alternatives* _Opt _Owner e = m->buckets[i];
        while (e)
        {
            struct flow3_key_alternatives* _Opt _Owner n = e->next;
            flow3_alternatives_clear(&e->alternatives);
            free(e);
            e = n;
        }
        m->buckets[i] = NULL;
    }
    free(m->buckets);
    m->buckets = NULL;
    m->num_of_buckets = 0;
    m->num_of_entries = 0;
}

static void flow3_map_move_entries(struct flow3_map* dest, struct flow3_map* src)
{
    if (src->buckets == NULL)
        return;

    for (int i = 0; i < src->num_of_buckets; i++)
    {
        struct flow3_key_alternatives* _Opt _Owner e = src->buckets[i];
        while (e)
        {
            struct flow3_key_alternatives* _Opt _Owner next = e->next;
            struct flow3_key_alternatives* _Opt d = flow3_map_find_add(dest, e->p_obj_key);
            if (d)
            {
                flow3_alternatives_clear(&d->alternatives);
                d->alternatives = e->alternatives;
                /* Zero out e's alternatives: ownership moved into d */
                memset(&e->alternatives, 0, sizeof(e->alternatives));
            }
            else
            {
                flow3_alternatives_clear(&e->alternatives);
            }
            free(e);
            e = next;
        }
        src->buckets[i] = NULL;
    }
}

static bool flow3_map_arm_has_entries(const struct flow3_map* arm, const struct flow3_map* parent)
{
    for (const struct flow3_map* _Opt cur = arm; cur && cur != parent; cur = cur->p_parent_map)
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

/*
   Merge the states of num_arms branch arms into parent.

   Every arm passed here must correspond to a reachable path: an arm
   that did not write a variable inherits the pre-branch value
   (re-tagged with that arm as origin). Two exceptions are skipped
   automatically: NULL arms and dead maps (constant-folded unreachable
   branches). Joins that may never have been reached (e.g. a break join
   with no breaks) must be filtered out by the caller using
   flow3_map_arm_has_entries.
*/
static void flow3_map_merge_arms(struct flow3_map* parent, const struct flow3_map* const arms[], int num_arms)
{
    /* Write into a temporary map parented to parent so pre-branch
       reads are clean and don't see our own partial writes. */
    struct flow3_map temp = { .p_parent_map = parent, .kind = FLOW3_MAP_MERGE_TEMP };

    struct object_set objs = { 0 };

    for (int i = 0; i < num_arms; i++)
    {
        if (arms[i] == NULL || arms[i]->is_unreachable)
        {
            continue;
        }
        for (const struct flow3_map* _Opt cur = arms[i]; cur && cur != parent; cur = cur->p_parent_map)
        {
            if (cur->buckets == NULL)
            {
                continue;
            }

            for (int j = 0; j < cur->num_of_buckets; j++)
            {
                for (struct flow3_key_alternatives* _Opt e = cur->buckets[j]; e; e = e->next)
                {
                    object_set_add(&objs, e->p_obj_key);
                }
            }
        }
    }

    for (int i = 0; i < objs.size; i++)
    {
        const struct object* obj = objs.items[i];

        struct flow3_key_alternatives* _Opt p_temp_entry = flow3_map_find_add(&temp, obj);
        if (p_temp_entry == NULL)
        {
            continue;
        }
        flow3_alternatives_clear(&p_temp_entry->alternatives);

        for (int j = 0; j < num_arms; j++)
        {
            if (arms[j] == NULL || arms[j]->is_unreachable)
            {
                continue;
            }

            struct flow3_key_alternatives* _Opt p_entry = NULL;
            for (const struct flow3_map* _Opt cur = arms[j]; cur && cur != parent; cur = cur->p_parent_map)
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
            else
            {
                /* Arm is reachable but didn't write this variable:
                   inherit the pre-branch value from parent, but re-tag each
                   alternative's origin as arms[j] (this arm's map) instead of
                   keeping the ancestor's origin. This lets later logic (e.g.
                   flow3_map_transfer_true_false_set) tell values that arrived
                   via this arm's path apart from an ancestor's original
                   value, without mutating the ancestor's own entry. */
                struct flow3_key_alternatives* _Opt p_pre_entry = flow3_map_search_up(parent, obj);
                if (p_pre_entry)
                {
                    for (int k = 0; k < p_pre_entry->alternatives.size; k++)
                    {
                        const struct flow3_alternative* a2 = p_pre_entry->alternatives.data[k];
                        struct flow3_alternative tagged = *a2;
                        tagged.origin = arms[j];
                        tagged.line = a2->line;
                        flow3_alternatives_add(&p_temp_entry->alternatives, &tagged);
                    }
                }
            }
        }
    }

    object_set_destroy(&objs);

    /* Move all merged results from temp into parent, then clean up temp. */
    flow3_map_move_entries(parent, &temp);
    flow3_map_clear(&temp);

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
       flow3_narrow_map_branch/flow3_map_arena_new_branch can be shared by
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
       (Confirmed via `_Assert(p && p->next); p->i = 1;`, equally
       reproducible via `if (!(p && p->next)) return; p->i = 1;`.)

       child_count is only ever incremented (see flow3_map_arena_new), and
       every child a map will ever have is created before that map stops
       being `ctx->p_current_flow3_map` for the branch that owns it, so by
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
       merge returns (every caller sets ctx->p_current_flow3_map to it),
       so `parent` and everything on ITS OWN ancestor chain must never be
       freed here even if child_count happens to read as 1 (e.g. `parent`
       is p_before's only child so far, with siblings still to be created
       later by code that hasn't run yet). Check this before the
       child_count check, since it must hold regardless of child_count. */
    for (int i = 0; i < num_arms; i++)
    {
        if (arms[i] == NULL || arms[i] == parent)
        {
            continue;
        }
        for (struct flow3_map* _Opt cur = (struct flow3_map*)arms[i]; cur && cur != parent; cur = cur->p_parent_map)
        {
            bool cur_is_parent_or_its_ancestor = false;
            for (const struct flow3_map* _Opt panc = parent; panc; panc = panc->p_parent_map)
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
            flow3_map_free_entries(cur);
        }
    }
}

static void flow3_map_merge_a_b(struct flow3_map* parent, const struct flow3_map* a, const struct flow3_map* b)
{
    const struct flow3_map* arms[2] = { a, b };
    flow3_map_merge_arms(parent, arms, 2);
}

/*
   Accumulate (union) the state visible from p_src into a join map
   (break/throw/goto label join). Unlike flow3_map_merge_arms this
   appends alternatives, so several jumps to the same join keep the
   states of every jump instead of the last one overwriting the rest.
*/
static void flow3_map_accumulate_into_join(struct flow3_map* p_join, struct flow3_map* _Opt p_src)
{
    struct object_set objs = { 0 };

    for (const struct flow3_map* _Opt cur = p_src; cur; cur = cur->p_parent_map)
    {
        if (cur->buckets == NULL)
        {
            continue;
        }
        for (int i = 0; i < cur->num_of_buckets; i++)
        {
            for (struct flow3_key_alternatives* _Opt e = cur->buckets[i]; e; e = e->next)
            {
                object_set_add(&objs, e->p_obj_key);
            }
        }
    }

    for (int i = 0; i < objs.size; i++)
    {
        const struct object* obj = objs.items[i];
        const struct flow3_key_alternatives* _Opt p_src_entry = flow3_map_search_up(p_src, obj);
        if (p_src_entry == NULL || p_src_entry->alternatives.size == 0)
        {
            continue;
        }
        struct flow3_key_alternatives* _Opt p_join_entry = flow3_map_find_add(p_join, obj);
        if (p_join_entry == NULL)
        {
            continue;
        }
        flow3_alternatives_append(&p_join_entry->alternatives, &p_src_entry->alternatives);
    }

    object_set_destroy(&objs);
}

static void print_object_ptr(struct osstream* ss, const struct object* _Opt p, bool is_parent)
{
    if (p == NULL)
    {
        return;
    }
      
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
    switch (alt->value_relation)
    {
    case FLOW3_RELATION_EQUAL: ss_fprintf(ss, "== "); break;
    case FLOW3_RELATION_NOT_EQUAL: ss_fprintf(ss, "!= "); break;
    case FLOW3_RELATION_ANY: ss_fprintf(ss, "ANY "); break;
    case FLOW3_RELATION_UNINITIALIZED: ss_fprintf(ss, "UNINIT "); break;
    case FLOW3_RELATION_GREATER: ss_fprintf(ss, "> "); break;
    case FLOW3_RELATION_GREATER_EQUAL: ss_fprintf(ss, ">= "); break;
    case FLOW3_RELATION_LESS: ss_fprintf(ss, "< "); break;
    case FLOW3_RELATION_LESS_EQUAL: ss_fprintf(ss, "<= "); break;
    }

    switch (alt->value_kind)
    {
    case FLOW3_VALUE_KIND_SIGNED:
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
    case FLOW3_VALUE_KIND_UNSIGNED:
        {
            ss_fprintf(ss, "%lluu", alt->value.u);
        }
        break;

    case FLOW3_VALUE_KIND_PTR:
        {
            print_object_ptr(ss, alt->value.p, false);
            break;
        }
    case FLOW3_VALUE_KIND_REF:
        {
            ss_fprintf(ss, "ref ");
            print_object_ptr(ss, alt->value.p, false);
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
        struct osstream name_ss = { 0 };
        flow3_map_name_to_string(alt->origin, &name_ss);
        ss_fprintf(ss, " \"%s\"", name_ss.c_str ? name_ss.c_str : "");
        ss_close(&name_ss);
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
        flow3_alternative_print(alternatives->data[i]);
    }
}

/* Gathers every (key, alternatives) entry live in `map`'s hash table into a
   flat array, so flow3_map_debug_print can know -- before it prints any of
   them -- which entry is the last one (needed to choose the "|--" vs "`--"
   connector). Returns the true entry count (may exceed `max`, in which case
   only the first `max` pointers are stored; caller must clamp with the
   returned value before indexing). */
static int flow3_map_collect_entries(const struct flow3_map* map, const struct flow3_key_alternatives** out, int max)
{
    int n = 0;
    if (map->buckets)
    {
        for (int b = 0; b < map->num_of_buckets; b++)
        {
            for (const struct flow3_key_alternatives* _Opt entry = map->buckets[b]; entry; entry = entry->next)
            {
                if (n < max)
                    out[n] = entry;
                n++;
            }
        }
    }
    return n;
}

/*
   static_debug(0) is the "dump everything" form, and its whole point is to
   let a human correlate WHERE a fact was set (which map) against WHERE it
   is being looked up from (another map, possibly several branches away).
   flow3_map_search_up answers "is there an entry for this object" by
   walking the CURRENT map, then its parent, then its parent's parent, and
   so on to the root -- so printing only the current map's own bucket
   contents (the original behavior here) hides every fact actually visible
   to that search unless it happens to live in this exact map. That made a
   real bug invisible: an object's ENDED fact recorded in one branch map
   ("var-true") was correctly found by flow3_map_search_up when reported by
   static_debug(expr) (which walks up), but flow3_object_leaves_in_state's
   origin-compatibility filter rejected it against a pointer alternative
   whose own origin was a sibling map ("call-opt-nonnull") -- and there was
   no way to SEE that origin mismatch from a single-level dump. Walking the
   whole lineage here, one level per line with its own entries directly
   underneath, makes exactly that kind of origin mismatch visible at a
   glance instead of requiring several rounds of ad-hoc static_debug(expr)
   probing at different points in the source.
*/
/* Max number of ancestor maps flow3_map_debug_print will chain through
   before giving up and printing a "chain truncated" marker. This is a
   sanity ceiling, not something real flow3 nesting should ever approach. */
#define FLOW3_MAP_DEBUG_MAX_CHAIN 128
#define FLOW3_MAP_DEBUG_MAX_ENTRIES 256

/* ASCII-art tree connectors (UTF-8 box-drawing characters), written as raw
   byte escapes so this compiles the same regardless of the source file's
   declared/assumed character encoding. */
#define FLOW3_TREE_BRANCH "\xE2\x94\x9C\xE2\x94\x80\xE2\x94\x80 " /* "├── " */
#define FLOW3_TREE_LAST   "\xE2\x94\x94\xE2\x94\x80\xE2\x94\x80 " /* "└── " */

static void flow3_map_debug_print_name(const struct flow3_map* map)
{
    struct osstream name_ss = { 0 };
    flow3_map_name_to_string(map, &name_ss);
    printf("\"%s\"\n", name_ss.c_str ? name_ss.c_str : "");
    ss_close(&name_ss);
}

/*
   Prints `map`'s full ancestor lineage (root..map) as an indented tree,
   root at the top and `map` at the deepest branch -- the same order a human
   reading top-to-bottom would expect a parent/child relationship to read
   in. Each map's live entries are drawn as leaves directly under it, and
   the next map down the chain hangs off the same node as one more child
   (always the last one, since a map has exactly one child map but may have
   several sibling entries).

   Kept as a single, mostly-flat pass (collect the chain into an array
   first) rather than printing while recursing up, so the connector for
   each map/entry -- "├── " vs the terminal "└── " -- can be decided by
   simply checking "is there anything after me", instead of threading that
   information through recursive calls.
*/
static void flow3_map_debug_print(const struct flow3_map* _Opt map, int indent)
{
    (void)indent; /* kept for source compatibility with the existing call site */

    if (!map)
    {
        printf("(null map)\n");
        return;
    }

    const struct flow3_map* chain[FLOW3_MAP_DEBUG_MAX_CHAIN];
    int chain_len = 0;
    for (const struct flow3_map* _Opt cur = map; cur; cur = cur->p_parent_map)
    {
        if (chain_len < FLOW3_MAP_DEBUG_MAX_CHAIN)
            chain[chain_len] = cur;
        chain_len++;
    }
    const int printed_len = chain_len < FLOW3_MAP_DEBUG_MAX_CHAIN ? chain_len : FLOW3_MAP_DEBUG_MAX_CHAIN;

    char prefix[1024] = "";

    /* chain[0] is the deepest (current) map, chain[printed_len-1] is the
       root -- walk it backwards so the root prints first. */
    for (int i = printed_len - 1; i >= 0; i--)
    {
        const struct flow3_map* m = chain[i];
        const bool is_root = (i == printed_len - 1);
        const bool has_child_map = (i > 0);

        if (is_root)
        {
            flow3_map_debug_print_name(m);
        }
        else
        {
            printf("%s%s", prefix, FLOW3_TREE_LAST);
            flow3_map_debug_print_name(m);
        }

        char child_prefix[1024];
        snprintf(child_prefix, sizeof(child_prefix), "%s%s", prefix, is_root ? "" : "    ");

        const struct flow3_key_alternatives* entries[FLOW3_MAP_DEBUG_MAX_ENTRIES] = {0};
        int n_entries = flow3_map_collect_entries(m, entries, FLOW3_MAP_DEBUG_MAX_ENTRIES);
        const int printed_entries = n_entries < FLOW3_MAP_DEBUG_MAX_ENTRIES ? n_entries : FLOW3_MAP_DEBUG_MAX_ENTRIES;

        if (printed_entries == 0 && !has_child_map)
        {
            printf("%s%s(empty)\n", child_prefix, FLOW3_TREE_LAST);
        }
        else
        {
            for (int e = 0; e < printed_entries; e++)
            {
                const bool last_entry = (e == printed_entries - 1) && !has_child_map;
                printf("%s%s", child_prefix, last_entry ? FLOW3_TREE_LAST : FLOW3_TREE_BRANCH);
                flow3_print_object_ptr((void*)entries[e]->p_obj_key);
                flow3_alternatives_print(&entries[e]->alternatives);
                printf("\n");
            }
            if (n_entries > printed_entries)
            {
                printf("%s%s... (%d more entries)\n", child_prefix, FLOW3_TREE_LAST, n_entries - printed_entries);
            }
        }

        /* Next iteration (one level deeper) prints its own map header and
           entries under this node's child slot, i.e. at child_prefix. */
        snprintf(prefix, sizeof(prefix), "%s", child_prefix);
    }

    if (chain_len > FLOW3_MAP_DEBUG_MAX_CHAIN)
    {
        printf("%s... (chain truncated, %d more ancestors)\n", prefix, chain_len - FLOW3_MAP_DEBUG_MAX_CHAIN);
    }
}

/*
   Translate an alternative that carries a numeric value + relation into the
   inclusive integer interval [*lo, *hi] of values it permits. Handles the
   concrete EQUAL case and the four relational (half-line) constraints.
   Returns false when the alternative isn't a plain signed/unsigned numeric
   constraint (pointers, ABSENT, ANY, UNINITIALIZED, NOT_EQUAL), in which
   case callers must fall back to conservative handling.
*/
static bool flow3_alt_to_interval(const struct flow3_alternative* alt, long long* lo, long long* hi)
{
    if (alt->imaginary == FLOW3_IMAGINARY_ABSENT)
        return false;
    long long v = 0;
    if (alt->value_kind == FLOW3_VALUE_KIND_SIGNED)
    {
        v = alt->value.i;
    }
    else if (alt->value_kind == FLOW3_VALUE_KIND_UNSIGNED)
    {
        v = (long long)alt->value.u;
    }
    else
    {
        return false;
    }

    switch (alt->value_relation)
    {
    case FLOW3_RELATION_EQUAL: *lo = v; *hi = v; return true;
    case FLOW3_RELATION_GREATER: if (v == LLONG_MAX) return false; *lo = v + 1; *hi = LLONG_MAX; return true;
    case FLOW3_RELATION_GREATER_EQUAL: *lo = v; *hi = LLONG_MAX; return true;
    case FLOW3_RELATION_LESS: if (v == LLONG_MIN) return false; *lo = LLONG_MIN; *hi = v - 1; return true;
    case FLOW3_RELATION_LESS_EQUAL: *lo = LLONG_MIN; *hi = v; return true;
    default: return false;
    }
}

/*
   Map one alternative through unary minus: if x satisfies (x OP v) then -x
   satisfies (-x OP' -v) with the comparison mirrored. Returns false when the
   alternative can't be mapped (non-integer kind, ANY/UNINITIALIZED, or a value
   whose negation would overflow), in which case the caller falls back to ANY.

   Without this, `-b` lost everything known about `b`, so
   `if (b < 0) ... LLONG_MAX / -b` reported a false "division by zero" even
   though b < 0 makes -b strictly positive (object.c:350).
*/
static bool flow3_alt_negate(const struct flow3_alternative* in,
    struct flow3_alternative* out)
{
    long long v = 0;
    if (in->value_kind == FLOW3_VALUE_KIND_SIGNED)
        v = in->value.i;
    else
        return false;

    if (v == LLONG_MIN) /* -LLONG_MIN is not representable */
        return false;

    enum flow3_relation r = 0;
    switch (in->value_relation)
    {
        /* Deliberately NOT mapping FLOW3_RELATION_EQUAL. A genuinely constant
       operand is already folded by the object_has_constant_value branch above;
       an EQUAL here comes from flow state, which may be a stale seed that no
       write-effect has invalidated yet -- e.g. `int ids[32] = {0};` passed to a
       function that fills it in. Propagating that through `-ids[i]` folded
       `ids[i] < 0` to always-false and reported live code as unreachable
       (parser.c:2064). The half-line relations below are what this fix needs. */
    case FLOW3_RELATION_NOT_EQUAL: r = FLOW3_RELATION_NOT_EQUAL; break;
    case FLOW3_RELATION_GREATER: r = FLOW3_RELATION_LESS; break;
    case FLOW3_RELATION_GREATER_EQUAL: r = FLOW3_RELATION_LESS_EQUAL; break;
    case FLOW3_RELATION_LESS: r = FLOW3_RELATION_GREATER; break;
    case FLOW3_RELATION_LESS_EQUAL: r = FLOW3_RELATION_GREATER_EQUAL; break;
    default: return false;
    }

    *out = *in;
    out->value.i = -v;
    out->value_relation = r;
    return true;
}

/* Is x OP rhs true for a single scalar x? */
static bool flow3_scalar_relation_holds(long long x, enum expression_type op, long long rhs)
{
    switch (op)
    {
    case EXPR_RELATIONAL_BIGGER_THAN: return x > rhs;
    case EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN: return x >= rhs;
    case EXPR_RELATIONAL_LESS_THAN: return x < rhs;
    case EXPR_RELATIONAL_LESS_OR_EQUAL_THAN: return x <= rhs;
    default: return false;
    }
}

static bool flow3_alternative_is_zero(const struct flow3_alternative* alt)
{
    return alt->value_relation == FLOW3_RELATION_EQUAL && flow3_value_is_false(alt);
}

static bool flow3_alternative_is_false(const struct flow3_alternative* alt)
{
    return flow3_alternative_is_zero(alt);
}

static bool flow3_alternative_is_true(const struct flow3_alternative* alt)
{
    return (alt->value_relation == FLOW3_RELATION_NOT_EQUAL && flow3_value_is_false(alt)) ||
    (alt->value_relation == FLOW3_RELATION_EQUAL && !flow3_alternative_is_false(alt));
}

static bool flow3_alternative_can_be_zero(const struct flow3_alternative* alt)
{
    if (flow3_alternative_is_zero(alt))
    {
        return true;
    }
    if (alt->value_relation == FLOW3_RELATION_ANY)
    {
        return true;
    }
    /* NOT_EQUAL with a PTR value means "pointer is this non-null object" — cannot be null. */
    if (alt->value_relation == FLOW3_RELATION_NOT_EQUAL && alt->value_kind == FLOW3_VALUE_KIND_PTR)
    {
        return false;
    }
    /* NOT_EQUAL integer/unsigned nonzero: e.g. !=2 doesn't exclude 0. */
    if (alt->value_relation == FLOW3_RELATION_NOT_EQUAL && flow3_value_is_true(alt))
    {
        return true;
    }
    /* Relational (half-line) constraints: zero is reachable iff it lies
       inside the permitted interval. e.g. `> 0` excludes zero, `>= 0` and
       `< 5` include it. */
    {
        long long lo = 0;
        long long hi = 0;
        if (flow3_alt_to_interval(alt, &lo, &hi) &&
            alt->value_relation != FLOW3_RELATION_EQUAL)
        {
            return lo <= 0 && 0 <= hi;
        }
    }
    return false;
}

/* `line` is the guard's own source line (the `if`/`while`/`&&` site doing the
   narrowing). Every alternative this function SYNTHESIZES below (a value not
   already present verbatim in p_existing, but newly derived from proving the
   condition true/false) is stamped with `line`, not the line copied from the
   pre-narrowing alternative -- flow3_alternative.line means "line of the last
   state change", and narrowing is exactly that: a new fact ("must be exactly
   0", "must be != X", ...) established here. It used to reuse alt->line (the
   ORIGINAL value's line) for these synthesized facts, which is not where the
   narrowed fact was actually established, and is directly user-visible since
   this field feeds diagnostic messages like "(see line %d)" in
   flow3_check_object_access. The plain "keep the value unchanged" path further
   down still copies alt verbatim (including alt->line) on purpose: nothing
   was actually learned there beyond what the pre-branch value already said. */
static void flow3_narrow_map_into(struct flow3_map* p_dest, struct flow3_map* _Opt p_before, const struct object* p_obj_key, bool true_branch, int line)
{
    struct flow3_key_alternatives* _Opt p_existing = flow3_map_search_up(p_before, p_obj_key);
    if (p_existing == NULL || p_existing->alternatives.size == 0)
    {
        /*
           The object has no known value here (either untracked, or a
           degenerate EMPTY entry left behind by an earlier operation --
           e.g. a linked-list cursor `p` right after `p = next;` inside a
           `while (p) { ...; p = next; }` loop, whose entry collapses to
           empty). "No known value" is exactly the ANY case: a plain
           boolean test on it is still informative about the tested
           object itself, so synthesize the same narrowing the
           FLOW3_RELATION_ANY branch below produces -- true arm => the
           object is nonzero, false arm => it is exactly zero.

           Without this, the false/exit arm carried no `p == 0` fact at
           all. In the loop above that let the loop-exit merge
           (flow3_map_merge_arms) fall back to `p`'s STALE pre-loop value
           (a live, non-null _Owner) for the exit arm, so
           flow3_check_object_at_exit reported a false "owner object (p)
           not moved" even though the loop leaves p == NULL. Note this
           only fabricates the tested object's own 0/nonzero fact; it does
           NOT pull any ancestor alternatives (which would wrongly drag in
           unrelated state such as a pointee's "lifetime ended").
        */
        struct flow3_key_alternatives* _Opt p_dest_entry0 = flow3_map_find_add(p_dest, p_obj_key);
        if (p_dest_entry0 != NULL)
        {
            flow3_alternatives_clear(&p_dest_entry0->alternatives);
            struct flow3_alternative a = {
                .value_kind = FLOW3_VALUE_KIND_SIGNED,
                .value = {.i = 0},
                .value_relation = true_branch ? FLOW3_RELATION_NOT_EQUAL : FLOW3_RELATION_EQUAL,
                .imaginary = FLOW3_IMAGINARY_NONE,
                .origin = p_dest,
                .line = 0
            };
            flow3_alternatives_add(&p_dest_entry0->alternatives, &a);
        }
        return;
    }

    /* NOTE: this function used to also run a "correlation filter" here,
       speculatively rewriting OTHER, unrelated tracked objects' entries
       directly into p_dest whenever their alternatives' origins happened
       to be an ancestor/descendant of an origin that survives narrowing
       p_obj_key. The intent (per an old comment) was to support patterns
       like:
           if (a) { b = 1; }
           if (a) { static_debug(b); }  // must see only b==1
       but "ancestor-or-equal of a surviving origin" is a coincidental,
       structural relationship (shared map ancestry), not an actual
       correlation between the two variables -- essentially every object
       tracked in the same function ends up sharing SOME common ancestor
       origin. In practice this corrupted completely unrelated objects:
       e.g. narrowing an unrelated int (`if (!opened) ...`) inside a
       do-while loop with several nested ifs would spuriously overwrite a
       pointer's (`ptk`) entry in the freshly narrowed map with a stale,
       degraded ANY alternative pulled from an unrelated ancestor entry,
       producing false "possible null pointer" / "not moved" diagnostics
       later in the same function. No regression sample exercised the
       intended correlation behavior, so it has been removed rather than
       further special-cased. Real per-branch narrowing of unrelated
       objects continues to work correctly via flow3_map_merge_arms's
       ordinary origin re-tagging (see the comment there), which only
       ever inherits an object's own, real pre-branch value -- it never
       substitutes a different object's alternatives in its place. */

    /* Write the narrowed variable itself into p_dest. */
    struct flow3_key_alternatives* _Opt p_dest_entry = flow3_map_find_add(p_dest, p_obj_key);
    if (p_dest_entry == NULL) return;

    flow3_alternatives_clear(&p_dest_entry->alternatives);

    for (int k = 0; k < p_existing->alternatives.size; k++)
    {
        struct flow3_alternative* alt = p_existing->alternatives.data[k];

        if (alt->value_relation == FLOW3_RELATION_ANY)
        {
            if (true_branch)
            {
                struct flow3_alternative a = {
                    .value_kind = FLOW3_VALUE_KIND_SIGNED,
                    .value = {.i = 0},
                    .value_relation = FLOW3_RELATION_NOT_EQUAL,
                    .imaginary = FLOW3_IMAGINARY_NONE,
                    .origin = p_dest,
                    .line = line
                };
                flow3_alternatives_add(&p_dest_entry->alternatives, &a);
            }
            else
            {
                struct flow3_alternative a = {
                    .value_kind = FLOW3_VALUE_KIND_SIGNED,
                    .value = {.i = 0},
                    .value_relation = FLOW3_RELATION_EQUAL,
                    .imaginary = FLOW3_IMAGINARY_NONE,
                    .origin = p_dest,
                    .line = line
                };
                flow3_alternatives_add(&p_dest_entry->alternatives, &a);
            }
            continue;
        }

        if (alt->value_relation == FLOW3_RELATION_NOT_EQUAL && flow3_value_is_false(alt))
        {
            if (true_branch)
            {
                struct flow3_alternative a = {
                    .value_kind = FLOW3_VALUE_KIND_SIGNED,
                    .value = {.i = 0},
                    .value_relation = FLOW3_RELATION_NOT_EQUAL,
                    .imaginary = FLOW3_IMAGINARY_NONE,
                    .origin = p_dest,
                    .line = line
                };
                flow3_alternatives_add(&p_dest_entry->alternatives, &a);
            }
            else
            {
                struct flow3_alternative a = {
                    .value_kind = FLOW3_VALUE_KIND_SIGNED,
                    .value = {.i = 0},
                    .value_relation = FLOW3_RELATION_EQUAL,
                    .imaginary = FLOW3_IMAGINARY_NONE,
                    .origin = p_dest,
                    .line = line
                };
                flow3_alternatives_add(&p_dest_entry->alternatives, &a);
            }
            continue;
        }

        bool keep = false;
        if (alt->value_relation == FLOW3_RELATION_EQUAL)
        {
            if (flow3_value_is_false(alt) && !true_branch)
            {
                keep = true;
            }
            if (flow3_value_is_true(alt) && true_branch)
            {
                keep = true;
            }
            if (flow3_value_is_true(alt) && !true_branch)
            {
                /*
                   Object is known "== X" for a concrete NONZERO value X
                   (e.g. a linked-list cursor `it` that was just assigned
                   `it = next;`, where next resolved to a specific non-null
                   arena pointer), and we are narrowing the FALSE arm of a
                   test on the object itself (`while (it) {...}`'s exit,
                   `if (it)`'s else). The condition being false means the
                   object is zero here -- which contradicts "== X" (X != 0),
                   so this alternative's path cannot actually reach the
                   false arm.

                   The previous code left `keep` false and added nothing,
                   producing an EMPTY narrowed entry. An empty entry makes
                   later lookups fall through (flow3_map_search_up) to the
                   ancestor map, where the object is still "== X" (non-null)
                   -- so on the false/exit arm the object was wrongly seen
                   as a live, non-null value. For an `_Owner` cursor this
                   surfaced as a false "owner object (it) not moved" at
                   scope exit: `while (it) { ...; it = next; }` leaves `it`
                   null on exit, but flow3 still thought it held a live
                   owner. Found dogfooding flow3 on cake's own
                   defer_visit_ctx_destroy.

                   Record the only value consistent with "condition false"
                   -- exactly 0 -- mirroring the FLOW3_RELATION_ANY false
                   arm above. (0 is also correctly recognized as
                   moved-to-null / released by flow3_alternative_is_zero,
                   so the exit check no longer fires.)
                */
                struct flow3_alternative a = {
                    .value_kind = FLOW3_VALUE_KIND_SIGNED,
                    .value = {.i = 0},
                    .value_relation = FLOW3_RELATION_EQUAL,
                    .imaginary = alt->imaginary,
                    .origin = p_dest,
                    .line = line
                };
                flow3_alternatives_add(&p_dest_entry->alternatives, &a);
                continue;
            }
        }
        else if (alt->value_relation == FLOW3_RELATION_UNINITIALIZED)
        {
            keep = false;
        }
        else if (alt->value_relation == FLOW3_RELATION_NOT_EQUAL)
        {
            /*
               By this point flow3_value_is_false(alt) is known false (the
               "!= 0" -- i.e. definitely-non-null -- case was already
               handled and `continue`d above), so alt->value here is a
               specific NONZERO value: "the object is known to be != X"
               for some concrete X != 0 (e.g. the non-null pointer
               sentinel a decayed array/string-literal address is seeded
               with -- see EXPR_PRIMARY_STRING_LITERAL and the
               array-to-pointer-decay branch of
               flow3_check_object_init_assigment).

               true_branch (object is truthy/nonzero): "!= X" is still
               exactly as informative as before -- keep it as-is.

               false_branch (object is falsy, i.e. == 0): the object's
               value is now KNOWN to be exactly 0, which supersedes "!=
               X" entirely (0 != X was already implied and adds nothing).
               The previous code instead emitted "== X" here -- asserting
               the object equals the very nonzero value it can't be once
               proven falsy -- which produced alternatives that could
               never be recognized as null/zero later (flow3_alternative_is_zero
               requires value 0), so e.g. `while (it) { it = next; }`
               exiting the loop kept reporting `it`'s post-loop value as
               a live nonzero owner, never as the "moved to NULL" case,
               and flagged a false "owner object not moved" at scope
               exit. Found via a user-reported false positive dogfooding
               flow3 on cake's own defer_visit_ctx_destroy.
            */
            if (true_branch)
            {
                struct flow3_alternative a = {
                    .value_kind = alt->value_kind,
                    .value = alt->value,
                    .value_relation = FLOW3_RELATION_NOT_EQUAL,
                    .imaginary = alt->imaginary,
                    .origin = p_dest,
                    .line = line
                };
                flow3_alternatives_add(&p_dest_entry->alternatives, &a);
            }
            else
            {
                struct flow3_alternative a = {
                    .value_kind = FLOW3_VALUE_KIND_SIGNED,
                    .value = {.i = 0},
                    .value_relation = FLOW3_RELATION_EQUAL,
                    .imaginary = alt->imaginary,
                    .origin = p_dest,
                    .line = line
                };
                flow3_alternatives_add(&p_dest_entry->alternatives, &a);
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
            flow3_alternatives_add(&p_dest_entry->alternatives, alt);
        }
    }

    if (p_dest_entry->alternatives.size == 0)
    {
        for (int k = 0; k < p_existing->alternatives.size; k++)
        {
            if (p_existing->alternatives.data[k]->value_relation == FLOW3_RELATION_UNINITIALIZED)
            {
                flow3_alternatives_add(&p_dest_entry->alternatives, p_existing->alternatives.data[k]);
            }
        }
    }

}

static struct flow3_map* _Opt flow3_map_arena_new_branch(struct flow3_map_arena* a,
    struct flow3_map* _Opt parent, bool is_true, const struct expression* _Opt p_expr);

/* Narrows p_obj_key's alternatives onto a new true/false child map, tagged for
   on-demand naming (see flow3_map_arena_new_branch). `true_branch` doubles as
   both which side of p_obj_key this map narrows to AND which of "true
   branch"/"false branch" it renders as -- the two always agree at every call
   site. */
static struct flow3_map* _Opt flow3_narrow_map_branch(struct flow3_map_arena* arena, struct flow3_map* _Opt p_before,
    const struct object* p_obj_key, bool true_branch, const struct expression* _Opt p_expr, int line)
{
    struct flow3_map* _Opt p_dest = flow3_map_arena_new_branch(arena, p_before, true_branch, p_expr);
    if (p_dest == NULL)
        return NULL;

    flow3_narrow_map_into(p_dest, p_before, p_obj_key, true_branch, line);
    return p_dest;
}

/*
   Expressions that cannot narrow return the identity pair
   (p_true == p_false == current map). If a branch body is then visited
   directly on it, the body's writes land in the pre-branch map and leak
   unconditionally past the join. Force two distinct (initially empty)
   child maps so that flow3_map_merge_arms can re-introduce the
   pre-branch alternatives for the arm that did not write.
*/
/* Monotonic source of unique branch ids (one per condition's true/false map
   pair). Uniqueness within a run is all that's required. */
static int g_flow3_next_branch_id = 0;

/* Give a freshly created true/false map pair a shared branch id with opposite
   sides, so later correlation can tell "then" values from "else" values even
   after the branch has merged away. */
static void flow3_tag_branch_pair(struct flow3_map* _Opt p_true, struct flow3_map* _Opt p_false)
{
    if (p_true == NULL || p_false == NULL || p_true == p_false)
        return; /* folded/degenerate pair -- no real branch */
    int id = ++g_flow3_next_branch_id;
    p_true->branch_id = id;
    p_false->branch_id = id;
}

/* --- Same-predicate branch correlation -----------------------------------
   Two branches controlled by the IDENTICAL predicate (same comparison of the
   same operands, with no intervening write to those operands) should share a
   branch id, so a value guarded by that predicate stays correlated across both
   branches:

       if (c > 2) p = &i;
       if (c > 2) i = *p;     // entered only when the first was -> p non-null

   The cache (held on the visit context, flow3_visit_ctx::predicate_cache) maps
   a comparison predicate to the branch id first assigned to it. It is reset per
   function and invalidated for any operand that gets written (a reassigned
   operand makes the predicate's truth independent again, so reusing the id
   would be unsound). */
static void flow3_predicate_cache_reset(struct flow3_visit_ctx* ctx)
{
    ctx->predicate_cache_size = 0;
}

/* Drop any cached predicate that mentions p_obj -- its truth may have changed. */
static void flow3_predicate_invalidate(struct flow3_visit_ctx* ctx, const struct object* _Opt p_obj)
{
    if (p_obj == NULL)
        return;
    int w = 0;
    for (int r = 0; r < ctx->predicate_cache_size; r++)
    {
        if (ctx->predicate_cache[r].left_obj == p_obj || ctx->predicate_cache[r].right_obj == p_obj)
            continue; /* drop this entry */
        ctx->predicate_cache[w++] = ctx->predicate_cache[r];
    }
    ctx->predicate_cache_size = w;
}

/* Extract (op, left_obj, right) from a simple comparison condition. Returns
   false for anything that isn't `object OP constant` or `object OP object`. */
static bool flow3_predicate_key(const struct expression* _Opt p_cond,
    enum expression_type* op,
    const struct object** left_obj,
    const struct object** right_obj,
    long long* right_const)
{
    if (p_cond == NULL)
        return false;

    switch (p_cond->expression_type)
    {
    case EXPR_EQUALITY_EQUAL:
    case EXPR_EQUALITY_NOT_EQUAL:
    case EXPR_RELATIONAL_BIGGER_THAN:
    case EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN:
    case EXPR_RELATIONAL_LESS_THAN:
    case EXPR_RELATIONAL_LESS_OR_EQUAL_THAN:
        break;
    default:
        return false;
    }

    if (p_cond->left == NULL || p_cond->right == NULL)
        return false;

    const struct object* _Opt l = object_get_referenced(&p_cond->left->object);
    if (l == NULL)
        return false;

    *op = p_cond->expression_type;
    *left_obj = l;

    if (object_has_known_value(&p_cond->right->object))
    {
        *right_obj = NULL;
        *right_const = object_to_signed_long_long(&p_cond->right->object);
    }
    else
    {
        const struct object* _Opt r = object_get_referenced(&p_cond->right->object);
        if (r == NULL)
            return false;
        *right_obj = r;
        *right_const = 0;
    }
    return true;
}

/* Return the branch id this predicate should use: a previously-recorded id if
   the identical predicate was seen (and its operands untouched since), else
   fresh_id (which is also recorded for next time). Non-comparison conditions
   just keep their fresh id. */
static int flow3_predicate_shared_id(struct flow3_visit_ctx* ctx, const struct expression* p_cond, int fresh_id)
{
    enum expression_type op = 0;
    const struct object* _Opt lo = NULL;
    const struct object* _Opt ro = NULL;
    long long rc = 0;
    if (!flow3_predicate_key(p_cond, &op, &lo, &ro, &rc))
        return fresh_id;

    for (int i = 0; i < ctx->predicate_cache_size; i++)
    {
        if (ctx->predicate_cache[i].op == op &&
            ctx->predicate_cache[i].left_obj == lo &&
            ctx->predicate_cache[i].right_obj == ro &&
            ctx->predicate_cache[i].right_const == rc)
        {
            return ctx->predicate_cache[i].branch_id; /* reuse */
        }
    }

    if (ctx->predicate_cache_size < (int)(sizeof ctx->predicate_cache / sizeof ctx->predicate_cache[0]))
    {
        struct flow3_predicate_entry* e = &ctx->predicate_cache[ctx->predicate_cache_size];
        e->op = op;
        e->left_obj = lo;
        e->right_obj = ro;
        e->right_const = rc;
        e->branch_id = fresh_id;
        ctx->predicate_cache_size++;
    }
    return fresh_id;
}

/*
   Render a map's debug/diagnostic name from its `kind` (see enum
   flow3_map_kind) -- for TRUE_BRANCH/FALSE_BRANCH a GCC-style
   "true branch (p)" / "false branch (p)" built from branch_expr, for
   GOTO_LABEL the label's own name, everything else a fixed string -- instead
   of the opaque internal tags this used to be ("if-true", "eq-false",
   "call-opt-nonnull", ...). Those meant nothing to a reader trying to make
   sense of a static_debug(0) dump or a diagnostic's "(... see line N
   \"origin-name\")" note: printing the actual branch condition alongside
   true/false (matching how GCC's own branch notes read) makes the dump
   self-explanatory instead of requiring a trip back to the source to figure
   out what "if-true" even refers to. User-suggested.

   No caching: every call re-renders into `ss` (which the caller owns and
   must ss_close()). kind/branch_expr are stashed on the map at
   creation, all plain values with no allocation, so creating any map costs
   nothing beyond that; only an actual read -- a diagnostic or a
   static_debug(0) dump, a small fraction of all maps created -- pays for
   building the string, and paying for it again on every such read (rather
   than keeping a cached copy around) is the simplest way to guarantee
   nothing outlives the map that owns it.
*/
static void flow3_map_name_to_string(const struct flow3_map* _Opt map, struct osstream* ss)
{
    ss_clear(ss);

    if (map == NULL)
    {
        ss_fprintf(ss, "?");
        return;
    }

    switch (map->kind)
    {
    case FLOW3_MAP_ROOT: ss_fprintf(ss, "root"); return;
    case FLOW3_MAP_BREAK_JOIN: ss_fprintf(ss, "break join"); return;
    case FLOW3_MAP_THROW_JOIN: ss_fprintf(ss, "throw join"); return;
    case FLOW3_MAP_TRY_BRANCH: ss_fprintf(ss, "try branch"); return;
    case FLOW3_MAP_CATCH_BRANCH: ss_fprintf(ss, "catch branch"); return;
    case FLOW3_MAP_SWITCH_BODY: ss_fprintf(ss, "switch body"); return;
    case FLOW3_MAP_OPT_NULL: ss_fprintf(ss, "opt-null"); return;
    case FLOW3_MAP_OPT_NONNULL: ss_fprintf(ss, "opt-nonnull"); return;
    case FLOW3_MAP_OPT_MEM_NULL: ss_fprintf(ss, "opt-mem-null"); return;
    case FLOW3_MAP_OPT_MEM_NONNULL: ss_fprintf(ss, "opt-mem-nonnull"); return;
    case FLOW3_MAP_CALL_OPT_NULL: ss_fprintf(ss, "call-opt-null"); return;
    case FLOW3_MAP_CALL_OPT_NONNULL: ss_fprintf(ss, "call-opt-nonnull"); return;
    case FLOW3_MAP_DO_WHILE_BODY_DIAG: ss_fprintf(ss, "do-while body (diagnostics only)"); return;
    case FLOW3_MAP_DO_WHILE_FALSE: ss_fprintf(ss, "do-while false branch"); return;
    case FLOW3_MAP_FOR_BODY_DIAG: ss_fprintf(ss, "for body (diagnostics only)"); return;
    case FLOW3_MAP_DEFAULT: ss_fprintf(ss, "default"); return;
    case FLOW3_MAP_CASE: ss_fprintf(ss, "case"); return;
    case FLOW3_MAP_REL_DEAD: ss_fprintf(ss, "rel-dead"); return;
    case FLOW3_MAP_EQ_DEAD: ss_fprintf(ss, "eq-dead"); return;
    case FLOW3_MAP_NOT_DEAD_TRUE: ss_fprintf(ss, "not-dead-true"); return;
    case FLOW3_MAP_NOT_DEAD_FALSE: ss_fprintf(ss, "not-dead-false"); return;
    case FLOW3_MAP_OR_DEAD_TRUE: ss_fprintf(ss, "or-dead-true"); return;
    case FLOW3_MAP_OR_DEAD_FALSE: ss_fprintf(ss, "or-dead-false"); return;
    case FLOW3_MAP_AND_DEAD_TRUE: ss_fprintf(ss, "and-dead-true"); return;
    case FLOW3_MAP_AND_DEAD_FALSE: ss_fprintf(ss, "and-dead-false"); return;

    case FLOW3_MAP_GOTO_LABEL: ss_fprintf(ss, "goto label"); return;

    case FLOW3_MAP_TRUE_BRANCH:
    case FLOW3_MAP_FALSE_BRANCH:
        ss_fprintf(ss, "%s", map->kind == FLOW3_MAP_TRUE_BRANCH ? "true branch" : "false branch");
        if (map->branch_expr)
        {
            /* flow3_expression_to_string() starts by ss_clear()-ing whatever
               stream it is given -- so it must NOT be handed `ss` directly,
               or it wipes out the "what" text already written above. Render
               the expression into its own scratch stream instead, then
               append. */
            struct osstream expr_ss = { 0 };
            flow3_expression_to_string(map->branch_expr, &expr_ss);
            ss_fprintf(ss, " (%s)", expr_ss.c_str ? expr_ss.c_str : "");
            ss_close(&expr_ss);
        }
        return;

    case FLOW3_MAP_MERGE_TEMP:
        ss_fprintf(ss, "merge-temp");
        return;
    }

    ss_fprintf(ss, "?");
}

/* Max ancestors flow3_explain_origin walks before giving up. Same
   sanity ceiling as FLOW3_MAP_DEBUG_MAX_CHAIN, kept separate so the
   compact one-line path and the full tree dump can be tuned apart. */
#define FLOW3_MAP_PATH_MAX_CHAIN 128

/*
   Renders the chain of decisions that led to `map`, root first, as a single
   line:

       true branch (p != NULL) -> false branch (p->next) -> opt-null

   flow3_map_name_to_string() names ONE map, which is all a diagnostic used
   to report ("... set at line N in \"false branch (p->next)\""). That
   answers "which map recorded this fact" but not the question a reader
   actually has when staring at a null-deref warning they believe is
   impossible: *how did control get into that map* -- which conditions were
   assumed true, and which false, along the way. That information is
   already present (every branch map keeps its kind and branch_expr, and
   p_parent_map chains them back to the root) but was never rendered, so
   answering it meant re-running with static_debug(0) and reading the full
   tree dump by hand.

   Only decision points are printed. FLOW3_MAP_ROOT contributes nothing (it
   is where every path starts, so naming it is pure noise on every single
   line) and FLOW3_MAP_MERGE_TEMP is the short-lived scratch map from
   flow3_map_merge_arms, which is an implementation detail of the join, not
   a branch the source code took. A map whose entire chain is root-only
   renders as "root" rather than an empty string, so a caller can always
   splice the result into a sentence without checking for emptiness.

   Returns the stream by value, transferring its buffer to the caller, who
   must ss_close() it -- same shape as type_dup(). Nothing is allocated up
   front and the text is re-rendered on every call.
*/
static struct osstream flow3_explain_origin(const struct flow3_map* _Opt map)
{
    struct osstream ss_storage = { 0 };
    struct osstream* ss = &ss_storage;

    if (map == NULL)
    {
        ss_fprintf(ss, "?");
        return ss_storage;
    }

    /* Collect deepest-first, then walk backwards, so the line reads in the
       order control flow actually took -- same reason flow3_map_debug_print
       collects the chain into an array before printing it. */
    const struct flow3_map* chain[FLOW3_MAP_PATH_MAX_CHAIN];
    int chain_len = 0;
    for (const struct flow3_map* _Opt cur = map; cur; cur = cur->p_parent_map)
    {
        if (chain_len < FLOW3_MAP_PATH_MAX_CHAIN)
            chain[chain_len] = cur;
        chain_len++;
    }
    const int collected = chain_len < FLOW3_MAP_PATH_MAX_CHAIN ? chain_len : FLOW3_MAP_PATH_MAX_CHAIN;

    if (chain_len > FLOW3_MAP_PATH_MAX_CHAIN)
        ss_fprintf(ss, "... -> ");

    int printed = 0;
    for (int i = collected - 1; i >= 0; i--)
    {
        const struct flow3_map* m = chain[i];

        if (m->kind == FLOW3_MAP_ROOT || m->kind == FLOW3_MAP_MERGE_TEMP)
            continue;

        struct osstream name_ss = { 0 };
        flow3_map_name_to_string(m, &name_ss);
        ss_fprintf(ss, "%s%s", printed > 0 ? " -> " : "", name_ss.c_str ? name_ss.c_str : "");
        ss_close(&name_ss);

        /* Branch maps know the condition they came from, and that
           expression knows where it was written -- so each step of the path
           can point at the exact `if` the reader needs to look at, instead
           of leaving them to find which of several identically-spelled
           conditions this one was. Only branch maps carry branch_expr; the
           other kinds have no single source line to name. Guarded on
           line > 0 because compiler-generated tokens carry no real
           position. */
        if (m->branch_expr != NULL &&
            m->branch_expr->first_token->line > 0)
        {
            ss_fprintf(ss, " at line %d", m->branch_expr->first_token->line);
        }

        if (m->is_unreachable)
            ss_fprintf(ss, " [unreachable]");

        printed++;
    }

    if (printed == 0)
        ss_fprintf(ss, "root");

    return ss_storage;
}

/*
   Reports `map`'s decision path as a series of W_INFO notes, one per branch
   taken, each pointing at the condition token it came from -- so the reader
   gets the actual source line rendered under the warning for every step,
   root first:

       warning 35: 'p->last_token' may be null ...
       note: true branch (a)
         5 |     if (a)
       note: false branch (b)
         7 |         if (b)

   Emitted as notes carrying a real token rather than as more text spliced
   into the parent's message: the diagnostic machinery then renders the
   source line and caret itself instead of leaving the reader to decode
   "at line 7" by hand, and a path several branches deep would otherwise
   make the parent's single line unreadably long.

   W_INFO rather than W_LOCATION is a deliberate choice by the author.
   The tradeoff to know: W_LOCATION entries attach to the preceding
   diagnostic as children, so they are freed with it and a `//lint N` that
   suppresses the warning suppresses its path too. W_INFO notes are
   independent -- they survive such a suppression and count toward the
   report's note total.

   Still called only when the parent diagnostic() returned true, so a
   suppressed warning does not leave orphaned path notes behind.

   Only branch maps produce a note: they are the decisions the source
   actually took, and they are the only kinds carrying both a name worth
   printing and a token to point at. Maps with no branch_expr (opt-null,
   joins, cases) and root/merge-temp are skipped for the same reason
   flow3_explain_origin leaves them out.
*/
static void flow3_diagnose_map_path(struct flow3_visit_ctx* ctx, const struct flow3_map* _Opt map)
{
    if (map == NULL)
        return;

    /* Collect deepest-first then emit backwards, so notes read root-first,
       in the order control flow took them (same shape as
       flow3_explain_origin / flow3_map_debug_print). */
    const struct flow3_map* chain[FLOW3_MAP_PATH_MAX_CHAIN];
    int chain_len = 0;
    for (const struct flow3_map* _Opt cur = map; cur; cur = cur->p_parent_map)
    {
        if (chain_len < FLOW3_MAP_PATH_MAX_CHAIN)
            chain[chain_len] = cur;
        chain_len++;
    }
    const int collected = chain_len < FLOW3_MAP_PATH_MAX_CHAIN ? chain_len : FLOW3_MAP_PATH_MAX_CHAIN;

    /* Already-reported (condition, side) pairs, used to drop the repeats a
       loop produces. Analyzing a loop walks its body more than once, so the
       same `while (ctx->current->type == TK_STRING_LITERAL)` decision lands
       on the chain several times -- printing it once per visit padded a
       real path out to ten notes when six carried all the information, and
       the duplicates are indistinguishable from each other on screen
       (identical text, identical line, identical caret), so they read as a
       rendering glitch rather than as "the loop went round again".
       Keyed on the expression AND the kind, so a condition genuinely taken
       both ways -- true on one visit, false on another -- still shows both;
       only an exact repeat of the same decision is dropped. Bounded by the
       chain length, so no allocation. */
    const struct expression* seen_expr[FLOW3_MAP_PATH_MAX_CHAIN];
    enum flow3_map_kind seen_kind[FLOW3_MAP_PATH_MAX_CHAIN];
    int seen_count = 0;

    for (int i = collected - 1; i >= 0; i--)
    {
        const struct flow3_map* m = chain[i];

        if (m->branch_expr == NULL)
            continue;

        bool already_reported = false;
        for (int s = 0; s < seen_count; s++)
        {
            if (seen_expr[s] == m->branch_expr && seen_kind[s] == m->kind)
            {
                already_reported = true;
                break;
            }
        }
        if (already_reported)
            continue;

        if (seen_count < FLOW3_MAP_PATH_MAX_CHAIN)
        {
            seen_expr[seen_count] = m->branch_expr;
            seen_kind[seen_count] = m->kind;
            seen_count++;
        }

        /* Underline the whole condition (first_token..last_token) rather
           than passing just first_token as the caret: with only the first
           token the marker rendered "~~~" under `ctx` alone for a condition
           like `ctx->current->type == TK_IDENTIFIER`, pointing at a
           sub-expression that is not what the branch turned on. */
        const struct marker branch_marker =
        {
            .p_token_begin = m->branch_expr->first_token,
            .p_token_end = m->branch_expr->last_token,
        };

        const bool is_true_branch = (m->kind == FLOW3_MAP_TRUE_BRANCH);

        /* One note per decision, naming what was assumed about the
           condition. Clang's analyzer splits this in two ("Assuming 'p' is
           null" then "Taking false branch"), which was tried here first and
           read as noise: both notes carry the same bit, printed against the
           same caret on the same line, so
             note: Assuming 'x->type==TK_STRING_LITERAL' is true
             note: Taking true branch
           says one thing twice. The assumption is the more informative half
           -- it states nullness for pointers, which is the very fact the
           warning goes on to complain about -- and the branch taken follows
           from it, so only that half is printed. */
        struct osstream cond_ss = { 0 };
        flow3_expression_to_string(m->branch_expr, &cond_ss);
        const char* cond = cond_ss.c_str ? cond_ss.c_str : "";

        /* Phrase the assumption in terms of the condition's own type. For a
           pointer, "is null"/"is non-null" is what the reader cares about
           and what the resulting warning will talk about; for anything else
           claiming nullness would be wrong, so fall back to true/false. */
        const char* assumption;
        if (type_is_pointer(&m->branch_expr->type))
            assumption = is_true_branch ? "is non-null" : "is null";
        else
            assumption = is_true_branch ? "is true" : "is false";

        diagnostic(W_INFO, ctx->ctx, NULL, &branch_marker,
            "Assuming '%s' %s%s", cond, assumption,
            m->is_unreachable ? " (unreachable)" : "");

        ss_close(&cond_ss);
    }
}

/*
   Reports where an alternative's state was established, as its own note
   instead of a "(see line N)" tail on the warning's text.

   The alternative records the line but no token, and diagnostic() renders
   the source line from the token, not from marker.line -- a note built on
   the warning's own marker would claim one line while displaying another.
   The origin map does carry a token: branch maps stash the condition they
   were created for in branch_expr. Pointing the note there shows the
   branch the state was established under, which is the context a reader
   needs, without plumbing a token through every state setter.

   The caret and the line therefore say different things on purpose -- the
   branch condition vs. the assignment inside it -- so the text names the
   caret's role explicitly. Kinds other than branch maps carry no
   branch_expr and have nothing to point at; there the line is named in the
   text alone, against the caller's own marker.
*/
static void flow3_diagnose_state_origin(struct flow3_visit_ctx* ctx,
    const struct flow3_alternative* p_alternative,
    const struct marker* p_fallback_marker)
{
    const struct expression* _Opt p_state_expr =
    p_alternative->origin ? p_alternative->origin->branch_expr : NULL;

    if (p_state_expr == NULL)
    {
        /* Nothing to point at, so the note would render the warning's own
           source line. When the state was established on that same line --
           `p = NULL;` warned about at the assignment itself -- it would
           restate the line the reader is already looking at, caret and
           all. Say nothing rather than say it twice. */
        const struct token* _Opt p_at = p_fallback_marker->p_token_caret ?
        p_fallback_marker->p_token_caret : p_fallback_marker->p_token_begin;

        if (p_at != NULL && p_at->line == p_alternative->line)
            return;

        diagnostic(W_INFO, ctx->ctx, NULL, p_fallback_marker,
            "the null state comes from line %d", p_alternative->line);
        return;
    }

    const struct marker state_marker =
    {
        .p_token_begin = p_state_expr->first_token,
        .p_token_end = p_state_expr->last_token,
    };

    diagnostic(W_INFO, ctx->ctx, NULL, &state_marker,
        "the null state comes from line %d, in this branch", p_alternative->line);
}

/* Create a branch map tagged for on-demand naming: `is_true` and the
   condition expression are stashed on the map, not rendered, until
   flow3_map_name_to_string() is actually asked to build the name (a
   diagnostic, a static_debug(0) dump). */
static struct flow3_map* _Opt flow3_map_arena_new_branch(struct flow3_map_arena* a,
    struct flow3_map* _Opt parent, bool is_true, const struct expression* _Opt p_expr)
{
    struct flow3_map* _Opt m = flow3_map_arena_new(a, parent, is_true ? FLOW3_MAP_TRUE_BRANCH : FLOW3_MAP_FALSE_BRANCH);
    if (m)
    {
        m->branch_expr = p_expr;
    }
    return m;
}

static struct flow3_branch_pair flow3_ensure_branch_pair(struct flow3_visit_ctx* ctx,
    struct flow3_map* _Opt p_fallback,
    struct flow3_branch_pair pair,
    const struct expression* _Opt p_expr)
{
    if (pair.p_true == pair.p_false)
    {
        struct flow3_map* base = pair.p_true ? pair.p_true : p_fallback;
        pair.p_true = flow3_map_arena_new_branch(&ctx->flow3_map_arena, base, true, p_expr);
        pair.p_false = flow3_map_arena_new_branch(&ctx->flow3_map_arena, base, false, p_expr);
        if (pair.p_true == NULL || pair.p_false == NULL)
        {
            /* allocation failure: degrade to the old identity behaviour */
            pair.p_true = base;
            pair.p_false = base;
        }
        flow3_tag_branch_pair(pair.p_true, pair.p_false);
        return pair;
    }

    /*
       pair.p_true and pair.p_false are already distinct (e.g. `a && b`:
       p_true is whatever right's own branch produced, p_false is a
       freshly-merged map) -- but one side can still literally BE
       p_fallback itself, rather than a fresh child map. This happens
       whenever that side's chain of sub-expressions never narrowed
       anything (e.g. plain function calls): each identity pair just
       passes the live pre-branch map straight through unchanged. If the
       branch body visited on that side later mutates it in place --
       most importantly, marking it is_unreachable on an unconditional
       return/break/continue/goto/throw -- it corrupts p_fallback itself
       (an ancestor shared with the OTHER arm and with whatever comes
       after this whole construct), which can make unrelated,
       still-reachable code get misreported as unreachable. Wrap any
       side that aliases p_fallback in its own fresh child map so branch
       bodies can only ever mutate their own arm.
    */
    if (pair.p_true == p_fallback)
    {
        struct flow3_map* _Opt fresh = flow3_map_arena_new_branch(&ctx->flow3_map_arena, pair.p_true, true, p_expr);
        if (fresh != NULL)
        {
            pair.p_true = fresh;
        }
    }
    if (pair.p_false == p_fallback)
    {
        struct flow3_map* _Opt fresh = flow3_map_arena_new_branch(&ctx->flow3_map_arena, pair.p_false, false, p_expr);
        if (fresh != NULL)
        {
            pair.p_false = fresh;
        }
    }
    return pair;
}

static void flow3_exit_block_visit_defer_item(struct flow3_visit_ctx* ctx, const struct defer_list_item* p_item, const struct token* position_token)
{
    if (p_item->defer_statement)
    {
        const int error_count = ctx->ctx->p_report->error_count;
        const int warnings_count = ctx->ctx->p_report->warnings_count;
        const int info_count = ctx->ctx->p_report->info_count;

        if (p_item->defer_statement->unlabeled_statement)
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
        struct token* _Opt p_token = NULL;
        if (p_declarator->name_opt)
            p_token = p_declarator->name_opt;
        else if (p_declarator->specifier_qualifier_list)
            p_token = p_declarator->specifier_qualifier_list->first_token;
        else if (p_declarator->declaration_specifiers)
            p_token = p_declarator->declaration_specifiers->first_token;
        else
            _Assert(false);

        const struct marker marker = { .p_token_begin = p_token, .p_token_end = p_token };
        flow3_check_object_at_exit(ctx,
            &p_declarator->type,
            &p_declarator->object,
            &marker,
            position_token,
            false,
            p_declarator->name_opt ? p_declarator->name_opt->lexeme : NULL);
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
    if (ctx->p_current_flow3_map == NULL)
    {
        return;
    }

    if (p_item->defer_statement)
    {
        /* defer statements are executable blocks, not objects — no lifetime to end. */
    }
    else if (p_item->declarator)
    {
        struct declarator* p_declarator = p_item->declarator;

        /* A static (or extern) declarator reached via the scope chain does
           not actually end its lifetime here -- only automatic storage
           (locals, parameters) does. Without this check, `static int x;`
           going out of its block's syntactic scope got marked ENDED just
           like a true local. */
        if (!is_automatic_variable(p_declarator->type.storage_class_specifier_flags))
        {
            return;
        }

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

static void flow3_visit_secondary_block(struct flow3_visit_ctx* ctx, struct secondary_block* _Opt p_secondary_block)
{
    /* _Owner _Opt in the AST: an absent secondary block is an empty body,
       so there is simply nothing to visit. Guarded here rather than at each
       of the fourteen call sites. */
    if (p_secondary_block == NULL)
        return;

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
    if (ctx->p_current_flow3_map == NULL)
    {
        return;
    }

    if (p_object->members.head)
    {
        struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, p_object);
        if (e == NULL) return;

        flow3_alternatives_clear(&e->alternatives);
        struct flow3_alternative a = {
            .value_kind = FLOW3_VALUE_KIND_REF,
            .value = {.p = p_object},
            .value_relation = FLOW3_RELATION_EQUAL,
            .imaginary = FLOW3_IMAGINARY_NONE,
            .origin = ctx->p_current_flow3_map,
            .line = line
        };
        flow3_alternatives_add(&e->alternatives, &a);

        struct object* _Opt  p_object_it = p_object->members.head;
        for (; p_object_it; p_object_it = p_object_it->next)
        {
            flow3_object_init(ctx, p_object_it, &p_object_it->type, line);
        }

        return;
    }

    enum flow3_relation relation = FLOW3_RELATION_UNINITIALIZED;

    struct flow3_alternative value = { 0 };
    if (type_is_pointer(&p_object->type))
    {
        value.value_kind = FLOW3_VALUE_KIND_PTR;
        value.value.p = (void*)(uintptr_t)p_object->value.host_u_long_long;
    }
    else if (type_is_signed(&p_object->type))
    {
        value.value_kind = FLOW3_VALUE_KIND_SIGNED;
        value.value.i = p_object->value.host_long_long;
    }
    else
    {
        value.value_kind = FLOW3_VALUE_KIND_UNSIGNED;
        value.value.u = p_object->value.host_u_long_long;
    }

    switch (p_object->state)
    {
    case CONSTANT_VALUE_STATE_UNINITIALIZED:
        relation = FLOW3_RELATION_UNINITIALIZED;
        break;

    case CONSTANT_VALUE_STATE_CONSTANT:
    case CONSTANT_VALUE_EQUAL:
        relation = FLOW3_RELATION_EQUAL;
        break;

    case CONSTANT_VALUE_STATE_ANY:
        relation = FLOW3_RELATION_ANY;
        break;
    }

    {
        struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, p_object);
        if (e == NULL)
            return; /* no entry to seed */

        flow3_alternatives_clear(&e->alternatives);
        struct flow3_alternative a = {
            .value_kind = value.value_kind,
            .value = value.value,
            .value_relation = relation,
            .imaginary = FLOW3_IMAGINARY_NONE,
            .origin = ctx->p_current_flow3_map,
            .line = line
        };
        flow3_alternatives_add(&e->alternatives, &a);
    }
}

static void flow3_parameter_object_init_r(struct flow3_visit_ctx* ctx, struct object* p_object, const struct type* p_type, int line, int depth, bool force_opt);

static void flow3_parameter_object_init(struct flow3_visit_ctx* ctx, struct object* p_object, const struct type* p_type, int line)
{
    flow3_parameter_object_init_r(ctx, p_object, p_type, line, 0, false);
}

/* force_opt: true once we're inside the pointee of an _Opt (or _Dtor-like,
   permissively-modeled) pointer -- see the call site inside the _Opt-pointer
   branch below. It makes every pointer member found from here down be
   modeled as _Opt (correlated null/non-null) REGARDLESS OF ITS OWN
   DECLARED nullability, and keeps propagating through further recursion,
   mirroring how dest_is_dtor propagates permissiveness down the recursion
   in flow3_check_object_init_assigment. Rationale: an _Opt pointer's target
   is not guaranteed to be a fully-formed object (same spirit as a _Dtor
   parameter accepting a partially-created one), so a non-_Opt member
   reached through it should not be trusted as unconditionally non-null
   either. See samples/flow3/opt-taints-members.c. */
static void flow3_parameter_object_init_r(struct flow3_visit_ctx* ctx, struct object* p_object, const struct type* p_type, int line, int depth, bool force_opt)
{
    if (ctx->p_current_flow3_map == NULL)
        return;

    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    try
    {
        if (p_object->members.head)
        {
            struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, p_object);
            if (e == NULL) throw;
            flow3_alternatives_clear(&e->alternatives);
            struct flow3_alternative a = {
                .value_kind = FLOW3_VALUE_KIND_REF,
                .value = {.p = p_object},
                .value_relation = FLOW3_RELATION_EQUAL,
                .imaginary = FLOW3_IMAGINARY_NONE,
                .origin = ctx->p_current_flow3_map,
                .line = line
            };
            flow3_alternatives_add(&e->alternatives, &a);

            p_object = p_object->members.head;
            for (; p_object; p_object = p_object->next)
            {
                flow3_parameter_object_init_r(ctx, p_object, &p_object->type, line, depth, force_opt);
            }
            return;
        }

        enum flow3_relation relation = FLOW3_RELATION_UNINITIALIZED;

        struct flow3_alternative value = { 0 };
        if (type_is_pointer(&p_object->type))
        {
            value.value_kind = FLOW3_VALUE_KIND_PTR;
            value.value.p = NULL;
        }
        else if (type_is_signed(&p_object->type))
        {
            value.value_kind = FLOW3_VALUE_KIND_SIGNED;
            value.value.i = p_object->value.host_long_long;
        }
        else
        {
            value.value_kind = FLOW3_VALUE_KIND_UNSIGNED;
            value.value.u = p_object->value.host_u_long_long;
        }

        switch (p_object->state)
        {
        case CONSTANT_VALUE_STATE_UNINITIALIZED:
            relation = FLOW3_RELATION_UNINITIALIZED;
            break;

        case CONSTANT_VALUE_STATE_CONSTANT:
        case CONSTANT_VALUE_EQUAL:
            relation = FLOW3_RELATION_EQUAL;
            break;

        case CONSTANT_VALUE_STATE_ANY:
            relation = FLOW3_RELATION_ANY;
            break;
        }

        /*parameter are any by default*/
        relation = FLOW3_RELATION_ANY;

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
            (type_is_pointer(p_type) || type_is_array(p_type)) &&
            !type_is_opt(p_type, nullable_enabled) &&
            !force_opt)
        {
            /* Non-optional pointer (or array parameter, which decays to a
           pointer per C's parameter-adjustment rule and is likewise
           always non-null): assume non-null. Allocate an arena object
           to carry the concrete alias so the analyser can track the
           pointed state (e.g. _Ctor initialisation checks).

           Before adding the type_is_array(p_type) case here, an array
           parameter (e.g. `char new_name[]`) never took this branch at
           all -- type_is_pointer is false for TYPE_CATEGORY_ARRAY -- so
           it fell through to the generic "ANY" seed below instead of a
           definite NOT_EQUAL-0. Dereferencing it directly (new_name[0])
           still looked safe (that check doesn't consult this relation),
           but passing that same array on to another function's
           non-optional pointer parameter incorrectly warned "passing a
           possible null pointer", since ANY reads as "could be zero".
           Found via a user-reported false positive on real code
           (generate_file_scope_new_name's `char new_name[]`). */
            struct object* _Opt p_pointed = flow3_allocated_object_arena_new(&ctx->allocated_object_arena);
            /* `_Opt struct X* p` (qualifier BEFORE the struct-specifier) qualifies
           the POINTEE's type, not the pointer -- p itself stays a guaranteed
           non-null pointer (that's why this whole branch, which only runs for
           a non-_Opt *pointer*, still fires). But the pointee is marked _Opt
           in the same sense a _Dtor pointee is: not guaranteed a fully-formed
           object. So members reached through it should be force_opt-tainted
           just like an _Opt *pointer*'s pointee, even though p can't be null.
           Contrast `struct X* _Opt p` (qualifier AFTER '*'), which makes p
           itself nullable and is handled entirely by the _Opt-pointer branch
           further down -- these are two independent positions for _Opt. */
            bool pointee_is_opt = false;
            if (p_pointed != NULL)
            {
                struct type pointed_type = type_is_array(p_type)
                ? get_array_item_type(p_type)
                : type_remove_pointer(p_type);
                pointee_is_opt = type_is_opt(&pointed_type, nullable_enabled);
                make_object(&pointed_type, p_pointed, MAKE_STATE_ANY, ctx->ctx->options.target);
                type_destroy(&pointed_type);
            }

            struct flow3_key_alternatives* _Opt ep = flow3_map_find_add(ctx->p_current_flow3_map, p_object);
            if (ep == NULL) throw;
            flow3_alternatives_clear(&ep->alternatives);
            /* Single EQUAL alternative carrying the concrete arena pointer.
         * The parameter is non-null on entry and points exactly to this object. */
            if (p_pointed != NULL)
            {

                struct flow3_alternative a = {
                    .value_kind = FLOW3_VALUE_KIND_PTR,
                    .value = {.p = p_pointed},
                    .value_relation = FLOW3_RELATION_EQUAL,
                    .imaginary = FLOW3_IMAGINARY_NONE,
                    .origin = ctx->p_current_flow3_map,
                    .line = line
                };
                flow3_alternatives_add(&ep->alternatives, &a);

            }
            else
            {

                struct flow3_alternative a = {
                    .value_kind = FLOW3_VALUE_KIND_PTR,
                    .value = {.p = NULL},
                    .value_relation = FLOW3_RELATION_NOT_EQUAL,
                    .imaginary = FLOW3_IMAGINARY_NONE,
                    .origin = ctx->p_current_flow3_map,
                    .line = line
                };
                flow3_alternatives_add(&ep->alternatives, &a);

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
                        for (struct object* _Opt m = p_pointed->members.head; m; m = m->next)
                        {

                            struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, m);
                            if (e == NULL) throw;
                            flow3_alternatives_clear(&e->alternatives);
                            struct flow3_alternative a = {
                                .value_kind = FLOW3_VALUE_KIND_SIGNED,
                                .value = {.i = UNINITIALIZED_VALUE},
                                .value_relation = FLOW3_RELATION_UNINITIALIZED,
                                .imaginary = FLOW3_IMAGINARY_NONE,
                                .origin = ctx->p_current_flow3_map,
                                .line = line
                            };
                            flow3_alternatives_add(&e->alternatives, &a);

                        }
                        /* Parent node: NOT_APPLICABLE (aggregate, state lives in members). */
                        //flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                        //    p_pointed, NOT_APPLICABLE_VALUE, FLOW3_NOT_APPLICABLE, line);
                    }
                    else
                    {

                        struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, p_pointed);
                        if (e == NULL) throw;
                        flow3_alternatives_clear(&e->alternatives);
                        struct flow3_alternative a = {
                            .value_kind = FLOW3_VALUE_KIND_SIGNED,
                            .value = {.i = UNINITIALIZED_VALUE},
                            .value_relation = FLOW3_RELATION_UNINITIALIZED,
                            .imaginary = FLOW3_IMAGINARY_NONE,
                            .origin = ctx->p_current_flow3_map,
                            .line = line
                        };
                        flow3_alternatives_add(&e->alternatives, &a);

                    }
                }
                else if (depth < FLOW3_PARAMETER_OBJECT_INIT_MAX_DEPTH)
                {
                    /* Recurse with the pointee's ACTUAL type (not NULL). Passing
                   NULL here dropped the type, so the non-_Opt-pointer =>
                   non-null rule at the top of this function never fired for
                   the pointee: the *pp of a `T**` parameter (where *pp is a
                   non-_Opt `T*`) was left ANY = possibly-null, producing a
                   false "passing a possible null pointer '(*pp)' to
                   non-nullable pointer parameter" (parser.c:2184). With the
                   real type, a non-_Opt pointee pointer is seeded non-null. */
                    flow3_parameter_object_init_r(ctx, p_pointed, &p_pointed->type, line, depth + 1, force_opt || pointee_is_opt);
                }
                /* else: depth cap reached (see FLOW3_PARAMETER_OBJECT_INIT_MAX_DEPTH) --
               leave p_pointed's members in the ANY state make_object already
               gave them above, without recursing into their own pointer
               members. Prevents unbounded recursion on self-referential
               types (e.g. a linked-list node pointing to itself). */
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

        if (relation == FLOW3_RELATION_ANY &&
            p_type != NULL &&
            type_is_pointer(p_type) &&
            (type_is_opt(p_type, nullable_enabled) || force_opt))
        {
            /* Two child maps so alternatives from each arm have distinct origins. */
            struct flow3_map* p_null_map =
            flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, FLOW3_MAP_OPT_NULL);
            struct flow3_map* p_nonnull_map =
            flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, FLOW3_MAP_OPT_NONNULL);

            /* Allocate and initialise the arena object for the non-null arm. */
            struct object* _Opt p_pointed = flow3_allocated_object_arena_new(&ctx->allocated_object_arena);
            if (p_pointed != NULL)
            {
                struct type pointed_type = type_remove_pointer(p_type);
                make_object(&pointed_type, p_pointed, MAKE_STATE_ANY, ctx->ctx->options.target);
                type_destroy(&pointed_type);
            }

            /* --- pointer alternatives --- */
            struct flow3_key_alternatives* _Opt ep = flow3_map_find_add(ctx->p_current_flow3_map, p_object);
            if (ep == NULL) throw;
            flow3_alternatives_clear(&ep->alternatives);
            /* null arm */
            {
                struct flow3_alternative a = {
                    .value_kind = FLOW3_VALUE_KIND_PTR,
                    .value = {.p = NULL},
                    .value_relation = FLOW3_RELATION_EQUAL,
                    .imaginary = FLOW3_IMAGINARY_NONE,
                    .origin = p_null_map,
                    .line = line
                };
                flow3_alternatives_add(&ep->alternatives, &a);
            }
            /* non-null arm */
            if (p_pointed != NULL)
            {
                struct flow3_alternative a = {
                    .value_kind = FLOW3_VALUE_KIND_PTR,
                    .value = {.p = p_pointed},
                    .value_relation = FLOW3_RELATION_EQUAL,
                    .imaginary = FLOW3_IMAGINARY_NONE,
                    .origin = p_nonnull_map,
                    .line = line
                };
                flow3_alternatives_add(&ep->alternatives, &a);
            }
            else
            {
                struct flow3_alternative a = {
                    .value_kind = FLOW3_VALUE_KIND_PTR,
                    .value = {.p = NULL},
                    .value_relation = FLOW3_RELATION_NOT_EQUAL,
                    .imaginary = FLOW3_IMAGINARY_NONE,
                    .origin = p_nonnull_map,
                    .line = line
                };
                flow3_alternatives_add(&ep->alternatives, &a);
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

                struct flow3_key_alternatives* _Opt eo = flow3_map_find_add(ctx->p_current_flow3_map, p_pointed);
                if (eo == NULL) throw;
                flow3_alternatives_clear(&eo->alternatives);

                /* Non-null arm: ANY state, correlated with p_nonnull_map. */

                struct flow3_alternative a = {
                    .value_kind = FLOW3_VALUE_KIND_SIGNED,
                    .value = {.i = ANY_VALUE},
                    .value_relation = FLOW3_RELATION_ANY,
                    .imaginary = FLOW3_IMAGINARY_NONE,
                    .origin = p_nonnull_map,
                    .line = line
                };
                flow3_alternatives_add(&eo->alternatives, &a);

                /* Null arm: object does not exist when pointer is null. */
                flow3_alternatives_add_does_not_exist(&eo->alternatives, p_null_map, line);

                /* Deliberately NOT recursing into p_pointed's own members here
               (tried once, reverted): `struct X* _Opt p` only says p ITSELF
               may be null -- once narrowed non-null (`if (p) ...`), p_not_opt
               is governed by its own declared nullability same as always, see
               pattern2-member-through-narrowed-opt.c. Recursing into members
               here also raced against the eager non-opt-pointer branch's own
               recursion on self-referential types (struct token* _Opt next
               inside struct token) and corrupted arena/origin state, showing
               up as duplicated/misattributed diagnostics in an unrelated
               function (do-while-nested-if-unrelated-narrow.c). The _Opt
               position that DOES taint members is `_Opt struct X* p` (opt on
               the POINTEE, not the pointer) -- handled above via
               `pointee_is_opt` in the non-opt-pointer branch, since p itself
               is non-_Opt there. */
            }
        }
        else
        {
            /*
           Plain non-pointer type (or a pointer type but nullable
           checking is disabled, so no dedicated null/non-null tracking
           applies): no branch above wrote anything for this object.
           It is still a genuine value handed in by the caller (for a
           parameter) or produced by the callee (once
           EXPR_POSTFIX_FUNCTION_CALL is extended to seed non-pointer
           return values the same way) -- its concrete contents are
           unknown, but it must never be left UNINITIALIZED. Previously
           this whole branch wrote nothing at all, so any scalar reached
           this way (a plain int/enum/float parameter; a scalar member
           reached while seeding a pointer parameter's pointee; or a
           scalar member of a struct returned by value) silently kept
           whatever UNINITIALIZED state its declaration gave it, later
           misreported as "passing a possible uninitialized object" when
           read directly -- e.g. `int errcode = mkdir(to, 0700); if
           (errcode != 0) return errcode;` warned errcode was possibly
           uninitialized, and `struct X x = f(); use(x.a);` did too for
           every plain (non-pointer) member of a struct returned by
           value.
        */
            struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, p_object);
            if (e == NULL) throw;
            flow3_alternatives_clear(&e->alternatives);
            struct flow3_alternative a = {
                .value_kind = value.value_kind,
                .value = value.value,
                .value_relation = FLOW3_RELATION_ANY,
                .imaginary = FLOW3_IMAGINARY_NONE,
                .origin = ctx->p_current_flow3_map,
                .line = line
            };
            flow3_alternatives_add(&e->alternatives, &a);
        }
    }
    catch
    {
    }
}

/*
   After a braced initializer is evaluated, seed each leaf member that was set
   by a NON-CONSTANT initializer expression from that expression's flow3 value
   (its relation/alternatives). flow3_object_init only reads the parser
   object's CONSTANT value, so without this a member like `p.x` in
   `struct point p = { x, y };` stayed ANY even when `x` had a known relation
   (e.g. proven >= 0). Constant members are already correctly seeded, so leave
   them untouched. Mirrors the scalar init path (flow3_check_object_init_assigment).
*/
static void flow3_seed_aggregate_from_init_exprs(struct flow3_visit_ctx* ctx, struct object* p_object)
{
    if (p_object->members.head)
    {
        for (struct object* _Opt m = p_object->members.head; m != NULL; m = m->next)
            flow3_seed_aggregate_from_init_exprs(ctx, m);
        return;
    }

    /* Leaf member set by a non-constant initializer expression: copy the
       expression's flow3 state into the member (value relation for integers;
       initialized / null / non-null for plain pointers). Without this a plain
       pointer member set in a braced initializer -- e.g.
       `struct marker m = { .p_token_begin = p->first_token };` -- stayed
       "uninitialized" and was wrongly flagged when the aggregate was used.

       Owner members are included too: an owner member explicitly set in a
       braced initializer (e.g. `struct hash_item_set item = { .p_macro = macro };`)
       must be marked initialized (and its source moved), just like a plain
       pointer -- otherwise it was wrongly reported "possibly uninitialized"
       when the aggregate was used. Only members with an explicit initializer
       expression are touched. */
    const bool seedable_member =
    type_is_integer(&p_object->type) ||
    type_is_pointer(&p_object->type);

    if (p_object->p_init_expression != NULL &&
        seedable_member &&
        p_object->state != CONSTANT_VALUE_STATE_CONSTANT &&
        p_object->state != CONSTANT_VALUE_EQUAL)
    {
        flow3_check_object_init_assigment(ctx,
            p_object->p_init_expression,
            p_object,
            &p_object->p_init_expression->object,
            INIT_OBJ,
            false,
            false);
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
            flow3_check_static_ownership_assignment(ctx,
                &p_init_declarator->p_declarator->type, p_init_expr);
            flow3_check_object_init_assigment(ctx,
                p_init_declarator->initializer->assignment_expression,
                &p_init_declarator->p_declarator->object,
                &p_init_expr->object,
                INIT_OBJ,
                false,
                false
                );

            flow3_apply_alloc_contract_to_dest(ctx,
                &p_init_declarator->p_declarator->type,
                &p_init_declarator->p_declarator->object,
                p_init_expr);
            //  flow3_check_rhs_value(ctx, &p_init_declarator->p_declarator->type, p_init_expr);
            //flow3_assign_rhs_to_lhs(ctx, &p_init_declarator->p_declarator->object, p_init_expr);
        }
        else if (p_init_declarator->initializer->braced_initializer)
        {
            struct object* p_aggregate = &p_init_declarator->p_declarator->object;

            /* 1. Visit the whole braced initializer – evaluates all RHS expressions. */
            flow3_visit_bracket_initializer_list(ctx, p_init_declarator->initializer->braced_initializer);

            /* 2. Seed members set by non-constant initializers from those
                  expressions' flow3 values (constant members already seeded). */
            flow3_seed_aggregate_from_init_exprs(ctx, p_aggregate);

            /* 3. Zero‑initialise any scalar members not explicitly set. */
            if (type_is_struct_or_union(&p_aggregate->type))
            {
                //flow3_zero_initialize_uninitialized_members(ctx, p_aggregate, line);
            }
        }
    }
}

static void flow3_visit_init_declarator_list(struct flow3_visit_ctx* ctx, struct init_declarator_list* p_init_declarator_list);

static void flow3_visit_declaration_specifiers(struct flow3_visit_ctx* ctx, struct declaration_specifiers* p_declaration_specifiers, struct type* _Opt p_type);

static void flow3_visit_simple_declaration(struct flow3_visit_ctx* ctx, struct simple_declaration* p_simple_declaration)
{
    if (p_simple_declaration->p_declaration_specifiers)
    {
       flow3_visit_declaration_specifiers(ctx, p_simple_declaration->p_declaration_specifiers, NULL);
    }
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
        _Assert(p_selection_statement->condition->p_init_declarator->p_declarator->first_token_opt != NULL);
        hidden_expression.first_token = p_selection_statement->condition->p_init_declarator->p_declarator->first_token_opt;
        hidden_expression.last_token = hidden_expression.first_token;
        cond_pair = flow3_visit_full_expression(ctx, &hidden_expression);
    }

    _Assert(p_selection_statement->first_token->type == TK_KEYWORD_IF);

    if (ctx->p_current_flow3_map == NULL)
        return;

    struct flow3_map* p_before = ctx->p_current_flow3_map;

    /* Non-narrowing conditions return the identity pair (or none at all);
       force two distinct arm maps so branch-body writes cannot leak
       unconditionally into the state after the if. */
    const struct expression* _Opt p_if_cond =
    (p_selection_statement->condition && p_selection_statement->condition->expression)
    ? p_selection_statement->condition->expression
    : ((p_selection_statement->condition && p_selection_statement->condition->p_init_declarator)
        ? &hidden_expression
        : NULL);
    cond_pair = flow3_ensure_branch_pair(ctx, p_before, cond_pair, p_if_cond);

    /* If this exact predicate controlled an earlier branch (and its operands
       weren't written since), reuse that branch's id so values guarded by the
       predicate stay correlated across both branches. */
    if (cond_pair.p_true != NULL && cond_pair.p_false != NULL &&
        cond_pair.p_true != cond_pair.p_false &&
        cond_pair.p_true->branch_id != 0 &&
        p_selection_statement->condition != NULL)
    {
        const int fresh = cond_pair.p_true->branch_id;
        const int shared = flow3_predicate_shared_id(ctx, p_selection_statement->condition->expression, fresh);
        if (shared != fresh)
        {
            cond_pair.p_true->branch_id = shared;
            cond_pair.p_false->branch_id = shared;
        }
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

    if (ctx->p_current_flow3_map == NULL)
        return;

    struct flow3_map* p_before = ctx->p_current_flow3_map;

    /* throw paths are accumulated into this map */
    struct flow3_map* _Opt p_throw_join = flow3_map_arena_new(&ctx->flow3_map_arena, p_before, FLOW3_MAP_THROW_JOIN);
    if (p_throw_join == NULL)
        return; /* no map to work with */

    ctx->p_throw_join_map = p_throw_join;

    /* --- visit the try body --- */
    struct flow3_map* _Opt p_try_branch = flow3_map_arena_new(&ctx->flow3_map_arena, p_before, FLOW3_MAP_TRY_BRANCH);
    if (p_try_branch == NULL)
        return; /* no map to work with */

    ctx->p_current_flow3_map = p_try_branch;
    flow3_visit_secondary_block(ctx, p_try_statement->secondary_block);

    /*
       Restore the enclosing throw-join BEFORE visiting the catch body:
       a throw inside catch is a re-throw that must propagate to the
       *enclosing* try (or nowhere, if there isn't one), not recurse
       into this try's own throw join. That join only exists to collect
       throws out of the TRY body; it is fully populated by this point,
       and any statement in the catch body must resolve throw/goto/break
       targets against the context this whole try-statement was itself
       entered with.
    */
    ctx->p_throw_join_map = p_throw_join_map_old;

    const bool try_reached_the_end = !secondary_block_ends_with_jump(p_try_statement->secondary_block);
    const bool catch_reached_the_end = !secondary_block_ends_with_jump(p_try_statement->catch_secondary_block_opt);

    if (p_try_statement->catch_secondary_block_opt)
    {
        /* --- visit the catch body, starting from the throw-join map --- */
        struct flow3_map* _Opt p_catch_branch = flow3_map_arena_new(&ctx->flow3_map_arena, p_throw_join, FLOW3_MAP_CATCH_BRANCH);
        ctx->p_current_flow3_map = p_catch_branch;

        flow3_visit_secondary_block(ctx, p_try_statement->catch_secondary_block_opt);

        /* flow3_map_arena_new is _Opt: a null p_catch_branch means the arena
           had no map to give, so there is no catch outcome to merge. Read it
           as an empty catch arm -- the arms below that would have used it are
           skipped, leaving the try arm to stand on its own, which is what the
           "catch jumps away" case already does. */
        if (try_reached_the_end && catch_reached_the_end && p_catch_branch != NULL)
        {
            flow3_map_merge_a_b(p_before, p_try_branch, p_catch_branch);
        }
        else if (try_reached_the_end)
        {
            /* catch jumps away (or has no map); only the try-branch outcome matters */
            flow3_map_merge_a_b(p_before, p_try_branch, p_try_branch);
        }
        else if (catch_reached_the_end && p_catch_branch != NULL)
        {
            /* try jumps away; only the catch-branch outcome matters */
            flow3_map_merge_a_b(p_before, p_catch_branch, p_catch_branch);
        }
        /* else: both jump — p_before stays as-is (unreachable continuation) */
    }
    else
    {
        /* No catch block: `throw` still exits the try body immediately (it
           just has nowhere of its own to land), so the state AT THE THROW
           POINT is exactly as live after the try statement as the state
           from reaching the end of the try body normally -- both are
           possible outcomes the code after the try must account for. Only
           merging p_try_branch here (as if throw simply didn't happen)
           silently dropped whatever the throw path had done up to that
           point, e.g. `int* _Owner p2 = p; if (c) throw; p = 0;` left p
           looking unconditionally null afterward, when the throw path
           actually leaves it MOVED (into p2), never reset. User-reported. */
        const struct flow3_map* arms[2];
        int num_arms = 0;
        if (try_reached_the_end)
        {
            arms[num_arms++] = p_try_branch;
        }
        if (flow3_map_arm_has_entries(p_throw_join, p_before))
        {
            arms[num_arms++] = p_throw_join;
        }
        if (num_arms > 0)
        {
            flow3_map_merge_arms(p_before, arms, num_arms);
        }
    }

    ctx->p_current_flow3_map = p_before;
    /* ctx->p_throw_join_map was already restored before the catch body
       was visited, above. */
}

static void flow3_visit_switch_statement(struct flow3_visit_ctx* ctx, struct selection_statement* p_selection_statement)
{
    if (ctx->p_current_flow3_map == NULL)
        return;

    struct flow3_map* p_before = ctx->p_current_flow3_map;
    struct flow3_map* _Opt p_body = flow3_map_arena_new(&ctx->flow3_map_arena, p_before, FLOW3_MAP_SWITCH_BODY);
    struct flow3_map* _Opt p_break_join = flow3_map_arena_new(&ctx->flow3_map_arena, p_before, FLOW3_MAP_BREAK_JOIN);
    if (p_body == NULL || p_break_join == NULL)
        return; /* no map to work with */

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

    {
        const struct flow3_map* arms[2];
        int num_arms = 0;
        if (reached_the_end)
        {
            arms[num_arms++] = ctx->p_current_flow3_map;
        }
        if (flow3_map_arm_has_entries(p_break_join, p_before))
        {
            arms[num_arms++] = p_break_join;
        }
        if (num_arms > 0)
        {
            flow3_map_merge_arms(p_before, arms, num_arms);
        }
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
        _Assert(false);
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
    _Assert(p_initializer->initializer != NULL);
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
   True when `m` is `ancestor` or a descendant of it (walking the
   flow3_map parent chain). Used to check whether a fact recorded with
   origin `ancestor` is actually visible/applicable from the branch
   that produced `m` -- i.e. whether `ancestor` lies on the path from
   the root down to `m`, as opposed to being an unrelated sibling
   branch's own map.
*/
static bool flow3_map_is_ancestor_or_self(const struct flow3_map* _Opt ancestor, const struct flow3_map* _Opt m)
{
    if (ancestor == NULL)
        return true; /* no filter: everything is visible */

    for (const struct flow3_map* _Opt cur = m; cur; cur = cur->p_parent_map)
    {
        if (cur == ancestor)
            return true;
    }
    return false;
}

/*
  Check the state of some reachable object,
  struct X * f(){
      return p;
  }
  p2 = p1;

  p_origin_filter, when non-NULL, restricts which of p_object_src's OWN
  alternatives are considered to those whose ->origin is an ancestor-or-
  self of p_origin_filter (see flow3_map_is_ancestor_or_self). This
  matters when p_object_src is the POINTEE of one specific alternative
  of some pointer p (the caller passes that alternative's ->origin as
  the filter): after a branch merge, the pointee's own tracked entry can
  legitimately hold alternatives from OTHER branches too (e.g. "freed
  and ended" from a catch arm that also reset p to null) -- alternatives
  that never coexist with THIS particular "p points at the pointee"
  alternative (which only arose on a branch where the pointee was never
  touched). Without the filter, an unrelated branch's "ended" fact leaks
  into a dereference that is only ever reached along a branch where the
  object is still perfectly alive. See
  owner-reassigned-null-in-catch-false-positive.c for a worked example
  (found dogfooding flow3 on cake's own codegen.c, a similar
  free()-then-reset-to-null-in-a-catch-block shape).
*/
/* Does this object currently have any initialized flow state? (used to decide
   whether a union -- whose members share storage -- counts as initialized). */
static bool flow3_object_has_initialized_state(struct flow3_visit_ctx* ctx, const struct object* obj)
{
    const struct flow3_key_alternatives* _Opt e = flow3_map_search_up(ctx->p_current_flow3_map, obj);
    for (int i = 0; e != NULL && i < e->alternatives.size; i++)
    {
        if (e->alternatives.data[i]->value_relation != FLOW3_RELATION_UNINITIALIZED)
            return true;
    }
    /* A member that is itself an aggregate has no direct entry; recurse. */
    for (const struct object* _Opt m = obj->members.head; m != NULL; m = m->next)
    {
        if (flow3_object_has_initialized_state(ctx, m))
            return true;
    }
    return false;
}

/* Is p_union a union with at least one initialized member? Then the shared
   storage is initialized and no member should be reported uninitialized. */
static bool flow3_union_is_initialized(struct flow3_visit_ctx* ctx, const struct object* p_union)
{
    if (!type_is_union(&p_union->type))
        return false;
    for (const struct object* _Opt m = p_union->members.head; m != NULL; m = m->next)
    {
        if (flow3_object_has_initialized_state(ctx, m))
            return true;
    }
    return false;
}

enum flow3_leaf_state
{
    FLOW3_LEAF_UNINITIALIZED,
    FLOW3_LEAF_MOVED,
    FLOW3_LEAF_ENDED,
};

/*
   True when EVERY leaf reachable from p_obj is in `state`, with *p_line set to
   the first such leaf's line.

   Used to collapse a per-leaf report into a single one naming the aggregate.
   Two shapes hit this hard when dogfooding flow3 on cake's own sources:

     - `char buffer[20]` passed to a non-optional parameter produced 20
       identical "possible uninitialized object" diagnostics, one per element
       (samples/flow3/array-161.c);
     - `struct type a = pa->enum_specifier->integer_type;` moves the whole
       struct, and a later use reported "is moved" once per member -- 21 of
       them on type.c:3302 alone.

   Besides being noise, the per-leaf form is unsuppressable: diagnostic()
   flushes the queue past 5 pending entries (parser.c), so a `//lint` can never
   match more than a couple of them.

   "all" for UNINITIALIZED and MOVED: those states really can apply to just
   some members -- a half-filled struct, or a single owner member moved out of
   one -- and naming the affected members is the more useful report.

   "any" for ENDED, via require_all. A lifetime does not end member by member:
   the object is consumed as a whole (passed to an _Owner or _Dtor parameter)
   and flow3_map_set_object_lifetime_ended stamps every leaf in one go. So one
   ENDED leaf means the object ended, and the aggregate is the honest name for
   it. Requiring ALL leaves made the collapse fail whenever a single leaf had no
   map entry -- and a leaf that was never touched has none. `struct expression`
   nests `struct type`, `struct object`, and two lists, so some deep leaf always
   qualifies: one consumed expression at expressions.c:2758 reported 222 times,
   once per member, instead of once.
*/
static bool flow3_object_leaves_in_state_2(struct flow3_visit_ctx* ctx,
    const struct object* p_obj,
    enum flow3_leaf_state state,
    const struct flow3_map* p_origin_filter,
    const struct flow3_map* _Opt p_origin_filter2,
    bool require_all,
    int* p_line);

static bool flow3_object_leaves_in_state(struct flow3_visit_ctx* ctx,
    const struct object* p_obj,
    enum flow3_leaf_state state,
    const struct flow3_map* p_origin_filter,
    bool require_all,
    int* p_line)
{
    return flow3_object_leaves_in_state_2(ctx, p_obj, state, p_origin_filter, NULL, require_all, p_line);
}

/*
   Same as flow3_object_leaves_in_state, but accepts a SECOND, independent
   origin filter: a leaf's fact is accepted if its origin is an
   ancestor-or-self of EITHER filter, not just the first.

   Why two: for the `->` operator's lifetime-ended check, neither filter
   alone is correct for every shape that actually occurs.

     - dangling-pointer-to-local.c needs the POINTER ALTERNATIVE's own
       origin: `if (c) { struct X x={}; p=&x; } p->i=1;` -- after the merge,
       p's "points at x" alternative still carries the if-branch's origin,
       and x's "ended" fact (from the if-branch's own scope exit) shares
       that same origin. Checking against ctx->p_current_flow3_map (the
       POST-merge map) fails here: the if-branch is not ctx->p_current_flow3_map's
       ancestor once merged, only a sibling of what fed into it.

     - the aliased-owner case needs ctx->p_current_flow3_map (the CURRENT
       map, not yet merged with anything): `struct X* _Opt p = pX; free(pY);
       p->pY->i = 1;` (pX->pY == pY) -- .i's ended fact carries the CURRENT
       branch's origin (from wherever free() ran), but the POINTER
       alternative being dereferenced (p->pY, i.e. pX->pY's own tracked
       value) carries pY's origin from when `pX->pY = pY` was assigned,
       much earlier and unrelated to where free() later ran. Checking
       against the pointer alternative's own origin fails here: that origin
       predates and has no ancestor relationship with the branch the ended
       fact was recorded in, even though both are simultaneously active
       right now -- confirmed via static_debug(0), which is what showed the
       two origins as unrelated siblings from each other's perspective.

   The two false-positive regression tests that motivated the ORIGINAL
   single-filter check (owner-reassigned-null-in-catch-false-positive.c,
   deref-after-catch-reset-false-positive.c) both involve a MERGE where the
   rejected fact's origin is a sibling branch that was never open at the
   same time as the current path -- neither the pointer alternative's own
   origin nor ctx->p_current_flow3_map is its ancestor in those cases, so
   OR-ing the two extra options in does not resurrect either false
   positive; both still pass. User-reported (aliased-owner-lifetime-not-
   tracked).
*/
static bool flow3_object_leaves_in_state_2(struct flow3_visit_ctx* ctx,
    const struct object* p_obj,
    enum flow3_leaf_state state,
    const struct flow3_map* p_origin_filter,
    const struct flow3_map* _Opt p_origin_filter2,
    bool require_all,
    int* p_line)
{
    if (p_obj->members.head)
    {
        for (const struct object* _Opt m = p_obj->members.head; m; m = m->next)
        {
            const bool hit = flow3_object_leaves_in_state_2(ctx, m, state, p_origin_filter, p_origin_filter2, require_all, p_line);
            if (require_all)
            {
                if (!hit) return false;
            }
            else if (hit)
                return true;
        }
        return require_all;
    }

    const struct flow3_key_alternatives* _Opt e =
    flow3_map_search_up(ctx->p_current_flow3_map, p_obj);

    if (e == NULL)
        return false;

    for (int i = 0; i < e->alternatives.size; i++)
    {
        const struct flow3_alternative* a = e->alternatives.data[i];

        /* Same branch scoping the per-alternative loop applies. Without it a
           fact recorded on one arm leaks onto another: `p = tail(p)` inside a
           loop ends the pre-loop object, and the zero-iteration arm -- whose
           alternative still names that object, never having consumed it -- was
           reported as a use-after-end (samples/flow3/self-consuming-reassignment.c). */
        if (!flow3_map_is_ancestor_or_self(a->origin, p_origin_filter) &&
            !(p_origin_filter2 != NULL && flow3_map_is_ancestor_or_self(a->origin, p_origin_filter2)))
            continue;

        const bool match =
        (state == FLOW3_LEAF_UNINITIALIZED)
        ? (a->imaginary != FLOW3_IMAGINARY_ENDED &&
            a->value_relation == FLOW3_RELATION_UNINITIALIZED)
        : (state == FLOW3_LEAF_MOVED)
        ? (a->imaginary == FLOW3_IMAGINARY_MOVED)
        : (a->imaginary == FLOW3_IMAGINARY_ENDED);

        if (match)
        {
            if (*p_line == 0)
                *p_line = a->line;
            return true;
        }
    }
    return false;
}

/*
   Findings already reported while checking the assignment currently in
   progress, so one fact is reported once however many paths reach it.

   flow3_check_object_init_assigment loops over the source's alternatives and
   recurses into the pointee's members for each one. When several alternatives
   share a pointee -- the usual case for a function with one `return` and
   several ways to reach it -- every member finding is rediscovered per
   alternative and was reported per alternative: expressions.c:4300 emitted
   "'p_expression_node->first_token' may be null ... (see line 3183)" seven
   times, once per route through the `'&' || '*' || '+' || '-' || '~' || '!'`
   chain, all identical. The existing `nullable_reported` flags only cover the
   object being assigned, not these recursive member checks.

   Keyed on line as well as object, so genuinely distinct origins (a member
   left null by the calloc at 3142 vs. the one at 3183) still report
   separately -- only exact repeats are dropped. Reset per assignment, not per
   function: the same member being null at two different statements is two
   findings.

   File scope rather than a member of struct flow3_visit_ctx: this is private
   bookkeeping for one check in this file, and nothing outside it -- nothing
   that merely includes flow3.h -- has any use for it. Safe because flow3
   analysis is single-threaded and non-reentrant, like the visit itself.

   Terminated by the first entry with a NULL p_object rather than by a
   separate count: an entry is only ever meaningful when it names an object,
   so the array carries its own end marker and there is no second variable
   that can drift out of step with it.
*/
static struct
{
    const struct object* _Opt p_object; /* NULL marks the end of the used entries */
    int line;
    int diagnostic_id;
} s_reported_findings[256];

static void flow3_reported_findings_clear(void)
{
    s_reported_findings[0].p_object = NULL;
}

/*
   True when (p_object, line, diagnostic_id) has already been reported for the
   assignment being checked; records it as reported otherwise.

   Fails open: once the table is full every finding is reported again, because
   silently dropping a real one is worse than printing a duplicate.
*/
static bool flow3_finding_already_reported(const struct object* _Opt p_object, int line, int diagnostic_id)
{
    const int max = (int)(sizeof(s_reported_findings) / sizeof(s_reported_findings[0]));

    int i = 0;
    while (i < max && s_reported_findings[i].p_object != NULL)
    {
        if (s_reported_findings[i].p_object == p_object &&
            s_reported_findings[i].line == line &&
            s_reported_findings[i].diagnostic_id == diagnostic_id)
        {
            return true;
        }
        i++;
    }

    /* Stop one short of the end so slot max-1 stays NULL and keeps
       terminating the scan above. */
    if (i < max - 1)
    {
        s_reported_findings[i].p_object = p_object;
        s_reported_findings[i].line = line;
        s_reported_findings[i].diagnostic_id = diagnostic_id;
        s_reported_findings[i + 1].p_object = NULL;
    }

    return false;
}

static void flow3_check_object_access(struct flow3_visit_ctx* ctx,
    const char* parent_expression_str,
    struct expression* p_expression,
    const struct object* p_object_src,
    bool check_unitialized,
    const struct flow3_map* p_origin_filter,
    bool dest_is_dtor,
    const struct type* _Opt p_dest_governing_type,
    /* false once an ancestor aggregate has already been reported "is moved",
       so the leaves below it stay silent. Callers outside the recursion pass
       true. */
    bool check_moved,
    /*
       True once we are inside an ARRAY's elements, which suppresses the
       "possible null pointer ... to non-nullable parameter" report below.

       TEMPORARY, and deliberately blunt. The motivating shape is a fixed
       array used only up to a companion count:

           struct flow3_label_state labels[100];   // flow3.h
           int labels_size;                        // only [0, labels_size) live

       `struct flow3_visit_ctx ctx4 = {...}` zeroes all 100 elements, so every
       element's non-_Opt pointer members really are null -- 200 diagnostics
       for one argument at parser.c:12365, all technically true and none
       actionable, because the tail elements are never read.

       Deciding this properly means relating an array to the count that bounds
       its live prefix, which flow3 does not model. Until it does, elements are
       not reported. Note this only affects members reached by walking INTO an
       aggregate being passed; a direct `arr[i]` dereference goes through the
       subscript path and is still checked, as is a zeroed non-_Opt member of a
       plain struct (samples/flow3/zero-is-not-ok.c).
    */
    bool in_array_element,
    /* false once an ancestor aggregate has already been reported "lifetime has
       ended", so its members stay silent. Callers outside the recursion pass
       true. */
    bool check_ended,
    /* True only on the two external entry points (the "does this pointer's
       pointee...") calls, where parent_expression_str is a bare pointer
       expression like "p" that has not been dereferenced in the string yet.
       Used to spell this object's own name as "*p" (bare leaf use) or
       "p->member" (member-loop use) -- source-code-accurate -- instead of
       the previous "(*p)"/"(*p).member", which read like generated code
       rather than something a person would write. Always false for every
       recursive call this function makes to itself: once a member has been
       appended (via "->" or "."), or a REF has been followed (which changes
       WHAT object is being named but not HOW its text was built), the
       accumulated parent_expression_str is already a complete, correctly
       punctuated expression on its own. User-reported. */
    bool base_is_ptr)
{
    struct marker marker = expression_to_marker(p_expression);

    /* Bare display name for this object itself (used wherever the leaf
       diagnostics below print parent_expression_str with nothing appended):
       "*p" when this is the root pointee of an as-yet-undereferenced pointer
       expression, otherwise parent_expression_str unchanged (it is already
       a complete expression, e.g. "p->text" or "s.a"). */
    struct osstream bare_name_ss = { 0 };
    ss_fprintf(&bare_name_ss, base_is_ptr ? "*%s" : "%s", parent_expression_str);
    const char* bare_name = bare_name_ss.c_str ? bare_name_ss.c_str : parent_expression_str;

    if (p_object_src->members.head)
    {
        /* A union's members share storage: if ANY member is initialized (e.g.
           after `struct S s = { 0 };` sets the first union member) the whole
           union is initialized, so the sibling members must not be reported as
           "possibly uninitialized". Treating union members like independent
           struct members warned on every member other than the one written. */
        bool members_check_uninit = check_unitialized;
        if (members_check_uninit && flow3_union_is_initialized(ctx, p_object_src))
            members_check_uninit = false;

        /* Whole aggregate uninitialized, or wholly moved: report it once, by
           the aggregate's own name, instead of once per leaf. See
           flow3_object_leaves_in_state. */
        bool members_check_moved = check_moved;

        if (members_check_uninit)
        {
            int uninit_line = 0;
            if (flow3_object_leaves_in_state(ctx, p_object_src, FLOW3_LEAF_UNINITIALIZED, p_origin_filter, true, &uninit_line) &&
                uninit_line != 0)
            {
                /* At the aggregate level parent_expression_str is empty (the
                   per-leaf reports built their names by appending the member
                   designator to it), so recover the argument's own text. */
                struct osstream ss = { 0 };
                if (parent_expression_str != NULL && parent_expression_str[0] != '\0')
                    ss_fprintf(&ss, "%s", bare_name);
                else
                    flow3_expression_to_string(p_expression, &ss);

                diagnostic(W_FLOW_UNINITIALIZED,
                    ctx->ctx, NULL, &marker,
                    "passing a possible uninitialized object '%s' (see line %d)",
                    ss.c_str ? ss.c_str : "", uninit_line);
                ss_close(&ss);
                members_check_uninit = false;
            }
        }

        /* Whole aggregate's lifetime ended: one report for the object rather
           than one per member. Deleting a struct ends every member at once, so
           the per-member form said the same thing 66 times for one
           `expression_delete(...)` (expressions.c:5667). */
        bool members_check_ended = check_ended;
        if (members_check_ended)
        {
            int ended_line = 0;
            if (flow3_object_leaves_in_state_2(ctx, p_object_src, FLOW3_LEAF_ENDED, p_origin_filter, ctx->p_current_flow3_map, false, &ended_line) &&
                ended_line != 0)
            {
                struct osstream ss = { 0 };
                if (parent_expression_str != NULL && parent_expression_str[0] != '\0')
                    ss_fprintf(&ss, "%s", bare_name);
                else
                    flow3_expression_to_string(p_expression, &ss);

                diagnostic(W_FLOW_LIFETIME_ENDED,
                    ctx->ctx, NULL, &marker,
                    "object '%s' lifetime has ended (see line %d)",
                    ss.c_str ? ss.c_str : "", ended_line);
                ss_close(&ss);
                members_check_ended = false;
            }
        }

        if (members_check_moved)
        {
            int moved_line = 0;
            if (flow3_object_leaves_in_state(ctx, p_object_src, FLOW3_LEAF_MOVED, p_origin_filter, true, &moved_line) &&
                moved_line != 0)
            {
                struct osstream ss = { 0 };
                if (parent_expression_str != NULL && parent_expression_str[0] != '\0')
                    ss_fprintf(&ss, "%s", bare_name);
                else
                    flow3_expression_to_string(p_expression, &ss);

                diagnostic(W_FLOW_MOVED,
                    ctx->ctx, NULL, &marker,
                    "object '%s' is moved (see line %d)",
                    ss.c_str ? ss.c_str : "", moved_line);
                ss_close(&ss);
                members_check_moved = false;
            }
        }

        /* When the destination is an array parameter, its elements are governed
           by the parameter's ELEMENT type (indexed) -- exactly as a struct's
           members are governed by name. Without this, an argument array of
           non-_Opt elements (e.g. `const char* argv[100] = {0}`) passed to a
           `const char* _Opt argv[]` parameter warned once per element even though
           the parameter accepts null elements (compile.c:1006). Derive the
           element governing type once and hand it to each element's check. */
        struct type dest_elem_type = { 0 };
        const struct type* _Opt p_member_governing = NULL;
        if (p_dest_governing_type != NULL && type_is_array(p_dest_governing_type))
        {
            dest_elem_type = get_array_item_type(p_dest_governing_type);
            p_member_governing = &dest_elem_type;
        }

        /* Once inside an array, stay inside it: the elements are usually
           structs, and it is their leaves that would report. */
        const bool members_in_array_element =
        in_array_element || type_is_array(&p_object_src->type);

        struct object* _Opt member = p_object_src->members.head;
        while (member)
        {
            struct osstream ss = { 0 };
            const char* only_member_name = flow3_get_only_member_name(member->member_designator);
            /* base_is_ptr: parent_expression_str is a bare, as-yet-undereferenced
               pointer expression ("p") -- spell this member access as "p->text"
               (source-code-accurate), not the old "(*p).text". Otherwise
               parent_expression_str is already a complete expression (a
               struct value, e.g. "s" or "p->in"), so a plain "." continues
               it as before: "s.a", "p->in.a". Either way, the recursive call
               below is never itself a fresh pointer dereference, so it always
               passes base_is_ptr = false. */
            if (base_is_ptr)
                ss_fprintf(&ss, "%s->%s", parent_expression_str,
                    only_member_name[0] == '.' ? only_member_name + 1 : only_member_name);
            else
                ss_fprintf(&ss, "%s%s", parent_expression_str, only_member_name);
            flow3_check_object_access(ctx, ss.c_str, p_expression, member, members_check_uninit, p_origin_filter, dest_is_dtor, p_member_governing, members_check_moved, members_in_array_element, members_check_ended, false);
            member = member->next;
            ss_close(&ss);
        }

        if (p_member_governing != NULL)
            type_destroy(&dest_elem_type);
    }

    const struct flow3_key_alternatives* _Opt p_source_key_alternatives = flow3_map_search_up(ctx->p_current_flow3_map, p_object_src);
    if (p_source_key_alternatives == NULL)
    {
        ss_close(&bare_name_ss);
        return;
    }

    /* An object can carry more than one alternative that independently
       "can be zero" (e.g. an _Opt pointer's null arm plus a second,
       separately-derived possibly-zero fact from a merge). Without this
       guard the loop below reported the same "possible null pointer ...
       (see line N)" diagnostic once per such alternative instead of once
       per call-site -- seen dogfooding flow3 on cake's own expressions.c
       (unary_expression's `p_type` reported twice at several call sites,
       e.g. expressions.c:4149). Scoped per invocation (not static/global):
       each recursive call below (for a member or a REF-resolved object)
       gets its own object and its own fresh guard. */
    bool nullable_reported = false;

    for (int ri = 0; ri < p_source_key_alternatives->alternatives.size; ri++)
    {
        struct flow3_alternative* p_alternative = p_source_key_alternatives->alternatives.data[ri];
        //flow3_alternative_print(p_alternative);
        //printf("\n");

        if (!flow3_map_is_ancestor_or_self(p_alternative->origin, p_origin_filter))
        {
            /* This alternative was established on a branch that isn't
               an ancestor of the branch we're checking from -- it
               belongs to a sibling arm and never actually coexists with
               the specific pointer alternative that led here. Skip it;
               see the function comment above. */
            continue;
        }

        if (p_alternative->imaginary == FLOW3_IMAGINARY_ENDED && check_ended)
        {
            diagnostic(W_FLOW_LIFETIME_ENDED,
                ctx->ctx, NULL, &marker,
                "object '%s' lifetime has ended (see line %d)",
                bare_name, p_alternative->line);

            continue; //no need to print other errors
        }

        if (p_alternative->value_kind == FLOW3_VALUE_KIND_REF)
        {
            if (p_alternative->value.p != p_object_src)
            {
                flow3_check_object_access(ctx,
                    parent_expression_str,
                    p_expression,
                    p_alternative->value.p, check_unitialized, p_origin_filter, dest_is_dtor,
                    p_dest_governing_type, check_moved, in_array_element, check_ended, base_is_ptr);

                continue;
            }
        }

        /* Skipped for a _Dtor destination: a destructor must accept a
           partially-created object, so a null member is allowed there. */
        /* The pointer's nullability is governed by the destination (parameter)
                 type when one was threaded in (array-element case above); otherwise by
                 the object's own declared type. */
        const struct type* p_null_type =
        p_dest_governing_type != NULL ? p_dest_governing_type : &p_object_src->type;
        if (!dest_is_dtor &&
            type_is_pointer(&p_object_src->type) &&
            !type_is_opt(p_null_type, ctx->ctx->options.null_checks_enabled) &&
            flow3_alternative_can_be_zero(p_alternative) &&
            !nullable_reported &&
            !in_array_element &&
            /* Same fact, reached by another path, already reported for this
               assignment -- see reported_findings. Evaluated last so it only
               records findings that would actually have been printed. */
            !flow3_finding_already_reported(p_object_src, p_alternative->line,
            W_FLOW_NULLABLE_TO_NON_NULLABLE))
        {
            nullable_reported = true;
            /* Two different facts get reported through this one call,
               distinguished by whether a destination type was threaded in:

               - p_dest_governing_type != NULL: bare_name/%s is a value
                 being read INTO something else (an array element read
                 against the destination's element type) that doesn't
                 allow null -- %s's own declared type may well be _Opt.
                 Saying "%s is declared non-nullable" here would be
                 outright wrong.
               - p_dest_governing_type == NULL: p_null_type falls back to
                 &p_object_src->type, i.e. bare_name/%s's OWN declared
                 type is what's being checked -- this is the recursive
                 struct-member case (e.g. `attribute_list(ctx,
                 p_attribute_specifier)` where p_attribute_specifier->
                 last_token, itself declared non-_Opt, is still NULL right
                 after calloc). Here %s genuinely IS declared non-nullable.
               User-reported: the single unconditional wording tried here
               previously got this backwards for parser.c:5214
               (`p_token = ctx->current;` -- ctx->current is legitimately
               _Opt; it's p_token, the destination, that disallows null). */
            bool reported = 0;
            if (p_dest_governing_type != NULL)
            {
                reported = diagnostic(W_FLOW_NULLABLE_TO_NON_NULLABLE,
                    ctx->ctx, NULL, &marker,
                    "'%s' may be null, but the destination does not allow null",
                    bare_name);
            }
            else
            {
                reported = diagnostic(W_FLOW_NULLABLE_TO_NON_NULLABLE,
                    ctx->ctx, NULL, &marker,
                    "'%s' may be null, but is declared non-nullable",
                    bare_name);
            }

            /* Where the null state came from, as its own note rather than
               a "(see line N)" tail on the warning. */
            if (reported)
                flow3_diagnose_state_origin(ctx, p_alternative, &marker);

            /* "may be null" is exactly the warning a reader tends to
               believe is impossible, so show how control reached the line
               being reported: the walk from the map we are in now up to
               the root. Not p_alternative->origin -- that is the map where
               the null was recorded, a different chain, and walking to root
               from there names decisions the reader isn't standing in. */
            if (reported)
                flow3_diagnose_map_path(ctx, ctx->p_current_flow3_map);
        }

        if (p_alternative->imaginary != FLOW3_IMAGINARY_ENDED && p_alternative->value_relation == FLOW3_RELATION_UNINITIALIZED)
        {
            if (check_unitialized)
            {

                /* W_FLOW_UNINITIALIZED (30), not the nullability id: reading
                   something never written is a different analysis from passing
                   a null pointer, and 35 cannot distinguish them for `//lint`
                   or -W. flow1 already reports this shape as 30. */
                diagnostic(W_FLOW_UNINITIALIZED,
                    ctx->ctx, NULL, &marker,
                    "passing a possible uninitialized object '%s' (see line %d)",
                    bare_name, p_alternative->line);
            }
        }

        if (p_alternative->imaginary == FLOW3_IMAGINARY_MOVED && check_moved)
        {
            /* W_FLOW_MOVED (32): use-after-move is its own condition, and 31
               is already carrying "lifetime has ended". flow1 uses 32 here. */
            diagnostic(W_FLOW_MOVED,
                ctx->ctx, NULL, &marker,
                "object '%s' is moved (see line %d)",
                bare_name, p_alternative->line);
        }
    }
    ss_close(&bare_name_ss);
}

/*
   type_is_pointed_const/ctor/dtor/clear/uninit all start with
   "if (!type_is_pointer(p_type)) return false;" -- they assume p_type IS a
   pointer and inspect its pointee (p_type->next).

   An array PARAMETER (e.g. `char errmsg[]`) is deliberately kept as
   TYPE_CATEGORY_ARRAY by this analyser rather than adjusted to a pointer
   (see the type_is_array discussion elsewhere in this file), so
   type_is_pointer() is false for it and every type_is_pointed_* call above
   silently answers "false" for an array destination -- even though, per
   C's parameter-adjustment rule, `char errmsg[]` and `char *errmsg` name
   the exact same parameter and must be analysed identically.

   These wrappers restore that equivalence: for an array destination they
   check the array's element type directly (the array analogue of
   "the pointee"); for anything else they defer to the normal
   type_is_pointed_* helpers unchanged. */
static bool flow3_dest_pointee_is_const(const struct type* p_type)
{
    if (type_is_array(p_type))
    {
        struct type item = get_array_item_type(p_type);
        bool r = type_is_const(&item);
        type_destroy(&item);
        return r;
    }
    return type_is_pointed_const(p_type);
}

static bool flow3_dest_pointee_is_ctor(const struct type* p_type)
{
    if (type_is_array(p_type))
    {
        struct type item = get_array_item_type(p_type);
        bool r = type_is_ctor(&item);
        type_destroy(&item);
        return r;
    }
    return type_is_pointed_ctor(p_type);
}

static bool flow3_dest_pointee_is_dtor(const struct type* p_type)
{
    if (type_is_array(p_type))
    {
        struct type item = get_array_item_type(p_type);
        bool r = type_is_dtor(&item);
        type_destroy(&item);
        return r;
    }
    return type_is_pointed_dtor(p_type);
}

static bool flow3_dest_pointee_is_clear(const struct type* p_type)
{
    if (type_is_array(p_type))
    {
        struct type item = get_array_item_type(p_type);
        bool r = type_is_clear(&item);
        type_destroy(&item);
        return r;
    }
    return type_is_pointed_clear(p_type);
}

/* An _Owner member is effectively a plain VIEW when the containing object is
   _View: `_View struct X x` makes every _Owner member of x a view -- assigning
   into it does not move the source, and it is not "owner not moved" at exit.
   The _View qualifier sits on the aggregate while members keep their own _Owner
   type, so detect it by walking the object's parent chain. */
static bool flow3_object_under_view(const struct object* obj)
{
    for (const struct object* _Opt p = obj->parent; p != NULL; p = p->parent)
    {
        if (type_is_view(&p->type))
            return true;
    }
    return false;
}

/* A pointee's members exist structurally (fabricated on first arrow-access,
   see the ON-DEMAND pointee fabrication comment near
   EXPR_POSTFIX_ARROW/EXPR_MEMBER handling) but each member's own flow state
   is seeded lazily, on that member's own first read, by
   flow3_seed_member_default. A member the source code never happens to read
   -- e.g. the one member a caller FORGOT to free before erasing the whole
   struct to void* -- is exactly the common shape of this bug: nothing about
   it was ever an accident of not reading it, it is simply the member that
   was skipped. Force every leaf member's flow state to exist before a
   leak check walks them, the same way it would already exist had the code
   happened to read that member first.

   Restricted to _Opt pointer members. flow3_seed_member_default seeds a
   non-_Opt pointer member as a flat, unconditional "definitely non-null"
   fact (NOT_EQUAL null, imaginary NONE) -- sound for an ordinary READ,
   where reaching the member at all already implies the struct was validly
   constructed, but not sound here: we are not reading it, we are asking
   "was this ever given a value on THIS path", and a branch that never
   touched the member (e.g. the impossible-in-practice but still-modeled
   `if (p)` false arm on a non-_Opt owner `p`) has no such guarantee. Forcing
   "definitely non-null, unmoved" onto it manufactured a leak that was never
   real: owner-resource-059.c's `if (p) { p->name = ...; free(p->name); }
   free(p);` reported ".name not moved" on the branch that provably never ran
   `p->name = ...` at all. An _Opt member's seeding is honest uncertainty
   (a correlated null/non-null pair, only the latter arm flagged) rather
   than a manufactured certainty, so it stays safe to force. */
static void flow3_seed_all_members_default(struct flow3_visit_ctx* ctx, struct object* p_obj, int line)
{
    if (p_obj->members.head)
    {
        for (struct object* _Opt member = p_obj->members.head; member; member = member->next)
        {
            flow3_seed_all_members_default(ctx, member, line);
        }
        return;
    }
    if (type_is_pointer(&p_obj->type) &&
        !type_is_opt(&p_obj->type, ctx->ctx->options.null_checks_enabled))
    {
        return;
    }
    flow3_seed_member_default(ctx, p_obj, line);
}

/*
   True if p_obj IS ctx's pending pre-reported object, or is a member of it
   (walking p_obj's own `parent` chain -- bounded, no cycles: struct members
   cannot contain themselves by value). Used only to consume
   ctx->p_pending_ended_report_obj; see the field comment in flow3.h.
*/
static bool flow3_object_is_pending_ended_report(const struct flow3_visit_ctx* ctx, const struct object* p_obj)
{
    if (ctx->p_pending_ended_report_obj == NULL)
        return false;

    for (const struct object* _Opt cur = p_obj; cur; cur = cur->parent)
    {
        if (cur == ctx->p_pending_ended_report_obj)
            return true;
    }
    return false;
}

static void flow3_check_object_init_assigment(struct flow3_visit_ctx* ctx,
    struct expression* p_expression,
    const struct object* _Opt p_object_dest, //uninitialized always
    const struct object* _Opt p_object_src,
    enum init_type init_type,
    bool dest_is_dtor,
    bool dest_is_view)
{
    if (ctx->p_current_flow3_map == NULL)
        return;

    try
    {
        if (p_object_src == NULL || p_object_dest == NULL)
            return;

        /* A _Dtor destination (destructor parameter) accepts a partially-created
       object: any of its members may legitimately be null, because a
       constructor that failed halfway must still be destroyable. Once we are
       inside a _Dtor pointee, that permission applies to every member below it,
       so the flag propagates down the recursion. See samples/flow3/dtor_is_opt.c. */
        const bool dtor_here = dest_is_dtor || flow3_dest_pointee_is_dtor(&p_object_dest->type);

        /* A _View destination borrows and never takes ownership, so nothing moves
       into it. The qualifier sits on the aggregate (e.g. a `_View struct X`
       parameter) while its members keep their own _Owner types, so it has to
       travel down the recursion just like dest_is_dtor. Without it, passing an
       owner-bearing struct to a _View parameter moved the caller's owners and
       silently dropped the "owner not moved" leak (samples/flow3/ownership.c). */
        const bool view_here = dest_is_view || type_is_view(&p_object_dest->type) ||
        flow3_object_under_view(p_object_dest);

        /* A directly-_Ctor destination (e.g. an array out-parameter
       `_Ctor char errmsg[100]`) receives uninitialized memory on purpose:
       the callee constructs it. There is nothing to check or propagate from
       the argument, and recursing would report every element as "possibly
       uninitialized". */
        if (type_is_ctor(&p_object_dest->type))
            return;

        struct marker marker = expression_to_marker(p_expression);

        if (p_object_src->members.head && p_object_dest->members.head)
        {
            /* Walking the source's own members is only correct when the source IS
           the object that holds the state. A dereference (`*p`) is not: it is a
           placeholder whose members carry no state, while its real state lives
           in its alternatives (a REF to the pointer's tracked pointee). So when
           the source has alternatives, fall through to the per-alternative loop
           below -- that loop already recurses through each REF into the
           referenced object, which does have the real members and state.
           Short-circuiting here walked the placeholder's empty members instead,
           leaving `struct S temp = *p;` uninitialized and never moving *p's
           owners (dogfooded from object.c object_swap, see
           samples/flow3/swap-through-pointers.c). */
            const struct flow3_key_alternatives* _Opt p_src_alts =
            flow3_map_search_up(ctx->p_current_flow3_map, p_object_src);

            /* Inspect EACH alternative: a source that stands for another object
           (a dereference's REF to the pointer's tracked pointee) must be read
           through that object, which holds the real member state. An aggregate
           that owns its state directly (e.g. the result of `x = f()`) has no
           such REF and keeps the member-by-member path below. */
            bool handled_by_reference = false;
            for (int ai = 0; p_src_alts != NULL && ai < p_src_alts->alternatives.size; ai++)
            {
                const struct flow3_alternative* a = p_src_alts->alternatives.data[ai];
                if (a->value_kind != FLOW3_VALUE_KIND_REF) continue;
                if (a->value.p == NULL || a->value.p == p_object_src) continue;
                if ((uintptr_t)a->value.p < 0x100000) continue; /* defensive, as elsewhere */
                if (!a->value.p->members.head) continue;

                flow3_check_object_init_assigment(ctx, p_expression, p_object_dest,
                    a->value.p, init_type, dtor_here, view_here);
                handled_by_reference = true;
            }
            if (handled_by_reference)
                return;

            {
                /* A union's members share storage: once any member is initialized the
               whole union is, so recurse only on the initialized member(s) and skip
               the uninitialized siblings. */
                const bool union_init = flow3_union_is_initialized(ctx, p_object_src);

                struct object* _Opt member_dest = p_object_dest->members.head;
                struct object* _Opt member_src = p_object_src->members.head;
                while (member_src && member_dest)
                {
                    if (!(union_init && !flow3_object_has_initialized_state(ctx, member_src)))
                        flow3_check_object_init_assigment(ctx, p_expression, member_dest, member_src, init_type, dtor_here, view_here);
                    member_src = member_src->next;
                    member_dest = member_dest->next;
                }

                /* C zero-fills the trailing elements of an array whose initializer is
               shorter than the array (`char new_file[512] = "";` zeroes [1..511]).
               The loop above copies only the elements the initializer supplies,
               leaving the rest looking uninitialized -- one false "possible
               uninitialized object" per element (codegen.c:5001). Zero the rest. */
                if (init_type == INIT_OBJ && type_is_array(&p_object_dest->type))
                {
                    while (member_dest)
                    {
                        flow3_map_set_object_zero(ctx->p_current_flow3_map, member_dest,
                            p_expression->first_token->line);
                        member_dest = member_dest->next;
                    }
                }
                return;
            }
        }
        /* Array-to-pointer decay: `&arr[0]` is never null, so a pointer
       initialized/assigned from an array is non-null. The array source carries
       no pointer-value alternative to copy (an array identifier expression has
       no tracked pointer value), so without this the destination was left empty
       and read as possibly-null -- e.g. `const char* p = path;` (path is
       `char[400]`) then `*p`, a false "possible null pointer dereference" at
       compile.c:213. Seed the destination as a plain non-null pointer. */
        if (init_type == INIT_OBJ &&
            type_is_array(&p_object_src->type) &&
            type_is_pointer(&p_object_dest->type))
        {
            struct flow3_key_alternatives* _Opt e =
            flow3_map_find_add(ctx->p_current_flow3_map, p_object_dest);
            if (e == NULL) throw;
            flow3_alternatives_clear(&e->alternatives);
            struct flow3_alternative a = {
                .value_kind = FLOW3_VALUE_KIND_PTR,
                .value = {.p = NULL},
                .value_relation = FLOW3_RELATION_NOT_EQUAL,
                .imaginary = FLOW3_IMAGINARY_NONE,
                .origin = ctx->p_current_flow3_map,
                .line = p_expression->first_token->line
            };
            flow3_alternatives_add(&e->alternatives, &a);
            return;
        }

        if (p_object_src->members.head && !p_object_dest->members.head)
        {
            /* Array-to-pointer decay: handled above. */
            /* fall through */
        }

        const struct flow3_key_alternatives* _Opt p_src_key_alternatives =
        flow3_map_search_up(ctx->p_current_flow3_map, p_object_src);
        if (p_src_key_alternatives == NULL)
        {
            /*
               The source has no tracked state -- "we know nothing about this
               value", which is NOT the same as "this value is uninitialized".
               Returning here left the destination at whatever it was before,
               and for a fresh local that is its declaration-time UNINITIALIZED
               marking. So `const bool e = (a || b);` reported e as possibly
               uninitialized whenever the operator's own per-path seeding
               declined to run (tokenizer.c:4830, and
               samples/flow3/logical-result-is-always-boolean.c).

               An assignment happened, so the destination is initialized with an
               unknown value: record ANY. Only for INIT_OBJ -- a real assignment
               to a real object. Arguments and returns have no destination
               object of their own to update.
            */
            if (init_type == INIT_OBJ)
            {
                /* Only replace an UNINITIALIZED destination. Specialised paths
                   (array/string zero-fill, array-to-pointer decay) may already
                   have given it a precise state before we got here, and that is
                   strictly better than ANY -- clobbering it broke a
                   compile_assert in array-string-init-zero-fill.c. */
                const struct flow3_key_alternatives* _Opt e_cur =
                flow3_map_search_up(ctx->p_current_flow3_map, p_object_dest);

                bool dest_is_uninitialized = true;
                if (e_cur != NULL)
                {
                    for (int i = 0; i < e_cur->alternatives.size; i++)
                    {
                        if (e_cur->alternatives.data[i]->value_relation != FLOW3_RELATION_UNINITIALIZED)
                        {
                            dest_is_uninitialized = false;
                            break;
                        }
                    }
                }

                struct flow3_key_alternatives* _Opt e_any = dest_is_uninitialized
                ? flow3_map_find_add(ctx->p_current_flow3_map, p_object_dest)
                : NULL;

                if (e_any != NULL)
                {
                    flow3_alternatives_clear(&e_any->alternatives);
                    struct flow3_alternative a = {
                        .value_kind = FLOW3_VALUE_KIND_SIGNED,
                        .value = {.i = ANY_VALUE},
                        .value_relation = FLOW3_RELATION_ANY,
                        .imaginary = FLOW3_IMAGINARY_NONE,
                        .origin = ctx->p_current_flow3_map,
                        .line = p_expression->first_token->line
                    };
                    flow3_alternatives_add(&e_any->alternatives, &a);
                }
            }
            return;
        }

        struct flow3_key_alternatives* _Opt p_dest_key_alternatives = NULL;
        if (init_type == INIT_OBJ)
        {
            p_dest_key_alternatives = flow3_map_find_add(ctx->p_current_flow3_map, p_object_dest);
            if (p_dest_key_alternatives == NULL) throw;
            flow3_alternatives_clear(&p_dest_key_alternatives->alternatives);
        }

        /*
           Report each finding once per assignment, not once per source
           alternative -- the same rule flow3_check_object_access already
           applies (see nullable_reported there). An object that accumulated
           many alternatives repeats one identical message: `return
           identity_pair;` at flow3.c:8757 emitted the same "possible null
           pointer 'identity_pair'" 174 times, and flow3.c:8754 twice more at
           87 each. The alternatives differ only in provenance, so the extra
           copies carry no information the first one lacks.
        */
        bool nullable_reported = false;
        bool uninitialized_reported = false;
        bool moved_reported = false;
        bool lifetime_ended_reported = false;

        /* Those four flags cover the object being assigned; the recursive
           member checks below run once per source alternative and need the
           same rule applied across calls. Reset here so the scope is exactly
           one assignment. */
        flow3_reported_findings_clear();

        for (int ri = 0; ri < p_src_key_alternatives->alternatives.size; ri++)
        {
            struct flow3_alternative* p_src_alternative = p_src_key_alternatives->alternatives.data[ri];

            if (p_src_alternative->imaginary == FLOW3_IMAGINARY_ENDED &&
                !flow3_map_is_ancestor_or_self(p_src_alternative->origin, ctx->p_current_flow3_map))
            {
                /* This ENDED fact's origin is a sibling branch that was never
                   open at the same time as the current path (e.g. a catch arm
                   that released the object and reset the pointer to null,
                   already excluded by a null check ahead of this call) --
                   same class of false positive as flow3_check_object_access's
                   already-fixed lifetime check. See
                   deref-after-catch-reset-false-positive.c. */
                continue;
            }

            if (p_src_alternative->imaginary == FLOW3_IMAGINARY_ENDED)
            {
                /* Already reported by EXPR_UNARY_CONTENT's own dereference
                   check for this exact object/line, when this same
                   expression was visited just before this call -- see the
                   field comment in flow3.h. Consume it once so it does not
                   suppress an unrelated later report. */
                if (p_src_alternative->line == ctx->pending_ended_report_line &&
                    flow3_object_is_pending_ended_report(ctx, p_object_src))
                {
                    lifetime_ended_reported = true;
                    ctx->p_pending_ended_report_obj = NULL;
                    continue;
                }

                if (!lifetime_ended_reported)
                {
                    lifetime_ended_reported = true;
                    struct osstream ss = { 0 };
                    flow3_expression_to_string(p_expression, &ss);
                    diagnostic(W_FLOW_LIFETIME_ENDED,
                        ctx->ctx, NULL, &marker,
                        "object '%s' lifetime has ended (see line %d)",
                        ss.c_str, p_src_alternative->line);
                    ss_close(&ss);
                }
                continue;
            }

            if (p_src_alternative->value_kind == FLOW3_VALUE_KIND_REF)
            {
                if (p_src_alternative->value.p != NULL &&
                    (uintptr_t)p_src_alternative->value.p < 0x100000)
                {
                    /* Defensive: skip corrupted REF. */
                    continue;
                }

                if (p_src_alternative->value.p != p_object_src)
                {
                    flow3_check_object_init_assigment(ctx,
                        p_expression,
                        p_object_dest,
                        p_src_alternative->value.p,
                        init_type,
                        dtor_here,
                        view_here);

                    if (type_is_owner(&p_object_dest->type) && !view_here)
                    {
                        flow3_map_set_object_moved(ctx->p_current_flow3_map,
                            p_src_alternative->value.p,
                            p_expression->first_token->line);
                    }
                    continue;
                }

                if (init_type == INIT_PARAMETER &&
                    type_is_pointer_or_array(&p_object_dest->type) &&
                    p_src_alternative->value.p != NULL)
                {
                    /* Array parameters are handled the same as pointer
                   parameters via the flow3_dest_pointee_is_* wrappers,
                   which route array destinations through the element type
                   instead of a pointee (type_is_pointed_* alone would
                   silently report "false" for every check here, since an
                   array destination's type_is_pointer() is false). */
                    const struct object* pointee = p_src_alternative->value.p;
                    enum flow3_pointee_effect_kind effect_kind = FLOW3_EFFECT_NONE;
                    const int line = p_expression->first_token->line;

                    if (flow3_dest_pointee_is_clear(&p_object_dest->type))
                        effect_kind = FLOW3_EFFECT_CLEAR; // zero every member (+ end reachable owner pointees)
                    else if (flow3_dest_pointee_is_dtor(&p_object_dest->type))
                        effect_kind = FLOW3_EFFECT_DTOR; // uninitialize every member (+ end reachable owner pointees)
                    else if (flow3_dest_pointee_is_ctor(&p_object_dest->type))
                        effect_kind = FLOW3_EFFECT_ANY; // uninitialized / any (constructor)
                    else if (type_is_owner(&p_object_dest->type))
                        effect_kind = FLOW3_EFFECT_LIFETIME_ENDED; // owner takes ownership -> whole object moved/ended
                    else if (!flow3_dest_pointee_is_const(&p_object_dest->type))
                        effect_kind = FLOW3_EFFECT_ANY; // plain mutable pointer (or array) -> ANY

                    if (effect_kind != FLOW3_EFFECT_NONE)
                    {
                        if (ctx->collect_deferred_effects)
                        {
                            /* Defer effects until all arguments are evaluated. */
                            if (ctx->deferred_effects_count <
                                (int)(sizeof ctx->deferred_effects / sizeof ctx->deferred_effects[0]))
                            {
                                ctx->deferred_effects[ctx->deferred_effects_count].pointee = pointee;
                                ctx->deferred_effects[ctx->deferred_effects_count].kind = effect_kind;
                                ctx->deferred_effects[ctx->deferred_effects_count].line = line;
                                ctx->deferred_effects_count++;
                            }
                        }
                        else
                        {
                            switch (effect_kind)
                            {
                            case FLOW3_EFFECT_CLEAR: flow3_map_apply_dtor_or_clear_effect(ctx->p_current_flow3_map, pointee, true, line); break;
                            case FLOW3_EFFECT_LIFETIME_ENDED: flow3_map_set_object_lifetime_ended(ctx->p_current_flow3_map, pointee, line); break;
                            case FLOW3_EFFECT_ANY: flow3_map_set_object_any_n(ctx->p_current_flow3_map, pointee, line,
                                    ctx->ctx->options.null_checks_enabled); break;
                            case FLOW3_EFFECT_DTOR: flow3_map_apply_dtor_or_clear_effect(ctx->p_current_flow3_map, pointee, false, line); break;
                            default: break;
                            }
                        }
                    }

                    /* Also check the pointee for uninitialized / moved state (when the argument itself is read). */
                    const bool source_uninit = type_is_uninit(&p_expression->type) || type_is_pointed_uninit(&p_expression->type);
                    const bool check_uninitialized = !flow3_dest_pointee_is_ctor(&p_object_dest->type) && !source_uninit;
                    /* For an array parameter, pass its type so the argument array's
                   elements are checked against the parameter's element _Opt. */
                    const struct type* _Opt gov =
                    type_is_array(&p_object_dest->type) ? &p_object_dest->type : NULL;
                    struct osstream arg_ss = { 0 };
                    flow3_expression_to_string(p_expression, &arg_ss);
                    flow3_check_object_access(ctx, arg_ss.c_str ? arg_ss.c_str : "", p_expression, pointee, check_uninitialized, p_src_alternative->origin, dtor_here, gov, true, false, true, true);
                    ss_close(&arg_ss);
                }
                /* =================================================================
               END ADDED
               ================================================================= */
            }

            if (init_type == INIT_OBJ)
            {
                flow3_alternatives_add(&p_dest_key_alternatives->alternatives, p_src_alternative);
            }

            if (p_src_alternative->value_relation == FLOW3_RELATION_EQUAL &&
                p_src_alternative->value_kind == FLOW3_VALUE_KIND_PTR &&
                p_src_alternative->value.p)
            {
                struct osstream ss = { 0 };
                flow3_expression_to_string(p_expression, &ss);
                struct osstream ss2 = { 0 };
                ss_fprintf(&ss2, "%s", ss.c_str);

                if (!type_is_void_ptr(&p_object_dest->type))
                {
                    const bool source_uninit = type_is_uninit(&p_expression->type) || type_is_pointed_uninit(&p_expression->type);

                    /*
                       Returning a POINTER does not read what it points at, so
                       the pointee's uninitialized state is not a finding on the
                       return path -- `int* _Owner p = malloc(n); return p;` is
                       the ordinary way to write an allocator wrapper, and every
                       such sample reported "passing a possible uninitialized
                       object '(*p)'" once malloc was correctly declared
                       _Uninitialized (samples/flow3/owner-return-no-false-positive.c).

                       INIT_PARAMETER is deliberately still checked: handing a
                       pointer to uninitialized storage to a callee that may read
                       it IS a bug, and a callee that intends to fill it says so
                       with _Ctor (flow3_dest_pointee_is_ctor, just below).
                    */
                    const bool check_unitialized =
                    !flow3_dest_pointee_is_ctor(&p_object_dest->type) &&
                    !source_uninit &&
                    init_type != INIT_RETURN;

                    flow3_check_object_access(ctx, ss2.c_str, p_expression, p_src_alternative->value.p,
                        check_unitialized, p_src_alternative->origin, dtor_here, NULL, true, false, true, true);
                }

                /* Moving an owner pointer into a void* _Owner destination (e.g.
                   free(p), `void* _Owner v = p;`, or `return (void* _Owner)p;`)
                   erases the pointee's type: whoever receives the void* can
                   never reach back into *pointee to release its own _Owner
                   members individually. flow3_check_object_access above is
                   already skipped for void* destinations (see
                   samples/flow3/void-owner-param-skips-member-checks.c), which
                   is correct for members whose lifetime already legitimately
                   ended (e.g. a prior _Dtor call) -- but it must not become a
                   blanket pass for members that were simply never released.
                   Applies to every init_type (parameter, assignment/init,
                   return): all three erase the pointee's type just the same.
                   Run the same recursive "owner not moved" check scope-exit
                   uses (flow3_check_object_at_exit) on the pointee before its
                   lifetime is erased, so this still catches a leaked
                   p->surname the way it would have if p had gone out of scope
                   instead. See
                   tests/unit-tests/flow3/void-owner-erasure-leaks-member.c. */
                if (type_is_owner(&p_object_dest->type) &&
                    type_is_void_ptr(&p_object_dest->type) &&
                    p_src_alternative->value.p != NULL)
                {
                    /* The member the caller forgot to release is, by
                       definition, one nothing in the code ever happened to
                       read -- so its flow state may not exist yet. Force it
                       into existence for every member before checking, or
                       the check below silently finds nothing to complain
                       about (flow3_map_search_up returns NULL and
                       flow3_check_object_at_exit treats "no state" as
                       "nothing to report", same as an object that was
                       legitimately never touched).

                       Line: use p_src_alternative->line (where "p" itself
                       -- the pointer being erased -- got its current
                       value, e.g. the parameter's own declaration line for
                       an untouched _Owner parameter) rather than
                       p_expression->first_token->line (this call site,
                       e.g. the `free(p)` line). The seeded member never had
                       a real assignment of its own in this function -- its
                       state is fabricated here purely so the check below has
                       something to read -- so the closest honest "acquired"
                       reference is wherever the CONTAINING pointer came
                       from, not the fabrication site itself, which produced
                       a meaningless "acquired at line N" identical to the
                       warning's own line. User-reported. */
                    flow3_seed_all_members_default(ctx,
                        (struct object*)(uintptr_t)p_src_alternative->value.p,
                        p_src_alternative->line);

                    /* Render the expression being erased (e.g. "p" in
                       `free(p)`) as the root name so the recursive check's
                       message reads "p->integer_type.name_opt" instead of
                       just ".integer_type.name_opt" -- see the
                       p_root_name_opt comment on flow3_check_object_at_exit.
                       osstream must outlive the call, so it's closed after. */
                    struct osstream root_name_ss = { 0 };
                    flow3_expression_to_string(p_expression, &root_name_ss);
                    flow3_check_object_at_exit(ctx,
                        &p_src_alternative->value.p->type,
                        p_src_alternative->value.p,
                        &marker,
                        p_expression->first_token,
                        view_here,
                        root_name_ss.c_str);
                    ss_close(&root_name_ss);
                }

                if (init_type == INIT_PARAMETER)
                {
                    /* Determine write-effect on the pointee (same as above, but for a PTR alternative). */
                    const struct object* pointee = p_src_alternative->value.p;
                    const int effect_line = p_expression->first_token->line;
                    enum flow3_pointee_effect_kind effect_kind = FLOW3_EFFECT_NONE;

                    if (flow3_dest_pointee_is_clear(&p_object_dest->type))
                        effect_kind = FLOW3_EFFECT_CLEAR;
                    else if (flow3_dest_pointee_is_dtor(&p_object_dest->type))
                        effect_kind = FLOW3_EFFECT_DTOR;
                    else if (flow3_dest_pointee_is_ctor(&p_object_dest->type))
                        effect_kind = FLOW3_EFFECT_ANY;
                    else if (type_is_owner(&p_object_dest->type))
                        effect_kind = FLOW3_EFFECT_LIFETIME_ENDED;
                    else if (!flow3_dest_pointee_is_const(&p_object_dest->type) &&
                        pointee != &p_expression->object)
                    {
                        effect_kind = FLOW3_EFFECT_ANY;
                    }

                    if (effect_kind != FLOW3_EFFECT_NONE)
                    {
                        if (ctx->collect_deferred_effects)
                        {
                            if (ctx->deferred_effects_count <
                                (int)(sizeof ctx->deferred_effects / sizeof ctx->deferred_effects[0]))
                            {
                                ctx->deferred_effects[ctx->deferred_effects_count].pointee = pointee;
                                ctx->deferred_effects[ctx->deferred_effects_count].kind = effect_kind;
                                ctx->deferred_effects[ctx->deferred_effects_count].line = effect_line;
                                ctx->deferred_effects_count++;
                            }
                        }
                        else
                        {
                            switch (effect_kind)
                            {
                            case FLOW3_EFFECT_CLEAR: flow3_map_apply_dtor_or_clear_effect(ctx->p_current_flow3_map, pointee, true, effect_line); break;
                            case FLOW3_EFFECT_LIFETIME_ENDED: flow3_map_set_object_lifetime_ended(ctx->p_current_flow3_map, pointee, effect_line); break;
                            case FLOW3_EFFECT_ANY: flow3_map_set_object_any_n(ctx->p_current_flow3_map, pointee, effect_line,
                                    ctx->ctx->options.null_checks_enabled); break;
                            case FLOW3_EFFECT_DTOR: flow3_map_apply_dtor_or_clear_effect(ctx->p_current_flow3_map, pointee, false, effect_line); break;
                            default: break;
                            }
                        }
                    }
                }

                ss_close(&ss);
                ss_close(&ss2);
            }

            /* Null-pointer check for non-optional destination.
           Skipped for a _Dtor destination: a destructor must accept a
           partially-created object, so a null member is allowed there. */
            if (!dtor_here &&
                type_is_pointer(&p_object_dest->type) &&
                !type_is_opt(&p_object_dest->type, ctx->ctx->options.null_checks_enabled) &&
                flow3_alternative_can_be_zero(p_src_alternative) &&
                !nullable_reported)
            {
                nullable_reported = true;
                struct osstream ss = { 0 };
                flow3_expression_to_string(p_expression, &ss);
                /* This check always tests p_object_dest's type (the
                   destination: a parameter, an assignment target, or a
                   return type), never the source expression's own type --
                   so the source (ss.c_str, e.g. "ctx->current") may well
                   be legitimately _Opt itself. Saying "'%s' ... is
                   declared non-nullable" would wrongly claim that about
                   the SOURCE. The destination is what disallows null; say
                   so. (Compare flow3_check_object_access above, which can
                   correctly say the value's own type is non-nullable when
                   checking a struct member with no destination type
                   threaded through -- a genuinely different case.)
                   User-reported: parser.c:5214, `p_token = ctx->current;`
                   -- ctx->current is `struct token* _Opt`; p_token isn't. */
                if (diagnostic(W_FLOW_NULLABLE_TO_NON_NULLABLE,
                    ctx->ctx, NULL, &marker,
                    "'%s' may be null, but the destination does not allow null",
                    ss.c_str))
                {
                    flow3_diagnose_state_origin(ctx, p_src_alternative, &marker);
                    flow3_diagnose_map_path(ctx, ctx->p_current_flow3_map);
                }
                ss_close(&ss);
            }

            /* Uninitialized check */
            if (p_src_alternative->value_relation == FLOW3_RELATION_UNINITIALIZED)
            {
                /* A union member shares storage with its siblings, so if any sibling
               is initialized the storage is initialized and this member is not
               genuinely uninitialized. A BRANCHED write (`if (c) v.i=..; else
               v.u=..`) writes a different member per branch, so after the merge
               each member carries a mixed init/uninit state -- the member-loop's
               union guard (which only skips siblings with NO init state at all)
               lets the uninit alternative through to here. Suppress it when the
               containing union has any initialized member. (object.c:501 /
               samples/flow3/union-branched-member-init.c) */
                const bool in_initialized_union =
                p_object_src->parent != NULL &&
                flow3_union_is_initialized(ctx, p_object_src->parent);

                /*
                   Honour the _Uninitialized contract here, as the read path at
                   the top of this file already does (see source_uninit in
                   flow3_check_object_access). Storage that is uninitialized BY
                   CONTRACT is not a finding:

                     source: `void* _Owner _Opt _Uninitialized malloc(size_t)`
                             -- `return malloc(n)` hands back memory the caller
                             is expected to fill; returning a pointer does not
                             read what it points at.

                     dest:   `int* _Owner _Opt _Uninitialized f(void)`
                             -- a wrapper declaring that it, too, returns
                             uninitialized storage. Without this the annotation
                             was silently ignored on the assignment/return path,
                             so there was no way to express the contract.

                   Genuinely uninitialized locals (`struct X x; f(&x);`) still
                   warn: neither side carries a contract there.
                */
                const bool source_uninit =
                type_is_uninit(&p_expression->type) ||
                type_is_pointed_uninit(&p_expression->type);

                const bool dest_accepts_uninit =
                type_is_uninit(&p_object_dest->type) ||
                type_is_pointed_uninit(&p_object_dest->type);

                if (!type_is_pointed_ctor(&p_object_dest->type) && !in_initialized_union &&
                    !source_uninit && !dest_accepts_uninit &&
                    !uninitialized_reported)
                {
                    uninitialized_reported = true;

                    const char* verb =
                    init_type == INIT_PARAMETER ? "passing" :
                    init_type == INIT_RETURN ? "returning" :
                    "assigning";

                    struct osstream name_ss = { 0 };
                    const char* object_name = p_object_src->member_designator;
                    if (object_name == NULL || object_name[0] == '\0')
                    {
                        flow3_expression_to_string(p_expression, &name_ss);
                        object_name = name_ss.c_str ? name_ss.c_str : "";
                    }

                    diagnostic(W_FLOW_UNINITIALIZED,
                        ctx->ctx, NULL, &marker,
                        "%s a possible uninitialized object '%s' (see line %d)",
                        verb, object_name, p_src_alternative->line);
                    ss_close(&name_ss);
                }
            }

            /* Moved check (use-after-move) */
            if (p_src_alternative->imaginary == FLOW3_IMAGINARY_MOVED &&
                type_is_owner(&p_object_dest->type) &&
                !moved_reported)
            {
                moved_reported = true;
                struct osstream ss = { 0 };
                flow3_expression_to_string(p_expression, &ss);
                /* W_FLOW_MOVED (32) -- see the note at the other move site. */
                diagnostic(W_FLOW_MOVED,
                    ctx->ctx, NULL, &marker,
                    "object '%s' is moved (see line %d)",
                    ss.c_str, p_src_alternative->line);
                ss_close(&ss);
            }
        }

        /* "A copy is a move": copying an _Owner value out of a source consumes it.
       This is the leaf rule -- an aggregate reaches here once per member via the
       member-by-member recursion above, so `struct S temp = *p;` moves each
       _Owner member of the pointed object one by one (and a plain
       `T* _Owner q = p;` moves the pointer itself). Without it, only the REF
       branch above moved anything, so a struct copied out of a dereference left
       the source's owners looking live -- the false "discards _Owner" /
       "uninitialized" pair on the swap idiom (dogfooded from object.c
       object_swap, see samples/flow3/swap-through-pointers.c). */
        if (!view_here &&
            type_is_owner(&p_object_src->type) &&
            type_is_owner(&p_object_dest->type) &&
            !p_object_src->members.head)
        {
            flow3_map_set_object_moved(ctx->p_current_flow3_map,
                p_object_src,
                p_expression->first_token->line);
        }
    }
    catch
    {
    }
}

/*
   An assignment into an _Owner object overwrites whatever it currently
   holds. If that current value is live (not already moved/ended, not
   uninitialized, and not already zero/null -- i.e. nothing to leak),
   the assignment discards it without releasing it first: a leak. This
   recurses into struct members so `*p = value;` on an _Owner-bearing
   struct is caught the same way a single _Owner pointer member is.

   Also warns when the current value is merely UNKNOWN (relation ANY,
   including the fabricated "must be valid" placeholder
   flow3_parameter_object_init invents for everything reachable through
   a plain, non-_Opt pointer parameter): an _Owner member reached that
   way could easily be live, and flow3 has no way to prove otherwise, so
   it now flags the overwrite instead of silently assuming it's safe.

   This is a deliberate trade-off: it previously skipped ANY/fabricated
   values entirely, because warning on them fired on this real-code
   shape (found dogfooding flow3 on cake's own codegen.c):

     char* _Opt _Owner temp = strdup(new_name);
     if (temp == NULL) throw;
     p_init_declarator->p_declarator->name_opt->lexeme = temp;

   (name_opt->lexeme's incoming state was exactly this kind of
   fabricated/unknown placeholder, not a value flow3 ever actually
   tracked as live -- so that line will now warn too.) A caller that
   knows a plain pointer parameter's _Owner fields are always null or
   are the caller's responsibility can suppress case by case; missing a
   genuine leak silently was judged the worse failure mode. See
   owner-moved-into-struct-member.c.
*/
/*
   Result of walking a destination for owner leaves that an overwrite would
   discard. Collected first and reported afterwards, so one assignment produces
   one diagnostic: assigning a whole struct discards every owner leaf it holds
   at once, and saying so once per leaf repeats one fact N times
   (`p_declarator->type = make_type_using_declarator(...)` at parser.c:3230 said
   it for .type.name_opt, .type.next and .type.params.head separately).

   The count decides the name: exactly one leaf is still named individually --
   `x->data = p;` should say `.data`, not the struct that contains it -- while
   several collapse to the aggregate the assignment actually targets.
*/
struct flow3_discarded_owner_scan
{
    int count;
    const char* _Opt name; /* designator of the first leaf found */
    int line;
};

static void flow3_scan_discarded_owners(struct flow3_visit_ctx* ctx,
    const struct object* _Opt p_object_dest,
    struct flow3_discarded_owner_scan* scan)
{
    if (p_object_dest == NULL)
    {
        /* A REF alternative's value.p can be NULL when the destination
           pointer (e.g. an _Opt parameter) may itself be null at this
           point -- there is no pointee object to check in that case.
           (The null-dereference itself is reported elsewhere.) */
        return;
    }

    if (p_object_dest->members.head)
    {
        for (const struct object* _Opt member = p_object_dest->members.head; member; member = member->next)
        {
            flow3_scan_discarded_owners(ctx, member, scan);
        }
        return;
    }

    if (!type_is_owner(&p_object_dest->type))
    {
        return;
    }

    /*
       This USED to also skip "synthetic parameter-seed" objects here
       (flow3_object_is_synthetic_parameter_seed) -- the fabricated
       "must be valid" placeholder flow3_parameter_object_init invents
       for everything reachable through a plain, non-_Opt pointer
       parameter, since a non-optional pointer promises its pointee is
       valid but flow3 never actually observed a real value there. That
       guard is deliberately no longer applied here, for the same
       reason the ANY-relation skip above was removed: an _Owner member
       reached that way is still "unknown, possibly live", and silently
       overwriting it is exactly the same class of leak as overwriting a
       confirmed-live one -- flow3 just can't prove it's live, only that
       it can't prove it's safe to discard. See
       owner-moved-into-struct-member.c.
    */

    const struct flow3_key_alternatives* _Opt e = flow3_map_search_up(ctx->p_current_flow3_map, p_object_dest);
    if (e == NULL)
    {
        return;
    }

    for (int i = 0; i < e->alternatives.size; i++)
    {
        const struct flow3_alternative* p_alternative = e->alternatives.data[i];

        if (p_alternative->imaginary == FLOW3_IMAGINARY_MOVED ||
            p_alternative->imaginary == FLOW3_IMAGINARY_ENDED ||
            p_alternative->imaginary == FLOW3_IMAGINARY_ABSENT)
        {
            continue;
        }

        if (p_alternative->value_relation == FLOW3_RELATION_UNINITIALIZED)
        {
            continue;
        }

        /*
           ANY means "could be literally anything, including null" -- it's
           the same seed flow3_parameter_object_init gives every field
           reached through a plain (unannotated) pointer parameter on
           entry (we have no idea what's really there). This USED to be
           silently skipped ("no positive evidence of a live resource"),
           because warning here fired on every assignment into an
           _Owner member reached through such a parameter, e.g.
           `p->declarator->name_opt->lexeme = temp;` (a real false
           positive found dogfooding flow3 on cake's own codegen.c).

           That suppression was deliberately reverted: an ANY-valued
           _Owner member is, by definition, "unknown, possibly live" --
           silently discarding it is exactly as unsound as discarding a
           confirmed-live one, just without the compile-time proof. The
           false positive above is a real trade-off, not a bug: callers
           are expected to guarantee (by convention/contract) that a
           plain, non-_Ctor pointer parameter's _Owner fields are either
           null or already the caller's problem to release -- flow3 has
           no way to verify that without seeing the caller, so it now
           flags the write and lets the user judge/suppress case by
           case, rather than silently missing genuine leaks like
           `x->data = p;` where the previous value of `x->data` truly
           could have been a live, un-freed resource. See
           owner-moved-into-struct-member.c for both sides of this.
        */

        if (flow3_alternative_is_zero(p_alternative))
        {
            continue;
        }

        /* One report per LEAF, not per alternative. Alternatives differ only in
           provenance -- the same rule flow3_check_object_access and
           flow3_check_object_init_assigment already apply (see
           nullable_reported there). Without the break, an object that
           accumulated many alternatives repeated one identical message once
           for each. */
        if (scan->count == 0)
        {
            scan->name = p_object_dest->member_designator;
            scan->line = p_alternative->line;
        }
        scan->count++;
        break;
    }
}

static void flow3_check_discarding_owner_before_overwrite(struct flow3_visit_ctx* ctx,
    const struct expression* p_expression_dest,
    const struct object* _Opt p_object_dest,
    const struct marker* marker)
{
    struct flow3_discarded_owner_scan scan = { 0 };
    flow3_scan_discarded_owners(ctx, p_object_dest, &scan);

    if (scan.count == 0)
        return;

    /* One leaf: name it. Several: name the object being assigned, since they
       are all discarded by the same write.

       A top-level destination (a whole object, not a member) carries an EMPTY
       designator rather than a null one -- `*obj = *p;` at object.c:1632 is the
       shape -- so test for content, not for null, and fall back to the first
       leaf's name. Naming one of the discarded members still points at the
       right assignment; an empty string would say nothing at all. */
    const char* _Opt designator = scan.name;
    if (scan.count > 1 &&
        p_object_dest != NULL &&
        p_object_dest->member_designator != NULL &&
        p_object_dest->member_designator[0] != '\0')
    {
        designator = p_object_dest->member_designator;
    }

    /* designator (when present) is relative to the destination expression's
       own object, e.g. ".head" for `*list = new_list;` -- printing that alone
       loses which object it's a member of. Render the destination expression
       itself (e.g. "*list") and splice the two together: "*list" + ".head"
       reads as `list->head`, matching how it would actually be written in
       source, rather than the misleading standalone ".head". */
    struct osstream expr_ss = { 0 };
    flow3_expression_to_string(p_expression_dest, &expr_ss);
    const char* base = expr_ss.c_str ? expr_ss.c_str : "";

    struct osstream name_ss = { 0 };
    if (designator != NULL && designator[0] != '\0')
    {
        const char* member = designator[0] == '.' ? designator + 1 : designator;
        if (base[0] == '*')
        {
            /* "*list" + ".head" -> "list->head" */
            ss_fprintf(&name_ss, "%s->%s", base + 1, member);
        }
        else
        {
            ss_fprintf(&name_ss, "%s.%s", base, member);
        }
    }
    else
    {
        ss_fprintf(&name_ss, "%s", base);
    }

    diagnostic(W_FLOW_DISCARDING_OWNER,
        ctx->ctx,
        NULL,
        marker,
        "assignment discards _Owner '%s' without releasing it first (see line %d)",
        name_ss.c_str ? name_ss.c_str : "",
        scan.line);

    ss_close(&name_ss);
    ss_close(&expr_ss);
}

/*
   STATIC (type-level) ownership rules for an assignment / initialization /
   argument / return. These are pure type checks -- they consult no flow state.

   Moved out of expressions.c's check_assigment so that every diagnostic that
   mentions _Owner lives in flow3: the core language parses the ownership
   annotations as ordinary qualifiers and attaches no meaning to them, and
   flow3 is a self-contained type-system addition on top.

   Call this once per assignment SITE (from the top-level callers of
   flow3_check_object_init_assigment), never from its member recursion --
   otherwise an aggregate would report the same type error once per member.
*/
static void flow3_check_static_ownership_assignment(struct flow3_visit_ctx* ctx,
    const struct type* _Opt p_dest_type,
    const struct expression* _Opt p_src_expression)
{
    if (p_dest_type == NULL || p_src_expression == NULL)
        return;

    const struct type* const p_src_type = &p_src_expression->type;

    if (type_is_owner(p_dest_type) && !type_is_owner(p_src_type))
    {
        /* A null pointer constant is always assignable to an owner. */
        if (!expression_is_null_pointer_constant(p_src_expression))
        {
            diagnostic(W_FLOW_NON_OWNER_TO_OWNER_ASSIGN,
                ctx->ctx,
                p_src_expression->first_token, NULL,
                "cannot assign a non-owner to owner");
            return;
        }
    }

    if (!type_is_owner(p_dest_type) && type_is_owner_or_pointer_to_dtor(p_src_type))
    {
        /* The source is a temporary owner (a function return value): storing it
           in a non-owner would leak it, since nothing takes ownership. */
        if (p_src_type->storage_class_specifier_flags & STORAGE_SPECIFIER_FUNCTION_RETURN)
        {
            diagnostic(W_FLOW_USING_TEMPORARY_OWNER,
                ctx->ctx,
                p_src_expression->first_token, NULL,
                "cannot assign a temporary owner to non-owner object.");
        }
    }
}

/*
   Apply a `_Clear` / `_Uninitialized` allocation contract to the DESTINATION.

   calloc/malloc return `void*`, so a contract applied at the call site lands on
   a typeless `void` pointee with no members and dies at the conversion to the
   real type. Here the destination's declared type IS known, so materialize a
   pointee of the destination's pointee type and seed it:

     _Clear         -> every member concretely 0   (calloc)
     _Uninitialized -> every member uninitialized  (malloc)

   Note this legitimately produces NEW diagnostics, and that is the point: after
   calloc a non-_Opt `_Owner` member really is null, and after malloc it really
   is uninitialized. "Zero" is the true state -- it does not mean "OK". Using
   such a member (returning it, passing it on) must warn until it is assigned.
*/
static void flow3_apply_alloc_contract_to_dest(struct flow3_visit_ctx* ctx,
    const struct type* _Opt p_dest_type,
    const struct object* _Opt p_object_dest,
    const struct expression* _Opt p_src_expression)
{
    if (p_dest_type == NULL || p_object_dest == NULL || p_src_expression == NULL)
        return;

    const struct type* const p_src_type = &p_src_expression->type;
    const bool want_zero = type_is_clear(p_src_type) || type_is_pointed_clear(p_src_type);
    const bool want_uninit = type_is_uninit(p_src_type) || type_is_pointed_uninit(p_src_type);
    if (!want_zero && !want_uninit)
        return;

    if (!type_is_pointer(p_dest_type))
        return;

    struct type pointed_type = type_remove_pointer(p_dest_type);
    if (!type_is_void(&pointed_type))
    {
        struct object* _Opt p_pointed = flow3_allocated_object_arena_new(&ctx->allocated_object_arena);
        if (p_pointed != NULL)
        {
            const int line = p_src_expression->first_token->line;
            make_object(&pointed_type, p_pointed, MAKE_STATE_ANY, ctx->ctx->options.target);
            if (want_zero)
                flow3_map_set_object_zero(ctx->p_current_flow3_map, p_pointed, line);
            else
                flow3_map_set_object_uninitialized(ctx->p_current_flow3_map, p_pointed, line);

            /* Repoint the destination's non-null arm; keep its null arm. */
            struct flow3_key_alternatives* _Opt e =
            flow3_map_find_add(ctx->p_current_flow3_map, p_object_dest);
            if (e != NULL)
            {
                for (int i = 0; i < e->alternatives.size; i++)
                {
                    struct flow3_alternative* a = e->alternatives.data[i];
                    if (a->value_kind != FLOW3_VALUE_KIND_PTR) continue;
                    if (flow3_alternative_is_zero(a)) continue;
                    a->value.p = p_pointed;
                    a->value_relation = FLOW3_RELATION_EQUAL;
                }
            }
        }
    }
    type_destroy(&pointed_type);
}

static void flow3_check_assigment(struct flow3_visit_ctx* ctx,
    struct expression* p_expression_dest,
    struct expression* p_expression_src)
{
    /* Static type rules once per assignment, before the per-alternative loop. */
    flow3_check_static_ownership_assignment(ctx, &p_expression_dest->type, p_expression_src);

    const struct flow3_key_alternatives* _Opt p_expression_dest_key_alternatives =
    flow3_map_search_up(ctx->p_current_flow3_map, &p_expression_dest->object);

    for (int i = 0;
        p_expression_dest_key_alternatives != NULL &&
        i < p_expression_dest_key_alternatives->alternatives.size; i++)
    {
        const struct flow3_alternative* p_expression_dest_alternative =
        p_expression_dest_key_alternatives->alternatives.data[i];

        if (p_expression_dest_alternative->value_kind == FLOW3_VALUE_KIND_REF)
        {
            const struct object* _Opt p_object_dest = p_expression_dest_alternative->value.p;

            if (p_object_dest != NULL && (uintptr_t)p_object_dest < 0x100000)
            {
                /* Defensive: see the matching guard in
                   flow3_check_object_init_assigment -- a REF alternative
                   should always carry a real object* alias, but addresses
                   this low can't be valid heap/arena pointers. Found via
                   dogfooding flow3 on cake's own sources; skip rather than
                   crash on what looks like a corrupted/mistagged
                   alternative. */
                continue;
            }

            struct marker marker = expression_to_marker(p_expression_dest);
            flow3_check_discarding_owner_before_overwrite(ctx, p_expression_dest, p_object_dest, &marker);

            flow3_check_object_init_assigment(ctx,
                p_expression_src,
                p_object_dest,
                &p_expression_src->object,
                INIT_OBJ,
                false,
                false);

            /* Same allocation-contract retyping the DECLARATOR-INITIALIZER path
               does (flow3_visit_init_declarator). Without it, only

                   struct T* _Owner _Opt p = calloc(1, sizeof *p);

               got a pointee typed `struct T`, while the equally common

                   struct T* _Owner _Opt p = NULL;
                   ...
                   p = calloc(1, sizeof *p);

               kept calloc's typeless `void` pointee. A void object has no
               members, so every later `p->m` failed to resolve to a real member
               object and manufactured a fresh ANY alternative at each read --
               which meant a guard like `if (p->m)` narrowed one throwaway entry
               while the use inside the branch read another, and the member was
               reported "possibly null" despite being guarded on the line above.

               The split declare/assign form is the norm in cake's own parser,
               because the object must exist before the `try` for the `catch` to
               release it (parser.c's declaration_core: p_declaration is declared
               NULL at the top, calloc'd inside the try, and every subsequent
               p_declaration->member access false-warned -- ids 33 and 35). */
            flow3_apply_alloc_contract_to_dest(ctx,
                &p_expression_dest->type,
                p_object_dest,
                p_expression_src);
        }
    }

    /* Assigning to an owner-typed lvalue moves the source owner out of it.
       The loop above records that whenever the destination object is
       tracked, but a deep member destination whose pointee was not
       synthesized has no tracked object, so the loop does nothing and the
       source would be left as a false "owner not moved" at scope exit.
       Record the move here as well: mark each REF source alternative's
       target moved, the same rule flow3_check_object_init_assigment applies.
       Idempotent with the loop (which already sets IMAGINARY_MOVED on the
       same objects). */
    if (type_is_owner(&p_expression_dest->type))
    {
        const struct flow3_key_alternatives* _Opt p_src =
        flow3_map_search_up(ctx->p_current_flow3_map, &p_expression_src->object);
        for (int i = 0; p_src != NULL && i < p_src->alternatives.size; i++)
        {
            const struct flow3_alternative* a = p_src->alternatives.data[i];
            if (a->value_kind == FLOW3_VALUE_KIND_REF && a->value.p != NULL)
                flow3_map_set_object_moved(ctx->p_current_flow3_map, a->value.p, p_expression_dest->first_token->line);
        }
    }
}

static void flow3_visit_function_arguments(struct flow3_visit_ctx* ctx,
    const struct type* p_type,
    const struct argument_expression_list* p_argument_expression_list)
{
    if (ctx->p_current_flow3_map == NULL)
        return;

    try
    {
        const struct param_list* _Opt p_param_list = type_get_func_or_func_ptr_params(p_type);
        if (p_param_list == NULL)
        {
            throw;
        }

        struct param* _Opt p_current_parameter_type = p_param_list->head;
        struct argument_expression* _Opt p_current_argument = p_argument_expression_list->head;

        /* Collect pointee write-effects instead of applying them per-argument:
           C evaluates ALL arguments first, then the callee runs. Applying them
           immediately would let an earlier argument (e.g. `&x`) invalidate a
           later one (e.g. `x.text`) before it is read. Nested calls in an
           argument save/restore this state so their effects don't leak out. */
        const bool saved_collect = ctx->collect_deferred_effects;
        const int saved_count = ctx->deferred_effects_count;
        ctx->collect_deferred_effects = true;
        ctx->deferred_effects_count = saved_count;

        while (p_current_argument && p_current_parameter_type)
        {
            struct expression* p_arg_expr = p_current_argument->expression;
            const struct type* p_param_type = &p_current_parameter_type->type;

            struct object param_object = { 0 };
            make_object(p_param_type, &param_object, MAKE_STATE_UNITIALIZED, ctx->ctx->options.target);

            flow3_visit_full_expression(ctx, p_arg_expr);
            flow3_check_static_ownership_assignment(ctx, p_param_type, p_arg_expr);
            flow3_check_object_init_assigment(ctx, p_arg_expr, &param_object, &p_arg_expr->object, INIT_PARAMETER, false, false);

            p_current_argument = p_current_argument->next;
            p_current_parameter_type = p_current_parameter_type->next;
            object_destroy(&param_object);
        }

        /* All arguments evaluated: now apply the deferred write-effects (the
           callee "runs"). Restore the collection state for the caller. */
        ctx->collect_deferred_effects = saved_collect;
        for (int i = saved_count; i < ctx->deferred_effects_count; i++)
        {
            const struct flow3_deferred_pointee_effect* e = &ctx->deferred_effects[i];
            switch (e->kind)
            {
            case FLOW3_EFFECT_CLEAR: flow3_map_apply_dtor_or_clear_effect(ctx->p_current_flow3_map, e->pointee, true, e->line); break;
            case FLOW3_EFFECT_LIFETIME_ENDED: flow3_map_set_object_lifetime_ended(ctx->p_current_flow3_map, e->pointee, e->line); break;
            case FLOW3_EFFECT_ANY: flow3_map_set_object_any_n(ctx->p_current_flow3_map, e->pointee, e->line, ctx->ctx->options.null_checks_enabled); break;
            case FLOW3_EFFECT_DTOR: flow3_map_apply_dtor_or_clear_effect(ctx->p_current_flow3_map, e->pointee, false, e->line); break;
            default: break;
            }
        }
        ctx->deferred_effects_count = saved_count;

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
        _Assert(expr->right != NULL);
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

static void narrow_by_constant(const struct flow3_alternatives* src,
    long long c,
    bool is_equal, /* true for ==, false for != */
    struct flow3_alternatives* true_alts,
    struct flow3_alternatives* false_alts,
    const struct flow3_map* origin,
    int line)
{
    for (int i = 0; i < src->size; i++)
    {
        const struct flow3_alternative* alt = src->data[i];
        if (alt->value_relation == FLOW3_RELATION_UNINITIALIZED)
            continue;

        /* Handle absent: treat as value 0 (null) */
        if (alt->imaginary == FLOW3_IMAGINARY_ABSENT)
        {
            bool result = is_equal ? (0 == c) : (0 != c);
            if (result)
            {
                struct flow3_alternative tagged = *alt;
                tagged.origin = origin;
                tagged.line = line;
                flow3_alternatives_add(true_alts, &tagged);
            }
            else
            {
                struct flow3_alternative tagged = *alt;
                tagged.origin = origin;
                tagged.line = line;
                flow3_alternatives_add(false_alts, &tagged);
            }
            continue;
        }

        /* Extract numeric value from the alternative if possible */
        long long val = 0;
        bool is_ptr = false;
        if (alt->value_kind == FLOW3_VALUE_KIND_SIGNED)
            val = alt->value.i;
        else if (alt->value_kind == FLOW3_VALUE_KIND_UNSIGNED)
            val = (long long)alt->value.u;
        else if (alt->value_kind == FLOW3_VALUE_KIND_PTR)
        {
            val = (uintptr_t)alt->value.p;
            is_ptr = true;
        }
        else if (alt->value_kind == FLOW3_VALUE_KIND_REF)
        {
            /* REF should have been resolved before calling this function */
            continue;
        }
        else
        {
            continue;
        }

        if (alt->value_relation == FLOW3_RELATION_EQUAL)
        {
            bool result = is_equal ? (val == c) : (val != c);
            if (result)
            {
                struct flow3_alternative tagged = *alt;
                tagged.origin = origin;
                tagged.line = line;
                flow3_alternatives_add(true_alts, &tagged);
            }
            else
            {
                struct flow3_alternative tagged = *alt;
                tagged.origin = origin;
                tagged.line = line;
                flow3_alternatives_add(false_alts, &tagged);
            }
        }
        else if (alt->value_relation == FLOW3_RELATION_NOT_EQUAL)
        {
            if (val == c)
            {
                /* Already known to be NOT_EQUAL c */
                if (is_equal)
                {
                    /* == c is false, keep NOT_EQUAL c in false branch only */
                    struct flow3_alternative tagged = *alt;
                    tagged.origin = origin;
                    tagged.line = line;
                    flow3_alternatives_add(false_alts, &tagged);
                }
                else
                {
                    /* != c is true, keep NOT_EQUAL c in true branch only */
                    struct flow3_alternative tagged = *alt;
                    tagged.origin = origin;
                    tagged.line = line;
                    flow3_alternatives_add(true_alts, &tagged);
                }
            }
            else
            {
                /* val != c */
                if (is_equal)
                {
                    /* For true branch of ==, we must enforce x == c, overriding the NOT_EQUAL val */
                    struct flow3_alternative v = { 0 };
                    if (is_ptr)
                    {
                        v.value_kind = FLOW3_VALUE_KIND_PTR;
                        v.value.p = (const struct object*)(uintptr_t)c;
                    }
                    else
                    {
                        v.value_kind = FLOW3_VALUE_KIND_SIGNED;
                        v.value.i = c;
                    }
                    struct flow3_alternative a_eq = {
                        .value_kind = v.value_kind,
                        .value = v.value,
                        .value_relation = FLOW3_RELATION_EQUAL,
                        .imaginary = alt->imaginary,
                        .origin = origin,
                        .line = line
                    };
                    flow3_alternatives_add(true_alts, &a_eq);
                    /* For false branch, keep the original NOT_EQUAL val (we lose the info x != c) */
                    struct flow3_alternative tagged = *alt;
                    tagged.origin = origin;
                    tagged.line = line;
                    flow3_alternatives_add(false_alts, &tagged);
                }
                else
                {
                    /* For !=, the condition x != c is not determined by x != val when val != c,
                       so we keep NOT_EQUAL val in both branches */
                    {
                        struct flow3_alternative tagged = *alt;
                        tagged.origin = origin;
                        tagged.line = line;
                        flow3_alternatives_add(true_alts, &tagged);
                    }
                    {
                        struct flow3_alternative tagged = *alt;
                        tagged.origin = origin;
                        tagged.line = line;
                        flow3_alternatives_add(false_alts, &tagged);
                    }
                }
            }
        }
        else if (alt->value_relation == FLOW3_RELATION_ANY)
        {
            /* Unknown value – refine to EQUAL c for true, NOT_EQUAL c for false */
            struct flow3_alternative v = { 0 };
            if (is_ptr)
            {
                v.value_kind = FLOW3_VALUE_KIND_PTR;
                v.value.p = (const struct object*)(uintptr_t)c;
            }
            else
            {
                v.value_kind = FLOW3_VALUE_KIND_SIGNED;
                v.value.i = c;
            }
            if (is_equal)
            {
                struct flow3_alternative a_eq = {
                    .value_kind = v.value_kind,
                    .value = v.value,
                    .value_relation = FLOW3_RELATION_EQUAL,
                    .imaginary = alt->imaginary,
                    .origin = origin,
                    .line = line
                };
                struct flow3_alternative a_ne = {
                    .value_kind = v.value_kind,
                    .value = v.value,
                    .value_relation = FLOW3_RELATION_NOT_EQUAL,
                    .imaginary = alt->imaginary,
                    .origin = origin,
                    .line = line
                };
                flow3_alternatives_add(true_alts, &a_eq);
                flow3_alternatives_add(false_alts, &a_ne);
            }
            else
            {
                struct flow3_alternative a_ne = {
                    .value_kind = v.value_kind,
                    .value = v.value,
                    .value_relation = FLOW3_RELATION_NOT_EQUAL,
                    .imaginary = alt->imaginary,
                    .origin = origin,
                    .line = line
                };
                struct flow3_alternative a_eq = {
                    .value_kind = v.value_kind,
                    .value = v.value,
                    .value_relation = FLOW3_RELATION_EQUAL,
                    .imaginary = alt->imaginary,
                    .origin = origin,
                    .line = line
                };
                flow3_alternatives_add(true_alts, &a_ne);
                flow3_alternatives_add(false_alts, &a_eq);
            }
        }
        else
        {
            /* Relational (half-line) constraint (>, >=, <, <=) meets an
               ==/!= test. The equality branch pins the variable to the
               constant (strictly more precise); the opposite branch keeps
               the range unchanged. This mirrors the ANY handling so that
               e.g. `x > 0 && x == 5` still narrows x to exactly 5 instead
               of losing the `== 5` to the pre-existing `> 0`. */
            struct flow3_alternative a_eq = {
                .value_kind = FLOW3_VALUE_KIND_SIGNED,
                .value = {.i = c},
                .value_relation = FLOW3_RELATION_EQUAL,
                .imaginary = alt->imaginary,
                .origin = origin,
                .line = line
            };
            struct flow3_alternative a_range = *alt;
            a_range.origin = origin;
            a_range.line = line;
            if (is_equal)
            {
                flow3_alternatives_add(true_alts, &a_eq);
                flow3_alternatives_add(false_alts, &a_range);
            }
            else
            {
                flow3_alternatives_add(true_alts, &a_range);
                flow3_alternatives_add(false_alts, &a_eq);
            }
        }
    }
}

/* Map a relational operator (variable on the LEFT: `var OP c`) to the
   flow3_relation recorded for the variable on the requested branch. */
static enum flow3_relation flow3_relation_for_op(enum expression_type op, bool true_branch)
{
    switch (op)
    {
    case EXPR_RELATIONAL_BIGGER_THAN:
        return true_branch ? FLOW3_RELATION_GREATER : FLOW3_RELATION_LESS_EQUAL;
    case EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN:
        return true_branch ? FLOW3_RELATION_GREATER_EQUAL : FLOW3_RELATION_LESS;
    case EXPR_RELATIONAL_LESS_THAN:
        return true_branch ? FLOW3_RELATION_LESS : FLOW3_RELATION_GREATER_EQUAL;
    case EXPR_RELATIONAL_LESS_OR_EQUAL_THAN:
        return true_branch ? FLOW3_RELATION_LESS_EQUAL : FLOW3_RELATION_GREATER;
    default:
        return FLOW3_RELATION_ANY;
    }
}

/* `c OP var` is equivalent to `var swapped(OP) c`. */
static enum expression_type flow3_swap_relational(enum expression_type op)
{
    switch (op)
    {
    case EXPR_RELATIONAL_BIGGER_THAN: return EXPR_RELATIONAL_LESS_THAN;
    case EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN: return EXPR_RELATIONAL_LESS_OR_EQUAL_THAN;
    case EXPR_RELATIONAL_LESS_THAN: return EXPR_RELATIONAL_BIGGER_THAN;
    case EXPR_RELATIONAL_LESS_OR_EQUAL_THAN: return EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN;
    default: return op;
    }
}

/* Narrow `src` (a variable's alternatives) for the condition `var OP c`,
   producing refined alternatives for the true and false branches.
   - concrete EQUAL values are routed to the branch they satisfy;
   - an unknown scalar (ANY) becomes a half-line constraint on each branch;
   - everything else is carried unchanged into both branches (conservative). */
static void narrow_by_relational(const struct flow3_alternatives* src,
    long long c,
    enum expression_type op, /* var OP c */
    struct flow3_alternatives* true_alts,
    struct flow3_alternatives* false_alts,
    const struct flow3_map* origin,
    int line)
{
    for (int i = 0; i < src->size; i++)
    {
        const struct flow3_alternative* alt = src->data[i];
        if (alt->value_relation == FLOW3_RELATION_UNINITIALIZED)
            continue;

        long long lo = 0, hi = 0;
        if (alt->value_relation == FLOW3_RELATION_EQUAL &&
            flow3_alt_to_interval(alt, &lo, &hi))
        {
            /* Concrete value: route to whichever branch it satisfies. */
            bool t = flow3_scalar_relation_holds(lo, op, c);
            struct flow3_alternative tagged = *alt;
            tagged.origin = origin;
            tagged.line = line;
            flow3_alternatives_add(t ? true_alts : false_alts, &tagged);
            continue;
        }

        if (alt->value_relation == FLOW3_RELATION_ANY &&
            (alt->value_kind == FLOW3_VALUE_KIND_SIGNED ||
            alt->value_kind == FLOW3_VALUE_KIND_UNSIGNED) &&
            alt->imaginary == FLOW3_IMAGINARY_NONE)
        {
            struct flow3_alternative a_true = {
                .value_kind = FLOW3_VALUE_KIND_SIGNED,
                .value = {.i = c},
                .value_relation = flow3_relation_for_op(op, true),
                .imaginary = FLOW3_IMAGINARY_NONE,
                .origin = origin,
                .line = line
            };
            struct flow3_alternative a_false = {
                .value_kind = FLOW3_VALUE_KIND_SIGNED,
                .value = {.i = c},
                .value_relation = flow3_relation_for_op(op, false),
                .imaginary = FLOW3_IMAGINARY_NONE,
                .origin = origin,
                .line = line
            };
            flow3_alternatives_add(true_alts, &a_true);
            flow3_alternatives_add(false_alts, &a_false);
            continue;
        }

        /* An existing half-line constraint (e.g. `<= 0`, left over from an
           earlier narrowing) being narrowed again by a DIFFERENT relational
           condition on the same variable: intersect it with each branch's
           range instead of blindly duplicating it into both unchanged.

           Without this, a variable already known `<= 0` on one path (e.g. the
           false arm of `a > 0 && b > 0`, merged with the unconstrained value
           from the other path) kept `<= 0` as a live alternative even on the
           TRUE branch of a later, logically incompatible `b > 0` -- so it
           still looked possibly-zero there. Two sequential ifs on the same
           variable is exactly cake's own pattern (object.c
           signed_long_long_mul/_div: `if (a>0 && b>0) ...; else { if (b>0)
           .../b; }`), and produced false "division by zero" (samples/flow3/
           narrow-half-line-against-relational.c). If the old interval is
           entirely outside a branch's range, this alternative cannot occur on
           that branch at all: omit it there instead of asserting an
           impossible value. Only handles a clean numeric interval (EQUAL or a
           half-line relation, imaginary NONE); anything else (NOT_EQUAL,
           pointers, MOVED/ENDED) falls through to the original
           keep-unchanged-in-both behavior below. */
        long long alt_lo = 0, alt_hi = 0;
        if (alt->imaginary == FLOW3_IMAGINARY_NONE && flow3_alt_to_interval(alt, &alt_lo, &alt_hi))
        {
            struct flow3_alternative cond_true = {
                .value_kind = FLOW3_VALUE_KIND_SIGNED, .value = {.i = c},
                .value_relation = flow3_relation_for_op(op, true)
            };
            struct flow3_alternative cond_false = {
                .value_kind = FLOW3_VALUE_KIND_SIGNED, .value = {.i = c},
                .value_relation = flow3_relation_for_op(op, false)
            };
            long long t_lo = 0, t_hi = 0, f_lo = 0, f_hi = 0;
            bool have_t = flow3_alt_to_interval(&cond_true, &t_lo, &t_hi);
            bool have_f = flow3_alt_to_interval(&cond_false, &f_lo, &f_hi);

            if (have_t && alt_lo <= t_hi && t_lo <= alt_hi)
            {
                struct flow3_alternative tagged = *alt;
                tagged.origin = origin;
                tagged.line = line;
                flow3_alternatives_add(true_alts, &tagged);
            }
            if (have_f && alt_lo <= f_hi && f_lo <= alt_hi)
            {
                struct flow3_alternative tagged = *alt;
                tagged.origin = origin;
                tagged.line = line;
                flow3_alternatives_add(false_alts, &tagged);
            }
            continue;
        }

        /* NOT_EQUAL, pointers, or MOVED/ENDED state: keep unchanged in both
           branches (conservative fallback). */
        struct flow3_alternative tagged = *alt;
        tagged.origin = origin;
        tagged.line = line;
        flow3_alternatives_add(true_alts, &tagged);
        flow3_alternatives_add(false_alts, &tagged);
    }
}

/* True if operand `p_expr` evaluates to exactly one constant integer,
   considering ALL of its alternatives with per-alternative REF resolution
   (the flow3_evaluate_binary_arithmetic pattern -- no `data[0]`). If several
   alternatives are present they must all carry the same EQUAL constant. */
static bool flow3_operand_is_single_constant(struct flow3_visit_ctx* ctx,
    const struct expression* p_expr,
    long long* out)
{
    const struct flow3_key_alternatives* _Opt entry = flow3_map_search_up(ctx->p_current_flow3_map, &p_expr->object);
    if (entry == NULL)
        return false;

    bool found = false;
    long long val = 0;

    for (int i = 0; i < entry->alternatives.size; i++)
    {
        const struct flow3_alternative* alt = entry->alternatives.data[i];
        bool is_ref = (alt->value_kind == FLOW3_VALUE_KIND_REF && alt->value.p != NULL);
        const struct flow3_key_alternatives* _Opt resolved = entry;
        if (is_ref)
        {
            resolved = flow3_map_search_up(ctx->p_current_flow3_map, alt->value.p);
            if (resolved == NULL) return false;
        }
        int count = is_ref ? resolved->alternatives.size : 1;
        for (int j = 0; j < count; j++)
        {
            const struct flow3_alternative* v = is_ref ? resolved->alternatives.data[j] : alt;
            if (v->value_relation != FLOW3_RELATION_EQUAL)
                return false;
            long long c = 0;
            if (v->value_kind == FLOW3_VALUE_KIND_SIGNED) c = v->value.i;
            else if (v->value_kind == FLOW3_VALUE_KIND_UNSIGNED) c = (long long)v->value.u;
            else if (v->value_kind == FLOW3_VALUE_KIND_PTR) c = (long long)(uintptr_t)v->value.p;
            else return false;
            if (found && c != val)
                return false; /* more than one distinct value */
            val = c;
            found = true;
        }
    }

    if (!found)
        return false;
    *out = val;
    return true;
}

/* Narrow the variable operand `p_expr` for the condition `var OP c`, writing
   refined alternatives into p_true/p_false. Follows the per-alternative REF
   pattern: for every REF alternative of the operand, narrow the object it
   references (so it works when the operand aliases one -- or several --
   tracked objects, not just a single one). */
static void flow3_narrow_operand_relational(struct flow3_visit_ctx* ctx,
    const struct expression* p_expr,
    long long c,
    enum expression_type op,
    struct flow3_map* p_true,
    struct flow3_map* p_false,
    int line)
{
    const struct flow3_key_alternatives* _Opt entry = flow3_map_search_up(ctx->p_current_flow3_map, &p_expr->object);
    if (entry == NULL)
        return;

    for (int i = 0; i < entry->alternatives.size; i++)
    {
        const struct flow3_alternative* alt = entry->alternatives.data[i];
        if (alt->value_kind != FLOW3_VALUE_KIND_REF || alt->value.p == NULL)
            continue;

        const struct object* obj = alt->value.p;
        const struct flow3_key_alternatives* _Opt obj_entry =
        flow3_map_search_up(ctx->p_current_flow3_map, obj);
        if (obj_entry == NULL)
            continue;

        struct flow3_alternatives true_alts = { 0 }, false_alts = { 0 };
        narrow_by_relational(&obj_entry->alternatives, c, op,
            &true_alts, &false_alts, p_true, line);
        /* Tag each branch's alternatives with ITS OWN map so join
           correlation can tell them apart (true values belong to p_true,
           false values to p_false -- not both to p_true). */
        for (int k = 0; k < true_alts.size; k++) true_alts.data[k]->origin = p_true;
        for (int k = 0; k < false_alts.size; k++) false_alts.data[k]->origin = p_false;
        if (true_alts.size > 0)
        {
            struct flow3_key_alternatives* _Opt e = flow3_map_find_add(p_true, obj);
            if (e != NULL)
            {
                flow3_alternatives_clear(&e->alternatives);
                e->alternatives = true_alts;
            }
            else
            {
                /* nowhere to move them: release rather than leak */
                flow3_alternatives_clear(&true_alts);
            }
        }
        if (false_alts.size > 0)
        {
            struct flow3_key_alternatives* _Opt e = flow3_map_find_add(p_false, obj);
            if (e != NULL)
            {
                flow3_alternatives_clear(&e->alternatives);
                e->alternatives = false_alts;
            }
            else
            {
                /* nowhere to move them: release rather than leak */
                flow3_alternatives_clear(&false_alts);
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
        const struct flow3_alternative* alt = alts->data[i];
        if (alt->imaginary == FLOW3_IMAGINARY_ABSENT)
        {
            bool is_null = true;
            bool result = is_equal ? is_null : !is_null;
            if (result) all_false = false; else all_true = false;
            continue;
        }
        bool result = false, known = false;
        if (alt->value_kind == FLOW3_VALUE_KIND_PTR)
        {
            if (c == 0)
            {
                if (alt->value_relation == FLOW3_RELATION_EQUAL)
                {
                    result = is_equal ? (alt->value.p == NULL) : (alt->value.p != NULL);
                    known = true;
                }
                else if (alt->value_relation == FLOW3_RELATION_NOT_EQUAL)
                {
                    /* A NOT_EQUAL pointer alternative is non-null (whether it
                       records `!= null` directly or a concrete non-null target,
                       e.g. the result of array-decay pointer arithmetic) --
                       consistent with flow3_alternative_can_be_zero. */
                    result = is_equal ? false : true;
                    known = true;
                }
            }
        }
        else if (alt->value_kind == FLOW3_VALUE_KIND_SIGNED || alt->value_kind == FLOW3_VALUE_KIND_UNSIGNED)
        {
            long long val = (alt->value_kind == FLOW3_VALUE_KIND_SIGNED) ? alt->value.i : (long long)alt->value.u;
            if (alt->value_relation == FLOW3_RELATION_EQUAL)
            {
                result = is_equal ? (val == c) : (val != c);
                known = true;
            }
            else if (alt->value_relation == FLOW3_RELATION_NOT_EQUAL && val == c)
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

/* Is `anc` on the parent chain of `m` (i.e. an ancestor-or-equal map)? */
static bool flow3_map_is_ancestor_or_equal(const struct flow3_map* anc, const struct flow3_map* m)
{
    for (const struct flow3_map* _Opt p = m; p != NULL; p = p->p_parent_map)
        if (p == anc)
            return true;
    return false;
}

/* Two alternatives can coexist on the same execution path iff their origin
   maps lie on a single ancestor chain. Sibling branch maps (e.g. an if's
   `var-true` and `var-false`) are NOT compatible, so combining alternatives
   only across compatible origins correlates a join: after
   `if (c){a=1;b=2;}else{a=3;b=4;}`, a and b keep their branch pairing, so
   a+b is {3,7} rather than the uncorrelated cross product {3,5,7}.
   An unconditional value (origin an ancestor of both branches, or NULL) is
   compatible with everything. */
/* Two origins can coexist on one execution path unless their branch-decision
         chains CONFLICT -- i.e. some branch id appears on both chains with opposite
         sides (one took the `then`, the other the `else`). A value with no branch
         decisions (a constant/constexpr, or an unconditional value) conflicts with
         nothing, so it correlates with any path. */
static bool flow3_origins_compatible(const struct flow3_map* _Opt o1, const struct flow3_map* _Opt o2)
{
    if (o1 == NULL || o2 == NULL || o1 == o2)
        return true;
    for (const struct flow3_map* _Opt a = o1; a != NULL; a = a->p_parent_map)
    {
        if (a->branch_id == 0)
            continue;
        for (const struct flow3_map* _Opt b = o2; b != NULL; b = b->p_parent_map)
        {
            if (b->branch_id == a->branch_id && b->kind != a->kind)
                return false;
        }
    }
    return true;
}

/* Given two COMPATIBLE origins (one an ancestor-or-equal of the other),
   return the more specific (descendant) one -- the narrower branch the
   combined value belongs to. This keeps a computed result correlated to the
   branch that produced it (e.g. 1+2 -> 3 stays tagged to the `then` branch). */
static const struct flow3_map* flow3_origin_more_specific(const struct flow3_map* _Opt o1,
    const struct flow3_map* _Opt o2)
{
    if (o1 == NULL) return o2;
    if (o2 == NULL) return o1;
    if (o1 == o2) return o1;
    if (flow3_map_is_ancestor_or_equal(o1, o2)) return o2; /* o1 is ancestor -> o2 deeper */
    return o1; /* o2 is ancestor -> o1 deeper */
}

/* Decide `lval == rval` (or `!=`) for one value pair. If one side is a
   concrete constant (EQUAL numeric/pointer, or an absent object treated as
   null) the other side is checked against it via
   flow3_evaluate_equality_against_constant (which also decides NOT_EQUAL
   cases); if both are concrete they compare directly. Returns 1/0/-1. */
static bool flow3_alt_concrete_int(const struct flow3_alternative* alt, long long* out)
{
    if (alt->imaginary == FLOW3_IMAGINARY_ABSENT) { *out = 0; return true; }
    if (alt->value_relation != FLOW3_RELATION_EQUAL) return false;
    switch (alt->value_kind)
    {
    case FLOW3_VALUE_KIND_SIGNED: *out = alt->value.i; return true;
    case FLOW3_VALUE_KIND_UNSIGNED: *out = (long long)alt->value.u; return true;
    case FLOW3_VALUE_KIND_PTR: *out = (long long)(uintptr_t)alt->value.p; return true;
    default: return false;
    }
}

static int flow3_pair_equality(const struct flow3_alternative* lval,
    const struct flow3_alternative* rval,
    bool is_equal)
{
    long long lc = 0, rc = 0;
    bool lk = flow3_alt_concrete_int(lval, &lc);
    bool rk = flow3_alt_concrete_int(rval, &rc);

    /* Two non-null POINTER values that name DIFFERENT objects cannot be
       concluded unequal: distinct pointer variables may alias at runtime (e.g.
       two _Opt parameters that happen to point at the same object -- their
       non-null arms are separate synthesized pointees, but nothing proves they
       differ). Reporting them as definitely-not-equal wrongly killed branches
       like `if (list->head == list->tail)` after a head!=NULL guard. Only NULL
       vs non-null, and same-object, remain decidable here. REF values (the
       address of a real named object) keep their exact comparison below. */
    if (lval->value_relation == FLOW3_RELATION_EQUAL && lval->value_kind == FLOW3_VALUE_KIND_PTR && lval->value.p != NULL &&
        rval->value_relation == FLOW3_RELATION_EQUAL && rval->value_kind == FLOW3_VALUE_KIND_PTR && rval->value.p != NULL &&
        lval->value.p != rval->value.p)
    {
        return -1; /* different non-null pointers: may or may not alias */
    }

    if (lk && rk)
    {
        bool eq = (lc == rc);
        return (is_equal ? eq : !eq) ? 1 : 0;
    }
    if (rk)
    {
        /* Wrap lval as a one-element array-of-pointers, not the struct
           itself: data[] holds POINTERS now, so a bare cast of lval to
           "look like" the array (the old zero-copy trick, valid when data[]
           held inline structs) reads the pointee's own bytes back as a
           pointer value instead of as lval -- garbage dereferenced one call
           later. flow3_evaluate_equality_against_constant only reads
           through `one`, never clears it, so a stack-local one-element
           pointer array is safe: nothing here frees lval or this array. */
        struct flow3_alternative* _Opt one_data[1] = { (struct flow3_alternative*)lval };
        struct flow3_alternatives one = { one_data, 1, 1 };
        return flow3_evaluate_equality_against_constant(&one, rc, is_equal);
    }
    if (lk)
    {
        struct flow3_alternative* _Opt one_data[1] = { (struct flow3_alternative*)rval };
        struct flow3_alternatives one = { one_data, 1, 1 };
        return flow3_evaluate_equality_against_constant(&one, lc, is_equal);
    }
    return -1;
}

/* Evaluate `a == b` (or `!=`) across ALL alternatives of both operands,
   following the flow3_evaluate_binary_arithmetic iteration (per-alternative
   REF resolution -- no `data[0]`, no constant special-case: a constant is
   simply an operand with one alternative). An operand with no value
   alternatives (an empty, e.g. zero-initialized-default, entry) contributes
   no counter-example, so the comparison holds vacuously -- matching the
   previous evaluate_equality_against_constant behavior on an empty list.
   Returns 1 (always), 0 (never), or -1 (unknown / a pair is undecidable). */
static int flow3_evaluate_equality_multi(struct flow3_visit_ctx* ctx,
    const struct expression* p_left,
    const struct expression* p_right,
    bool is_equal)
{
    const struct flow3_key_alternatives* _Opt left_entry =
    flow3_map_search_up(ctx->p_current_flow3_map, &p_left->object);
    const struct flow3_key_alternatives* _Opt right_entry =
    flow3_map_search_up(ctx->p_current_flow3_map, &p_right->object);
    if (left_entry == NULL || right_entry == NULL)
        return -1;

    /*
       An EMPTY operand (no alternatives) means "nothing is known", so the
       comparison is UNKNOWN.

       This used to hold vacuously as a stand-in for calloc: a zeroed member was
       modeled as an empty entry, so `x->i == 0` came out true. That overloaded
       "empty" to mean both *known zero* and *unknown*, and the "unknown" side
       folded `p->m == c` to always-true -- reporting live code as unreachable
       and, via a folded disjunct producing a dead branch, losing pointer
       narrowing across `if (a || b) return;`.

       calloc now carries `_Clear`, which seeds real zeros, so the zero case is
       genuine state and the vacuous rule is no longer needed.
    */
    if (left_entry->alternatives.size == 0 || right_entry->alternatives.size == 0)
        return -1;

    bool all_true = true, all_false = true, any_pair = false, any_skipped = false;

    for (int li = 0; li < left_entry->alternatives.size; li++)
    {
        const struct flow3_alternative* lalt = left_entry->alternatives.data[li];
        bool l_is_ref = (lalt->value_kind == FLOW3_VALUE_KIND_REF && lalt->value.p != NULL);
        const struct flow3_key_alternatives* _Opt left_resolved = left_entry;
        if (l_is_ref)
        {
            left_resolved = flow3_map_search_up(ctx->p_current_flow3_map, lalt->value.p);
            if (left_resolved == NULL) return -1;
        }
        int l_count = l_is_ref ? left_resolved->alternatives.size : 1;

        for (int lvi = 0; lvi < l_count; lvi++)
        {
            const struct flow3_alternative* lval = l_is_ref ? left_resolved->alternatives.data[lvi] : lalt;

            for (int ri = 0; ri < right_entry->alternatives.size; ri++)
            {
                const struct flow3_alternative* ralt = right_entry->alternatives.data[ri];
                bool r_is_ref = (ralt->value_kind == FLOW3_VALUE_KIND_REF && ralt->value.p != NULL);
                const struct flow3_key_alternatives* _Opt right_resolved = right_entry;
                if (r_is_ref)
                {
                    right_resolved = flow3_map_search_up(ctx->p_current_flow3_map, ralt->value.p);
                    if (right_resolved == NULL) return -1;
                }
                int r_count = r_is_ref ? right_resolved->alternatives.size : 1;

                for (int rvi = 0; rvi < r_count; rvi++)
                {
                    const struct flow3_alternative* rval = r_is_ref ? right_resolved->alternatives.data[rvi] : ralt;
                    if (!flow3_origins_compatible(lval->origin, rval->origin))
                    {
                        any_skipped = true; /* correlated join: cross-branch pair */
                        continue;
                    }

                    int c = flow3_pair_equality(lval, rval, is_equal);
                    if (c == -1) return -1;
                    any_pair = true;
                    if (c) all_false = false; else all_true = false;
                }
            }
        }
    }

    /* If no pair was decided only because every combination was cross-branch
       (operands non-empty), the result is unknown. A genuinely empty operand
       (no alternatives -- a zero-initialized default) holds vacuously. */
    if (!any_pair && any_skipped)
        return -1;
    if (all_true) return 1;
    if (all_false) return 0;
    return -1;
}

/* Narrow the variable operand `p_expr` for `var == c` / `var != c`, writing
   refined alternatives into p_true/p_false. Per-alternative REF pattern: for
   every REF alternative of the operand, narrow the object it references. */
static void flow3_narrow_operand_equality(struct flow3_visit_ctx* ctx,
    const struct expression* p_expr,
    long long c,
    bool is_equal,
    struct flow3_map* p_true,
    struct flow3_map* p_false,
    int line)
{
    const struct flow3_key_alternatives* _Opt entry = flow3_map_search_up(ctx->p_current_flow3_map, &p_expr->object);
    if (entry == NULL)
        return;

    for (int i = 0; i < entry->alternatives.size; i++)
    {
        const struct flow3_alternative* alt = entry->alternatives.data[i];
        if (alt->value_kind != FLOW3_VALUE_KIND_REF || alt->value.p == NULL)
            continue;

        const struct object* _Opt obj = alt->value.p;
        const struct flow3_key_alternatives* _Opt obj_entry =
        flow3_map_search_up(ctx->p_current_flow3_map, obj);
        if (obj_entry == NULL)
            continue;

        struct flow3_alternatives true_alts = { 0 }, false_alts = { 0 };
        narrow_by_constant(&obj_entry->alternatives, c, is_equal,
            &true_alts, &false_alts, p_true, line);
        /* Tag each branch's alternatives with its own map (see the relational
           narrow) so join correlation can distinguish them. */
        for (int k = 0; k < true_alts.size; k++) true_alts.data[k]->origin = p_true;
        for (int k = 0; k < false_alts.size; k++) false_alts.data[k]->origin = p_false;
        if (true_alts.size > 0)
        {
            struct flow3_key_alternatives* _Opt e = flow3_map_find_add(p_true, obj);
            if (e != NULL)
            {
                flow3_alternatives_clear(&e->alternatives);
                e->alternatives = true_alts;
            }
            else
            {
                /* nowhere to move them: release rather than leak */
                flow3_alternatives_clear(&true_alts);
            }
        }
        if (false_alts.size > 0)
        {
            struct flow3_key_alternatives* _Opt e = flow3_map_find_add(p_false, obj);
            if (e != NULL)
            {
                flow3_alternatives_clear(&e->alternatives);
                e->alternatives = false_alts;
            }
            else
            {
                /* nowhere to move them: release rather than leak */
                flow3_alternatives_clear(&false_alts);
            }
        }
    }
}

// Helper: get a display name (uses member_designator if present)
const char* obj_display(const struct object* _Opt obj)
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
    switch (alt->value_kind)
    {
    case FLOW3_VALUE_KIND_SIGNED: printf("%lld", alt->value.i); break;
    case FLOW3_VALUE_KIND_UNSIGNED: printf("%llu", alt->value.u); break;
    case FLOW3_VALUE_KIND_PTR: printf("%s", obj_display(alt->value.p)); break;
    case FLOW3_VALUE_KIND_REF: printf("REF->%s", obj_display(alt->value.p)); break;
    default: printf("?");
    }
}

/* Truth of `l OP r` for l anywhere in [llo,lhi] and r anywhere in [rlo,rhi].
   Each relational predicate is monotone in each operand, so the whole box is
   decided by two corners: the hardest-to-satisfy one and the easiest.
   Returns 1 (true for all combos), 0 (false for all), or -1 (mixed). */
static int flow3_interval_relational(long long llo, long long lhi,
    long long rlo, long long rhi,
    enum expression_type op)
{
    long long worst_l = 0, worst_r = 0, best_l = 0, best_r = 0;
    switch (op)
    {
    case EXPR_RELATIONAL_BIGGER_THAN:
    case EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN:
        /* increasing in l, decreasing in r */
        worst_l = llo; worst_r = rhi; best_l = lhi; best_r = rlo;
        break;
    case EXPR_RELATIONAL_LESS_THAN:
    case EXPR_RELATIONAL_LESS_OR_EQUAL_THAN:
        /* decreasing in l, increasing in r */
        worst_l = lhi; worst_r = rlo; best_l = llo; best_r = rhi;
        break;
    default:
        return -1;
    }
    if (flow3_scalar_relation_holds(worst_l, op, worst_r)) return 1;
    if (!flow3_scalar_relation_holds(best_l, op, best_r)) return 0;
    return -1;
}

/* Evaluate `a OP b` (a relational operator) across ALL alternatives of both
   operands, following the flow3_evaluate_binary_arithmetic iteration: for
   each top-level alternative resolve a REF to the object it aliases and loop
   over that object's value alternatives -- no `size == 1` assumption. Each
   (left value, right value) pair is decided via its inclusive integer
   interval. Returns 1 (always true), 0 (always false), or -1 (unknown: a
   pair couldn't be decided, or the pairs disagree). */
static int flow3_evaluate_relational_multi(struct flow3_visit_ctx* ctx,
    const struct expression* p_left,
    const struct expression* p_right,
    enum expression_type op)
{
    const struct flow3_key_alternatives* _Opt left_entry =
    flow3_map_search_up(ctx->p_current_flow3_map, &p_left->object);
    const struct flow3_key_alternatives* _Opt right_entry =
    flow3_map_search_up(ctx->p_current_flow3_map, &p_right->object);
    if (left_entry == NULL || right_entry == NULL)
        return -1;

    bool all_true = true, all_false = true, any = false;

    for (int li = 0; li < left_entry->alternatives.size; li++)
    {
        const struct flow3_alternative* lalt = left_entry->alternatives.data[li];
        bool l_is_ref = (lalt->value_kind == FLOW3_VALUE_KIND_REF && lalt->value.p != NULL);
        const struct flow3_key_alternatives* _Opt left_resolved = left_entry;
        if (l_is_ref)
        {
            left_resolved = flow3_map_search_up(ctx->p_current_flow3_map, lalt->value.p);
            if (left_resolved == NULL) return -1;
        }
        int l_count = l_is_ref ? left_resolved->alternatives.size : 1;

        for (int lvi = 0; lvi < l_count; lvi++)
        {
            const struct flow3_alternative* lval = l_is_ref ? left_resolved->alternatives.data[lvi] : lalt;
            long long llo = 0, lhi = 0;
            if (!flow3_alt_to_interval(lval, &llo, &lhi))
                return -1;

            for (int ri = 0; ri < right_entry->alternatives.size; ri++)
            {
                const struct flow3_alternative* ralt = right_entry->alternatives.data[ri];
                bool r_is_ref = (ralt->value_kind == FLOW3_VALUE_KIND_REF && ralt->value.p != NULL);
                const struct flow3_key_alternatives* _Opt right_resolved = right_entry;
                if (r_is_ref)
                {
                    right_resolved = flow3_map_search_up(ctx->p_current_flow3_map, ralt->value.p);
                    if (right_resolved == NULL) return -1;
                }
                int r_count = r_is_ref ? right_resolved->alternatives.size : 1;

                for (int rvi = 0; rvi < r_count; rvi++)
                {
                    const struct flow3_alternative* rval = r_is_ref ? right_resolved->alternatives.data[rvi] : ralt;
                    if (!flow3_origins_compatible(lval->origin, rval->origin))
                        continue; /* correlated join: skip cross-branch pairs */
                    long long rlo = 0, rhi = 0;
                    if (!flow3_alt_to_interval(rval, &rlo, &rhi))
                        return -1;

                    int c = flow3_interval_relational(llo, lhi, rlo, rhi, op);
                    if (c == -1) return -1;
                    any = true;
                    if (c) all_false = false; else all_true = false;
                }
            }
        }
    }

    if (!any) return -1;
    if (all_true) return 1;
    if (all_false) return 0;
    return -1;
}

/* Per value-pair boolean of `lval OP rval` for a comparison operator.
   Returns 1, 0, or -1 (undecidable). */
static int flow3_pair_boolean(const struct flow3_alternative* lval,
    const struct flow3_alternative* rval,
    enum expression_type op)
{
    switch (op)
    {
    case EXPR_EQUALITY_EQUAL: return flow3_pair_equality(lval, rval, true);
    case EXPR_EQUALITY_NOT_EQUAL: return flow3_pair_equality(lval, rval, false);
    case EXPR_RELATIONAL_BIGGER_THAN:
    case EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN:
    case EXPR_RELATIONAL_LESS_THAN:
    case EXPR_RELATIONAL_LESS_OR_EQUAL_THAN:
        {
            long long llo = 0, lhi = 0, rlo = 0, rhi = 0;
            if (!flow3_alt_to_interval(lval, &llo, &lhi) ||
                !flow3_alt_to_interval(rval, &rlo, &rhi))
                return -1;
            return flow3_interval_relational(llo, lhi, rlo, rhi, op);
        }
    default:
        return -1;
    }
}

/* Produce the PER-PATH boolean result of a comparison `left OP right`: one
   {EQUAL 0/1, origin} alternative for each compatible operand-value pair, so
   `a+b == 7` over a+b={3@then,7@else} yields {0@then, 1@else}. This keeps the
   comparison correlated for a later `||`/`&&`. Returns true and fills `out`
   only if every pair was decidable; otherwise returns false (fall back to an
   ANY result). */
static bool flow3_comparison_result_alts(struct flow3_visit_ctx* ctx,
    const struct expression* p_left,
    const struct expression* p_right,
    enum expression_type op,
    struct flow3_alternatives* out,
    int line)
{
    if (ctx->p_current_flow3_map == NULL)
        return false;

    const struct flow3_key_alternatives* _Opt left_entry =
    flow3_map_search_up(ctx->p_current_flow3_map, &p_left->object);
    const struct flow3_key_alternatives* _Opt right_entry =
    flow3_map_search_up(ctx->p_current_flow3_map, &p_right->object);
    if (left_entry == NULL || right_entry == NULL)
        return false;

    bool any = false;

    for (int li = 0; li < left_entry->alternatives.size; li++)
    {
        const struct flow3_alternative* lalt = left_entry->alternatives.data[li];
        bool l_is_ref = (lalt->value_kind == FLOW3_VALUE_KIND_REF && lalt->value.p != NULL);
        const struct flow3_key_alternatives* _Opt left_resolved = left_entry;
        if (l_is_ref)
        {
            left_resolved = flow3_map_search_up(ctx->p_current_flow3_map, lalt->value.p);
            if (left_resolved == NULL) { flow3_alternatives_clear(out); return false; }
        }
        int l_count = l_is_ref ? left_resolved->alternatives.size : 1;

        for (int lvi = 0; lvi < l_count; lvi++)
        {
            const struct flow3_alternative* lval = l_is_ref ? left_resolved->alternatives.data[lvi] : lalt;

            for (int ri = 0; ri < right_entry->alternatives.size; ri++)
            {
                const struct flow3_alternative* ralt = right_entry->alternatives.data[ri];
                bool r_is_ref = (ralt->value_kind == FLOW3_VALUE_KIND_REF && ralt->value.p != NULL);
                const struct flow3_key_alternatives* _Opt right_resolved = right_entry;
                if (r_is_ref)
                {
                    right_resolved = flow3_map_search_up(ctx->p_current_flow3_map, ralt->value.p);
                    if (right_resolved == NULL) { flow3_alternatives_clear(out); return false; }
                }
                int r_count = r_is_ref ? right_resolved->alternatives.size : 1;

                for (int rvi = 0; rvi < r_count; rvi++)
                {
                    const struct flow3_alternative* rval = r_is_ref ? right_resolved->alternatives.data[rvi] : ralt;
                    if (!flow3_origins_compatible(lval->origin, rval->origin))
                        continue;

                    int b = flow3_pair_boolean(lval, rval, op);
                    if (b == -1) { flow3_alternatives_clear(out); return false; }

                    struct flow3_alternative a = {
                        .value_kind = FLOW3_VALUE_KIND_SIGNED,
                        .value = {.i = b ? 1 : 0},
                        .value_relation = FLOW3_RELATION_EQUAL,
                        .imaginary = FLOW3_IMAGINARY_NONE,
                        .origin = flow3_origin_more_specific(lval->origin, rval->origin),
                        .line = line
                    };
                    flow3_alternatives_add(out, &a);
                    any = true;
                }
            }
        }
    }

    if (!any) { flow3_alternatives_clear(out); return false; }
    return true;
}

/* Seed a comparison expression's own value object with its per-path boolean
   result when computable, otherwise ANY. Used on the non-folded path so an
   enclosing `||`/`&&` (and compile_assert) can reason per path. */
static void flow3_seed_comparison_result(struct flow3_visit_ctx* ctx,
    const struct expression* p_expression)
{
    struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
    if (e == NULL) return;

    flow3_alternatives_clear(&e->alternatives);

    struct flow3_alternatives per_path = { 0 };
    if (flow3_comparison_result_alts(ctx, p_expression->left, p_expression->right,
        p_expression->expression_type, &per_path,
        p_expression->first_token->line))
    {
        e->alternatives = per_path; /* move */
    }
    else
    {
        struct flow3_alternative a = {
            .value_kind = FLOW3_VALUE_KIND_SIGNED,
            .value = {.i = ANY_VALUE},
            .value_relation = FLOW3_RELATION_ANY,
            .imaginary = FLOW3_IMAGINARY_NONE,
            .origin = ctx->p_current_flow3_map,
            .line = p_expression->first_token->line
        };
        flow3_alternatives_add(&e->alternatives, &a);
    }
}

static void flow3_evaluate_binary_arithmetic(struct flow3_visit_ctx* ctx,
    const struct expression* p_left,
    const struct expression* p_right,
    const struct expression* p_result,
    char op)
{
    if (ctx->p_current_flow3_map == NULL)
        return;

    const struct flow3_key_alternatives* _Opt left_entry = flow3_map_search_up(ctx->p_current_flow3_map, &p_left->object);
    const struct flow3_key_alternatives* _Opt right_entry = flow3_map_search_up(ctx->p_current_flow3_map, &p_right->object);

    if (!left_entry || !right_entry)
    {
        struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_result->object);
        if (e == NULL) return;

        flow3_alternatives_clear(&e->alternatives);
        struct flow3_alternative a = {
            .value_kind = FLOW3_VALUE_KIND_SIGNED,
            .value = {.i = ANY_VALUE},
            .value_relation = FLOW3_RELATION_ANY,
            .imaginary = FLOW3_IMAGINARY_NONE,
            .origin = ctx->p_current_flow3_map,
            .line = p_result->first_token->line
        };
        flow3_alternatives_add(&e->alternatives, &a);
        return;
    }

    struct flow3_alternatives result_alts = { 0 };
    struct marker marker = expression_to_marker(p_result);

    /* Emit division/modulo-by-zero at most once per expression, even
       though the divisor may be checked against several left-hand
       alternatives below. */
    bool zero_divisor_warned = false;

    // ---- Outer loop over left top‑level alternatives ----
    for (int li = 0; li < left_entry->alternatives.size; li++)
    {
        const struct flow3_alternative* lalt = left_entry->alternatives.data[li];
        if (lalt->imaginary == FLOW3_IMAGINARY_ABSENT) continue;

        // Resolve left REF if present
        const struct flow3_key_alternatives* _Opt left_resolved = left_entry;
        if (lalt->value_kind == FLOW3_VALUE_KIND_REF && lalt->value.p != NULL)
        {
            left_resolved = flow3_map_search_up(ctx->p_current_flow3_map, lalt->value.p);
            if (!left_resolved) continue;
        }

        // ---- Loop over resolved left values ----
        for (int lvi = 0; lvi < left_resolved->alternatives.size; lvi++)
        {
            const struct flow3_alternative* lval = left_resolved->alternatives.data[lvi];
            if (lval->imaginary == FLOW3_IMAGINARY_ABSENT) continue;

            // ---- Middle loop over right top‑level alternatives ----
            for (int ri = 0; ri < right_entry->alternatives.size; ri++)
            {
                const struct flow3_alternative* ralt = right_entry->alternatives.data[ri];
                if (ralt->imaginary == FLOW3_IMAGINARY_ABSENT) continue;

                // Resolve right REF if present
                const struct flow3_key_alternatives* _Opt right_resolved = right_entry;
                if (ralt->value_kind == FLOW3_VALUE_KIND_REF && ralt->value.p != NULL)
                {
                    right_resolved = flow3_map_search_up(ctx->p_current_flow3_map, ralt->value.p);
                    if (!right_resolved) continue;
                }

                // ---- Loop over resolved right values ----
                for (int rvi = 0; rvi < right_resolved->alternatives.size; rvi++)
                {
                    const struct flow3_alternative* rval = right_resolved->alternatives.data[rvi];
                    if (rval->imaginary == FLOW3_IMAGINARY_ABSENT) continue;

                    enum flow3_relation result_rel = FLOW3_RELATION_ANY;
                    long long result_val = ANY_VALUE;

                    /*
                       Divisor may be zero even when its exact value isn't
                       known (ANY, or NOT_EQUAL to some nonzero constant
                       still doesn't rule out 0). Warn as soon as we see a
                       divisor alternative that could be zero, not only
                       when both operands are known constants.
                    */
                    if ((op == '/' || op == '%') &&
                        !zero_divisor_warned &&
                        !ctx->expression_is_not_evaluated &&
                        rval->value_relation != FLOW3_RELATION_UNINITIALIZED &&
                        flow3_alternative_can_be_zero(rval))
                    {
                        /* Unevaluated contexts (sizeof/_Alignof, static asserts)
                           never perform the division at runtime. */
                        diagnostic(W_FLOW_DIVISION_BY_ZERO, ctx->ctx, NULL, &marker,
                            op == '/' ? "division by zero" : "modulo by zero");
                        zero_divisor_warned = true;
                    }

                    /* Correlate the join for the computed VALUE: only combine
                       operand values that can coexist on the same path
                       (compatible origins). The divisor-zero check above is a
                       property of the divisor alone and runs for every rval,
                       independent of this pairing. */
                    if (!flow3_origins_compatible(lval->origin, rval->origin))
                        continue;

                    // Only if both are EQUAL and numeric can we compute an exact result
                    if (lval->value_relation == FLOW3_RELATION_EQUAL && rval->value_relation == FLOW3_RELATION_EQUAL)
                    {
                        bool lnum = (lval->value_kind == FLOW3_VALUE_KIND_SIGNED || lval->value_kind == FLOW3_VALUE_KIND_UNSIGNED);
                        bool rnum = (rval->value_kind == FLOW3_VALUE_KIND_SIGNED || rval->value_kind == FLOW3_VALUE_KIND_UNSIGNED);
                        if (lnum && rnum)
                        {
                            long long l = (lval->value_kind == FLOW3_VALUE_KIND_SIGNED) ? lval->value.i : (long long)lval->value.u;
                            long long r = (rval->value_kind == FLOW3_VALUE_KIND_SIGNED) ? rval->value.i : (long long)rval->value.u;

                            switch (op)
                            {
                            case '+':
                                result_val = l + r;
                                result_rel = FLOW3_RELATION_EQUAL;
                                break;
                            case '-':
                                result_val = l - r;
                                result_rel = FLOW3_RELATION_EQUAL;
                                break;
                            case '*':
                                result_val = l * r;
                                result_rel = FLOW3_RELATION_EQUAL;
                                break;
                            case '/':
                                if (r == 0)
                                {
                                    /* already warned above by the
                                       possibly-zero-divisor check */
                                    // result remains ANY
                                }
                                else
                                {
                                    result_val = l / r;
                                    result_rel = FLOW3_RELATION_EQUAL;
                                }
                                break;
                            case '%':
                                if (r == 0)
                                {
                                    /* already warned above by the
                                       possibly-zero-divisor check */
                                    // result remains ANY
                                }
                                else
                                {
                                    result_val = l % r;
                                    result_rel = FLOW3_RELATION_EQUAL;
                                }
                                break;
                            case '<': /* << */
                            case '>': /* >> */
                                if (r < 0 || r >= (long long)(sizeof(long long) * 8))
                                {
                                    /* shift count out of range: result is
                                       undefined -- leave ANY. */
                                }
                                else
                                {
                                    result_val = (op == '<') ? (l << r) : (l >> r);
                                    result_rel = FLOW3_RELATION_EQUAL;
                                }
                                break;
                            default:
                                break;
                            }
                        }
                    }

                    /*
                       Pointer arithmetic (p + n / p - n / n + p): this
                       used to fall straight through to the generic
                       "unknown" alternative below -- which always builds
                       a plain FLOW3_VALUE_KIND_SIGNED value, discarding
                       pointer identity entirely regardless of operand
                       kind. flow3_alternative_can_be_zero treats that
                       degraded ANY as "could be zero", so an ordinary
                       non-optional pointer lost its non-null guarantee
                       the moment it went through `+`/`-` with an
                       integer -- e.g. `(unsigned char*)(p + 0)` warned
                       "possible null pointer" even though p itself was
                       never null. Same class of bug already fixed for
                       p++/p--/p+=n/p-=n (see the increment/decrement and
                       compound-assignment cases above), just reached
                       through the plain binary operator instead.

                       Keep the pointer operand's own alternative
                       unchanged: offsetting a valid pointer can never
                       turn it into a null one (and cake doesn't track
                       bounds precisely enough here to say anything
                       sharper about which element it now points to).
                    */
                    {
                        /* A "pointer base" is either a real pointer value (PTR)
                           or a non-null address (REF) -- the latter is how an
                           array-decay operand like `int arr[10]` shows up
                           (`arr + 3`). Offsetting either by an integer can never
                           produce a null pointer, so keep the result non-null
                           instead of degrading to a plain SIGNED ANY. Without
                           the REF case, `arr + 3` was wrongly treated as
                           possibly null. */
                        const bool lnum2 = (lval->value_kind == FLOW3_VALUE_KIND_SIGNED ||
                            lval->value_kind == FLOW3_VALUE_KIND_UNSIGNED);
                        const bool rnum2 = (rval->value_kind == FLOW3_VALUE_KIND_SIGNED ||
                            rval->value_kind == FLOW3_VALUE_KIND_UNSIGNED);
                        const bool lbase = (lval->value_kind == FLOW3_VALUE_KIND_PTR) ||
                        (lval->value_kind == FLOW3_VALUE_KIND_REF && lval->value.p != NULL);
                        const bool rbase = (rval->value_kind == FLOW3_VALUE_KIND_PTR) ||
                        (rval->value_kind == FLOW3_VALUE_KIND_REF && rval->value.p != NULL);

                        const struct flow3_alternative* _Opt base = NULL;
                        if (lbase && rnum2 && (op == '+' || op == '-'))
                            base = lval;
                        else if (rbase && lnum2 && op == '+')
                            base = rval;

                        if (base != NULL)
                        {
                            if (base->value_kind == FLOW3_VALUE_KIND_PTR)
                            {
                                /* Keep the pointer alternative unchanged. */
                                flow3_alternatives_add(&result_alts, base);
                            }
                            else
                            {
                                /* Array-decay / address base: the offset result
                                   is a non-null pointer of unknown exact target. */
                                struct flow3_alternative a = {
                                    .value_kind = FLOW3_VALUE_KIND_PTR,
                                    .value = {.p = base->value.p},
                                    .value_relation = FLOW3_RELATION_NOT_EQUAL,
                                    .imaginary = FLOW3_IMAGINARY_NONE,
                                    .origin = flow3_origin_more_specific(lval->origin, rval->origin),
                                    .line = p_result->first_token->line
                                };
                                flow3_alternatives_add(&result_alts, &a);
                            }
                            continue;
                        }
                    }

                    // Build the result alternative. Tag it with the branch
                    // the operand pair belongs to (the more specific of the
                    // two compatible origins) so the value stays correlated.
                    struct flow3_alternative a = {
                        .value_kind = FLOW3_VALUE_KIND_SIGNED,
                        .value_relation = result_rel,
                        .imaginary = FLOW3_IMAGINARY_NONE,
                        .origin = flow3_origin_more_specific(lval->origin, rval->origin),
                        .line = p_result->first_token->line
                    }; // could be refined based on type
                    if (result_rel == FLOW3_RELATION_EQUAL)
                    {
                        a.value.i = result_val;
                    }
                    else
                    {
                        a.value.i = ANY_VALUE;
                    }

                    flow3_alternatives_add(&result_alts, &a);
                }
            }
        }
    }

    // ---- Write the computed alternatives into the result expression ----
    struct flow3_key_alternatives* _Opt dst = flow3_map_find_add(ctx->p_current_flow3_map, &p_result->object);
    if (dst)
    {
        flow3_alternatives_clear(&dst->alternatives);
        if (result_alts.size > 0)
            dst->alternatives = result_alts; // move
        else
        {
            struct flow3_alternative a = {
                .value_kind = FLOW3_VALUE_KIND_SIGNED,
                .value = {.i = ANY_VALUE},
                .value_relation = FLOW3_RELATION_ANY,
                .imaginary = FLOW3_IMAGINARY_NONE,
                .origin = ctx->p_current_flow3_map,
                .line = p_result->first_token->line
            };
            flow3_alternatives_add(&dst->alternatives, &a);
        }
    }
    else
    {
        flow3_alternatives_clear(&result_alts);
    }
}

/* Cast one concrete value alternative to `p_target_type`, appending the
   result to `out` with `origin`. Returns false if the value can't be cast
   exactly (caller falls back to ANY). */
/* Seed an expression's result as a known EQUAL constant read from its parser
         object (used for operators the parser already folds but flow3 didn't seed,
         e.g. sizeof / _Alignof). The result is size_t-like, so it's unsigned. */
static void flow3_seed_constant_result(struct flow3_visit_ctx* ctx, const struct expression* p_expression)
{
    if (!object_has_known_value(&p_expression->object))
        return;

    struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
    if (e == NULL) return;

    flow3_alternatives_clear(&e->alternatives);
    struct flow3_alternative a = {
        .value_kind = FLOW3_VALUE_KIND_UNSIGNED,
        .value = {.u = object_to_unsigned_long_long(&p_expression->object)},
        .value_relation = FLOW3_RELATION_EQUAL,
        .imaginary = FLOW3_IMAGINARY_NONE,
        .origin = ctx->p_current_flow3_map,
        .line = p_expression->first_token->line
    };
    flow3_alternatives_add(&e->alternatives, &a);
}

/* When a struct member is read but has no flow state yet -- e.g. it belongs to
   the pointee of a narrowed _Opt pointer, whose members parameter-init did not
   seed -- fall back to its DECLARED nullability: a non-optional pointer member
   is non-null by contract. Without this, reading such a member came back
   possibly-null and a later dereference falsely warned. Only touches unseeded
   non-_Opt pointer members. */
static void flow3_seed_member_default(struct flow3_visit_ctx* ctx, struct object* _Opt member_obj, int line)
{
    if (member_obj == NULL)
        return;
    if (flow3_map_search_up(ctx->p_current_flow3_map, member_obj) != NULL)
        return; /* already has flow state */

    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;
    if (type_is_pointer(&member_obj->type))
    {
        /* Seed an unseeded pointer member from its declared nullability:
             non-_Opt member -> non-null (!= null) by contract;
             _Opt member     -> possibly-null (ANY).
           Leaving an _Opt member EMPTY was wrong: a later `m != NULL` then
           found no null alternative and treated the pointer as definitely
           non-null, killing the else branch (e.g. `list->head = old->next;`
           where next is _Opt, then `if (list->head != NULL) ... else ...`). */
        const bool is_opt = type_is_opt(&member_obj->type, nullable_enabled);
        struct flow3_key_alternatives* _Opt me = flow3_map_find_add(ctx->p_current_flow3_map, member_obj);
        if (me == NULL)
            return; /* no entry to seed */

        flow3_alternatives_clear(&me->alternatives);

        if (is_opt)
        {
            /* Model an _Opt member the same way an _Opt PARAMETER is modeled:
               two correlated arms, and give the non-null arm a CONCRETE pointee.

               A single `{PTR, ANY, p == NULL}` says "may be null" but points at
               nothing, so `a->b->c` had to invent a pointee at the access and
               the narrowing from `if (a->b)` was fragile -- it survived a
               straight-line guard but not a guard inside a loop, whose body is
               re-analysed and re-merged (codegen.c:533). Giving the non-null arm
               a real object up front makes the deeper member a stable object
               that narrowing can attach to. */
            struct flow3_map* p_null_map =
            flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, FLOW3_MAP_OPT_MEM_NULL);
            struct flow3_map* p_nonnull_map =
            flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, FLOW3_MAP_OPT_MEM_NONNULL);

            struct object* _Opt p_pointed = NULL;
            {
                struct type pointed_type = type_remove_pointer(&member_obj->type);
                if (!type_is_void(&pointed_type))
                {
                    p_pointed = flow3_allocated_object_arena_new(&ctx->allocated_object_arena);
                    if (p_pointed != NULL)
                        make_object(&pointed_type, p_pointed, MAKE_STATE_ANY, ctx->ctx->options.target);
                }
                type_destroy(&pointed_type);
            }

            struct flow3_alternative a_null = {
                .value_kind = FLOW3_VALUE_KIND_PTR,
                .value = {.p = NULL},
                .value_relation = FLOW3_RELATION_EQUAL,
                .imaginary = FLOW3_IMAGINARY_NONE,
                .origin = p_null_map,
                .line = line
            };
            flow3_alternatives_add(&me->alternatives, &a_null);

            struct flow3_alternative a_nonnull = {
                .value_kind = FLOW3_VALUE_KIND_PTR,
                .value = {.p = p_pointed},
                .value_relation = p_pointed != NULL ? FLOW3_RELATION_EQUAL : FLOW3_RELATION_NOT_EQUAL,
                .imaginary = FLOW3_IMAGINARY_NONE,
                .origin = p_nonnull_map,
                .line = line
            };
            flow3_alternatives_add(&me->alternatives, &a_nonnull);
        }
        else
        {
            struct flow3_alternative a = {
                .value_kind = FLOW3_VALUE_KIND_PTR,
                .value = {.p = NULL},
                .value_relation = FLOW3_RELATION_NOT_EQUAL,
                .imaginary = FLOW3_IMAGINARY_NONE,
                .origin = ctx->p_current_flow3_map,
                .line = line
            };
            flow3_alternatives_add(&me->alternatives, &a);
        }
    }
}

static bool flow3_cast_one_value(struct flow3_visit_ctx* ctx,
    const struct flow3_alternative* alt,
    const struct type* p_target_type,
    struct flow3_alternatives* out,
    const struct flow3_map* origin,
    int line)
{
    if (alt->imaginary == FLOW3_IMAGINARY_ABSENT)
    {
        return false;
    }

    if (alt->value_kind == FLOW3_VALUE_KIND_PTR && type_is_pointer(p_target_type) &&
        (alt->value_relation == FLOW3_RELATION_EQUAL || alt->value_relation == FLOW3_RELATION_NOT_EQUAL))
    {
        /* A pointer known only as "non-null, unknown target" (NOT_EQUAL,
           the ordinary state of a non-_Opt pointer) still has that fact
           preserved across a cast -- casting doesn't change whether a
           pointer is null. Handling only the EQUAL case here (e.g. a
           literal address, or the null constant) dropped this common
           case to the "can't handle" fallback below, which reseeds the
           result as fully unknown and false-warns it as possibly null
           at its next use (see samples/flow3/cast-preserves-not-equal-pointer.c). */
        struct flow3_alternative tagged = *alt;

        /* A void*-returning allocator (calloc/malloc) gets a fabricated
           pointee typed `void` (see the _Opt pointer-return handling in
           EXPR_POSTFIX_FUNCTION_CALL). Casting that value to a concrete
           pointer type (e.g. `struct expr* p = calloc(...)`) kept pointing
           at that same void-typed pointee, so any later `p->member`
           silently failed to resolve (object_get_member on a void object
           has no members) -- which meant the arrow access added no REF
           alternative for its result at all, which in turn made an
           unrelated `&p->member` look like it carried no fact whatsoever
           and get flagged as possibly null downstream (traced from
           expressions.c's `struct type* p_type = &new_expression->type;`
           right after `new_expression = calloc(...)`). Re-fabricate a
           pointee typed to match the CAST'S target pointee type instead
           of carrying the stale void-typed one forward. */
        if (tagged.value.p != NULL && type_is_void(&tagged.value.p->type))
        {
            struct type target_pointee = type_remove_pointer(p_target_type);
            if (!type_is_void(&target_pointee))
            {
                struct object* _Opt p_new = flow3_allocated_object_arena_new(&ctx->allocated_object_arena);
                if (p_new != NULL)
                {
                    make_object(&target_pointee, p_new, MAKE_STATE_ANY, ctx->ctx->options.target);
                    tagged.value.p = p_new;
                }
            }
            type_destroy(&target_pointee);
        }

        tagged.origin = origin;
        tagged.line = line;
        flow3_alternatives_add(out, &tagged);
        return true;
    }

    if (alt->value_relation != FLOW3_RELATION_EQUAL)
    {
        return false;
    }

    if (alt->value_kind == FLOW3_VALUE_KIND_SIGNED || alt->value_kind == FLOW3_VALUE_KIND_UNSIGNED)
    {
        long long val = (alt->value_kind == FLOW3_VALUE_KIND_SIGNED) ? alt->value.i : (long long)alt->value.u;
        if (type_is_integer(p_target_type))
        {
            struct flow3_alternative a = {
                .value_kind = FLOW3_VALUE_KIND_SIGNED,
                .value = {.i = flow3_cast_integer_value(ctx, val, p_target_type)},
                .value_relation = FLOW3_RELATION_EQUAL,
                .imaginary = alt->imaginary, .origin = origin, .line = line
            };
            flow3_alternatives_add(out, &a);
            return true;
        }
        if (type_is_pointer(p_target_type) && val == 0)
        {
            /* (T*)0 is the null pointer constant (e.g. NULL == ((void*)0)). */
            struct flow3_alternative a = {
                .value_kind = FLOW3_VALUE_KIND_PTR, .value = {.p = NULL},
                .value_relation = FLOW3_RELATION_EQUAL,
                .imaginary = alt->imaginary, .origin = origin, .line = line
            };
            flow3_alternatives_add(out, &a);
            return true;
        }
        return false; /* nonzero int->ptr, or int->other */
    }

    if (alt->value_kind == FLOW3_VALUE_KIND_PTR && type_is_pointer(p_target_type))
    {
        struct flow3_alternative tagged = *alt;
        tagged.origin = origin;
        tagged.line = line;
        flow3_alternatives_add(out, &tagged);
        return true;
    }

    return false;
}

static struct flow3_branch_pair flow3_visit_expression(struct flow3_visit_ctx* ctx, const struct expression* _Opt p_expression)
{
    /* left/right are _Owner _Opt in the AST, and callers hand them straight
       in; an absent operand is nothing to visit. */
    if (p_expression == NULL || ctx->p_current_flow3_map == NULL)
    {
        struct flow3_branch_pair empty = { 0 };
        return empty;
    }

    struct flow3_map* p_before = ctx->p_current_flow3_map;
    (void)p_before; /* used in some cases below */

    try
    {
        switch (p_expression->expression_type)
        {
        case EXPR_INVALID:
            _Assert(false);
            break;

        case EXPR_PRIMARY__FUNC__:
            break;

        case EXPR_PRIMARY_ENUMERATOR:
            /* An enumerator is a compile-time constant (the parser folded its
           value into the expression object). Seed it so it can be used in
           flow-checked comparisons, like a numeric literal. Enum values may be
           negative, so seed it as signed. */
            if (object_has_known_value(&p_expression->object))
            {
                struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                if (e == NULL) throw;
                flow3_alternatives_clear(&e->alternatives);
                struct flow3_alternative a = {
                    .value_kind = FLOW3_VALUE_KIND_SIGNED,
                    .value = {.i = object_to_signed_long_long(&p_expression->object)},
                    .value_relation = FLOW3_RELATION_EQUAL,
                    .imaginary = FLOW3_IMAGINARY_NONE,
                    .origin = ctx->p_current_flow3_map,
                    .line = p_expression->first_token->line
                };
                flow3_alternatives_add(&e->alternatives, &a);
            }
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
                    {
                        struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, p_obj);
                        if (e == NULL) throw;
                        flow3_alternatives_clear(&e->alternatives);

                        struct flow3_alternative a = {
                            .value_kind = FLOW3_VALUE_KIND_SIGNED,
                            .value = {.i = ANY_VALUE},
                            .value_relation = FLOW3_RELATION_ANY,
                            .imaginary = FLOW3_IMAGINARY_NONE,
                            .origin = ctx->p_current_flow3_map,
                            .line = p_expression->first_token->line
                        };

                        /* A pointer global respects its declared nullability, just like
           a parameter or member: a non-_Opt global pointer is non-null
           (e.g. `stdout`), an _Opt one is possibly-null. Without this a
           plain `FILE* stdout` read as ANY and passing it to a
           non-nullable parameter falsely warned. */
                        if (type_is_pointer(&p_expression->type))
                        {
                            a.value_kind = FLOW3_VALUE_KIND_PTR;
                            a.value.p = NULL;
                            a.value_relation = type_is_opt(&p_expression->type, ctx->ctx->options.null_checks_enabled)
                            ? FLOW3_RELATION_ANY
                            : FLOW3_RELATION_NOT_EQUAL;
                        }

                        flow3_alternatives_add(&e->alternatives, &a);
                    }
                }
                else if (!type_is_function(&p_expression->type) &&
                    p_obj->state == CONSTANT_VALUE_STATE_CONSTANT &&
                    flow3_map_search_up(ctx->p_current_flow3_map, p_obj) == NULL)
                {
                    /* Compile-time constant (e.g. constexpr) whose value was not
       carried over from its own declaration analysis (each top-level
       declaration gets a fresh flow map). Seed it with its real,
       unchanging value instead of leaving it untracked. */
                    struct flow3_alternative value = { 0 };
                    if (type_is_pointer(&p_obj->type))
                    {
                        value.value_kind = FLOW3_VALUE_KIND_PTR;
                        value.value.p = (void*)(uintptr_t)p_obj->value.host_u_long_long;
                    }
                    else if (type_is_signed(&p_obj->type))
                    {
                        value.value_kind = FLOW3_VALUE_KIND_SIGNED;
                        value.value.i = p_obj->value.host_long_long;
                    }
                    else
                    {
                        value.value_kind = FLOW3_VALUE_KIND_UNSIGNED;
                        value.value.u = p_obj->value.host_u_long_long;
                    }

                    struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, p_obj);
                    if (e == NULL) throw;
                    flow3_alternatives_clear(&e->alternatives);
                    struct flow3_alternative a = {
                        .value_kind = value.value_kind,
                        .value = value.value,
                        .value_relation = FLOW3_RELATION_EQUAL,
                        .imaginary = FLOW3_IMAGINARY_NONE,
                        .origin = ctx->p_current_flow3_map,
                        .line = p_expression->first_token->line
                    };
                    flow3_alternatives_add(&e->alternatives, &a);
                }

                _Assert(p_expression->declarator != NULL);

                {
                    struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                    if (e == NULL) throw;
                    flow3_alternatives_clear(&e->alternatives);
                    struct flow3_alternative a = {
                        .value_kind = FLOW3_VALUE_KIND_REF,
                        .value = {.p = p_obj},
                        .value_relation = FLOW3_RELATION_EQUAL,
                        .imaginary = FLOW3_IMAGINARY_NONE,
                        .origin = ctx->p_current_flow3_map,
                        .line = p_expression->first_token->line
                    };
                    flow3_alternatives_add(&e->alternatives, &a);
                }

                /* Build true/false branch maps narrowed on this variable. */
                const struct object* p_obj2 = &p_expression->declarator->object;
                struct flow3_map* _Opt p_true = flow3_narrow_map_branch(&ctx->flow3_map_arena, ctx->p_current_flow3_map, p_obj2, true, p_expression, p_expression->first_token->line);
                struct flow3_map* _Opt p_false = flow3_narrow_map_branch(&ctx->flow3_map_arena, ctx->p_current_flow3_map, p_obj2, false, p_expression, p_expression->first_token->line);
                if (p_true == NULL || p_false == NULL)
                    throw;

                flow3_tag_branch_pair(p_true, p_false);
                return (struct flow3_branch_pair) { p_true, p_false };
            }

        case EXPR_PRIMARY_PARENTHESIS:
            {
                _Assert(p_expression->right != NULL);
                const struct expression* p_inner = skip_parenthesis(p_expression->right);
                struct flow3_branch_pair paren_pair = flow3_visit_expression(ctx, p_inner);

                /*
   Narrowing (the branch pair) already passes through correctly,
   but the inner expression's computed VALUE lives keyed on the
   inner node's own &object -- a synthesized temporary (e.g. a
   nested conditional operator's result) has nowhere else to
   live. Any caller that looks up a value by THIS node's
   address (e.g. EXPR_CONDITIONAL's own result-value merge,
   when one arm of a ternary is a parenthesized sub-expression:
   `a ? (b ? 1 : 2) : 3`) would otherwise find nothing, since
   `(b ? 1 : 2)` and `b ? 1 : 2` are different expression nodes
   with different &object storage. Copy the value forward.
*/
                const struct flow3_key_alternatives* _Opt p_inner_entry =
                flow3_map_search_up(ctx->p_current_flow3_map, &p_inner->object);
                if (p_inner_entry)
                {
                    struct flow3_key_alternatives* _Opt p_entry = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                    if (p_entry == NULL) throw;
                    if (p_entry)
                    {
                        flow3_alternatives_clear(&p_entry->alternatives);
                        flow3_alternatives_append(&p_entry->alternatives, &p_inner_entry->alternatives);
                    }
                }

                return paren_pair;
            }

        case EXPR_PRIMARY_STATEMENT_EXPRESSION:
            _Assert(p_expression->compound_statement != NULL);
            flow3_visit_compound_statement(ctx, p_expression->compound_statement);
            break;

        case EXPR_PRIMARY_STRING_LITERAL:
            {
                /*
   A string literal has static storage duration and its address
   is never null -- unlike EXPR_PRIMARY_CHAR_LITERAL/NUMBER/
   PREDEFINED_CONSTANT below, its ->object is an array (it has
   per-character members), not a scalar constant, so
   object_to_signed_long_long(&p_expression->object) has no
   sensible integer value to return for it (falls through to the
   `default: _Assert(0); return 0;` branch, which in an NDEBUG
   build silently yields 0). That previously tagged every string
   literal as value_kind SIGNED, value 0, relation EQUAL -- i.e.
   indistinguishable from a literal `0`/NULL -- which would have
   misfired "passing a possible null pointer" once a pointer
   initialized from it reached a non-nullable parameter.

   Seed a SIGNED, definitely-nonzero, definitely-initialized
   placeholder value instead (1, not a real numeric meaning --
   string literals are never used in an arithmetic context, only
   null/uninitialized-checked as pointers). Deliberately NOT
   FLOW3_VALUE_KIND_PTR pointing at &p_expression->object: that
   was tried first, but it makes flow3_check_object_init_assigment
   treat the literal as a trackable "pointee" and walk into
   flow3_check_object_access, which recurses into the literal's
   per-character member objects and calls
   flow3_get_only_member_name on their (unset) member_designator
   -- a NULL deref/crash, since those synthetic char members were
   never given real struct-member names.
*/
                struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                if (e == NULL) throw;
                flow3_alternatives_clear(&e->alternatives);
                struct flow3_alternative a = {
                    .value_kind = FLOW3_VALUE_KIND_SIGNED,
                    .value = {.i = 1},
                    .value_relation = FLOW3_RELATION_EQUAL,
                    .imaginary = FLOW3_IMAGINARY_NONE,
                    .origin = ctx->p_current_flow3_map,
                    .line = p_expression->first_token->line
                };
                flow3_alternatives_add(&e->alternatives, &a);
            }
            break;

        case EXPR_PRIMARY_CHAR_LITERAL:
        case EXPR_PRIMARY_NUMBER:
        case EXPR_PRIMARY_PREDEFINED_CONSTANT:
            {
                {
                    struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                    if (e == NULL) throw;
                    flow3_alternatives_clear(&e->alternatives);
                    struct flow3_alternative a = {
                        .value_kind = FLOW3_VALUE_KIND_SIGNED,
                        .value = {.i = object_to_signed_long_long(&p_expression->object)},
                        .value_relation = FLOW3_RELATION_EQUAL,
                        .imaginary = FLOW3_IMAGINARY_NONE,
                        .origin = ctx->p_current_flow3_map,
                        .line = p_expression->first_token->line
                    };
                    flow3_alternatives_add(&e->alternatives, &a);
                }
            }
            break;

        case EXPR_PRIMARY_GENERIC:
            _Assert(p_expression->generic_selection != NULL);
            flow3_visit_generic_selection(ctx, p_expression->generic_selection);
            break;

        case EXPR_POSTFIX_DOT:
            {
                const int member_index = p_expression->member_index;
                flow3_visit_expression(ctx, p_expression->left);

                struct flow3_key_alternatives* _Opt result_entry = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                if (result_entry == NULL) throw;
                flow3_alternatives_clear(&result_entry->alternatives);

                const struct flow3_key_alternatives* _Opt p_left_alternatives = flow3_map_search_up(ctx->p_current_flow3_map, &skip_parenthesis(p_expression->left)->object);

                struct flow3_map* _Opt p_true = flow3_map_arena_new_branch(&ctx->flow3_map_arena, ctx->p_current_flow3_map, true, p_expression);
                struct flow3_map* _Opt p_false = flow3_map_arena_new_branch(&ctx->flow3_map_arena, ctx->p_current_flow3_map, false, p_expression);
                if (p_true == NULL || p_false == NULL)
                    throw;

                flow3_tag_branch_pair(p_true, p_false);

                for (int i = 0; p_left_alternatives != NULL && i < p_left_alternatives->alternatives.size; i++)
                {
                    const struct flow3_alternative* p_left_alternative = p_left_alternatives->alternatives.data[i];

                    if (p_left_alternative->value_relation == FLOW3_RELATION_EQUAL &&
                        p_left_alternative->value_kind == FLOW3_VALUE_KIND_REF &&
                        p_left_alternative->value.p != NULL)
                    {
                        struct object* _Opt p_member = object_get_member(p_left_alternative->value.p, member_index);

                        /* Give an unseeded member its default flow state before
                           anything narrows or reads it, exactly like
                           EXPR_POSTFIX_ARROW already does for `p->member`. Without
                           this, a member reached through `.` (as opposed to `->`)
                           never got the "possibly null" two-alternative seed at
                           all -- it kept whatever raw state make_object gave it
                           (e.g. plain ANY from an on-demand-fabricated base
                           object), which no later `!= 0` comparison can narrow.
                           A second, independent evaluation of the SAME `a.b`
                           expression (e.g. re-evaluating an initializer's RHS)
                           would then resolve `b`'s object identity without ever
                           having been through the seeding path the first
                           evaluation's narrowing depended on, so the narrowed
                           fact silently failed to carry over. Confirmed via
                           `if (p) { if (p->data.p_enumerator != 0) { T* p_enumerator
                           = p->data.p_enumerator; use(p_enumerator); } }` where
                           `data` is reached via `->` (seeded) but `.p_enumerator`
                           is reached via `.` (previously never seeded). */
                        flow3_seed_member_default(ctx, p_member, p_expression->first_token->line);

                        {
                            struct flow3_alternative a = {
                                .value_kind = FLOW3_VALUE_KIND_REF,
                                .value = {.p = p_member},
                                .value_relation = FLOW3_RELATION_EQUAL,
                                .imaginary = FLOW3_IMAGINARY_NONE,
                                .origin = ctx->p_current_flow3_map,
                                .line = p_expression->first_token->line
                            };
                            flow3_alternatives_add(&result_entry->alternatives, &a);
                        }

                        flow3_narrow_map_into(p_true, ctx->p_current_flow3_map, p_member, true, p_expression->first_token->line);
                        flow3_narrow_map_into(p_false, ctx->p_current_flow3_map, p_member, false, p_expression->first_token->line);
                    }
                }

                /* Narrow on the member field used as bool. */
                return (struct flow3_branch_pair) { p_true, p_false };
            }

        case EXPR_POSTFIX_ARROW:
            {
                _Assert(p_expression->left != NULL);

                flow3_visit_expression(ctx, p_expression->left);

                const int member_index = p_expression->member_index;
                struct marker marker = expression_to_marker(p_expression->left);

                const struct flow3_key_alternatives* _Opt p_left_alternatives = flow3_map_search_up(ctx->p_current_flow3_map, &p_expression->left->object);

                struct flow3_key_alternatives* _Opt result_entry = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                if (result_entry == NULL) throw;
                flow3_alternatives_clear(&result_entry->alternatives);

                struct flow3_map* _Opt p_true = flow3_map_arena_new_branch(&ctx->flow3_map_arena, ctx->p_current_flow3_map, true, p_expression);
                struct flow3_map* _Opt p_false = flow3_map_arena_new_branch(&ctx->flow3_map_arena, ctx->p_current_flow3_map, false, p_expression);
                if (p_true == NULL || p_false == NULL)
                    throw;

                flow3_tag_branch_pair(p_true, p_false);

                bool any_member_resolved = false;

                /* A merge (e.g. a while-loop's "ran and exited null" vs "never
           entered, started null" paths) can leave the base pointer with
           more than one alternative that is independently null. Without
           this guard, the loop below over alternatives reported the same
           "operator -> applied to a possible null pointer" diagnostic once
           per null alternative instead of once per access
           (null-narrow-while-traverse-post-loop.c warned twice on one
           line). */
                bool null_deref_reported = false;

                /* Same one-report-per-access rule for the pointee's lifetime:
               a merged pointer can carry many alternatives aiming at the
               same pointee, and without this the identical "operator ->:
               pointed object lifetime has ended" was emitted once per
               (pointer alternative x pointee alternative) pair -- 30 times
               on a single line of cake's own parser.c. */
                bool lifetime_ended_reported = false;

                /*
   ON-DEMAND pointee fabrication (arena-allocated).

   A base modeled `{PTR, NOT_EQUAL, value.p == NULL}` is "non-null but
   points at nothing we track". Every member read through it is then
   unresolvable and -- worse -- UNNARROWABLE: there is no object for
   `if (pX->p)` to narrow, so the guard cannot take effect and the member
   stays possibly-null forever (safety-049).

   Fabricate the pointee HERE, at the access, rather than eagerly at
   every pointer-returning call: most such results are never
   dereferenced, and allocating for all of them would grow the arena for
   nothing.

   make_object only -- deliberately NOT flow3_object_init, which
   pre-seeds every _Opt member as a correlated null/non-null pair, i.e.
   asserts "possibly null" about members nothing is known about. Left
   unseeded, flow3_seed_member_default seeds each member on first read
   from its declared nullability, and because the member is now a REAL
   object a guard can narrow it.
*/
                if (p_left_alternatives != NULL && type_is_pointer(&p_expression->left->type))
                {
                    /* The PTR alternatives usually live on the VARIABLE object, reached
       through a REF from this expression's temporary. Fabricate into
       that entry -- keying off the temporary would mint a fresh pointee
       on every read, so a guard narrowed on one read would not be
       visible on the next (safety-049). */
                    const struct object* p_key = &p_expression->left->object;
                    if (p_left_alternatives->alternatives.size > 0 &&
                        p_left_alternatives->alternatives.data[0]->value_kind == FLOW3_VALUE_KIND_REF &&
                        p_left_alternatives->alternatives.data[0]->value.p != NULL)
                    {
                        const struct object* p_ref = p_left_alternatives->alternatives.data[0]->value.p;
                        const struct flow3_key_alternatives* _Opt p_resolved =
                        flow3_map_search_up(ctx->p_current_flow3_map, p_ref);
                        if (p_resolved != NULL)
                        {
                            p_key = p_ref;
                            p_left_alternatives = p_resolved;
                        }
                    }

                    /*
                   A pointee can also be present but USELESS: `obj = calloc(...)`
                   is an implicit void*->T* conversion with no EXPR_CAST node, so
                   the fabricated pointee behind obj stays typed `void`.
                   object_get_member(void_obj, ...) then always returns NULL, so
                   every `obj->m` falls back to seeding an untyped ANY -- member
                   writes and narrowings on it go nowhere, and the member reads as
                   possibly-null forever. Re-fabricate with the base pointer's real
                   pointed type in that case, exactly as for a missing pointee.
                   See samples/flow3/branch-merge-loses-ref-nonnull.c.
                */
                    bool needs_pointee = false;
                    for (int i = 0; i < p_left_alternatives->alternatives.size; i++)
                    {
                        const struct flow3_alternative* a = p_left_alternatives->alternatives.data[i];
                        if (a->value_kind == FLOW3_VALUE_KIND_PTR &&
                            a->value.p == NULL &&
                            a->value_relation == FLOW3_RELATION_NOT_EQUAL)
                        {
                            needs_pointee = true;
                            break;
                        }
                    }

                    if (needs_pointee)
                    {
                        struct type pointed_type = type_remove_pointer(&p_expression->left->type);
                        if (!type_is_void(&pointed_type))
                        {
                            struct flow3_alternatives rebuilt = { 0 };
                            /* One pointee for the WHOLE rebuild pass, not one per
                               matching alternative, and not cached across separate
                               accesses either -- both used to be handled by a
                               ctx-level fabricated_pointees[] side-cache keyed by
                               p_key, but that cache had two real problems: (1) a
                               base coming out of an if/else merge with no concrete
                               identity (e.g. `p = make();` in both arms) is modeled
                               as MULTIPLE correlated alternatives that each
                               independently match this "known non-null, no
                               pointee" pattern -- same variable, same access,
                               different provenance -- so fabricating one pointee
                               PER ALTERNATIVE split one logical `*p` into several,
                               making a later write through one alternative (e.g.
                               `p->text = strdup(...)`) invisible when read back
                               through a different alternative of the SAME p at the
                               SAME statement (owner-if-else-052.c); and (2) the
                               cache was never invalidated on reassignment (`p =
                               q;` left a stale {base=p, pointee=old *p} entry
                               behind for flow3_predicate_invalidate to miss). The
                               ACTUAL fix for "same base, same pointee" is just to
                               write the fabricated pointer straight back into
                               p_key's own map entry below (already done) and let
                               ordinary flow3_map_search_up find it on every later
                               access -- no separate cache needed once fabrication
                               isn't fragmented per-alternative within a single
                               access. Confirmed via the full flow3 suite with the
                               cache removed entirely: one pre-existing failure
                               (owner-if-else-052.c) is FIXED by this change, none
                               newly broken. */
                            struct object* _Opt p_new_shared = NULL;
                            for (int i = 0; i < p_left_alternatives->alternatives.size; i++)
                            {
                                struct flow3_alternative a = *p_left_alternatives->alternatives.data[i];
                                if (a.value_kind == FLOW3_VALUE_KIND_PTR &&
                                    a.value.p == NULL &&
                                    a.value_relation == FLOW3_RELATION_NOT_EQUAL)
                                {
                                    if (p_new_shared == NULL)
                                    {
                                        p_new_shared = flow3_allocated_object_arena_new(&ctx->allocated_object_arena);
                                        if (p_new_shared != NULL)
                                        {
                                            make_object(&pointed_type, p_new_shared, MAKE_STATE_ANY, ctx->ctx->options.target);
                                        }
                                    }

                                    if (p_new_shared != NULL)
                                    {
                                        a.value.p = p_new_shared;
                                        a.value_relation = FLOW3_RELATION_EQUAL;
                                    }
                                }
                                flow3_alternatives_add(&rebuilt, &a);
                            }

                            struct flow3_key_alternatives* _Opt e_base = flow3_map_find_add(ctx->p_current_flow3_map, p_key);
                            if (e_base == NULL) throw;
                            if (e_base != NULL)
                            {
                                flow3_alternatives_clear(&e_base->alternatives);
                                e_base->alternatives = rebuilt; /* move */
                                p_left_alternatives = e_base;
                            }
                            else
                            {
                                flow3_alternatives_clear(&rebuilt);
                            }
                        }
                        type_destroy(&pointed_type);
                    }
                }

                if (p_left_alternatives != NULL)
                {
                    for (int i = 0; i < p_left_alternatives->alternatives.size; i++)
                    {
                        const struct flow3_alternative* ptr_alt = p_left_alternatives->alternatives.data[i];

                        if (ptr_alt->imaginary == FLOW3_IMAGINARY_ABSENT)
                            continue;

                        /* Resolve LHS to concrete pointer alternatives */
                        const struct flow3_key_alternatives* _Opt p_pointer_alts = NULL;
                        if (ptr_alt->value_relation == FLOW3_RELATION_EQUAL &&
                            ptr_alt->value_kind == FLOW3_VALUE_KIND_REF &&
                            ptr_alt->value.p != NULL)
                        {
                            p_pointer_alts = flow3_map_search_up(ctx->p_current_flow3_map, ptr_alt->value.p);
                        }
                        else if (ptr_alt->value_kind == FLOW3_VALUE_KIND_PTR)
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
                                const struct flow3_alternative* p_pointer_alt = p_pointer_alts->alternatives.data[k];

                                if (p_pointer_alt->imaginary == FLOW3_IMAGINARY_ABSENT)
                                    continue;

                                /* Null check (skipped in unevaluated contexts like
                   sizeof/_Alignof: the -> is never applied at runtime). */
                                if (p_pointer_alt->value_relation == FLOW3_RELATION_EQUAL &&
                                    p_pointer_alt->value_kind == FLOW3_VALUE_KIND_PTR &&
                                    p_pointer_alt->value.p == NULL)
                                {
                                    if (!ctx->expression_is_not_evaluated && !null_deref_reported)
                                    {
                                        null_deref_reported = true;
                                        struct osstream ss = { 0 };
                                        flow3_expression_to_string(p_expression->left, &ss);
                                        diagnostic(W_FLOW_NULL_DEREFERENCE, ctx->ctx, NULL, &marker,
                                            "'%s->' possible null dereference",
                                            ss.c_str ? ss.c_str : "");
                                        ss_close(&ss);
                                    }
                                    continue;
                                }

                                if (p_pointer_alt->value_kind != FLOW3_VALUE_KIND_PTR || p_pointer_alt->value.p == NULL)
                                    continue;

                                const struct object* p_pointed_obj = p_pointer_alt->value.p;

                                struct object* _Opt member_obj = object_get_member(p_pointed_obj, member_index);
                                if (member_obj == NULL)
                                {
                                    //_Assert(false);
                                    continue;
                                }

                                /* Lifetime check for the ACCESSED MEMBER only. An
                               aggregate's own top-level entry is never marked ended
                               -- only its LEAF members are, by
                               flow3_map_set_object_lifetime_ended -- so walk for
                               "any leaf ended" the same way flow3_check_object_access
                               already does. This must be scoped to member_obj (the
                               specific member this -> access reaches), NOT the whole
                               p_pointed_obj: a _Dtor call on one member (e.g.
                               hashmap_destroy(&ctx->tag_names)) marks only that
                               member's leaves ended, and accessing an unrelated
                               sibling member (ctx->structs_map) must not be flagged.
                               When the WHOLE object's lifetime ended (e.g. free(p)),
                               every leaf -- including this member's -- was marked
                               ended, so this still catches that case.

                               The origin filter must be ctx->p_current_flow3_map (the
                               map at THIS access), not p_pointer_alt->origin (wherever
                               the pointer's OWN value was last set). Those are
                               different questions: p_pointer_alt->origin answers "is
                               this ended fact compatible with how we got this pointer
                               value", but member_obj can be reached through an ALIAS
                               whose own origin (e.g. an _Opt-pointer correlation
                               branch set at declaration time) has no ancestor
                               relationship to the branch the free() that ended it
                               actually ran in, even though both are simultaneously
                               active right here. Confirmed via static_debug(0): `struct
                               X* _Opt p = pX; free(pY); p->pY->i = 1;` (pX->pY == pY)
                               left p_pointer_alt->origin at pY's own "opt-nonnull" seed
                               origin while the real ENDED fact on .i carried the
                               *current* branch's origin ("var-true") -- an unrelated
                               sibling from p_pointer_alt->origin's point of view, so
                               the filter silently rejected a fact that was genuinely
                               visible from here. ctx->p_current_flow3_map still
                               correctly excludes facts from sibling/unrelated branches
                               the same way p_pointer_alt->origin did -- see
                               owner-reassigned-null-in-catch-false-positive.c and
                               deref-after-catch-reset-false-positive.c, both still
                               passing with this change -- because those facts live in
                               a branch that is not an ancestor of the CURRENT map
                               either (a sibling merged in, not something still open).
                               User-reported (aliased-owner-lifetime-not-tracked). */
                                int ended_line = 0;
                                if (!lifetime_ended_reported &&
                                    flow3_object_leaves_in_state_2(ctx, member_obj, FLOW3_LEAF_ENDED,
                                    p_pointer_alt->origin, ctx->p_current_flow3_map, false, &ended_line))
                                {
                                    lifetime_ended_reported = true;
                                    /* Include the expression itself, matching the null-deref
                                   sites' "operator -> applied to a possible null pointer
                                   '%s'" -- without it there was no way to tell which
                                   access, in a function with more than one `->`, the
                                   diagnostic was even about. User-reported. */
                                    struct osstream ss = { 0 };
                                    flow3_expression_to_string(p_expression, &ss);
                                    diagnostic(W_FLOW_LIFETIME_ENDED, ctx->ctx, NULL, &marker,
                                        "operator -> applied to '%s': pointed object lifetime has ended (see line %d)",
                                        ss.c_str ? ss.c_str : "", ended_line);
                                    ss_close(&ss);
                                }

                                flow3_seed_member_default(ctx, member_obj, p_expression->first_token->line);

                                {
                                    struct flow3_alternative a = {
                                        .value_kind = FLOW3_VALUE_KIND_REF,
                                        .value = {.p = member_obj},
                                        .value_relation = FLOW3_RELATION_EQUAL,
                                        .imaginary = FLOW3_IMAGINARY_NONE,
                                        .origin = ctx->p_current_flow3_map,
                                        .line = p_expression->first_token->line
                                    };
                                    flow3_alternatives_add(&result_entry->alternatives, &a);
                                }

                                flow3_narrow_map_into(p_true, ctx->p_current_flow3_map, member_obj, true, p_expression->first_token->line);
                                flow3_narrow_map_into(p_false, ctx->p_current_flow3_map, member_obj, false, p_expression->first_token->line);

                                any_member_resolved = true;
                            }
                        }
                        else
                        {
                            /* Direct pointer alternative (ptr_alt is FLOW3_VALUE_PTR) */
                            if (ptr_alt->value_relation == FLOW3_RELATION_EQUAL &&
                                ptr_alt->value_kind == FLOW3_VALUE_KIND_PTR &&
                                ptr_alt->value.p == NULL)
                            {
                                if (!ctx->expression_is_not_evaluated && !null_deref_reported)
                                {
                                    null_deref_reported = true;
                                    struct osstream ss = { 0 };
                                    flow3_expression_to_string(p_expression->left, &ss);
                                    diagnostic(W_FLOW_NULL_DEREFERENCE, ctx->ctx, NULL, &marker,
                                        "operator -> applied to a possible null pointer '%s'",
                                        ss.c_str ? ss.c_str : "");
                                    ss_close(&ss);
                                }
                                continue;
                            }

                            if (ptr_alt->value_kind != FLOW3_VALUE_KIND_PTR || ptr_alt->value.p == NULL)
                                continue;

                            const struct object* p_pointed_obj = ptr_alt->value.p;

                            struct object* _Opt member_obj = object_get_member(p_pointed_obj, member_index);
                            if (member_obj == NULL)
                            {
                                _Assert(false);
                                continue;
                            }

                            /* "Any leaf ended" walk, scoped to the ACCESSED MEMBER, checking
                           against BOTH ptr_alt's own origin and ctx->p_current_flow3_map
                           -- see the identical fix and rationale (aliased-owner-
                           lifetime-not-tracked) at the REF-alternative site above. */
                            int ended_line = 0;
                            if (!lifetime_ended_reported &&
                                flow3_object_leaves_in_state_2(ctx, member_obj, FLOW3_LEAF_ENDED,
                                ptr_alt->origin, ctx->p_current_flow3_map, false, &ended_line))
                            {
                                lifetime_ended_reported = true;
                                /* Include the expression itself -- see the identical fix and
                               rationale at the REF-alternative site above. */
                                struct osstream ss = { 0 };
                                flow3_expression_to_string(p_expression, &ss);
                                diagnostic(W_FLOW_LIFETIME_ENDED, ctx->ctx, NULL, &marker,
                                    "operator -> applied to '%s': pointed object lifetime has ended (see line %d)",
                                    ss.c_str ? ss.c_str : "", ended_line);
                                ss_close(&ss);
                            }

                            flow3_seed_member_default(ctx, member_obj, p_expression->first_token->line);

                            {
                                struct flow3_alternative a = {
                                    .value_kind = FLOW3_VALUE_KIND_REF,
                                    .value = {.p = member_obj},
                                    .value_relation = FLOW3_RELATION_EQUAL,
                                    .imaginary = FLOW3_IMAGINARY_NONE,
                                    .origin = ctx->p_current_flow3_map,
                                    .line = p_expression->first_token->line
                                };
                                flow3_alternatives_add(&result_entry->alternatives, &a);
                            }

                            flow3_narrow_map_into(p_true, ctx->p_current_flow3_map, member_obj, true, p_expression->first_token->line);
                            flow3_narrow_map_into(p_false, ctx->p_current_flow3_map, member_obj, false, p_expression->first_token->line);

                            any_member_resolved = true;
                        }
                    }
                }

                if (!any_member_resolved)
                {
                    /*
       The member has no concrete object behind it (base pointer has no
       modeled pointee). Seed the result from the member's DECLARED type
       rather than leaving it EMPTY -- empty now means "unknown", and an
       unknown operand must not be mistaken for a proof.

         non-_Opt pointer -> non-null (its type guarantees it)
         _Opt pointer     -> possibly null
         integer          -> ANY
    */
                    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;
                    struct flow3_key_alternatives* _Opt e_unres = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                    if (e_unres == NULL) throw;
                    if (e_unres != NULL && e_unres->alternatives.size == 0)
                    {
                        struct flow3_alternative a = {
                            .imaginary = FLOW3_IMAGINARY_NONE,
                            .origin = ctx->p_current_flow3_map,
                            .line = p_expression->first_token->line
                        };
                        if (type_is_pointer(&p_expression->type))
                        {
                            a.value_kind = FLOW3_VALUE_KIND_PTR;
                            a.value.p = NULL;
                            a.value_relation = type_is_opt(&p_expression->type, nullable_enabled)
                            ? FLOW3_RELATION_ANY : FLOW3_RELATION_NOT_EQUAL;
                            flow3_alternatives_add(&e_unres->alternatives, &a);
                        }
                        else if (type_is_integer(&p_expression->type))
                        {
                            a.value_kind = type_is_signed(&p_expression->type)
                            ? FLOW3_VALUE_KIND_SIGNED : FLOW3_VALUE_KIND_UNSIGNED;
                            a.value_relation = FLOW3_RELATION_ANY;
                            flow3_alternatives_add(&e_unres->alternatives, &a);
                        }
                    }

                    p_true = flow3_narrow_map_branch(&ctx->flow3_map_arena, ctx->p_current_flow3_map, &p_expression->object, true, p_expression, p_expression->first_token->line);
                    p_false = flow3_narrow_map_branch(&ctx->flow3_map_arena, ctx->p_current_flow3_map, &p_expression->object, false, p_expression, p_expression->first_token->line);
                    if (p_true == NULL || p_false == NULL)
                        throw;

                }

                flow3_map_remove(ctx->p_current_flow3_map, &p_expression->left->object);
                return (struct flow3_branch_pair) { p_true, p_false };
            }
            break;

        case EXPR_POSTFIX_ARRAY:
            {
                _Assert(p_expression->right != NULL);
                _Assert(p_expression->left != NULL);
                flow3_visit_expression(ctx, p_expression->left);
                flow3_visit_expression(ctx, p_expression->right);

                /* Bounds check for a FLOW-DERIVED index (a narrowed range or a
   branch-constant). A literal/constant-folded index is already checked
   by the parser (object_has_known_value is true for those), so we skip
   it here to avoid double-warning. We warn only when the index is
   PROVABLY out of bounds on some path -- i.e. its whole interval lies
   past the end (lo >= N) or below zero (hi < 0) -- so an ordinary
   unknown index (ANY, no interval) is never flagged. */
                {
                    const struct type* p_arr_type = &skip_parenthesis(p_expression->left)->type;
                    if (!ctx->expression_is_not_evaluated &&
                        type_is_array(p_arr_type) &&
                        p_arr_type->array_num_elements > 0 &&
                        !object_has_known_value(&p_expression->right->object))
                    {
                        const long long N = (long long)p_arr_type->array_num_elements;
                        const struct flow3_key_alternatives* _Opt idx_alts =
                        flow3_map_search_up(ctx->p_current_flow3_map, &p_expression->right->object);
                        struct marker idx_marker = expression_to_marker(p_expression);
                        bool warned = false;
                        for (int i = 0; !warned && idx_alts != NULL && i < idx_alts->alternatives.size; i++)
                        {
                            const struct flow3_alternative* idx = idx_alts->alternatives.data[i];

                            /* The index expression usually resolves to a REF to the
               variable object; follow it to the value alternatives. */
                            const struct flow3_key_alternatives* _Opt value_alts = NULL;
                            if (idx->value_kind == FLOW3_VALUE_KIND_REF && idx->value.p != NULL)
                                value_alts = flow3_map_search_up(ctx->p_current_flow3_map, idx->value.p);

                            const struct flow3_alternative* vlist_one = idx;
                            int vcount = 1;
                            /* data is now an array of OWNED POINTERS, not an inline
                           array of structs -- this is itself already the
                           element pointer type, so vlist_many[j] below needs
                           no '&'. */
                            struct flow3_alternative* _Owner _Opt* _Opt vlist_many = NULL;
                            if (value_alts != NULL)
                            {
                                vlist_many = value_alts->alternatives.data;
                                vcount = value_alts->alternatives.size;
                            }

                            for (int j = 0; !warned && j < vcount; j++)
                            {
                                const struct flow3_alternative* v = vlist_many ? vlist_many[j] : vlist_one;
                                long long lo = 0, hi = 0;
                                if (!flow3_alt_to_interval(v, &lo, &hi))
                                    continue;
                                if (lo >= N)
                                {
                                    diagnostic(W_FLOW_OUT_OF_BOUNDS, ctx->ctx, NULL, &idx_marker,
                                        "array index is past the end of the array (size %lld)", N);
                                    warned = true;
                                }
                                else if (hi < 0)
                                {
                                    diagnostic(W_FLOW_OUT_OF_BOUNDS, ctx->ctx, NULL, &idx_marker,
                                        "array index is negative");
                                    warned = true;
                                }
                            }
                        }
                    }
                }

                /* For a CONSTANT index, resolve v[i] to the array's element object and
   seed this expression as a REF to it -- so element values/relations
   (including initializers) are tracked, mirroring EXPR_POSTFIX_DOT.
   A non-constant index can't be pinned to one element, so we fall back
   to narrowing on this expression's own object. */
                if (object_has_known_value(&p_expression->right->object))
                {
                    const signed long long index = object_to_signed_long_long(&p_expression->right->object);

                    struct flow3_key_alternatives* _Opt result_entry = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                    if (result_entry == NULL) throw;
                    flow3_alternatives_clear(&result_entry->alternatives);

                    const struct flow3_key_alternatives* _Opt p_left_alternatives =
                    flow3_map_search_up(ctx->p_current_flow3_map, &skip_parenthesis(p_expression->left)->object);

                    struct flow3_map* _Opt p_true = flow3_map_arena_new_branch(&ctx->flow3_map_arena, ctx->p_current_flow3_map, true, p_expression);
                    struct flow3_map* _Opt p_false = flow3_map_arena_new_branch(&ctx->flow3_map_arena, ctx->p_current_flow3_map, false, p_expression);
                    if (p_true == NULL || p_false == NULL)
                        throw;

                    flow3_tag_branch_pair(p_true, p_false);

                    bool any_resolved = false;
                    for (int i = 0; index >= 0 && p_left_alternatives != NULL && i < p_left_alternatives->alternatives.size; i++)
                    {
                        const struct flow3_alternative* p_left_alternative = p_left_alternatives->alternatives.data[i];

                        if (p_left_alternative->value_relation == FLOW3_RELATION_EQUAL &&
                            p_left_alternative->value_kind == FLOW3_VALUE_KIND_REF &&
                            p_left_alternative->value.p != NULL)
                        {
                            struct object* _Opt p_element = object_get_member(p_left_alternative->value.p, (size_t)index);
                            if (p_element == NULL)
                                continue;

                            struct flow3_alternative a = {
                                .value_kind = FLOW3_VALUE_KIND_REF,
                                .value = {.p = p_element},
                                .value_relation = FLOW3_RELATION_EQUAL,
                                .imaginary = FLOW3_IMAGINARY_NONE,
                                .origin = flow3_origin_more_specific(ctx->p_current_flow3_map, p_left_alternative->origin),
                                .line = p_expression->first_token->line
                            };
                            flow3_alternatives_add(&result_entry->alternatives, &a);

                            flow3_narrow_map_into(p_true, ctx->p_current_flow3_map, p_element, true, p_expression->first_token->line);
                            flow3_narrow_map_into(p_false, ctx->p_current_flow3_map, p_element, false, p_expression->first_token->line);
                            any_resolved = true;
                        }
                    }

                    flow3_map_remove(ctx->p_current_flow3_map, &p_expression->left->object);
                    flow3_map_remove(ctx->p_current_flow3_map, &p_expression->right->object);

                    if (any_resolved)
                        return (struct flow3_branch_pair) { p_true, p_false };

                    /* Nothing resolved (unknown array, out-of-range, etc.): fall through
       to plain narrowing on this expression's object. */
                }

                /* Seed the (unresolved) subscript result as an ANY value of its
   element type, so it is never an EMPTY operand. An empty operand
   makes flow3_evaluate_equality_multi fold `v[i] == c` to "always
   true" (the vacuous-empty rule), which marked the else branch of
   e.g. `if (s->current[0] == '\n')` as unreachable code. `*p`
   (EXPR_UNARY *) already seeds ANY; subscript did not. */
                if (type_is_integer(&p_expression->type))
                {
                    struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                    if (e == NULL) throw;
                    if (e->alternatives.size == 0)
                    {
                        struct flow3_alternative a = {
                            .value_kind = type_is_signed(&p_expression->type)
                            ? FLOW3_VALUE_KIND_SIGNED : FLOW3_VALUE_KIND_UNSIGNED,
                            .value_relation = FLOW3_RELATION_ANY,
                            .imaginary = FLOW3_IMAGINARY_NONE,
                            .origin = ctx->p_current_flow3_map,
                            .line = p_expression->first_token->line
                        };
                        flow3_alternatives_add(&e->alternatives, &a);
                    }
                }
                else if (type_is_pointer(&p_expression->type) &&
                    ctx->ctx->options.null_checks_enabled &&
                    !type_is_opt(&p_expression->type, ctx->ctx->options.null_checks_enabled))
                {
                    /* An unresolved element of a non-_Opt pointer array is non-null by
           the non-_Opt => non-null rule -- e.g. `argv[i]` for
           `char** argv` (argv[0..argc-1] are non-null per the C standard).
           Seed NOT_EQUAL null, not ANY: seeding ANY here would (re)introduce
           ~28 false "possible null" -- the reason pointer elements were left
           unseeded originally. Non-null is the correct, narrower state and
           keeps `argv[i]` (and `argv[i] + n`) off the possibly-null path. */
                    struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                    if (e == NULL) throw;
                    if (e->alternatives.size == 0)
                    {
                        struct flow3_alternative a = {
                            .value_kind = FLOW3_VALUE_KIND_PTR,
                            .value = {.p = NULL},
                            .value_relation = FLOW3_RELATION_NOT_EQUAL,
                            .imaginary = FLOW3_IMAGINARY_NONE,
                            .origin = ctx->p_current_flow3_map,
                            .line = p_expression->first_token->line
                        };
                        flow3_alternatives_add(&e->alternatives, &a);
                    }
                }

                /* Array element used as bool (unknown index or unresolved). */
                const struct object* p_obj = &p_expression->object;
                struct flow3_map* _Opt p_true = flow3_narrow_map_branch(&ctx->flow3_map_arena, ctx->p_current_flow3_map, p_obj, true, p_expression, p_expression->first_token->line);
                struct flow3_map* _Opt p_false = flow3_narrow_map_branch(&ctx->flow3_map_arena, ctx->p_current_flow3_map, p_obj, false, p_expression, p_expression->first_token->line);
                if (p_true == NULL || p_false == NULL)
                    throw;

                flow3_tag_branch_pair(p_true, p_false);
                flow3_map_remove(ctx->p_current_flow3_map, &p_expression->left->object);
                flow3_map_remove(ctx->p_current_flow3_map, &p_expression->right->object);
                return (struct flow3_branch_pair) { p_true, p_false };
            }

        case EXPR_POSTFIX_FUNCTION_CALL:
            {
                _Assert(p_expression->left != NULL);

                flow3_visit_expression(ctx, p_expression->left);
                flow3_visit_function_arguments(ctx, &p_expression->left->type, &p_expression->argument_expression_list);

                const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;
                const struct type* p_ret_type = &p_expression->type;
                const int call_line = p_expression->first_token->line;
                /* `_Clear` in RETURN position means the returned pointee is all-zero
   (calloc) -- the return-side reading of the same qualifier that, on a
   parameter, means "the callee zeroes the pointee". */
                const bool ret_zero = type_is_pointer(p_ret_type) &&
                (type_is_clear(p_ret_type) || type_is_pointed_clear(p_ret_type));
                const bool ret_uninit = type_is_pointer(p_ret_type) &&
                (type_is_uninit(p_ret_type) || type_is_pointed_uninit(p_ret_type));

                if (nullable_enabled && type_is_pointer(p_ret_type) && type_is_opt(p_ret_type, nullable_enabled))
                {
                    struct flow3_key_alternatives* _Opt p_result_alternatives = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                    if (p_result_alternatives == NULL) throw;
                    flow3_alternatives_clear(&p_result_alternatives->alternatives);

                    struct flow3_map* _Opt p_null_map = flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, FLOW3_MAP_CALL_OPT_NULL);
                    if (p_null_map == NULL)
                        throw;

                    {
                        struct flow3_alternative a = {
                            .value_kind = FLOW3_VALUE_KIND_PTR,
                            .value = {.p = NULL},
                            .value_relation = FLOW3_RELATION_EQUAL,
                            .imaginary = FLOW3_IMAGINARY_NONE,
                            .origin = p_null_map,
                            .line = call_line
                        };
                        flow3_alternatives_add(&p_result_alternatives->alternatives, &a);
                    }

                    struct flow3_map* _Opt p_nonnull_map = flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, FLOW3_MAP_CALL_OPT_NONNULL);
                    if (p_nonnull_map == NULL)
                        throw;

                    struct object* _Opt p_pointed = flow3_allocated_object_arena_new(&ctx->allocated_object_arena);
                    if (p_pointed != NULL)
                    {
                        struct type pointed_type = type_remove_pointer(p_ret_type);
                        make_object(&pointed_type, p_pointed, MAKE_STATE_ANY, ctx->ctx->options.target);
                        if (ctx->p_current_flow3_map == NULL)
                            throw;

                        struct flow3_map* old = ctx->p_current_flow3_map;
                        ctx->p_current_flow3_map = p_nonnull_map;
                        flow3_object_init(ctx, p_pointed, &pointed_type, call_line);
                        ctx->p_current_flow3_map = old;
                        /* Return-type contract on the pointee: `_Clear` (e.g. calloc)
           means the returned region is all-zero -- seed each member
           EQUAL 0 so `p->m == 0` is concretely true; `_Uninitialized`
           (e.g. malloc) means the contents are indeterminate. The
           qualifier may sit on the returned pointer (`T* _Opt _Clear f()`,
           next to _Owner/_Opt) or on the pointee (`_Clear T* f()`).
           Seed into the live (parent) map -- not p_nonnull_map, whose
           member state is a sibling branch that execution never enters
           after the call; p_pointed is only ever reached through the
           surviving non-null alternative, so the parent map is where a
           later `x->m` read resolves it. */
                        if (ret_zero)
                            flow3_map_set_object_zero(ctx->p_current_flow3_map, p_pointed, call_line);
                        else if (ret_uninit)
                            flow3_map_set_object_uninitialized(ctx->p_current_flow3_map, p_pointed, call_line);
                        type_destroy(&pointed_type);
                    }

                    {
                        /* p_pointed == NULL (allocation failure): fall back to a plain
           "non-null" alternative with no concrete pointee. */
                        struct flow3_alternative a = {
                            .value_kind = FLOW3_VALUE_KIND_PTR,
                            .value = {.p = p_pointed},
                            .value_relation = p_pointed != NULL ? FLOW3_RELATION_EQUAL : FLOW3_RELATION_NOT_EQUAL,
                            .imaginary = FLOW3_IMAGINARY_NONE,
                            .origin = p_nonnull_map,
                            .line = call_line
                        };
                        flow3_alternatives_add(&p_result_alternatives->alternatives, &a);
                    }

                }
                else if (type_is_pointer(p_ret_type))
                {
                    /* Non-_Opt pointer return: non-null. For a `_Clear`/`_Uninitialized`
       pointee contract, build a concrete pointee so members can be
       seeded zero/uninitialized (mirrors the _Opt branch above). */
                    struct object* _Opt p_pointed = NULL;
                    if (ret_zero || ret_uninit)
                    {
                        p_pointed = flow3_allocated_object_arena_new(&ctx->allocated_object_arena);
                        if (p_pointed != NULL)
                        {
                            struct type pointed_type = type_remove_pointer(p_ret_type);
                            make_object(&pointed_type, p_pointed, MAKE_STATE_ANY, ctx->ctx->options.target);
                            if (ret_zero)
                                flow3_map_set_object_zero(ctx->p_current_flow3_map, p_pointed, call_line);
                            else
                                flow3_map_set_object_uninitialized(ctx->p_current_flow3_map, p_pointed, call_line);
                            type_destroy(&pointed_type);
                        }
                    }
                    {
                        struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                        if (e == NULL) throw;
                        flow3_alternatives_clear(&e->alternatives);
                        struct flow3_alternative a = {
                            .value_kind = FLOW3_VALUE_KIND_PTR,
                            .value = {.p = p_pointed},
                            .value_relation = p_pointed != NULL ? FLOW3_RELATION_EQUAL : FLOW3_RELATION_NOT_EQUAL,
                            .imaginary = FLOW3_IMAGINARY_NONE,
                            .origin = ctx->p_current_flow3_map,
                            .line = call_line
                        };
                        flow3_alternatives_add(&e->alternatives, &a);
                    }
                }
                else if (!type_is_void(p_ret_type))
                {
                    /*
       Non-pointer return type (scalar, or struct/union returned
       by value): nothing above seeded p_expression->object at
       all, so it -- and every member, for a struct/union --
       silently stayed whatever pre-existing UNINITIALIZED state
       happened to be in the map (there usually isn't one for a
       fresh temporary, but the DESTINATION of an assignment or
       initialization from this call falls back to ITS OWN
       pre-existing declared state when this source has no
       entry to copy from -- see flow3_check_object_init_assigment).
       Concretely this was reported as two real false positives:

         int errcode = mkdir(to, 0700);
         if (errcode != 0) return errcode; // "errcode" possibly uninitialized

         struct X x = f(); // or: struct X x; x = f();
         use(x.a);          // "x.a" possibly uninitialized

       A function's return value is always a genuine, fully
       initialized value by the time the call returns (the
       callee cannot return without having constructed it) --
       the exact same "assumed valid on entry" rule flow3 already
       applies to non-_Opt pointer parameters and pointer return
       values above applies here too, just for non-pointer types
       and struct members. Reuse flow3_parameter_object_init,
       which already knows how to seed a (possibly aggregate)
       object as ANY/non-null recursively member-by-member --
       it works identically whether the object in hand came from
       a parameter or, as here, a call's own result object. */
                    /* p_expression is const here (flow3_visit_expression's own
parameter), so &p_expression->object is a const struct
object* -- but flow3_parameter_object_init's signature
(shared with the parameter-seeding call site) takes a
non-const struct object*, matching every other call site
where the object being seeded belongs to a non-const
declarator. It only ever reads this object's own
.members list (to recurse) and writes into the flow map
keyed by its address; it never mutates the object itself.
Cast away const explicitly rather than relaxing the
shared signature for every other caller. */
                    flow3_parameter_object_init(ctx, (struct object*)&p_expression->object, p_ret_type, call_line);
                }
                flow3_map_remove(ctx->p_current_flow3_map, &p_expression->left->object);
            }
            break;

        case EXPR_POSTFIX_FUNCTION_LITERAL:
            _Assert(p_expression->compound_statement != NULL);
            flow3_visit_compound_statement(ctx, p_expression->compound_statement);
            break;

        case EXPR_POSTFIX_COMPOUND_LITERAL:
            {
                _Assert(p_expression->left == NULL);
                _Assert(p_expression->right == NULL);
                _Assert(p_expression->type_name != NULL);
                _Assert(p_expression->braced_initializer != NULL);

                //const struct object* p_agg = &p_expression->object;
                int line = p_expression->first_token->line;

                /* 1. Evaluate all RHS expressions. */
                flow3_visit_bracket_initializer_list(ctx, p_expression->braced_initializer);

                /*
               2/3. Seed the compound literal's own object the SAME way
               flow3_visit_init_declarator seeds a declarator's object
               for a braced initializer: flow3_object_init picks up each
               member's compile-time constant state (already computed by
               the object model during semantic analysis -- the same
               information that makes `struct X x = {0};` correctly seed
               every member to 0/null), then
               flow3_seed_aggregate_from_init_exprs fills in members set
               by a non-constant initializer expression.

               Before this fix, the compound literal's own object was
               unconditionally marked ANY here, discarding all per-member
               zero/constant information: `x = (struct X){0};` left every
               member of x showing ANY afterward (confirmed via
               static_debug), unlike the always-correct
               `struct X x = {0};` declaration form. Since assigning a
               struct copies member-by-member from the SOURCE's tracked
               state, that lost information propagated straight into the
               destination -- silently defeating null/zero-narrowing
               checks on every field after a compound-literal assignment
               like `x = (struct X){};` or `x = (struct X){0};`.
               User-reported.
            */
                flow3_object_init(ctx, (struct object*)&p_expression->object, &p_expression->type, line);
                flow3_seed_aggregate_from_init_exprs(ctx, (struct object*)&p_expression->object);
                break;
            }

        case EXPR_UNARY_STATIC_ASSERTION:
            _Assert(p_expression->static_assertion);
            flow3_visit_static_assertion(ctx, p_expression->static_assertion);
            break;

        case EXPR_UNARY_ALIGNOF_EXPRESSION:
            {
                _Assert(p_expression->right);
                const bool t2 = ctx->expression_is_not_evaluated;
                ctx->expression_is_not_evaluated = true;
                flow3_visit_expression(ctx, p_expression->right);
                ctx->expression_is_not_evaluated = t2;
                flow3_seed_constant_result(ctx, p_expression);
                break;
            }

        case EXPR_UNARY_ALIGNOF_TYPE:
            flow3_seed_constant_result(ctx, p_expression);
            break;

        case EXPR_UNARY_ASSERT:
            /*
         * _Assert(expr) is equivalent to:
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
            {
                _Assert(p_expression->right);

                const bool t2 = ctx->expression_is_not_evaluated;
                ctx->expression_is_not_evaluated = true;
                flow3_visit_expression(ctx, p_expression->right);
                ctx->expression_is_not_evaluated = t2;
                flow3_seed_constant_result(ctx, p_expression);
                break;
            }

        case EXPR_UNARY_NEG:
        case EXPR_UNARY_PLUS:
            _Assert(p_expression->right != NULL);
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
                {
                    struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                    if (e == NULL) throw;
                    flow3_alternatives_clear(&e->alternatives);
                    struct flow3_alternative a = {
                        .value_kind = FLOW3_VALUE_KIND_SIGNED,
                        .value = {.i = result},
                        .value_relation = FLOW3_RELATION_EQUAL,
                        .imaginary = FLOW3_IMAGINARY_NONE,
                        .origin = ctx->p_current_flow3_map,
                        .line = p_expression->first_token->line
                    };
                    flow3_alternatives_add(&e->alternatives, &a);
                }
            }
            else
            {
                /* Operand has no constant value, but it may still carry a RELATION
               (e.g. `b < 0` narrowed by an enclosing if). Carry that through:
               unary + preserves it, unary - mirrors it. Only if nothing can be
               mapped do we fall back to a plain ANY. */
                const bool is_neg = (p_expression->expression_type == EXPR_UNARY_NEG);
                const struct flow3_key_alternatives* _Opt p_src =
                flow3_map_search_up(ctx->p_current_flow3_map, &p_expression->right->object);

                struct flow3_alternatives mapped = { 0 };
                bool all_mapped = (p_src != NULL && p_src->alternatives.size > 0);

                for (int i = 0; all_mapped && i < p_src->alternatives.size; i++)
                {
                    const struct flow3_alternative* a0 = p_src->alternatives.data[i];

                    /* The operand usually resolves to a REF to the variable object;
                   follow it to the actual value alternatives. */
                    const struct flow3_key_alternatives* _Opt p_vals = NULL;
                    if (a0->value_kind == FLOW3_VALUE_KIND_REF && a0->value.p != NULL)
                        p_vals = flow3_map_search_up(ctx->p_current_flow3_map, a0->value.p);

                    /* data is an array of pointers now, so list[j] is already a
                       struct flow3_alternative* -- when there's no REF to follow,
                       use a synthetic one-element array holding a0 itself instead
                       of treating a0 (one alternative) as if it were the array. */
                    struct flow3_alternative* _Opt single_list[1];
                    struct flow3_alternative* _Opt* _Opt list;
                    if (p_vals != NULL)
                    {
                        list = p_vals->alternatives.data;
                    }
                    else
                    {
                        single_list[0] = (struct flow3_alternative*)a0;
                        list = single_list;
                    }
                    const int count = p_vals ? p_vals->alternatives.size : 1;
                    if (count == 0) { all_mapped = false; break; }

                    for (int j = 0; j < count; j++)
                    {
                        struct flow3_alternative out;
                        if (is_neg)
                        {
                            if (!flow3_alt_negate(list[j], &out)) { all_mapped = false; break; }
                        }
                        else
                        {
                            out = *list[j];
                            if (out.value_kind != FLOW3_VALUE_KIND_SIGNED) { all_mapped = false; break; }
                        }
                        out.origin = ctx->p_current_flow3_map;
                        out.line = p_expression->first_token->line;
                        flow3_alternatives_add(&mapped, &out);
                    }
                }

                if (all_mapped && mapped.size > 0)
                {
                    struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                    if (e == NULL) throw;
                    flow3_alternatives_clear(&e->alternatives);
                    e->alternatives = mapped;
                    break;
                }
                flow3_alternatives_clear(&mapped);

                /* Operand value unknown — result is also unknown. */
                {
                    struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                    if (e == NULL) throw;
                    flow3_alternatives_clear(&e->alternatives);
                    struct flow3_alternative a = {
                        .value_kind = FLOW3_VALUE_KIND_SIGNED,
                        .value = {.i = ANY_VALUE},
                        .value_relation = FLOW3_RELATION_ANY,
                        .imaginary = FLOW3_IMAGINARY_NONE,
                        .origin = ctx->p_current_flow3_map,
                        .line = p_expression->first_token->line
                    };
                    flow3_alternatives_add(&e->alternatives, &a);
                }
            }
            flow3_map_remove(ctx->p_current_flow3_map, &p_expression->right->object);
            break;

        case EXPR_UNARY_NOT:
            {
                _Assert(p_expression->right != NULL);

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
                    {
                        struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                        if (e == NULL) throw;
                        flow3_alternatives_clear(&e->alternatives);
                        struct flow3_alternative a = {
                            .value_kind = FLOW3_VALUE_KIND_SIGNED,
                            .value = {.i = result},
                            .value_relation = FLOW3_RELATION_EQUAL,
                            .imaginary = FLOW3_IMAGINARY_NONE,
                            .origin = ctx->p_current_flow3_map,
                            .line = p_expression->first_token->line
                        };
                        flow3_alternatives_add(&e->alternatives, &a);
                    }
                    flow3_map_remove(ctx->p_current_flow3_map, &p_expression->right->object);
                    if (result != 0)
                    {
                        struct flow3_map* _Opt p_dead = flow3_map_arena_new_dead(&ctx->flow3_map_arena, p_before, FLOW3_MAP_NOT_DEAD_FALSE);
                        if (p_dead == NULL)
                            throw;

                        return (struct flow3_branch_pair) { ctx->p_current_flow3_map, p_dead };
                    }
                    else
                    {
                        struct flow3_map* _Opt p_dead = flow3_map_arena_new_dead(&ctx->flow3_map_arena, p_before, FLOW3_MAP_NOT_DEAD_TRUE);
                        if (p_dead == NULL)
                            throw;

                        return (struct flow3_branch_pair) { p_dead, ctx->p_current_flow3_map };
                    }
                }

                /* Seed the NOT result's OWN value: `!x` yields a boolean (0 or 1) and is
   always INITIALIZED. Without this, `bool c = !x;` (non-constant x) left
   c with no value and c was wrongly reported "possibly uninitialized". */
                {
                    struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                    if (e == NULL) throw;
                    flow3_alternatives_clear(&e->alternatives);
                    struct flow3_alternative a = {
                        .value_kind = FLOW3_VALUE_KIND_SIGNED,
                        .value = {.i = ANY_VALUE},
                        .value_relation = FLOW3_RELATION_ANY,
                        .imaginary = FLOW3_IMAGINARY_NONE,
                        .origin = ctx->p_current_flow3_map,
                        .line = p_expression->first_token->line
                    };
                    flow3_alternatives_add(&e->alternatives, &a);
                }

                flow3_map_remove(ctx->p_current_flow3_map, &p_expression->right->object);
                /* NOT swaps the two branches. */
                return (struct flow3_branch_pair) { child.p_false, child.p_true };
            }

        case EXPR_CHECKED:
            {
                _Assert(p_expression->left != NULL);
                struct flow3_branch_pair checked_pair = flow3_visit_expression(ctx, p_expression->left);
                flow3_exit_block_visit_defer_list(ctx, &p_expression->defer_list, p_expression->first_token);
                flow3_defer_list_set_end_of_lifetime(ctx, &p_expression->defer_list, p_expression->first_token);
                return checked_pair;
            }

        case EXPR_UNARY_SIZEOF_TYPE:
        case EXPR_UNARY_COUNTOF:
            /* Constant when the parser folded it. For a VLA `sizeof` the parser
           has no constant value, so this seeds nothing and it stays unknown. */
            flow3_seed_constant_result(ctx, p_expression);
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
                    _Assert(p_expression->right != NULL);
                    p_operand = p_expression->right;
                }
                else // postfix
                {
                    _Assert(p_expression->left != NULL);
                    p_operand = p_expression->left;
                }

                // Evaluate the operand first.
                flow3_visit_expression(ctx, p_operand);

                /* Mutating the operand invalidates any predicate over it. */
                flow3_predicate_invalidate(ctx, object_get_referenced(&p_operand->object));

                bool is_postfix = (p_expression->expression_type == EXPR_POSTFIX_INCREMENT ||
                    p_expression->expression_type == EXPR_POSTFIX_DECREMENT);
                bool is_increment = (p_expression->expression_type == EXPR_UNARY_INCREMENT ||
                    p_expression->expression_type == EXPR_POSTFIX_INCREMENT);

                /*
   ++ / -- are disallowed on an _Owner pointer: advancing it loses the
   very address that has to be freed, so the allocation could never be
   released through it.

   Moved here from expressions.c so that every diagnostic mentioning
   _Owner lives in flow3 -- and extended while moving: the parser only
   checked the POSTFIX forms, so `++p` / `--p` on an owner went
   completely unreported. All four forms land in this case.
*/
                if (type_is_owner(&p_operand->type))
                {
                    diagnostic(is_increment
                        ? C_ERROR_FLOW_OPERATOR_INCREMENT_CANNOT_BE_USED_IN_OWNER
                        : C_ERROR_FLOW_OPERATOR_DECREMENT_CANNOT_BE_USED_IN_OWNER,
                        ctx->ctx,
                        p_operand->first_token, NULL,
                        is_increment
                        ? "operator ++ cannot be used in _Owner pointers"
                        : "operator -- cannot be used in _Owner pointers");
                }

                // Resolve the operand's object to its actual alternatives.
                const struct object* p_obj = &p_operand->object;
                const struct flow3_key_alternatives* _Opt p_entry =
                flow3_map_search_up(ctx->p_current_flow3_map, p_obj);
                if (p_entry == NULL)
                {
                    {
                        struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, p_obj);
                        if (e == NULL) throw;
                        flow3_alternatives_clear(&e->alternatives);
                        struct flow3_alternative a = {
                            .value_kind = FLOW3_VALUE_KIND_SIGNED,
                            .value = {.i = ANY_VALUE},
                            .value_relation = FLOW3_RELATION_ANY,
                            .imaginary = FLOW3_IMAGINARY_NONE,
                            .origin = ctx->p_current_flow3_map,
                            .line = p_expression->first_token->line
                        };
                        flow3_alternatives_add(&e->alternatives, &a);
                    }
                    {
                        struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                        if (e == NULL) throw;
                        flow3_alternatives_clear(&e->alternatives);
                        struct flow3_alternative a = {
                            .value_kind = FLOW3_VALUE_KIND_SIGNED,
                            .value = {.i = ANY_VALUE},
                            .value_relation = FLOW3_RELATION_ANY,
                            .imaginary = FLOW3_IMAGINARY_NONE,
                            .origin = ctx->p_current_flow3_map,
                            .line = p_expression->first_token->line
                        };
                        flow3_alternatives_add(&e->alternatives, &a);
                    }
                    break;
                }

                /* Advance the object(s) the operand names. An lvalue may alias
   several objects -- e.g. `(*p)++` where p can point to a or b -- so
   iterate its REF alternatives the way flow3_check_assigment handles
   an assignment destination, rather than a size==1 / data[0] shortcut.
   Each referenced object's values are advanced, tagged with the branch
   the reference belongs to so the update stays correlated. */
                struct flow3_alternatives new_result_alts = { 0 };
                bool advanced_any = false;

                for (int ri = 0; ri < p_entry->alternatives.size; ri++)
                {
                    const struct flow3_alternative* ref = p_entry->alternatives.data[ri];
                    if (ref->value_kind != FLOW3_VALUE_KIND_REF || ref->value.p == NULL)
                        continue;
                    advanced_any = true;

                    const struct object* p_actual_obj = ref->value.p;
                    const struct flow3_key_alternatives* _Opt p_resolved =
                    flow3_map_search_up(ctx->p_current_flow3_map, p_actual_obj);
                    struct flow3_alternatives new_var_alts = { 0 };
                    int n = p_resolved ? p_resolved->alternatives.size : 0;

                    if (n == 0)
                    {
                        struct flow3_alternative a = { .value_kind = FLOW3_VALUE_KIND_SIGNED, .value = {.i = ANY_VALUE}, .value_relation = FLOW3_RELATION_ANY, .imaginary = FLOW3_IMAGINARY_NONE, .origin = ref->origin, .line = p_expression->first_token->line };
                        flow3_alternatives_add(&new_var_alts, &a);
                        flow3_alternatives_add(&new_result_alts, &a);
                    }

                    for (int i = 0; i < n; i++)
                    {
                        const struct flow3_alternative* alt = p_resolved->alternatives.data[i];
                        const struct flow3_map* org = flow3_origin_more_specific(alt->origin, ref->origin);

                        if (alt->imaginary == FLOW3_IMAGINARY_ABSENT || alt->value_relation == FLOW3_RELATION_UNINITIALIZED)
                        {
                            struct flow3_alternative a = { .value_kind = FLOW3_VALUE_KIND_SIGNED, .value = {.i = ANY_VALUE}, .value_relation = FLOW3_RELATION_ANY, .imaginary = FLOW3_IMAGINARY_NONE, .origin = org, .line = p_expression->first_token->line };
                            flow3_alternatives_add(&new_var_alts, &a);
                            flow3_alternatives_add(&new_result_alts, &a);
                        }
                        else if (alt->value_relation == FLOW3_RELATION_EQUAL &&
                            (alt->value_kind == FLOW3_VALUE_KIND_SIGNED || alt->value_kind == FLOW3_VALUE_KIND_UNSIGNED))
                        {
                            long long old = (alt->value_kind == FLOW3_VALUE_KIND_SIGNED) ? alt->value.i : (long long)alt->value.u;
                            long long new_val = is_increment ? old + 1 : old - 1;
                            struct flow3_alternative av = { .value_kind = FLOW3_VALUE_KIND_SIGNED, .value = {.i = new_val}, .value_relation = FLOW3_RELATION_EQUAL, .imaginary = FLOW3_IMAGINARY_NONE, .origin = org, .line = p_expression->first_token->line };
                            flow3_alternatives_add(&new_var_alts, &av);
                            long long result_val = is_postfix ? old : new_val;
                            struct flow3_alternative ar = { .value_kind = FLOW3_VALUE_KIND_SIGNED, .value = {.i = result_val}, .value_relation = FLOW3_RELATION_EQUAL, .imaginary = FLOW3_IMAGINARY_NONE, .origin = org, .line = p_expression->first_token->line };
                            flow3_alternatives_add(&new_result_alts, &ar);
                        }
                        else if (alt->value_kind == FLOW3_VALUE_KIND_PTR)
                        {
                            /* Advancing a pointer preserves its null-ness (it still
               points within the same object/array, so non-null stays
               non-null) but moves it to a DIFFERENT element -- the
               pointed-to VALUE is now unknown. Keeping the SAME pointee
               object would leave a stale fact like `*p == c` (from an
               earlier narrowing, e.g. a `while (*p != '"') p++;` loop
               exit) attached to the advanced pointer, which wrongly
               folded `*p != c` to false and reported dead code
               (tokenizer.c). Repoint to a fresh ANY pointee; a pointer
               copied off BEFORE the increment keeps the old pointee, so
               its knowledge of `*q` is correctly preserved. */
                            struct flow3_alternative a = *alt;
                            if (alt->value_relation == FLOW3_RELATION_EQUAL &&
                                alt->value.p != NULL &&
                                type_is_pointer(&p_operand->type))
                            {
                                struct object* _Opt p_fresh = flow3_allocated_object_arena_new(&ctx->allocated_object_arena);
                                if (p_fresh != NULL)
                                {
                                    struct type pointed_type = type_remove_pointer(&p_operand->type);
                                    make_object(&pointed_type, p_fresh, MAKE_STATE_ANY, ctx->ctx->options.target);
                                    type_destroy(&pointed_type);
                                    a.value.p = p_fresh;
                                }
                                else
                                {
                                    /* Can't allocate a fresh pointee: fall back to a
                       generic non-null pointer (drops the stale value
                       without inventing a bogus one). */
                                    a.value.p = NULL;
                                    a.value_relation = FLOW3_RELATION_NOT_EQUAL;
                                }
                            }
                            flow3_alternatives_add(&new_var_alts, &a);
                            flow3_alternatives_add(&new_result_alts, &a);
                        }
                        else
                        {
                            struct flow3_alternative a = { .value_kind = FLOW3_VALUE_KIND_SIGNED, .value = {.i = ANY_VALUE}, .value_relation = FLOW3_RELATION_ANY, .imaginary = FLOW3_IMAGINARY_NONE, .origin = org, .line = p_expression->first_token->line };
                            flow3_alternatives_add(&new_var_alts, &a);
                            flow3_alternatives_add(&new_result_alts, &a);
                        }
                    }

                    struct flow3_key_alternatives* _Opt p_var_entry = flow3_map_find_add(ctx->p_current_flow3_map, p_actual_obj);
                    if (p_var_entry == NULL) throw;
                    if (p_var_entry) { flow3_alternatives_clear(&p_var_entry->alternatives); p_var_entry->alternatives = new_var_alts; }
                    else flow3_alternatives_clear(&new_var_alts);
                }

                if (!advanced_any)
                {
                    struct flow3_alternative a = { .value_kind = FLOW3_VALUE_KIND_SIGNED, .value = {.i = ANY_VALUE}, .value_relation = FLOW3_RELATION_ANY, .imaginary = FLOW3_IMAGINARY_NONE, .origin = ctx->p_current_flow3_map, .line = p_expression->first_token->line };
                    flow3_alternatives_add(&new_result_alts, &a);
                }

                {
                    struct flow3_key_alternatives* _Opt p_result_entry = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                    if (p_result_entry == NULL) throw;
                    if (p_result_entry) { flow3_alternatives_clear(&p_result_entry->alternatives); p_result_entry->alternatives = new_result_alts; }
                    else flow3_alternatives_clear(&new_result_alts);
                }

                // Remove the temporary operand object entry.
                flow3_map_remove(ctx->p_current_flow3_map, &p_operand->object);
                break;
            }
            break;

        case EXPR_UNARY_BITNOT:
            _Assert(p_expression->right != NULL);
            /*
         * Visit the child first so that any sub-expression is fully evaluated
         * and its constant value — if any — is propagated into
         * p_expression->right->object before we inspect it.
         */
            flow3_visit_expression(ctx, p_expression->right);
            if (object_has_constant_value(&p_expression->right->object))
            {
                const long long rv = object_to_signed_long_long(&p_expression->right->object);
                {
                    struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                    if (e == NULL) throw;
                    flow3_alternatives_clear(&e->alternatives);
                    struct flow3_alternative a = {
                        .value_kind = FLOW3_VALUE_KIND_SIGNED,
                        .value = {.i = ~rv},
                        .value_relation = FLOW3_RELATION_EQUAL,
                        .imaginary = FLOW3_IMAGINARY_NONE,
                        .origin = ctx->p_current_flow3_map,
                        .line = p_expression->first_token->line
                    };
                    flow3_alternatives_add(&e->alternatives, &a);
                }
            }
            else
            {
                /* Operand value unknown — result is also unknown. */
                {
                    struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                    if (e == NULL) throw;
                    flow3_alternatives_clear(&e->alternatives);
                    struct flow3_alternative a = {
                        .value_kind = FLOW3_VALUE_KIND_SIGNED,
                        .value = {.i = ANY_VALUE},
                        .value_relation = FLOW3_RELATION_ANY,
                        .imaginary = FLOW3_IMAGINARY_NONE,
                        .origin = ctx->p_current_flow3_map,
                        .line = p_expression->first_token->line
                    };
                    flow3_alternatives_add(&e->alternatives, &a);
                }
            }
            flow3_map_remove(ctx->p_current_flow3_map, &p_expression->right->object);
            break;

        case EXPR_UNARY_ADDRESSOF:
            {
                flow3_visit_expression(ctx, p_expression->right);

                struct flow3_key_alternatives* _Opt result_entry = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                if (result_entry == NULL) throw;
                flow3_alternatives_clear(&result_entry->alternatives);

                const struct flow3_key_alternatives* _Opt p_right_alternatives = flow3_map_search_up(ctx->p_current_flow3_map, &p_expression->right->object);
                if (p_right_alternatives)
                {

                    for (int i = 0; i < p_right_alternatives->alternatives.size; i++)
                    {
                        const struct flow3_alternative* p_right_alternative = p_right_alternatives->alternatives.data[i];
                        if (p_right_alternative->value_relation == FLOW3_RELATION_EQUAL &&
                            p_right_alternative->value_kind == FLOW3_VALUE_KIND_REF &&
                            p_right_alternative->value.p != NULL)
                        {
                            {
                                struct flow3_alternative a = {
                                    .value_kind = FLOW3_VALUE_KIND_PTR,
                                    .value = {.p = p_right_alternative->value.p},
                                    .value_relation = FLOW3_RELATION_EQUAL,
                                    .imaginary = FLOW3_IMAGINARY_NONE,
                                    .origin = ctx->p_current_flow3_map,
                                    .line = p_expression->first_token->line
                                };
                                flow3_alternatives_add(&result_entry->alternatives, &a);
                            }
                        }
                    }
                    flow3_map_remove(ctx->p_current_flow3_map, &p_expression->right->object);
                }
            }

            break;

        case EXPR_UNARY_CONTENT:
            {
                _Assert(p_expression->right != NULL);

                flow3_visit_expression(ctx, p_expression->right);

                struct flow3_key_alternatives* _Opt result_entry = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                if (result_entry == NULL) throw;
                flow3_alternatives_clear(&result_entry->alternatives);
                struct marker marker = expression_to_marker(p_expression);

                const struct flow3_key_alternatives* _Opt p_right_alternatives = flow3_map_search_up(ctx->p_current_flow3_map, &p_expression->right->object);

                bool content_lifetime_ended_reported = false;

                for (int i = 0; p_right_alternatives != NULL && i < p_right_alternatives->alternatives.size; i++)
                {
                    const struct flow3_alternative* p_right_alt = p_right_alternatives->alternatives.data[i];

                    if (p_right_alt->imaginary == FLOW3_IMAGINARY_ABSENT)
                        continue;
                    if (p_right_alt->value_kind == FLOW3_VALUE_KIND_REF)
                    {
                        const struct flow3_key_alternatives* _Opt p_right_alternatives2 = flow3_map_search_up(ctx->p_current_flow3_map,
                            p_right_alt->value.p);

                        if (p_right_alternatives2 == NULL)
                            continue;

                        for (int j = 0; j < p_right_alternatives2->alternatives.size; j++)
                        {
                            const struct flow3_alternative* p_right_alt2 = p_right_alternatives2->alternatives.data[j];

                            /* Lifetime check: `*p` after p's pointee was freed/moved
                           (e.g. consumed by an _Owner parameter, or _Dtor'd)
                           mirrors the same check EXPR_POSTFIX_ARROW does for
                           `p->member` -- without it, `*p = 0;` after `consume(p)`
                           (p an _Owner pointer parameter, no member access
                           involved) went entirely unchecked. See the two-origin
                           rationale on flow3_object_leaves_in_state_2 above:
                           same shape applies here, just checking the WHOLE
                           pointee rather than one member (there's no member
                           index for `*p`, only a value it derefs to). */
                            int ended_line = 0;
                            if (p_right_alt2->value_kind == FLOW3_VALUE_KIND_PTR &&
                                p_right_alt2->value.p != NULL &&
                                !content_lifetime_ended_reported &&
                                flow3_object_leaves_in_state_2(ctx, p_right_alt2->value.p, FLOW3_LEAF_ENDED,
                                p_right_alt2->origin, ctx->p_current_flow3_map, false, &ended_line))
                            {
                                content_lifetime_ended_reported = true;
                                struct osstream ss = { 0 };
                                flow3_expression_to_string(p_expression, &ss);
                                diagnostic(W_FLOW_LIFETIME_ENDED, ctx->ctx, NULL, &marker,
                                    "dereference of '%s': pointed object lifetime has ended (see line %d)",
                                    ss.c_str ? ss.c_str : "", ended_line);
                                ss_close(&ss);

                                /* If this same dereference is ALSO used as
                               an assignment/return/argument source,
                               flow3_check_object_init_assigment runs
                               right after and would otherwise report
                               this identical fact a second time -- see
                               the field comment in flow3.h. */
                                ctx->p_pending_ended_report_obj = p_right_alt2->value.p;
                                ctx->pending_ended_report_line = ended_line;
                            }

                            if (flow3_alternative_can_be_zero(p_right_alt2) &&
                                !ctx->expression_is_not_evaluated &&
                                flow3_origins_compatible(p_right_alt2->origin, ctx->p_current_flow3_map))
                            {
                                /* The operand of sizeof/_Alignof (and other unevaluated
                   contexts) is never dereferenced at runtime -- only its
                   type is needed -- so a possibly-null pointer there is
                   not an actual null dereference.

                   The origin check drops a null value that cannot occur
                   on the current path: if its branch decisions conflict
                   with where we are (e.g. it is the "else" value of a
                   condition whose "then" branch we are inside), the
                   dereference is safe here. */
                                /* Include the dereference expression itself (p_expression,
                      the whole `*p`, not just the pointer operand p_expression->right)
                      -- a bare "possible null pointer dereference" with no
                      expression left the reader to guess which pointer, in a
                      function with more than one, the diagnostic was even
                      about. User-requested; user also specifically asked for
                      '*p' rather than 'p' here, unlike the `->` sites (which
                      print just the pointer operand, matching "operator ->
                      applied to a possible null pointer '%s'" -- there the
                      operator name already tells the reader what's being
                      done to the pointer, so showing the pointer alone reads
                      naturally; a bare "dereference 'p'" here would not). */
                                struct osstream ss = { 0 };
                                flow3_expression_to_string(p_expression, &ss);
                                diagnostic(W_FLOW_NULL_DEREFERENCE, ctx->ctx, NULL, &marker,
                                    "possible null pointer dereference '%s'",
                                    ss.c_str ? ss.c_str : "");
                                ss_close(&ss);
                            }

                            {
                                struct flow3_alternative a = {
                                    .value_kind = FLOW3_VALUE_KIND_REF,
                                    .value = {.p = p_right_alt2->value.p},
                                    .value_relation = FLOW3_RELATION_EQUAL,
                                    .imaginary = FLOW3_IMAGINARY_NONE,
                                    /* Carry the pointer value's branch origin so a deref
                       stays correlated: `p = &a@then / &b@else` gives
                       `*p = ref a@then / ref b@else`. */
                                    .origin = p_right_alt2->origin,
                                    .line = p_expression->first_token->line
                                };
                                flow3_alternatives_add(&result_entry->alternatives, &a);
                            }
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
                _Assert(p_expression->right != NULL);
                _Assert(p_expression->left != NULL);

                // Evaluate both sides (side effects, constant folding)
                flow3_visit_expression(ctx, p_expression->left);
                flow3_visit_expression(ctx, p_expression->right);

                flow3_check_assigment(ctx, p_expression->left, p_expression->right);

                /* Writing the destination invalidates any predicate over it. */
                flow3_predicate_invalidate(ctx, object_get_referenced(&p_expression->left->object));

                /*
   An assignment expression's OWN value (per C semantics: the
   value of the left operand after the assignment) was never
   seeded here at all -- only p_expression->left->object (the
   destination sub-expression's own node) got updated, via
   flow3_check_assigment. That's enough for a plain assignment
   STATEMENT (`dp = readdir(dir);` followed by a separate
   `if (dp != NULL)`), since that reads dp itself later, not
   this assignment expression's result. But when the assignment
   is embedded as an OPERAND of something else -- most commonly
   `while ((dp = readdir(dir)) != NULL)` -- the enclosing `!=`
   looks up THIS node's own &p_expression->object and found
   nothing there at all, so it couldn't narrow anything: dp kept
   looking possibly-null inside the loop body even though the
   very condition that let you in already proved it wasn't.

   Fix: resolve p_expression->left->object down to the real,
   persistent destination object (following one REF hop, same
   as every other consumer of a REF alternative) and seed this
   assignment expression's own object as a REF to THAT -- so any
   later lookup on &p_expression->object transparently finds
   dp's real, correctly narrowed alternatives.
*/
                {
                    const struct object* p_dest_obj = &p_expression->left->object;
                    const struct flow3_key_alternatives* _Opt p_dest_alts =
                    flow3_map_search_up(ctx->p_current_flow3_map, p_dest_obj);
                    if (p_dest_alts &&
                        p_dest_alts->alternatives.size == 1 &&
                        p_dest_alts->alternatives.data[0]->value_relation == FLOW3_RELATION_EQUAL &&
                        p_dest_alts->alternatives.data[0]->value_kind == FLOW3_VALUE_KIND_REF &&
                        p_dest_alts->alternatives.data[0]->value.p != NULL)
                    {
                        p_dest_obj = p_dest_alts->alternatives.data[0]->value.p;
                    }

                    struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                    if (e == NULL) throw;
                    flow3_alternatives_clear(&e->alternatives);
                    struct flow3_alternative a = {
                        .value_kind = FLOW3_VALUE_KIND_REF,
                        .value = {.p = p_dest_obj},
                        .value_relation = FLOW3_RELATION_EQUAL,
                        .imaginary = FLOW3_IMAGINARY_NONE,
                        .origin = ctx->p_current_flow3_map,
                        .line = p_expression->first_token->line
                    };
                    flow3_alternatives_add(&e->alternatives, &a);
                }

                // ---- Branch maps for boolean context ----
                struct flow3_map* _Opt p_true = flow3_narrow_map_branch(&ctx->flow3_map_arena,
                    ctx->p_current_flow3_map,
                    &p_expression->left->object,
                    true,
                    p_expression,
                    p_expression->first_token->line);
                if (p_true == NULL)
                    throw;

                struct flow3_map* _Opt p_false = flow3_narrow_map_branch(&ctx->flow3_map_arena,
                    ctx->p_current_flow3_map,
                    &p_expression->left->object,
                    false,
                    p_expression,
                    p_expression->first_token->line);
                if (p_false == NULL)
                    throw;

                flow3_tag_branch_pair(p_true, p_false);

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
                _Assert(p_expression->right != NULL);
                _Assert(p_expression->left != NULL);

                struct flow3_branch_pair lhs_pair2 = flow3_visit_expression(ctx, p_expression->left);
                flow3_visit_expression(ctx, p_expression->right);

                const struct object* _Opt p_left_obj = object_get_referenced(&p_expression->left->object);
                struct flow3_key_alternatives* _Opt p_lhs_entry = flow3_map_search_up(ctx->p_current_flow3_map, p_left_obj);

                /* Writing the destination invalidates any predicate over it. */
                flow3_predicate_invalidate(ctx, p_left_obj);

                /* Compound assignment folds per LHS alternative, so a correlated
   join survives it (e.g. `if(c)a=1;else a=3; a+=10;` -> {11,13}).
   Iterate every alternative -- never data[0] -- keeping each value's
   branch origin. A pointer alternative (p += n / p -= n) is kept as-is:
   arithmetic can't turn a valid pointer into a null one. If any
   alternative can't be folded, degrade the whole destination to ANY. */
                const bool rhs_known = object_has_known_value(&p_expression->right->object);
                const signed long long rv =
                rhs_known ? object_to_signed_long_long(&p_expression->right->object) : 0;

                struct flow3_alternatives new_alts = { 0 };
                bool all_handled = (p_lhs_entry != NULL && p_lhs_entry->alternatives.size > 0);

                for (int i = 0; all_handled && i < p_lhs_entry->alternatives.size; i++)
                {
                    const struct flow3_alternative* la = p_lhs_entry->alternatives.data[i];

                    if (la->value_kind == FLOW3_VALUE_KIND_PTR)
                    {
                        struct flow3_alternative a = *la; /* keep pointer alternative */
                        flow3_alternatives_add(&new_alts, &a);
                    }
                    else if (rhs_known &&
                        la->value_relation == FLOW3_RELATION_EQUAL &&
                        (la->value_kind == FLOW3_VALUE_KIND_SIGNED ||
                        la->value_kind == FLOW3_VALUE_KIND_UNSIGNED))
                    {
                        const signed long long lv =
                        la->value_kind == FLOW3_VALUE_KIND_SIGNED
                        ? la->value.i
                        : (signed long long)la->value.u;
                        signed long long result = lv;

                        switch (p_expression->expression_type)
                        {
                        case EXPR_ASSIGNMENT_PLUS_ASSIGN: result = lv + rv; break;
                        case EXPR_ASSIGNMENT_MINUS_ASSIGN: result = lv - rv; break;
                        case EXPR_ASSIGNMENT_MULTI_ASSIGN: result = lv * rv; break;
                        case EXPR_ASSIGNMENT_DIV_ASSIGN: result = rv != 0 ? lv / rv : lv; break;
                        case EXPR_ASSIGNMENT_MOD_ASSIGN: result = rv != 0 ? lv % rv : lv; break;
                        case EXPR_ASSIGNMENT_SHIFT_LEFT_ASSIGN: result = lv << rv; break;
                        case EXPR_ASSIGNMENT_SHIFT_RIGHT_ASSIGN: result = lv >> rv; break;
                        case EXPR_ASSIGNMENT_AND_ASSIGN: result = lv & rv; break;
                        case EXPR_ASSIGNMENT_OR_ASSIGN: result = lv | rv; break;
                        case EXPR_ASSIGNMENT_NOT_ASSIGN: result = lv ^ rv; break;
                        default: break;
                        }

                        struct flow3_alternative a = {
                            .value_kind = FLOW3_VALUE_KIND_SIGNED,
                            .value = {.i = result},
                            .value_relation = FLOW3_RELATION_EQUAL,
                            .imaginary = FLOW3_IMAGINARY_NONE,
                            .origin = la->origin,
                            .line = p_expression->right->first_token->line
                        };
                        flow3_alternatives_add(&new_alts, &a);
                    }
                    else
                    {
                        all_handled = false;
                    }
                }

                {
                    struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, p_left_obj);
                    if (e == NULL) throw;
                    flow3_alternatives_clear(&e->alternatives);
                    if (all_handled && new_alts.size > 0)
                    {
                        e->alternatives = new_alts; /* move */
                    }
                    else
                    {
                        flow3_alternatives_clear(&new_alts);
                        struct flow3_alternative a = {
                            .value_kind = FLOW3_VALUE_KIND_SIGNED,
                            .value = {.i = ANY_VALUE},
                            .value_relation = FLOW3_RELATION_ANY,
                            .imaginary = FLOW3_IMAGINARY_NONE,
                            .origin = ctx->p_current_flow3_map,
                            .line = p_expression->right->first_token->line
                        };
                        flow3_alternatives_add(&e->alternatives, &a);
                    }
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
                case EXPR_MULTIPLICATIVE_DIV: op = '/'; break;
                case EXPR_MULTIPLICATIVE_MOD: op = '%'; break;
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
                case EXPR_ADDITIVE_PLUS: op = '+'; break;
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
                _Assert(p_expression->left != NULL);

                flow3_visit_expression(ctx, p_expression->left);

                const struct type* p_target_type = &p_expression->type;

                /* Casting a TEMPORARY owner (a function return value) to a non-owner
   throws the ownership away with nothing left holding it -- e.g.
   `(int*) malloc(1)`. Moved here from expressions.c so that every
   diagnostic mentioning _Owner lives in flow3. */
                if ((p_expression->left->type.storage_class_specifier_flags & STORAGE_SPECIFIER_FUNCTION_RETURN) &&
                    type_is_owner(&p_expression->left->type) &&
                    !type_is_owner(p_target_type))
                {
                    diagnostic(W_FLOW_DISCARDING_OWNER,
                        ctx->ctx,
                        p_expression->first_token, NULL,
                        type_is_pointer(&p_expression->left->type)
                        ? "discarding _Owner pointer"
                        : "discarding _Owner");
                }

                if (type_is_owner(&p_expression->left->type) && type_is_owner(p_target_type))
                {
                    /* Owner-to-owner cast (e.g. `(void* _Owner)p_owner_field`)
               doesn't change identity -- it's the same object, just
               re-typed. Model the cast's result as a REF to the left
               operand's own object instead of manufacturing an
               independent derived value below, so a later move of the
               cast's result (e.g. passing it straight to free())
               correctly marks the ORIGINAL object as moved too.
               Without this, `free((void* _Owner)p->member); p->member
               = x;` treated the free() as moving only the cast's own
               throwaway temporary, leaving p->member looking
               still-live and falsely warning "discards _Owner without
               releasing it first" on the very next line (dogfooded on
               cake's own object.c). See
               samples/flow3/owner-cast-move-through-member.c. */
                    struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                    if (e == NULL) throw;
                    flow3_alternatives_clear(&e->alternatives);
                    struct flow3_alternative a = {
                        .value_kind = FLOW3_VALUE_KIND_REF,
                        .value = {.p = &p_expression->left->object},
                        .value_relation = FLOW3_RELATION_EQUAL,
                        .imaginary = FLOW3_IMAGINARY_NONE,
                        .origin = ctx->p_current_flow3_map,
                        .line = p_expression->first_token->line
                    };
                    flow3_alternatives_add(&e->alternatives, &a);
                    break;
                }

                const struct object* p_src_obj = &p_expression->left->object;
                const struct flow3_key_alternatives* _Opt p_src_entry =
                flow3_map_search_up(ctx->p_current_flow3_map, p_src_obj);

                if (p_src_entry == NULL)
                {
                    {
                        struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                        if (e == NULL) throw;
                        flow3_alternatives_clear(&e->alternatives);
                        struct flow3_alternative a = {
                            .value_kind = FLOW3_VALUE_KIND_SIGNED,
                            .value = {.i = ANY_VALUE},
                            .value_relation = FLOW3_RELATION_ANY,
                            .imaginary = FLOW3_IMAGINARY_NONE,
                            .origin = ctx->p_current_flow3_map,
                            .line = p_expression->first_token->line
                        };
                        flow3_alternatives_add(&e->alternatives, &a);
                    }
                    break;
                }

                struct flow3_alternatives new_alts = { 0 };
                bool all_handled = true;

                /* Cast every source value. Iterate REF alternatives per-alternative
   (an operand can alias several objects) instead of a size==1 /
   data[0] shortcut, and keep each value's branch origin so the cast
   stays correlated. */
                for (int i = 0; all_handled && i < p_src_entry->alternatives.size; i++)
                {
                    const struct flow3_alternative* src_alt = p_src_entry->alternatives.data[i];
                    if (src_alt->value_kind == FLOW3_VALUE_KIND_REF && src_alt->value.p != NULL)
                    {
                        const struct flow3_key_alternatives* _Opt resolved =
                        flow3_map_search_up(ctx->p_current_flow3_map, src_alt->value.p);
                        if (resolved == NULL) { all_handled = false; break; }
                        for (int j = 0; j < resolved->alternatives.size; j++)
                        {
                            const struct flow3_alternative* v = resolved->alternatives.data[j];
                            if (!flow3_cast_one_value(ctx, v, p_target_type, &new_alts,
                                flow3_origin_more_specific(v->origin, src_alt->origin),
                                p_expression->first_token->line))
                            {
                                all_handled = false; break;
                            }
                        }
                    }
                    else
                    {
                        if (!flow3_cast_one_value(ctx, src_alt, p_target_type, &new_alts,
                            src_alt->origin, p_expression->first_token->line))
                        {
                            all_handled = false; break;
                        }
                    }
                }

                if (all_handled && new_alts.size > 0)
                {
                    struct flow3_key_alternatives* _Opt dst = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                    if (dst == NULL) throw;
                    flow3_alternatives_clear(&dst->alternatives);
                    dst->alternatives = new_alts; /* move */
                }
                else
                {
                    flow3_alternatives_clear(&new_alts);
                    {
                        struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                        if (e == NULL) throw;
                        flow3_alternatives_clear(&e->alternatives);
                        struct flow3_alternative a = {
                            .value_kind = FLOW3_VALUE_KIND_SIGNED,
                            .value = {.i = ANY_VALUE},
                            .value_relation = FLOW3_RELATION_ANY,
                            .imaginary = FLOW3_IMAGINARY_NONE,
                            .origin = ctx->p_current_flow3_map,
                            .line = p_expression->first_token->line
                        };
                        flow3_alternatives_add(&e->alternatives, &a);
                    }
                }

                /* Casting an owner to an _Owner target transfers ownership: the source
   is moved into the cast result. Without this, `free((void* _Owner)s)`
   freed the cast temporary but left the original `s` looking un-moved,
   producing a false "owner object 's' not moved" leak warning. */
                if (type_is_owner(p_target_type) && type_is_owner(&p_expression->left->type))
                {
                    const struct object* _Opt p_src_var = object_get_referenced(&p_expression->left->object);
                    if (p_src_var != NULL)
                        flow3_map_set_object_moved(ctx->p_current_flow3_map, p_src_var, p_expression->first_token->line);
                }

                flow3_map_remove(ctx->p_current_flow3_map, &p_expression->left->object);
                break;
            }
            break;

        case EXPR_SHIFT_RIGHT:
        case EXPR_SHIFT_LEFT:
            {
                _Assert(p_expression->left != NULL);
                _Assert(p_expression->right != NULL);

                flow3_visit_expression(ctx, p_expression->left);
                flow3_visit_expression(ctx, p_expression->right);

                /* Fold across all alternatives (per-alternative REF resolution and
   join correlation), like the other binary arithmetic operators --
   no size==1 / data[0] shortcut. */
                flow3_evaluate_binary_arithmetic(ctx, p_expression->left, p_expression->right,
                    p_expression,
                    (p_expression->expression_type == EXPR_SHIFT_LEFT) ? '<' : '>');

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
                _Assert(p_expression->right != NULL);
                _Assert(p_expression->left != NULL);

                flow3_visit_expression(ctx, p_expression->left);
                flow3_visit_expression(ctx, p_expression->right);

                // Attempt to fold the relational expression across ALL alternatives
                // of both operands (per-alternative REF resolution; no size==1).
                int fold_result = flow3_evaluate_relational_multi(ctx,
                    p_expression->left,
                    p_expression->right,
                    p_expression->expression_type);

                if (fold_result != -1)
                {
                    // Expression always true or always false
                    {
                        struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                        if (e == NULL) throw;
                        flow3_alternatives_clear(&e->alternatives);
                        struct flow3_alternative a = {
                            .value_kind = FLOW3_VALUE_KIND_SIGNED,
                            .value = {.i = fold_result ? 1 : 0},
                            .value_relation = FLOW3_RELATION_EQUAL,
                            .imaginary = FLOW3_IMAGINARY_NONE,
                            .origin = ctx->p_current_flow3_map,
                            .line = p_expression->first_token->line
                        };
                        flow3_alternatives_add(&e->alternatives, &a);
                    }
                    struct flow3_map* _Opt p_dead = flow3_map_arena_new_dead(&ctx->flow3_map_arena,
                        ctx->p_current_flow3_map,
                        FLOW3_MAP_REL_DEAD);
                    if (p_dead == NULL)
                        throw;

                    return (struct flow3_branch_pair)
                    {
                        fold_result ? ctx->p_current_flow3_map : p_dead,
                        fold_result ? p_dead : ctx->p_current_flow3_map
                    };
                }

                // Not foldable: seed the per-path boolean result (or ANY) so an
                // enclosing ||/&& / compile_assert can reason per path.
                flow3_seed_comparison_result(ctx, p_expression);

                /* ... but if this compares a scalar variable against a constant, we
   can still narrow the variable on each branch (true: var OP c,
   false: var !OP c). This is what lets `if (a > 0)` -- and, via the
   EXPR_UNARY_ASSERT true-branch merge, `_Assert(a > 0)` -- record the
   half-line fact so a later compile_assert(a > 0) can prove it. */
                {
                    long long cst = 0;
                    const struct expression* _Opt p_var_expr = NULL;
                    enum expression_type narrow_op = p_expression->expression_type;

                    if (flow3_operand_is_single_constant(ctx, p_expression->right, &cst))
                    {
                        /* var OP const */
                        p_var_expr = p_expression->left;
                    }
                    else if (flow3_operand_is_single_constant(ctx, p_expression->left, &cst))
                    {
                        /* const OP var  ==  var swapped(OP) const */
                        p_var_expr = p_expression->right;
                        narrow_op = flow3_swap_relational(p_expression->expression_type);
                    }

                    if (p_var_expr)
                    {
                        struct flow3_map* _Opt p_true = flow3_map_arena_new_branch(&ctx->flow3_map_arena, ctx->p_current_flow3_map, true, p_expression);
                        struct flow3_map* _Opt p_false = flow3_map_arena_new_branch(&ctx->flow3_map_arena, ctx->p_current_flow3_map, false, p_expression);
                        if (p_true == NULL || p_false == NULL)
                            throw;

                        flow3_tag_branch_pair(p_true, p_false);
                        flow3_narrow_operand_relational(ctx, p_var_expr, cst, narrow_op,
                            p_true, p_false, p_expression->first_token->line);
                        return (struct flow3_branch_pair) { p_true, p_false };
                    }
                }
                return (struct flow3_branch_pair) { ctx->p_current_flow3_map, ctx->p_current_flow3_map };
            }
            break;
        case EXPR_EQUALITY_NOT_EQUAL:
        case EXPR_EQUALITY_EQUAL:
            {
                _Assert(p_expression->right != NULL);
                _Assert(p_expression->left != NULL);

                flow3_visit_expression(ctx, p_expression->left);
                flow3_visit_expression(ctx, p_expression->right);

                struct flow3_key_alternatives* _Opt p_result_alternatives = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                if (p_result_alternatives == NULL) throw;
                flow3_alternatives_clear(&p_result_alternatives->alternatives);

                const bool is_equal_op = (p_expression->expression_type == EXPR_EQUALITY_EQUAL);

                /* Fold across ALL alternatives of both operands. A constant is simply
   an operand with a single alternative -- no special case. */
                int fold = flow3_evaluate_equality_multi(ctx, p_expression->left, p_expression->right, is_equal_op);
                if (fold != -1)
                {
                    {
                        struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                        if (e == NULL) throw;
                        flow3_alternatives_clear(&e->alternatives);
                        struct flow3_alternative a = {
                            .value_kind = FLOW3_VALUE_KIND_SIGNED,
                            .value = {.i = fold ? 1 : 0},
                            .value_relation = FLOW3_RELATION_EQUAL,
                            .imaginary = FLOW3_IMAGINARY_NONE,
                            .origin = ctx->p_current_flow3_map,
                            .line = p_expression->first_token->line
                        };
                        flow3_alternatives_add(&e->alternatives, &a);
                    }
                    struct flow3_map* _Opt p_dead = flow3_map_arena_new_dead(&ctx->flow3_map_arena,
                        ctx->p_current_flow3_map,
                        FLOW3_MAP_EQ_DEAD);
                    if (p_dead == NULL)
                        throw;

                    return (struct flow3_branch_pair)
                    {
                        fold ? ctx->p_current_flow3_map : p_dead,
                        fold ? p_dead : ctx->p_current_flow3_map
                    };
                }

                /* Not foldable: if one operand is a single constant, narrow the other
   on each branch. (A constant naturally collapses to one value across
   its alternatives.) */
                long long cst = 0;
                const struct expression* _Opt p_var_expr = NULL;
                if (flow3_operand_is_single_constant(ctx, p_expression->right, &cst))
                    p_var_expr = p_expression->left;
                else if (flow3_operand_is_single_constant(ctx, p_expression->left, &cst))
                    p_var_expr = p_expression->right;

                if (p_var_expr)
                {
                    struct flow3_map* _Opt p_true = flow3_map_arena_new_branch(&ctx->flow3_map_arena, ctx->p_current_flow3_map, true, p_expression);
                    struct flow3_map* _Opt p_false = flow3_map_arena_new_branch(&ctx->flow3_map_arena, ctx->p_current_flow3_map, false, p_expression);
                    if (p_true == NULL || p_false == NULL)
                        throw;

                    flow3_tag_branch_pair(p_true, p_false);
                    flow3_narrow_operand_equality(ctx, p_var_expr, cst, is_equal_op,
                        p_true, p_false, p_expression->first_token->line);
                    flow3_seed_comparison_result(ctx, p_expression);
                    return (struct flow3_branch_pair) { p_true, p_false };
                }

                /* -------- Fallback: unknown -------- */
                flow3_seed_comparison_result(ctx, p_expression);
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
                _Assert(p_expression->right != NULL);
                _Assert(p_expression->left != NULL);

                if (object_has_constant_value(&p_expression->left->object) &&
                    object_has_constant_value(&p_expression->right->object))
                {
                    const long long result = (object_to_signed_long_long(&p_expression->left->object) ||
                        object_to_signed_long_long(&p_expression->right->object)) ? 1 : 0;
                    {
                        struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                        if (e == NULL) throw;
                        flow3_alternatives_clear(&e->alternatives);
                        struct flow3_alternative a = {
                            .value_kind = FLOW3_VALUE_KIND_SIGNED,
                            .value = {.i = result},
                            .value_relation = FLOW3_RELATION_EQUAL,
                            .imaginary = FLOW3_IMAGINARY_NONE,
                            .origin = ctx->p_current_flow3_map,
                            .line = p_expression->first_token->line
                        };
                        flow3_alternatives_add(&e->alternatives, &a);
                    }
                    if (result != 0)
                    {
                        struct flow3_map* _Opt p_dead = flow3_map_arena_new_dead(&ctx->flow3_map_arena, p_before, FLOW3_MAP_OR_DEAD_FALSE);
                        if (p_dead == NULL)
                            throw;

                        return (struct flow3_branch_pair) { ctx->p_current_flow3_map, p_dead };
                    }
                    else
                    {
                        struct flow3_map* _Opt p_dead = flow3_map_arena_new_dead(&ctx->flow3_map_arena, p_before, FLOW3_MAP_OR_DEAD_TRUE);
                        if (p_dead == NULL)
                            throw;

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
                struct flow3_map* _Opt p_or_true = flow3_map_arena_new_branch(&ctx->flow3_map_arena, p_before, true, p_expression);
                if (p_or_true == NULL)
                    throw;

                flow3_map_merge_a_b(p_or_true, left_pair.p_true, right_pair.p_true);

                /* Seed this OR's per-path boolean value. For each path (identified by
   origin), `L || R` is true if L is true there, else R's value there.
   L was evaluated on p_before; R on left's false map. Only applied
   when both sides are clean per-path booleans -- otherwise the result
   is left unseeded (previous behavior). This lets compile_assert see
   a 0 exactly on a path where neither disjunct holds. */
                {
                    const struct flow3_key_alternatives* _Opt L =
                    flow3_map_search_up(p_before, &p_expression->left->object);
                    const struct flow3_key_alternatives* _Opt R =
                    flow3_map_search_up(left_pair.p_false, &p_expression->right->object);

                    struct flow3_alternatives out = { 0 };
                    bool ok = (L != NULL && R != NULL);

                    for (int i = 0; ok && i < L->alternatives.size; i++)
                    {
                        const struct flow3_alternative* l = L->alternatives.data[i];
                        if (flow3_alternative_is_true(l))
                        {
                            struct flow3_alternative a = {
                                .value_kind = FLOW3_VALUE_KIND_SIGNED, .value = {.i = 1},
                                .value_relation = FLOW3_RELATION_EQUAL, .imaginary = FLOW3_IMAGINARY_NONE,
                                .origin = l->origin, .line = p_expression->first_token->line
                            };
                            flow3_alternatives_add(&out, &a);
                        }
                        else if (flow3_alternative_is_zero(l))
                        {
                            bool matched = false;
                            for (int j = 0; j < R->alternatives.size; j++)
                            {
                                const struct flow3_alternative* r = R->alternatives.data[j];
                                if (!flow3_origins_compatible(l->origin, r->origin))
                                    continue;
                                bool r_true = flow3_alternative_is_true(r);
                                bool r_zero = flow3_alternative_is_zero(r);
                                if (!r_true && !r_zero) { ok = false; break; }
                                struct flow3_alternative a = {
                                    .value_kind = FLOW3_VALUE_KIND_SIGNED, .value = {.i = r_true ? 1 : 0},
                                    .value_relation = FLOW3_RELATION_EQUAL, .imaginary = FLOW3_IMAGINARY_NONE,
                                    .origin = flow3_origin_more_specific(l->origin, r->origin),
                                    .line = p_expression->first_token->line
                                };
                                flow3_alternatives_add(&out, &a);
                                matched = true;
                            }
                            if (!matched) ok = false;
                        }
                        else
                        {
                            ok = false; /* L not a clean boolean on this path */
                        }
                    }

                    if (ok && out.size > 0)
                    {
                        struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                        if (e == NULL) throw;
                        flow3_alternatives_clear(&e->alternatives);
                        e->alternatives = out; /* move */
                    }
                    else
                    {
                        flow3_alternatives_clear(&out);
                    }
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
                _Assert(p_expression->right != NULL);
                _Assert(p_expression->left != NULL);

                if (object_has_constant_value(&p_expression->left->object) &&
                    object_has_constant_value(&p_expression->right->object))
                {
                    const long long result = (object_to_signed_long_long(&p_expression->left->object) &&
                        object_to_signed_long_long(&p_expression->right->object)) ? 1 : 0;
                    {
                        struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                        if (e == NULL) throw;
                        flow3_alternatives_clear(&e->alternatives);
                        struct flow3_alternative a = {
                            .value_kind = FLOW3_VALUE_KIND_SIGNED,
                            .value = {.i = result},
                            .value_relation = FLOW3_RELATION_EQUAL,
                            .imaginary = FLOW3_IMAGINARY_NONE,
                            .origin = ctx->p_current_flow3_map,
                            .line = p_expression->first_token->line
                        };
                        flow3_alternatives_add(&e->alternatives, &a);
                    }
                    if (result != 0)
                    {
                        struct flow3_map* _Opt p_dead = flow3_map_arena_new_dead(&ctx->flow3_map_arena, p_before, FLOW3_MAP_AND_DEAD_FALSE);
                        if (p_dead == NULL)
                            throw;

                        return (struct flow3_branch_pair) { ctx->p_current_flow3_map, p_dead };
                    }
                    else
                    {
                        struct flow3_map* _Opt p_dead = flow3_map_arena_new_dead(&ctx->flow3_map_arena, p_before, FLOW3_MAP_AND_DEAD_TRUE);
                        if (p_dead == NULL)
                            throw;

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
                struct flow3_map* _Opt p_and_false = flow3_map_arena_new_branch(&ctx->flow3_map_arena, p_before, false, p_expression);
                if (p_and_false == NULL)
                    throw;

                flow3_map_merge_a_b(p_and_false, left_pair.p_false, right_pair.p_false);

                /* Seed this AND's per-path boolean value (dual of ||): for each path,
   `L && R` is 0 if L is false there, else R's value there. L was
   evaluated on p_before; R on left's true map. Only when both sides
   are clean per-path booleans; otherwise leave unseeded (previous
   behavior). Lets compile_assert see a 0 where either side fails. */
                {
                    const struct flow3_key_alternatives* _Opt L =
                    flow3_map_search_up(p_before, &p_expression->left->object);
                    const struct flow3_key_alternatives* _Opt R =
                    flow3_map_search_up(left_pair.p_true, &p_expression->right->object);

                    struct flow3_alternatives out = { 0 };
                    bool ok = (L != NULL && R != NULL);

                    for (int i = 0; ok && i < L->alternatives.size; i++)
                    {
                        const struct flow3_alternative* l = L->alternatives.data[i];
                        if (flow3_alternative_is_zero(l))
                        {
                            struct flow3_alternative a = {
                                .value_kind = FLOW3_VALUE_KIND_SIGNED, .value = {.i = 0},
                                .value_relation = FLOW3_RELATION_EQUAL, .imaginary = FLOW3_IMAGINARY_NONE,
                                .origin = l->origin, .line = p_expression->first_token->line
                            };
                            flow3_alternatives_add(&out, &a);
                        }
                        else if (flow3_alternative_is_true(l))
                        {
                            bool matched = false;
                            for (int j = 0; j < R->alternatives.size; j++)
                            {
                                const struct flow3_alternative* r = R->alternatives.data[j];
                                if (!flow3_origins_compatible(l->origin, r->origin))
                                    continue;
                                bool r_true = flow3_alternative_is_true(r);
                                bool r_zero = flow3_alternative_is_zero(r);
                                if (!r_true && !r_zero) { ok = false; break; }
                                struct flow3_alternative a = {
                                    .value_kind = FLOW3_VALUE_KIND_SIGNED, .value = {.i = r_true ? 1 : 0},
                                    .value_relation = FLOW3_RELATION_EQUAL, .imaginary = FLOW3_IMAGINARY_NONE,
                                    .origin = flow3_origin_more_specific(l->origin, r->origin),
                                    .line = p_expression->first_token->line
                                };
                                flow3_alternatives_add(&out, &a);
                                matched = true;
                            }
                            if (!matched) ok = false;
                        }
                        else
                        {
                            ok = false;
                        }
                    }

                    if (ok && out.size > 0)
                    {
                        struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                        if (e == NULL) throw;
                        flow3_alternatives_clear(&e->alternatives);
                        e->alternatives = out; /* move */
                    }
                    else
                    {
                        flow3_alternatives_clear(&out);
                    }
                }

                return (struct flow3_branch_pair) { right_pair.p_true, p_and_false };
            }

        case EXPR_INCLUSIVE_OR:
            _Assert(p_expression->right != NULL);
            _Assert(p_expression->left != NULL);
            if (object_has_constant_value(&p_expression->left->object) &&
                object_has_constant_value(&p_expression->right->object))
            {
                const long long lv = object_to_signed_long_long(&p_expression->left->object);
                const long long rv = object_to_signed_long_long(&p_expression->right->object);
                {
                    struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                    if (e == NULL) throw;
                    flow3_alternatives_clear(&e->alternatives);
                    struct flow3_alternative a = {
                        .value_kind = FLOW3_VALUE_KIND_SIGNED,
                        .value = {.i = lv | rv},
                        .value_relation = FLOW3_RELATION_EQUAL,
                        .imaginary = FLOW3_IMAGINARY_NONE,
                        .origin = ctx->p_current_flow3_map,
                        .line = p_expression->first_token->line
                    };
                    flow3_alternatives_add(&e->alternatives, &a);
                }
                break;
            }
            flow3_visit_expression(ctx, p_expression->left);
            flow3_visit_expression(ctx, p_expression->right);
            {
                struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                if (e == NULL) throw;
                flow3_alternatives_clear(&e->alternatives);
                struct flow3_alternative a = {
                    .value_kind = FLOW3_VALUE_KIND_SIGNED,
                    .value = {.i = ANY_VALUE},
                    .value_relation = FLOW3_RELATION_ANY,
                    .imaginary = FLOW3_IMAGINARY_NONE,
                    .origin = ctx->p_current_flow3_map,
                    .line = p_expression->first_token->line
                };
                flow3_alternatives_add(&e->alternatives, &a);
            }
            break;

        case EXPR_AND:
        case EXPR_EXCLUSIVE_OR:
            _Assert(p_expression->right != NULL);
            _Assert(p_expression->left != NULL);
            if (object_has_constant_value(&p_expression->left->object) &&
                object_has_constant_value(&p_expression->right->object))
            {
                const long long lv = object_to_signed_long_long(&p_expression->left->object);
                const long long rv = object_to_signed_long_long(&p_expression->right->object);
                const long long result = (p_expression->expression_type == EXPR_AND)
                ? (lv & rv)
                : (lv ^ rv);
                {
                    struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                    if (e == NULL) throw;
                    flow3_alternatives_clear(&e->alternatives);
                    struct flow3_alternative a = {
                        .value_kind = FLOW3_VALUE_KIND_SIGNED,
                        .value = {.i = result},
                        .value_relation = FLOW3_RELATION_EQUAL,
                        .imaginary = FLOW3_IMAGINARY_NONE,
                        .origin = ctx->p_current_flow3_map,
                        .line = p_expression->first_token->line
                    };
                    flow3_alternatives_add(&e->alternatives, &a);
                }
                break;
            }
            flow3_visit_expression(ctx, p_expression->left);
            flow3_visit_expression(ctx, p_expression->right);
            {
                struct flow3_key_alternatives* _Opt e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                if (e == NULL) throw;
                flow3_alternatives_clear(&e->alternatives);
                struct flow3_alternative a = {
                    .value_kind = FLOW3_VALUE_KIND_SIGNED,
                    .value = {.i = ANY_VALUE},
                    .value_relation = FLOW3_RELATION_ANY,
                    .imaginary = FLOW3_IMAGINARY_NONE,
                    .origin = ctx->p_current_flow3_map,
                    .line = p_expression->first_token->line
                };
                flow3_alternatives_add(&e->alternatives, &a);
            }
            break;

        case EXPR_UNARY_TRAITS:
            break;

        case EXPR_UNARY_IS_SAME:
            break;

        case EXPR_UNARY_DECLARATOR_ATTRIBUTE:
            break;

        case EXPR_EXPRESSION:
            {
                _Assert(p_expression->left != NULL);
                _Assert(p_expression->right != NULL);
                flow3_visit_expression(ctx, p_expression->left);
                /* Comma: the value (and branch state) of the right operand is what matters. */
                struct flow3_branch_pair pair = flow3_visit_expression(ctx, p_expression->right);

                /* Forward the right operand's value to the comma's OWN object, so a
   consumer that reads this node (e.g. a function-argument check) sees
   the comma's result -- otherwise `f((p = 0, p))` found no value on the
   comma node and missed that p was just set to null. Mirrors the value
   forwarding done for EXPR_PRIMARY_PARENTHESIS. */
                const struct expression* p_inner = skip_parenthesis(p_expression->right);
                const struct flow3_key_alternatives* _Opt p_inner_entry =
                flow3_map_search_up(ctx->p_current_flow3_map, &p_inner->object);
                if (p_inner_entry)
                {
                    struct flow3_key_alternatives* _Opt p_entry = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                    if (p_entry == NULL) throw;
                    if (p_entry)
                    {
                        flow3_alternatives_clear(&p_entry->alternatives);
                        flow3_alternatives_append(&p_entry->alternatives, &p_inner_entry->alternatives);
                    }
                }
                return pair;
            }

        case EXPR_CONDITIONAL:
            {
                _Assert(p_expression->condition_expr != NULL);
                _Assert(p_expression->right != NULL);

                struct flow3_branch_pair cond_pair = flow3_visit_expression(ctx, p_expression->condition_expr);
                cond_pair = flow3_ensure_branch_pair(ctx, ctx->p_current_flow3_map, cond_pair,
                    p_expression->condition_expr);

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

                    const struct flow3_key_alternatives* _Opt p_true_entry = flow3_map_search_up(cond_pair.p_true, &p_true_expr->object);
                    const struct flow3_key_alternatives* _Opt p_false_entry = flow3_map_search_up(cond_pair.p_false, &p_false_expr->object);

                    struct flow3_key_alternatives* _Opt p_result_entry = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                    if (p_result_entry == NULL) throw;

                    if (p_result_entry)
                    {
                        /*
                       Resolve REF alternatives inside the ARM that produced
                       them, instead of storing the REF for later.

                       `p ? p : ""` leaves the true arm holding a REF to p.
                       The arm itself is narrowed correctly -- a probe shows
                       exactly one alternative there -- but a REF is resolved
                       lazily at the point of USE, which is after the merge,
                       where p is back to both arms. The narrowing was
                       therefore discarded and the result carried a null the
                       expression cannot produce.

                       Resolving here binds each arm's value to the state
                       that arm actually had. See
                       samples/flow3/conditional-operator-null-guard.c.
                    */
                        flow3_alternatives_clear(&p_result_entry->alternatives);

                        struct
                        {
                            const struct flow3_key_alternatives* _Opt entry;
                            struct flow3_map* map;
                        } arms[2] = {
                            { p_true_entry, cond_pair.p_true },
                            { p_false_entry, cond_pair.p_false }
                        };

                        for (int ai = 0; ai < 2; ai++)
                        {
                            const struct flow3_key_alternatives* _Opt e = arms[ai].entry;
                            if (e == NULL || e->alternatives.size == 0)
                                continue;

                            for (int i = 0; i < e->alternatives.size; i++)
                            {
                                const struct flow3_alternative* a = e->alternatives.data[i];

                                if (a->value_kind == FLOW3_VALUE_KIND_REF &&
                                    a->value.p != NULL)
                                {
                                    const struct flow3_key_alternatives* _Opt p_target =
                                    flow3_map_search_up(arms[ai].map, a->value.p);

                                    if (p_target != NULL && p_target->alternatives.size > 0)
                                    {
                                        flow3_alternatives_append(&p_result_entry->alternatives,
                                            &p_target->alternatives);
                                        continue;
                                    }
                                }

                                flow3_alternatives_add(&p_result_entry->alternatives, a);
                            }
                        }
                        if (p_result_entry->alternatives.size == 0)
                        {
                            {
                                struct flow3_alternative a = {
                                    .value_kind = FLOW3_VALUE_KIND_SIGNED,
                                    .value = {.i = ANY_VALUE},
                                    .value_relation = FLOW3_RELATION_ANY,
                                    .imaginary = FLOW3_IMAGINARY_NONE,
                                    .origin = ctx->p_current_flow3_map,
                                    .line = p_expression->first_token->line
                                };
                                flow3_alternatives_add(&p_result_entry->alternatives, &a);
                            }
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
    }
    catch
    {
    }

    struct flow3_branch_pair identity_pair = { ctx->p_current_flow3_map, ctx->p_current_flow3_map };

    /* Non-boolean or unhandled expression: both branches are identical (no narrowing). */
    return identity_pair;
}

static void flow3_visit_expression_statement(struct flow3_visit_ctx* ctx, struct expression_statement* p_expression_statement)
{
    /* Only meant to bridge a report from THIS statement's own expression
       visit into a check running right after it (see the field comment in
       flow3.h) -- must not leak into an unrelated later statement. */
    ctx->p_pending_ended_report_obj = NULL;

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

/*
   The body of a compound statement without the trailing `//lint N` check.

   A function body needs the split: falling off the end of a function is an
   exit point whose checks (file-scope objects, _Clear/_Dtor parameter
   contracts) live in flow3_visit_declaration and therefore run AFTER this
   function returns. Consuming the lint token here would check the
   suppression against a queue that does not hold those diagnostics yet --
   the comment gets reported as "diagnostic 'N' not recognized" and the
   warning fires anyway. So flow3_visit_declaration calls this core and
   checks the lint token itself, once every exit diagnostic is queued.
*/
static void flow3_visit_compound_statement_core(struct flow3_visit_ctx* ctx, struct compound_statement* p_compound_statement)
{
    flow3_visit_block_item_list(ctx, &p_compound_statement->block_item_list);
    flow3_exit_block_visit_defer_list(ctx, &p_compound_statement->defer_list, p_compound_statement->last_token);
    flow3_defer_list_set_end_of_lifetime(ctx, &p_compound_statement->defer_list, p_compound_statement->last_token);
}

static void flow3_visit_compound_statement(struct flow3_visit_ctx* ctx, struct compound_statement* p_compound_statement)
{
    flow3_visit_compound_statement_core(ctx, p_compound_statement);

    if (p_compound_statement->lint_token)
    {
        flow3_check_dianostic_suppression(ctx, p_compound_statement->lint_token);
    }
}

static void flow3_visit_do_while_statement(struct flow3_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    _Assert(p_iteration_statement->first_token->type == TK_KEYWORD_DO);

    struct flow3_map* _Opt old_p_initial_map = ctx->p_initial_map;
    struct flow3_map* _Opt old_p_break_join_map = ctx->p_break_join_map;

    if (ctx->p_current_flow3_map == NULL)
        return;

    struct flow3_map* p_before = ctx->p_current_flow3_map;
    struct flow3_map* _Opt p_break_join = flow3_map_arena_new(&ctx->flow3_map_arena, p_before, FLOW3_MAP_BREAK_JOIN);
    if (p_break_join == NULL)
        return; /* no map to work with */

    ctx->p_initial_map = p_before;
    ctx->p_break_join_map = p_break_join;

    struct flow3_branch_pair do_cond_pair1 = { 0 };

    /*
     * do-while: the body executes first, then the condition is checked.
     * First pass — suppress warnings (warm up state for second pass).
     */
    diagnostic_stack_push_empty(&ctx->ctx->options.diagnostic_stack);

    flow3_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

    /*
       If every path through the first pass's body unconditionally
       transferred control away, ctx->p_current_flow3_map is now marked
       is_unreachable (see flow3_visit_jump_statement); there is no sound
       "condition after one iteration" state to compute, and running the
       second pass on top of it would just build on dead/dead-code-
       polluted state (see the identical reasoning in
       flow3_visit_while_statement).
    */
    const bool body_falls_through =
    !(ctx->p_current_flow3_map != NULL && ctx->p_current_flow3_map->is_unreachable);

    if (body_falls_through && p_iteration_statement->expression1)
    {
        do_cond_pair1 = flow3_visit_full_expression(ctx, p_iteration_statement->expression1);
        /* Narrow to true branch before running second-pass body, mirroring while. */
        ctx->p_current_flow3_map = do_cond_pair1.p_true;
    }

    /* Second pass — warnings on */
    diagnostic_stack_pop(&ctx->ctx->options.diagnostic_stack);

    if (body_falls_through)
    {
        flow3_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
    }
    else
    {
        /*
           Every path through the body diverges, so there is nothing
           sound to compute for a second iteration -- but the only pass
           that visited this body ran with diagnostics suppressed. Give
           it one more visit, fresh from p_before, purely so real
           diagnostics can fire; the result is discarded (see the
           identical reasoning in flow3_visit_while_statement).
        */
        ctx->p_current_flow3_map =
        flow3_map_arena_new(&ctx->flow3_map_arena, p_before, FLOW3_MAP_DO_WHILE_BODY_DIAG);
        flow3_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
    }

    if (ctx->p_current_flow3_map == NULL)
        return;

    struct flow3_map* p_after_body = ctx->p_current_flow3_map;
    struct flow3_branch_pair do_cond_pair2 = { 0 };
    if (body_falls_through && p_iteration_statement->expression1)
    {
        do_cond_pair2 = flow3_visit_full_expression(ctx, p_iteration_statement->expression1);
    }

    flow3_exit_block_visit_defer_list(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);

    struct flow3_map* p_false_branch_dw = do_cond_pair2.p_false ? do_cond_pair2.p_false
    : flow3_map_arena_new(&ctx->flow3_map_arena, p_after_body, FLOW3_MAP_DO_WHILE_FALSE);

    {
        /* Loop exit paths: condition false after an iteration, or break. */
        const struct flow3_map* arms[2];
        int num_arms = 0;
        if (body_falls_through)
        {
            arms[num_arms++] = p_false_branch_dw;
        }
        if (flow3_map_arm_has_entries(p_break_join, p_before))
        {
            arms[num_arms++] = p_break_join;
        }
        if (num_arms > 0)
        {
            flow3_map_merge_arms(p_before, arms, num_arms);
        }
    }

    ctx->p_current_flow3_map = p_before;

    flow3_defer_list_set_end_of_lifetime(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);

    ctx->p_initial_map = old_p_initial_map;
    ctx->p_break_join_map = old_p_break_join_map;
}

static void flow3_visit_while_statement(struct flow3_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    _Assert(p_iteration_statement->first_token->type == TK_KEYWORD_WHILE);

    if (p_iteration_statement->expression1 == NULL)
    {
        return;
    }

    //const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    struct flow3_map* _Opt old_p_initial_map = ctx->p_initial_map;
    struct flow3_map* _Opt old_p_break_join_map = ctx->p_break_join_map;

    if (ctx->p_current_flow3_map == NULL)
        return;

    struct flow3_map* p_before = ctx->p_current_flow3_map;
    struct flow3_map* _Opt p_break_join = flow3_map_arena_new(&ctx->flow3_map_arena, p_before, FLOW3_MAP_BREAK_JOIN);
    if (p_break_join == NULL)
        return; /* no map to work with */

    ctx->p_initial_map = p_before;
    ctx->p_break_join_map = p_break_join;

    /* First pass — suppress warnings */
    diagnostic_stack_push_empty(&ctx->ctx->options.diagnostic_stack);

    struct flow3_branch_pair w_pair1 = flow3_visit_full_expression(ctx, p_iteration_statement->expression1);
    w_pair1 = flow3_ensure_branch_pair(ctx, p_before, w_pair1,
        p_iteration_statement->expression1);
    ctx->p_current_flow3_map = w_pair1.p_true;

    flow3_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

    /* Second pass — warnings on */
    diagnostic_stack_pop(&ctx->ctx->options.diagnostic_stack);

    /*
       If every path through the first pass's body unconditionally
       transferred control away (return/break/continue/goto/throw),
       ctx->p_current_flow3_map is now marked is_unreachable by
       flow3_visit_jump_statement, and there is no sound "after one
       iteration, re-check the condition" state to compute: the second
       pass would just build on top of dead (possibly dead-code-polluted)
       state. Skip the second pass entirely in that case, exactly the
       way merge_arms already skips is_unreachable arms elsewhere.
    */
    const bool body_falls_through =
    !(ctx->p_current_flow3_map != NULL && ctx->p_current_flow3_map->is_unreachable);

    struct flow3_branch_pair w_pair2 = { 0 };
    if (body_falls_through)
    {
        w_pair2 = flow3_visit_full_expression(ctx, p_iteration_statement->expression1);
        w_pair2 = flow3_ensure_branch_pair(ctx, ctx->p_current_flow3_map, w_pair2,
            p_iteration_statement->expression1);

        ctx->p_current_flow3_map = w_pair2.p_true;

        flow3_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
    }
    else
    {
        /*
           Every path through the body diverges (return/break/continue/
           goto/throw), so there is no "after one iteration" state worth
           computing -- but the only pass that visited this body ran
           with diagnostics suppressed (it was just warming up state).
           Re-visit once more, fresh from the original true-branch entry
           point, purely so real diagnostics (e.g. unreachable-code) can
           fire; the resulting map is discarded, not merged anywhere,
           since this arm still does not fall through.
        */
        struct flow3_branch_pair w_pair1_diag = { 0 };
        w_pair1_diag = flow3_visit_full_expression(ctx, p_iteration_statement->expression1);
        w_pair1_diag = flow3_ensure_branch_pair(ctx, p_before, w_pair1_diag,
            p_iteration_statement->expression1);
        ctx->p_current_flow3_map = w_pair1_diag.p_true;
        flow3_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
    }

    flow3_exit_block_visit_defer_list(ctx, &p_iteration_statement->defer_list,
        p_iteration_statement->secondary_block->last_token);

    {
        /* Loop exit paths: condition false before the first iteration
           (w_pair1.p_false), condition false after an iteration
           (w_pair2.p_false), or break. */
        const struct flow3_map* arms[3];
        int num_arms = 0;
        arms[num_arms++] = w_pair1.p_false;
        if (body_falls_through)
        {
            arms[num_arms++] = w_pair2.p_false;
        }
        if (flow3_map_arm_has_entries(p_break_join, p_before))
        {
            arms[num_arms++] = p_break_join;
        }
        flow3_map_merge_arms(p_before, arms, num_arms);
    }

    ctx->p_current_flow3_map = p_before;

    flow3_defer_list_set_end_of_lifetime(ctx, &p_iteration_statement->defer_list,
        p_iteration_statement->secondary_block->last_token);

    ctx->p_initial_map = old_p_initial_map;
    ctx->p_break_join_map = old_p_break_join_map;
}

static void flow3_visit_for_statement(struct flow3_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    _Assert(p_iteration_statement->first_token->type == TK_KEYWORD_FOR);
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

    if (ctx->p_current_flow3_map == NULL)
        return;

    struct flow3_map* p_before = ctx->p_current_flow3_map;
    struct flow3_map* _Opt p_break_join = flow3_map_arena_new(&ctx->flow3_map_arena, p_before, FLOW3_MAP_BREAK_JOIN);
    if (p_break_join == NULL)
        return; /* no map to work with */

    ctx->p_initial_map = p_before;
    ctx->p_break_join_map = p_break_join;

    /* First pass — suppress warnings */
    diagnostic_stack_push_empty(&ctx->ctx->options.diagnostic_stack);

    struct flow3_branch_pair for_pair1 = { 0 };
    if (p_condition)
    {
        for_pair1 = flow3_visit_full_expression(ctx, p_condition);
        for_pair1 = flow3_ensure_branch_pair(ctx, ctx->p_current_flow3_map, for_pair1,
            p_condition);
        ctx->p_current_flow3_map = for_pair1.p_true;
    }

    flow3_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

    /*
       If every path through the first pass's body unconditionally
       transferred control away, ctx->p_current_flow3_map is now marked
       is_unreachable (see flow3_visit_jump_statement). The loop's increment
       expression (p_next) and a second pass would then just build on
       top of dead/dead-code-polluted state — skip both, mirroring
       flow3_visit_while_statement / flow3_visit_do_while_statement.
    */
    const bool body_falls_through =
    !(ctx->p_current_flow3_map != NULL && ctx->p_current_flow3_map->is_unreachable);

    if (body_falls_through && p_next)
    {
        flow3_visit_full_expression(ctx, p_next);
    }

    /* Second pass — warnings on */
    diagnostic_stack_pop(&ctx->ctx->options.diagnostic_stack);

    //struct flow3_map* p_after_body = ctx->p_current_flow3_map;
    struct flow3_branch_pair for_pair2 = { 0 };
    if (body_falls_through && p_condition)
    {
        for_pair2 = flow3_visit_full_expression(ctx, p_condition);
        for_pair2 = flow3_ensure_branch_pair(ctx, ctx->p_current_flow3_map, for_pair2,
            p_condition);
        ctx->p_current_flow3_map = for_pair2.p_true;
    }

    if (body_falls_through)
    {
        flow3_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

        if (p_next)
        {
            flow3_visit_full_expression(ctx, p_next);
        }
    }
    else
    {
        /*
           Every path through the body diverges, so there is nothing
           sound to compute for a second iteration -- but the only pass
           that visited this body ran with diagnostics suppressed. Give
           it one more visit, fresh from p_before, purely so real
           diagnostics can fire; the result is discarded (see the
           identical reasoning in flow3_visit_while_statement).
        */
        ctx->p_current_flow3_map =
        flow3_map_arena_new(&ctx->flow3_map_arena, p_before, FLOW3_MAP_FOR_BODY_DIAG);
        flow3_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
    }

    flow3_exit_block_visit_defer_list(ctx, &p_iteration_statement->defer_list,
        p_iteration_statement->secondary_block->last_token);

    {
        /* Loop exit paths: condition false before the first iteration,
           condition false after an iteration, or break. */
        const struct flow3_map* arms[3];
        int num_arms = 0;
        if (p_condition)
        {
            arms[num_arms++] = for_pair1.p_false;
            if (body_falls_through)
            {
                arms[num_arms++] = for_pair2.p_false;
            }
        }
        if (flow3_map_arm_has_entries(p_break_join, p_before))
        {
            arms[num_arms++] = p_break_join;
        }
        if (num_arms > 0)
        {
            flow3_map_merge_arms(p_before, arms, num_arms);
        }
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
    case TK_KEYWORD_WHILE:
        flow3_visit_while_statement(ctx, p_iteration_statement);
        break;
    case TK_KEYWORD_DO:
        flow3_visit_do_while_statement(ctx, p_iteration_statement);
        break;
    case TK_KEYWORD_FOR:
        flow3_visit_for_statement(ctx, p_iteration_statement);
        break;
    default:
        _Assert(false);
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
        struct object* _Opt p_obj = ctx->allocated_object_arena.data[i];
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
    for (const struct flow3_map* _Opt cur = ctx->p_current_flow3_map; cur != NULL; cur = cur->p_parent_map)
    {
        if (!cur->buckets)
        {
            continue;
        }
        for (int i = 0; i < cur->num_of_buckets; i++)
        {
            for (const struct flow3_key_alternatives* _Opt e = cur->buckets[i]; e; e = e->next)
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

/*
   Render a _Clear/_Ctor/_Dtor parameter exit-check's subject as source code
   would actually spell it -- "source->tail" -- instead of the previous
   "'source' pointee (.tail)", which described the object's shape in terms
   only the analysis itself would use. p_obj->member_designator is always
   either empty (the parameter's pointee itself, no member) or a dotted path
   starting with '.' (".tail", ".in.a", ...): since `param_name` is always a
   pointer, the FIRST level of access is through '->', not '.', so the
   leading dot is stripped and replaced with '->'; any further dots (nested
   struct members reached after that first dereference) are kept as-is.
   User-reported.
*/
static void flow3_param_member_name_to_string(const char* param_name,
    const char* _Opt member_designator, struct osstream* ss)
{
    ss_clear(ss);
    ss_fprintf(ss, "%s", param_name);
    if (member_designator != NULL && member_designator[0] != '\0')
    {
        ss_fprintf(ss, "->%s",
            member_designator[0] == '.' ? member_designator + 1 : member_designator);
    }
}

/*
   _Clear is similar to _Dtor -- it describes a contract the CALLEE
   must fulfill by the time it exits, checked against the pointee that
   flow3_parameter_object_init already set up (the synthetic arena
   object every non-_Opt pointer parameter is wired to at entry). The
   difference from _Dtor: instead of the pointee's lifetime having
   ended, every member of the pointee must be exactly 0.
*/
static void flow3_check_clear_object_is_zero_at_exit(struct flow3_visit_ctx* ctx,
    const struct object* p_obj,
    const char* param_name,
    const struct marker* marker,
    const struct token* p_exit_token)
{
    if (p_obj->members.head)
    {
        for (const struct object* _Opt member = p_obj->members.head; member; member = member->next)
        {
            flow3_check_clear_object_is_zero_at_exit(ctx, member, param_name, marker, p_exit_token);
        }
        return;
    }

    const struct flow3_key_alternatives* _Opt e = flow3_map_search_up(ctx->p_current_flow3_map, p_obj);
    if (e == NULL || e->alternatives.size == 0)
    {
        /* Never touched at all -- the _Clear contract requires the
           callee to actively zero every member, so no evidence of a
           write is exactly as much a violation as a tracked non-zero
           value would be (unlike, say, an untouched local, there is no
           "inherits whatever it already was" story for a member the
           analysis has no information about here). */
        struct osstream name_ss = { 0 };
        flow3_param_member_name_to_string(param_name, p_obj->member_designator, &name_ss);
        if (diagnostic(W_FLOW_CLEAR_NOT_ZERO_AT_EXIT,
            ctx->ctx,
            NULL,
            marker,
            "_Clear parameter '%s' is never set to zero",
            name_ss.c_str ? name_ss.c_str : param_name))
        {
            /* W_LOCATION, not W_INFO: this note only exists to point at the
               diagnostic above, so it is attached as that entry's child (see
               the is_location branch in diagnostic()). Children are printed
               with the parent and freed with it, so a `//lint N` that removes
               the parent removes this too -- with W_INFO the note survived
               the suppression and still counted towards the report. */
            //diagnostic(W_LOCATION, ctx->ctx, p_exit_token, NULL, "exit point");
        }
        ss_close(&name_ss);
        return;
    }

    for (int i = 0; i < e->alternatives.size; i++)
    {
        const struct flow3_alternative* p_alternative = e->alternatives.data[i];

        if (p_alternative->imaginary == FLOW3_IMAGINARY_ABSENT)
        {
            continue;
        }

        if (!flow3_alternative_is_zero(p_alternative))
        {
            struct osstream name_ss2 = { 0 };
            flow3_param_member_name_to_string(param_name, p_obj->member_designator, &name_ss2);
            if (diagnostic(W_FLOW_CLEAR_NOT_ZERO_AT_EXIT,
                ctx->ctx,
                NULL,
                marker,
                "_Clear parameter '%s' is not zero at exit (see line %d)",
                name_ss2.c_str ? name_ss2.c_str : param_name,
                p_alternative->line))
            {
                /* child note -- see the W_LOCATION comment above. */
                //diagnostic(W_LOCATION, ctx->ctx, p_exit_token, NULL, "exit point");
            }
            ss_close(&name_ss2);
        }
    }
}

/*
   _Ctor requires EVERY member of the pointee -- not just _Owner ones -- to
   have been given a real value by every exit point: the same "definite
   assignment" obligation C#'s `out` enforces, for every field. A plain
   scalar member left uninitialized is just as much a violation as an
   untouched _Owner member; the caller is trusting that the whole object is
   now well-formed, not only its owned resources. flow3_parameter_object_init
   seeds every member of a _Ctor pointee as UNINITIALIZED at function entry
   (see the _Ctor branch there); this is the check that verifies every one
   of them left that state behind.
*/
static void flow3_check_ctor_object_is_initialized_at_exit(struct flow3_visit_ctx* ctx,
    const struct object* p_obj,
    const char* param_name,
    const struct marker* marker,
    const struct token* p_exit_token,
    bool top_level)
{
    if (p_obj->members.head)
    {
        for (const struct object* _Opt member = p_obj->members.head; member; member = member->next)
        {
            flow3_check_ctor_object_is_initialized_at_exit(ctx, member, param_name, marker, p_exit_token, false);
        }
        return;
    }

    if (top_level)
    {
        /* A `_Ctor T* p` pointee that is ITSELF a scalar/pointer (not a
           struct with members) is reached only through a bare `*p = ...`
           assignment, never through `->` member access. That specific
           write shape is not yet reliably tracked back onto this exact
           arena object (unlike a struct member write, `p->m = ...`, which
           is -- see the `.text` case in samples/flow3/ownership.c, checked
           below). Rather than emit an unreliable diagnostic here, skip the
           top-level object and only check members reached via `->`. See
           samples/flow3/owner-resource-130.c (`_Ctor _Owner*` pointee)
           for the shape that motivated this carve-out. */
        return;
    }

    const struct flow3_key_alternatives* _Opt e = flow3_map_search_up(ctx->p_current_flow3_map, p_obj);
    if (e == NULL || e->alternatives.size == 0)
    {
        /* No entry at all -- never written, still carries the UNINITIALIZED
           seed from function entry. */
        struct osstream name_ss = { 0 };
        flow3_param_member_name_to_string(param_name, p_obj->member_designator, &name_ss);
        if (diagnostic(W_FLOW_CTOR_NOT_INITIALIZED_AT_EXIT,
            ctx->ctx,
            NULL,
            marker,
            "_Ctor parameter '%s' is never initialized",
            name_ss.c_str ? name_ss.c_str : param_name))
        {
            //diagnostic(W_LOCATION, ctx->ctx, p_exit_token, NULL, "exit point");
        }
        ss_close(&name_ss);
        return;
    }

    for (int i = 0; i < e->alternatives.size; i++)
    {
        const struct flow3_alternative* p_alternative = e->alternatives.data[i];

        if (p_alternative->imaginary == FLOW3_IMAGINARY_ABSENT)
        {
            continue;
        }

        if (p_alternative->value_relation == FLOW3_RELATION_UNINITIALIZED)
        {
            struct osstream name_ss2 = { 0 };
            flow3_param_member_name_to_string(param_name, p_obj->member_designator, &name_ss2);
            if (diagnostic(W_FLOW_CTOR_NOT_INITIALIZED_AT_EXIT,
                ctx->ctx,
                NULL,
                marker,
                "_Ctor parameter '%s' is possibly not initialized at exit (see line %d)",
                name_ss2.c_str ? name_ss2.c_str : param_name,
                p_alternative->line))
            {
                //diagnostic(W_LOCATION, ctx->ctx, p_exit_token, NULL, "exit point");
            }
            ss_close(&name_ss2);
        }
    }
}

/*
   A plain pointer parameter (not _Dtor, not _Ctor) is a BORROW: the callee
   may read and write through it, but must hand back an object in the same
   basic shape it received -- every _Owner member the callee touches must
   still be a real, live value at every exit, exactly as if it were a local
   owner going out of scope. Only a _Dtor parameter is allowed to consume
   (move/free) the pointee's owner members; that side is already enforced by
   flow3_check_object_at_exit (called for _Dtor, immediately below).

   Without this check, code like:

       void set(struct person* p, char* name) {
           free(p->name);
           char* _Opt _Owner temp = strdup(name);
           if (temp == NULL) return;    // p->name freed but never restored!
           p->name = temp;
       }

   silently frees p->name and then, on the early-return path, leaves the
   caller's struct holding a dangling pointer -- no diagnostic anywhere,
   because p is neither _Ctor (whose contract is "must be initialized") nor
   _Dtor (whose contract is "must be fully released"). This is the mirror
   image of flow3_check_ctor_object_is_initialized_at_exit: instead of
   checking that every member ended up ASSIGNED, it checks that no _Owner
   member ended up MOVED or ENDED (consumed) without being reassigned before
   this exit point.

   Non-owner members are not tracked here (nothing to consume). A _View
   ancestor turns its subtree into non-owning references, same as
   flow3_check_object_at_exit's `in_view` handling, so a _View member's own
   moves are never this parameter's obligation.
*/
static void flow3_check_non_dtor_param_owner_not_consumed_at_exit(struct flow3_visit_ctx* ctx,
    const struct type* p_type,
    const struct object* p_obj,
    const char* param_name,
    const struct marker* marker,
    const struct token* p_exit_token,
    bool in_view,
    bool top_level)
{
    const bool view = in_view || type_is_view(p_type);

    if (p_obj->members.head)
    {
        for (const struct object* _Opt member = p_obj->members.head; member; member = member->next)
        {
            flow3_check_non_dtor_param_owner_not_consumed_at_exit(ctx,
                &member->type,
                member,
                param_name,
                marker,
                p_exit_token,
                view,
                false);
        }
        return;
    }

    if (top_level)
    {
        /* Same carve-out as flow3_check_ctor_object_is_initialized_at_exit:
           a scalar/pointer pointee reached only through a bare `*p = ...`
           write is not yet reliably tracked back onto this exact arena
           object. Only members reached via `->` are checked. */
        return;
    }

    if (!type_is_owner(&p_obj->type) || view)
    {
        return;
    }

    const struct flow3_key_alternatives* _Opt e = flow3_map_search_up(ctx->p_current_flow3_map, p_obj);
    if (e == NULL)
    {
        return;
    }

    bool consumed_reported = false;

    for (int i = 0; i < e->alternatives.size; i++)
    {
        const struct flow3_alternative* p_alternative = e->alternatives.data[i];

        if (p_alternative->imaginary == FLOW3_IMAGINARY_ABSENT)
        {
            continue;
        }

        if (!consumed_reported &&
            (p_alternative->imaginary == FLOW3_IMAGINARY_MOVED ||
            p_alternative->imaginary == FLOW3_IMAGINARY_ENDED))
        {
            consumed_reported = true;
            struct osstream name_ss = { 0 };
            flow3_param_member_name_to_string(param_name, p_obj->member_designator, &name_ss);
            if (diagnostic(W_FLOW_PARAM_OWNER_CONSUMED_AT_EXIT,
                ctx->ctx,
                NULL,
                marker,
                "parameter '%s' was moved/released here (see line %d) but never reassigned -- only a _Dtor parameter may leave the caller's object consumed",
                name_ss.c_str ? name_ss.c_str : param_name,
                p_alternative->line))
            {
                // diagnostic(W_LOCATION, ctx->ctx, p_exit_token, NULL, "exit point");
            }
            ss_close(&name_ss);
        }
    }
}

/*
   Exit-point contract check for the write-qualified pointer parameters,
   _Clear, _Dtor and _Ctor. All three are callee-side obligations verified
   against the same thing -- the synthetic pointee flow3_parameter_object_init
   wired to the parameter at function entry -- so they share one pass over the
   parameter list (this runs at every return and at fall-off-the-end, so
   walking the list twice was pure waste).

   _Clear: every member of the pointee must be exactly 0 at exit.

   _Ctor: every member of the pointee must have been assigned a value at
   exit (definite assignment, same obligation as a C# `out` parameter) --
   see flow3_check_ctor_object_is_initialized_at_exit.

   _Dtor: the other half of a contract whose caller side already exists.
   flow3_check_object_init_assigment ends the lifetime of x's owner members
   (FLOW3_IMAGINARY_ENDED) when &x is passed to a _Dtor parameter, so the
   caller is no longer asked to release them -- see
   samples/flow3/dtor-owner-struct-member-not-moved.c. That is only sound if
   the callee actually does the releasing, which is what we check here: every
   _Owner member reachable through the pointee must have been moved/released,
   exactly like a local owner object at scope exit. Without it,
   `void destroy_x(_Dtor struct X* p) { }` was silently accepted while
   simultaneously excusing every caller -- a leak with no diagnostic anywhere
   (samples/flow3/dtor_is_opt.c).
*/
static void flow3_check_write_qualified_params_at_exit(struct flow3_visit_ctx* ctx, const struct marker* marker, const struct token* p_exit_token)
{
    if (ctx->p_current_function_declaration == NULL ||
        ctx->p_current_function_declaration->init_declarator_list.head == NULL)
    {
        return;
    }

    struct declarator* p_func_declarator = ctx->p_current_function_declaration->init_declarator_list.head->p_declarator;
    struct function_declarator* _Opt p_fd = declarator_find_function_declarator(p_func_declarator);

    if (p_fd == NULL ||
        p_fd->parameter_type_list_opt == NULL ||
        p_fd->parameter_type_list_opt->parameter_list == NULL)
    {
        return;
    }

    for (struct parameter_declaration* _Opt p_param = p_fd->parameter_type_list_opt->parameter_list->head;
        p_param;
        p_param = p_param->next)
    {
        if (p_param->declarator == NULL)
        {
            continue;
        }

        const struct type* p_param_type = &p_param->declarator->type;

        if (!type_is_pointer(p_param_type))
        {
            continue;
        }

        const bool is_clear = type_is_pointed_clear(p_param_type);
        const bool is_dtor = type_is_pointed_dtor(p_param_type);
        const bool is_ctor = type_is_pointed_ctor(p_param_type);

        /* A plain pointer -- none of _Clear/_Dtor/_Ctor -- is a BORROW: see
           flow3_check_non_dtor_param_owner_not_consumed_at_exit just above
           for why it still needs an exit-point check (its _Owner members
           must not be left consumed). So it is NOT skipped here anymore;
           only the object lookup below can still legitimately find nothing
           to check.

           Exception: if the POINTER ITSELF is _Owner (`struct X* _Owner p`,
           as opposed to `_Dtor`/`_Ctor`/`_Clear` qualifying the pointee),
           the parameter transfers ownership of the whole pointee into this
           function -- consuming/erasing its members (e.g. via `free(p)`,
           `void* v = p;`, or `return p;`) is the expected, correct way to
           finish releasing it, and is already checked by the existing
           "owner not moved" mechanism at the point of erasure (see
           void-owner-erasure-leaks-member.c). Treating it as a borrow too
           double-reported the exact same finding under a different id. */
        const bool is_owner_param = type_is_owner(p_param_type);

        /* Find the concrete pointee: flow3_parameter_object_init wired
           this parameter's own alternative to an arena object at
           function entry (see the "non-optional pointer" case there). */
        const struct flow3_key_alternatives* _Opt e =
        flow3_map_search_up(ctx->p_current_flow3_map, &p_param->declarator->object);

        if (e == NULL)
        {
            continue;
        }

        const char* param_name = p_param->declarator->name_opt ? p_param->declarator->name_opt->lexeme : "";

        for (int i = 0; i < e->alternatives.size; i++)
        {
            const struct flow3_alternative* p_alternative = e->alternatives.data[i];

            if (p_alternative->value_kind != FLOW3_VALUE_KIND_PTR || p_alternative->value.p == NULL)
            {
                continue;
            }

            if (is_clear)
            {
                flow3_check_clear_object_is_zero_at_exit(ctx, p_alternative->value.p, param_name, marker, p_exit_token);
            }

            if (is_dtor)
            {
                flow3_check_object_at_exit(ctx,
                    &p_alternative->value.p->type,
                    p_alternative->value.p,
                    marker,
                    p_exit_token,
                    false,
                    param_name);
            }

            if (is_ctor)
            {
                flow3_check_ctor_object_is_initialized_at_exit(ctx, p_alternative->value.p, param_name, marker, p_exit_token, true);
            }

            if (!is_clear && !is_dtor && !is_ctor && !is_owner_param)
            {
                flow3_check_non_dtor_param_owner_not_consumed_at_exit(ctx,
                    &p_alternative->value.p->type,
                    p_alternative->value.p,
                    param_name,
                    marker,
                    p_exit_token,
                    false,
                    true);
            }
        }
    }
}

static void flow3_check_function_exit(struct flow3_visit_ctx* ctx, struct jump_statement* p_jump_statement)
{
    flow3_exit_block_visit_defer_list(ctx,
        &p_jump_statement->defer_list,
        p_jump_statement->first_token);

    flow3_check_arena_objects_at_function_exit(ctx); //TODO do file scope together

    const struct marker marker = {
        .p_token_begin = p_jump_statement->first_token,
        .p_token_end = p_jump_statement->first_token
    };
    flow3_check_file_scope_objects_at_function_exit(ctx, &marker);

    /*
       Must run BEFORE flow3_defer_list_set_end_of_lifetime, not after.

       p_jump_statement->defer_list -- built by walking up the scope chain
       from this return/throw to the function's start -- turns out to
       include the function's own PARAMETERS (p, name, ...), not just true
       block-scope locals. flow3_defer_list_set_end_of_lifetime then calls
       flow3_map_set_object_lifetime_ended on EVERY item in that list,
       including the parameters' own bindings -- which poisons the
       parameter's tracked alternative (value_kind stops being PTR, value.p
       becomes a sentinel) before flow3_check_write_qualified_params_at_exit
       gets a chance to look up the parameter's pointee. The is_clear/
       is_dtor/is_ctor checks below, and the plain-pointer "owner not
       consumed" check, all key off exactly that lookup
       (flow3_map_search_up(ctx->p_current_flow3_map,
       &p_param->declarator->object)) -- so with the old ordering, ALL FOUR
       of them silently found nothing to check on this exact exit path (an
       early return from inside a nested block), while the same checks
       worked fine at the fall-off-the-end exit (whose defer list is built
       differently and doesn't poison parameters the same way). Found via a
       user-reported false negative: `free(p->name); ...; if (x) return;`
       leaving p->name dangling on the early-return path went completely
       unreported. Running this check first means it always sees the real,
       live parameter state, regardless of what the defer list ends up
       lifetime-ending afterward.
    */
    flow3_check_write_qualified_params_at_exit(ctx, &marker, p_jump_statement->first_token);

    flow3_defer_list_set_end_of_lifetime(ctx,
        &p_jump_statement->defer_list,
        p_jump_statement->first_token);
}

static void flow3_visit_jump_statement(struct flow3_visit_ctx* ctx, struct jump_statement* p_jump_statement)
{
    if (ctx->p_current_flow3_map == NULL)
        return;

    /* Only meant to bridge a report from THIS statement's own expression
       visit into a check running right after it (see the field comment in
       flow3.h) -- must not leak into an unrelated later statement. */
    ctx->p_pending_ended_report_obj = NULL;

    try
    {
        if (p_jump_statement->first_token->type == TK_KEYWORD_CAKE_THROW)
        {

            if (ctx->p_throw_join_map != NULL)
            {
                flow3_map_accumulate_into_join(ctx->p_throw_join_map,
                    ctx->p_current_flow3_map);
            }

            flow3_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list,
                p_jump_statement->first_token);
            flow3_defer_list_set_end_of_lifetime(ctx, &p_jump_statement->defer_list,
                p_jump_statement->first_token);

            /*
               A throw unconditionally transfers control away from this
               point. Mark the current map dead so that any code that
               syntactically follows in the same block (dead code -- e.g.
               after an unconditional throw) does not leak its effects
               into whichever merge later combines this arm: merge_arms
               (used by if/try/while/do/for) already skips is_unreachable arms
               regardless of the syntax-based "ends with jump" check the
               caller used to select which arms to pass in.
            */
            if (ctx->p_current_flow3_map != NULL)
            {
                ctx->p_current_flow3_map->is_unreachable = true;
            }
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
                flow3_check_object_init_assigment(ctx, p_jump_statement->expression_opt, &param_object, &p_jump_statement->expression_opt->object, INIT_RETURN, false, false);
            }

            /*
             * On every explicit return: run deferred cleanup, verify
             * _Ctor/_Dtor/_Owner parameter exit conditions, and check
             * arena (synthetic pointed-to) objects.
             */
            flow3_check_function_exit(ctx, p_jump_statement);

            /* return unconditionally transfers control (see the "throw"
               case above for why the current map is marked dead). */
            if (ctx->p_current_flow3_map != NULL)
            {
                ctx->p_current_flow3_map->is_unreachable = true;
            }
        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_CONTINUE)
        {
            flow3_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
            flow3_defer_list_set_end_of_lifetime(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);

            /* continue unconditionally transfers control (see "throw"
               case above). */
            if (ctx->p_current_flow3_map != NULL)
            {
                ctx->p_current_flow3_map->is_unreachable = true;
            }
        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_BREAK)
        {
            if (ctx->p_break_join_map != NULL)
            {
                flow3_map_accumulate_into_join(ctx->p_break_join_map, ctx->p_current_flow3_map);
            }

            flow3_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
            flow3_defer_list_set_end_of_lifetime(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);

            /* break unconditionally transfers control (see "throw" case
               above). */
            if (ctx->p_current_flow3_map != NULL)
            {
                ctx->p_current_flow3_map->is_unreachable = true;
            }
        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_GOTO)
        {
            _Assert(p_jump_statement->label != NULL);

            bool found = false;
            for (int i = 0; i < ctx->labels_size; i++)
            {
                if (strcmp(ctx->labels[i].label_name, p_jump_statement->label->lexeme) == 0)
                {
                    flow3_map_accumulate_into_join(ctx->labels[i].p_flow3_map,
                        ctx->p_current_flow3_map);
                    found = true;
                    break;
                }
            }
            if (!found &&
                ctx->labels_size < (int)(sizeof(ctx->labels) / sizeof(ctx->labels[0])))
            {
                struct flow3_map* _Opt p_label_map =
                flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, FLOW3_MAP_GOTO_LABEL);
                if (p_label_map != NULL)
                {
                    /*
                       Eagerly snapshot the state at this goto into the
                       label's own map entries, exactly like subsequent
                       jumps to this same label do via
                       flow3_map_accumulate_into_join above. Without this,
                       the label map starts out empty and relies on its
                       parent chain (ctx->p_current_flow3_map) for lookups
                       -- but that parent is a live map that later code
                       (merges, assignments) keeps mutating in place, so by
                       the time the label is reached the "inherited" state
                       no longer reflects what existed at this goto.
                    */
                    flow3_map_accumulate_into_join(p_label_map, ctx->p_current_flow3_map);

                    ctx->labels[ctx->labels_size].label_name = p_jump_statement->label->lexeme;
                    ctx->labels[ctx->labels_size].p_flow3_map = p_label_map;
                    ctx->labels_size++;
                }
            }

            flow3_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
            flow3_defer_list_set_end_of_lifetime(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);

            /* goto unconditionally transfers control (see "throw" case
               above). */
            if (ctx->p_current_flow3_map != NULL)
            {
                ctx->p_current_flow3_map->is_unreachable = true;
            }
        }
        else
        {
            _Assert(false);
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
        _Assert(false);
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
                /*
                   A label can be reached two ways: by a goto (already
                   folded into ctx->labels[i].p_flow3_map above) or by
                   simply falling through from the previous statement
                   (ctx->p_current_flow3_map, right now). Union the
                   fall-through arrival into the label's accumulated
                   state instead of discarding it by overwriting
                   ctx->p_current_flow3_map outright -- otherwise
                   whichever gotos arrived first would be silently lost
                   the moment control also reaches the label normally.
                */
                flow3_map_accumulate_into_join(ctx->labels[i].p_flow3_map, ctx->p_current_flow3_map);
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
            /* default: label — no refinement, but still must be a FRESH
               child map, not literally ctx->p_initial_map (== the
               pre-switch map) itself. Reusing that ancestor directly
               here mirrors the same aliasing bug fixed for `&&` in
               flow3_ensure_branch_pair: if this case body ends with an
               unconditional jump (e.g. `default: break;`), the jump
               handling marks "the current map" dead -- which would then
               be the pre-switch map itself, corrupting an ancestor
               shared with the code that runs after the whole switch.
               Concretely: `switch (e) { ...; default: break; }
               return 0;` incorrectly warned "unreachable code" on
               `return 0;`, because the OTHER case(s) reaching `break;`
               first already marked their own (correctly fresh, per-case)
               maps dead with no issue, but the default case's break
               marked the shared ancestor dead instead. */
            ctx->p_current_flow3_map =
            flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_initial_map, FLOW3_MAP_DEFAULT);
            return;
        }

        /* case N: — create a child map that records the switch variable == N */
        struct flow3_map* p_case_map =
        flow3_map_arena_new(&ctx->flow3_map_arena,
            ctx->p_initial_map,
            FLOW3_MAP_CASE);

        /* ctx->p_switch_object_key is the object being switched on,
           set when entering the switch statement */
        if (ctx->p_switch_obj_key != NULL &&
            object_has_constant_value(&p_label->constant_expression->object))
        {
            signed long long case_value =
            object_to_signed_long_long(&p_label->constant_expression->object);

            {
                struct flow3_key_alternatives* _Opt e = flow3_map_find_add(p_case_map, ctx->p_switch_obj_key);
                if (e == NULL)
                    return; /* no entry to seed for this case */

                flow3_alternatives_clear(&e->alternatives);
                struct flow3_alternative a = {
                    .value_kind = FLOW3_VALUE_KIND_SIGNED,
                    .value = {.i = case_value},
                    .value_relation = FLOW3_RELATION_EQUAL,
                    .imaginary = FLOW3_IMAGINARY_NONE,
                    .origin = p_case_map,
                    .line = p_label->constant_expression->first_token->line
                };
                flow3_alternatives_add(&e->alternatives, &a);
            }
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

    /*
       Warn once per run of dead code. ctx->p_current_flow3_map is
       marked is_unreachable by flow3_visit_jump_statement right when an
       unconditional return/break/continue/goto/throw is processed, so
       any block item visited afterward -- until a label makes the map
       live again via a goto arrival -- can never actually execute.
    */
    bool warned_unreachable = false;

    while (p_block_item)
    {
        if (!warned_unreachable &&
            p_block_item->label == NULL &&
            ctx->p_current_flow3_map != NULL &&
            ctx->p_current_flow3_map->is_unreachable)
        {
            const struct marker m = {
                .p_token_begin = p_block_item->first_token,
                .p_token_end = p_block_item->first_token
            };
            diagnostic(W_FLOW_UNREACHABLE_CODE, ctx->ctx, NULL, &m, "unreachable code");
            warned_unreachable = true;
        }

        flow3_visit_block_item(ctx, p_block_item);

        if (p_block_item->label != NULL &&
            ctx->p_current_flow3_map != NULL &&
            !ctx->p_current_flow3_map->is_unreachable)
        {
            /* label arrival (goto and/or fall-through) made the map
               live again; a later jump can start a new dead-code run */
            warned_unreachable = false;
        }

        p_block_item = p_block_item->next;
    }
}

static void flow3_visit_pragma_declaration(struct flow3_visit_ctx* ctx, struct pragma_declaration* p_pragma_declaration)
{
    execute_pragma_declaration(ctx->ctx, p_pragma_declaration, true);
}

static void object_static_debug(struct flow3_visit_ctx* ctx, const struct object* p_object, struct token* first_token, struct token* last_token)
{
    const struct object* _Opt member = p_object->members.head;
    if (member)
    {
        while (member)
        {
            object_static_debug(ctx, member, first_token, last_token);
            member = member->next;
        }
        return;
    }

    struct flow3_key_alternatives* _Opt p_entry = flow3_map_search_up(ctx->p_current_flow3_map, p_object);
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
        struct flow3_alternative* p_flow3_alternative = p_entry->alternatives.data[i];

        if (p_flow3_alternative->imaginary == FLOW3_IMAGINARY_ABSENT)
        {
            continue;
        }
        if (p_flow3_alternative->value_kind == FLOW3_VALUE_KIND_REF)
        {
            /* A REF alternative is supposed to point at another tracked
               object, but that pointer can be NULL (e.g. an assignment
               through a pointer-to-pointer, `*pp = get();`, can leave a
               REF alternative whose target was never resolved) --
               recursing on NULL crashed here. Skip it instead: there is
               nothing further to print for a REF with no target. */
            if (p_flow3_alternative->value.p != NULL)
            {
                object_static_debug(ctx, p_flow3_alternative->value.p, first_token, last_token);
            }
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
    if (alt->value_relation == FLOW3_RELATION_UNINITIALIZED)
    {
        ss_fprintf(ss, "value is uninitialized");
    }
    else if (alt->value_relation == FLOW3_RELATION_ANY)
    {
        ss_fprintf(ss, "value could be any value, including zero");
    }
    else if (alt->value_relation == FLOW3_RELATION_EQUAL && flow3_value_is_false(alt))
    {
        ss_fprintf(ss, "value equals zero");
    }
    else if (alt->value_relation == FLOW3_RELATION_NOT_EQUAL)
    {
        ss_fprintf(ss, "value is only known to differ from a nonzero value, so it could still be zero");
    }
    else
    {
        ss_fprintf(ss, "value cannot be proven nonzero");
    }

    switch (alt->imaginary)
    {
    case FLOW3_IMAGINARY_MOVED: ss_fprintf(ss, " (object was moved)"); break;
    case FLOW3_IMAGINARY_ENDED: ss_fprintf(ss, " (object's lifetime has ended)"); break;
    case FLOW3_IMAGINARY_ABSENT: ss_fprintf(ss, " (object is absent)"); break;
    default: break;
    }

    ss_fprintf(ss, ", set at line %d", alt->line);

    if (alt->origin)
    {
        /* The full decision path, not just the name of the one map that
           recorded the fact: on a warning the reader believes is
           impossible, "which conditions were assumed along the way" is the
           question they actually need answered. See
           flow3_explain_origin. */
        struct osstream path_ss = flow3_explain_origin(alt->origin);
        ss_fprintf(ss, " in \"%s\"", path_ss.c_str ? path_ss.c_str : "");
        ss_close(&path_ss);
    }
}

static void check_object_true(struct flow3_visit_ctx* ctx, const struct object* p_object, const struct token* p_position_token)
{
    struct flow3_key_alternatives* _Opt p_entry = flow3_map_search_up(ctx->p_current_flow3_map, p_object);
    if (p_entry == NULL)
        return;

    for (int i = 0; i < p_entry->alternatives.size; i++)
    {
        const struct flow3_alternative* alt = p_entry->alternatives.data[i];
        if (alt->value_kind == FLOW3_VALUE_KIND_REF)
        {
            check_object_true(ctx, alt->value.p, p_position_token);
            continue;
        }

        if (!flow3_alternative_is_true(alt))
        {
            struct osstream ss = { 0 };
            flow3_explain_alternative_not_true(&ss, alt);
            /*
               A warning (67), not the 1080 error flow1 uses. Two reasons:
               1080 is >= BITSET_SIZE, so it is an error id -- not
               configurable, and get_diagnostic_phase returned 0 for it, which
               meant `//lint 1080` was checked while parsing, before flow
               analysis had queued anything: the suppression failed and
               reported "diagnostic '1080' not recognized" while the error
               fired anyway. W_COMPILE_ASSERT_UNPROVEM is a configurable
               warning listed as phase 2, so `//lint 67` works and a
               deliberately-unprovable compile_assert can be annotated.
               See samples/flow3/logical-or-per-path.c.
            */
            diagnostic(W_COMPILE_ASSERT_UNPROVEM, ctx->ctx,
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
    if (p_static_assertion->first_token->type == TK_KEYWORD_RUNTIME_ASSERT)
    {
        /* _Assert(cond) narrows like assert: after it, only the branch
           where cond is true is reachable (a failed _Assert aborts).
           Visit the condition EVALUATED so it narrows, then keep only the true
           arm. Unlike the `assert` macro this is a keyword, so it is never
           stripped and narrows in every build, including release.
           This mirrors exactly what `if (!(cond)) return;` does in
           flow3_visit_if_statement's no-else, "only the false arm reaches
           the end" case: merge_a_b(p_before, X, X) then continue in
           p_before -- so it relies on flow3_map_merge_arms's cleanup loop
           not over-freeing shared ancestor maps (see the ancestor-check
           there) rather than needing any special-casing here. */
        if (ctx->p_current_flow3_map == NULL)
            return;

        struct flow3_map* p_before = ctx->p_current_flow3_map;
        struct flow3_branch_pair pair = flow3_visit_full_expression(ctx, p_static_assertion->constant_expression);
        /* Same as flow3_visit_if_statement: force pair.p_true into its own
           fresh child map when it aliases p_before (or any intermediate
           arm of the condition aliases the map it was branched from) --
           otherwise flow3_map_merge_arms's cleanup loop, walking from an
           arm that IS an ancestor of p_before back up looking for
           p_before, never finds it (p_before is below, not above) and
           keeps climbing past it, wiping real ancestor state other
           already-evaluated operands still depend on. */
        pair = flow3_ensure_branch_pair(ctx, p_before, pair, p_static_assertion->constant_expression);
        flow3_map_merge_a_b(p_before, pair.p_true, pair.p_true);
        ctx->p_current_flow3_map = p_before;
        return;
    }

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

    /* compile_assert's diagnostic is queued above, during flow analysis, so
       its `//lint N` has to be checked here at phase 2. parser.c checks the
       same token at phase 0, which is too early to see it. */
    if (p_static_assertion->p_lint_token)
    {
        flow3_check_dianostic_suppression(ctx, p_static_assertion->p_lint_token);
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
            if (parameter->declarator)
            {
                flow3_visit_declaration_specifiers(ctx, parameter->declaration_specifiers, &parameter->declarator->type);
                flow3_visit_declarator(ctx, parameter->declarator);
                flow3_parameter_object_init(ctx, &parameter->declarator->object, &parameter->declarator->type, parameter->declaration_specifiers->first_token->line);
            }
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
    const struct token* p_exit_token,
    bool in_view,
    const char* _Opt p_root_name_opt)
{
    /* A `_View` object views its members as non-owner: `_View struct X x`
       treats every _Owner member of x as a plain view, so those members are not
       owned here and must not be reported "owner not moved" at scope exit
       (expressions.c:2706, `_View struct initializer initializer`). Once any
       ancestor is _View the whole subtree is a view. */
    const bool view = in_view || type_is_view(p_type);

    if (p_obj->members.head)
    {
        for (const struct object* _Opt member = p_obj->members.head; member; member = member->next)
        {
            flow3_check_object_at_exit(ctx,
                &member->type,
                member,
                marker,
                p_exit_token,
                view,
                p_root_name_opt);
        }
        return;
    }

    const bool is_owner = type_is_owner(&p_obj->type) && !view;

    const struct flow3_key_alternatives* _Opt e = flow3_map_search_up(ctx->p_current_flow3_map, p_obj);
    if (e == NULL) return;

    /* One report per object at this exit, not one per alternative: a leaked
       owner is a property of the object, and repeating it once per accumulated
       alternative only pads the output -- parser.c:6295 reported each of six
       owner members ten times over. */
    bool not_moved_reported = false;

    for (int i = 0; i < e->alternatives.size; i++)
    {
        const struct flow3_alternative* p_alternative = e->alternatives.data[i];

        if (p_alternative->value_kind == FLOW3_VALUE_KIND_REF)
        {
            //IS THIS POSSIBLE?
            if (p_alternative->value.p != p_obj)
            {
                flow3_check_object_at_exit(ctx,
                    p_type,
                    p_alternative->value.p,
                    marker,
                    p_exit_token,
                    view,
                    p_root_name_opt);
                continue;
            }
        }

        if (p_alternative->imaginary == FLOW3_IMAGINARY_ABSENT)
            continue;

        if (is_owner &&
            !not_moved_reported &&
            p_alternative->imaginary != FLOW3_IMAGINARY_MOVED &&
            p_alternative->imaginary != FLOW3_IMAGINARY_ENDED)
        {
            /* FLOW3_IMAGINARY_ENDED is how a _Dtor call resolves an owner
               (flow3_map_set_object_lifetime_ended, called from
               flow3_check_object_init_assigment for type_is_pointed_dtor
               parameters) -- just as legitimate a resolution as MOVED.
               Before this fix, passing an _Owner struct member to a
               _Dtor function (e.g. `hash_item_set_destroy(&item);`)
               correctly ended its lifetime (confirmed via static_debug:
               the alternative really was ANY/ENDED at this point) but
               this check still reported "owner object not moved",
               because it only ever recognized MOVED. Found via a
               user-reported false positive on real code. */
            if (flow3_alternative_is_zero(p_alternative) ||
                p_alternative->value_relation == FLOW3_RELATION_UNINITIALIZED)
            {
                /* An owner that was never given a value owns nothing --
                   there is no resource behind it to leak. UNINITIALIZED is
                   not "holds something and it wasn't released", it is "was
                   never told to hold anything at all"; only a value that was
                   actually assigned (a real allocation, or at minimum ANY
                   from an untracked source) represents a possible leak. This
                   matters once a struct's members are checked through a
                   void* erasure (see void-owner-erasure-leaks-member.c):
                   `struct X* x = malloc(sizeof *x); free(x);` with x's owner
                   members never touched is not a leak -- nothing was ever
                   allocated into them -- and must not be reported as one
                   (owner-resource-074.c). */
            }
            else
            {
                not_moved_reported = true;
                const char* member_suffix = p_obj->member_designator ? p_obj->member_designator : "";
                /* member_designator is attached to the OBJECT (e.g. the
                   pointee reached through some owner pointer), not to the
                   EXPRESSION used to reach it here, so on its own it reads
                   as ".integer_type.name_opt" with no leading "p" at all --
                   confusing, since the object could in principle be reached
                   through more than one pointer. p_root_name_opt is the
                   name of the declarator flow3_check_object_at_exit was
                   originally called on (e.g. "p"), threaded unchanged
                   through every recursive call below; combine it with
                   member_suffix so the message reads as "p->integer_type.
                   name_opt", matching what the user would actually type to
                   fix it. member_suffix's own leading "." (see
                   object.c's "%s.%s" pattern used to build it once the
                   first hop crosses a pointer) becomes "->" for that first
                   hop; falls back to the bare suffix when no root name is
                   available (e.g. call sites checking an arbitrary
                   expression rather than a simple declarator). */
                char object_name_buf[256];
                const char* object_name;
                if (p_root_name_opt != NULL && p_root_name_opt[0] != 0 &&
                    strcmp(p_root_name_opt, member_suffix) != 0)
                {
                    /* member_suffix with no leading "." is not a nested
                       member at all -- it's the ROOT object's own
                       designator (set directly to its name at creation,
                       see object.c), meaning this report is about the root
                       object itself, not something reached through it. The
                       strcmp above already caught the common case where it
                       equals p_root_name_opt exactly; this covers a root
                       object whose OWN name differs from the caller-supplied
                       display name (e.g. the void-erasure call site above,
                       which renders the full source expression rather than
                       a bare declarator name). Either way, concatenating
                       would just repeat the name twice ("p.p"). */
                    if (member_suffix[0] == '.')
                    {
                        snprintf(object_name_buf, sizeof(object_name_buf), "%s->%s", p_root_name_opt, member_suffix + 1);
                    }
                    else if (member_suffix[0] != 0)
                    {
                        object_name_buf[0] = 0;
                        snprintf(object_name_buf, sizeof(object_name_buf), "%s", member_suffix);
                    }
                    else
                    {
                        snprintf(object_name_buf, sizeof(object_name_buf), "%s", p_root_name_opt);
                    }
                    object_name = object_name_buf;
                }
                else
                {
                    object_name = member_suffix;
                }
                /*
                   W_FLOW_MISSING_DTOR (29), not W_FLOW_LIFETIME_ENDED (31).
                   This is the inverse condition of the rest of 31: those mean
                   "an object whose lifetime already ended was used", this means
                   "an owner whose lifetime never ended was leaked". Sharing one
                   id made `//lint 31` blind a file to both at once. flow1 already
                   reports this shape as 29 ("object referenced by owner '%s' was
                   not released").
                */
                /* Reported AT the exit point (p_exit_token), not at the
                   object's declaration/origin (marker): the leak is a
                   property of reaching this exit with the owner still
                   unmoved, so that's where the diagnostic should point --
                   matching what a user fixing the leak actually needs to
                   look at (where to add the missing free/move), rather than
                   where the object happened to be declared. Previously this
                   reported at `marker` with a separate "exit point" child
                   note pointing at p_exit_token; inverted on user request so
                   there is one diagnostic, at the exit point, with no
                   separate note.

                   Wording: previously "owner object '%s' not moved (%s see
                   line %d)", where %s was origin_name -- the internal
                   branch-map kind (e.g. "call-opt-nonnull"), dropped
                   entirely as implementation detail meaningless to a user.
                   "not moved" is move-semantics jargon; "goes out of scope"
                   (an earlier revision of this message) was also wrong for
                   call sites that aren't a block-scope exit at all (e.g. a
                   _Dtor parameter check at function exit) -- the actual,
                   general fact in every case is that the object's lifetime
                   has ended (matching the phrasing flow3 already uses for
                   W_FLOW_LIFETIME_ENDED) while it still owned its resource.
                   User-reported (twice). */
                diagnostic(W_FLOW_MISSING_DTOR,
                    ctx->ctx,
                    p_exit_token,
                    NULL,
                    "'%s' lifetime ended while still owning its resource (acquired at line %d)",
                    object_name,
                    p_alternative->line);
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
        p_compound_statement->block_item_list.tail->unlabeled_statement->jump_statement->first_token->type == TK_KEYWORD_RETURN)
    {
        return true;
    }
    return false;
}

/*
   _Clear, _Dtor, and _Ctor only make sense as a qualifier on the
   POINTED-TO type of a pointer parameter -- they describe something
   the callee does through the pointer (zero every member, end the
   pointee's lifetime, initialize it), not a property a plain by-value
   parameter could ever have. `_Clear int x`, `_Dtor int x`, `_Ctor int
   x` (qualifying a non-pointer directly) are rejected.

   And on a pointer parameter, every one of these three is a WRITE
   through the pointer, which directly contradicts const's promise not
   to modify what it points to: `const _Clear T* p`, `const _Dtor T*
   p`, `const _Ctor T* p` (in either qualifier order) are also rejected.

   Checked here, once per declared function (declaration or
   definition), against every parameter.
*/
/*
   Placement rule for the state/write qualifiers (_Clear, _Dtor, _Ctor,
   _Uninitialized).

   Each of them describes what happens to the POINTED-TO object, so putting one
   on a non-pointer is meaningless, and the pointee cannot simultaneously be
   const (the callee has to write it).

   One helper for every position -- variable declarators AND parameters. It used
   to be split: the parser checked variables (_Dtor/_Uninitialized/_Clear only)
   and flow3 checked parameters (_Clear/_Dtor/_Ctor only), so `_Ctor int x;` and
   `void f(_Uninitialized int x)` both slipped through. Sharing the helper closes
   both gaps and keeps every _Owner-family diagnostic inside flow3.
*/
static void flow3_check_write_qualifier_placement(struct flow3_visit_ctx* ctx,
    const struct type* _Opt p_type,
    struct token* _Opt p_token)
{
    if (p_type == NULL || p_token == NULL)
        return;

    /* A function declarator's own type is a function type; its qualifiers
       belong to the return type, which is checked in its own right. */
    if (type_is_function(p_type))
        return;

    if (!type_is_pointer(p_type))
    {
        if (type_is_clear(p_type))
        {
            diagnostic(C_ERROR_FLOW_WRITE_QUALIFIER_MUST_QUALIFY_POINTEE, ctx->ctx, p_token, NULL,
                "_Clear must be used only at the pointed object");
        }
        else if (type_is_dtor(p_type))
        {
            diagnostic(C_ERROR_FLOW_WRITE_QUALIFIER_MUST_QUALIFY_POINTEE, ctx->ctx, p_token, NULL,
                "_Dtor must be used only at the pointed object");
        }
        else if (type_is_ctor(p_type))
        {
            diagnostic(C_ERROR_FLOW_WRITE_QUALIFIER_MUST_QUALIFY_POINTEE, ctx->ctx, p_token, NULL,
                "_Ctor must be used only at the pointed object");
        }
        else if (type_is_uninit(p_type))
        {
            diagnostic(C_ERROR_FLOW_WRITE_QUALIFIER_MUST_QUALIFY_POINTEE, ctx->ctx, p_token, NULL,
                "_Uninitialized must be used only at the pointed object");
        }
        return;
    }

    if (!type_is_pointed_const(p_type))
        return;

    if (type_is_pointed_clear(p_type))
    {
        diagnostic(C_ERROR_FLOW_WRITE_QUALIFIER_CANNOT_BE_CONST, ctx->ctx, p_token, NULL,
            "_Clear pointee cannot also be const");
    }
    else if (type_is_pointed_dtor(p_type))
    {
        diagnostic(C_ERROR_FLOW_WRITE_QUALIFIER_CANNOT_BE_CONST, ctx->ctx, p_token, NULL,
            "_Dtor pointee cannot also be const");
    }
    else if (type_is_pointed_ctor(p_type))
    {
        diagnostic(C_ERROR_FLOW_WRITE_QUALIFIER_CANNOT_BE_CONST, ctx->ctx, p_token, NULL,
            "_Ctor pointee cannot also be const");
    }
    else if (type_is_pointed_uninit(p_type))
    {
        diagnostic(C_ERROR_FLOW_WRITE_QUALIFIER_CANNOT_BE_CONST, ctx->ctx, p_token, NULL,
            "_Uninitialized pointee cannot also be const");
    }
}

static void flow3_check_write_qualifier_parameters(struct flow3_visit_ctx* ctx, struct declarator* p_declarator)
{
    const struct param_list* _Opt p_param_list = type_get_func_or_func_ptr_params(&p_declarator->type);
    if (p_param_list == NULL)
    {
        return;
    }

    struct token* _Opt p_token = p_declarator->first_token_opt ? p_declarator->first_token_opt : p_declarator->name_opt;

    for (const struct param* _Opt p_current_parameter_type = p_param_list->head;
        p_current_parameter_type;
        p_current_parameter_type = p_current_parameter_type->next)
    {
        flow3_check_write_qualifier_placement(ctx, &p_current_parameter_type->type, p_token);
    }
}

/* Same rule for a plain variable declarator (moved out of parser.c's
   init_declarator, where it only covered _Dtor/_Uninitialized/_Clear). */
static void flow3_check_write_qualifier_declarator(struct flow3_visit_ctx* ctx, struct declarator* p_declarator)
{
    struct token* _Opt p_token = p_declarator->first_token_opt ? p_declarator->first_token_opt : p_declarator->name_opt;
    flow3_check_write_qualifier_placement(ctx, &p_declarator->type, p_token);
}

void flow3_visit_declaration(struct flow3_visit_ctx* ctx, struct declaration* p_declaration)
{
    try
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
            flow3_check_write_qualifier_parameters(ctx, p_declaration->init_declarator_list.head->p_declarator);

            /* ...and the declarators themselves (moved from parser.c). */
            for (struct init_declarator* _Opt p_id = p_declaration->init_declarator_list.head;
                p_id;
                p_id = p_id->next)
            {
                flow3_check_write_qualifier_declarator(ctx, p_id->p_declarator);
            }
        }

        if (p_declaration->init_declarator_list.head)
        {
            flow3_visit_init_declarator_list(ctx, &p_declaration->init_declarator_list);
        }

        if (p_declaration->function_body)
        {
            _Assert(ctx->p_return_type == NULL);
            if (p_declaration->init_declarator_list.head == NULL) throw;

            struct type type = get_function_return_type(&p_declaration->init_declarator_list.head->p_declarator->type);
            ctx->p_return_type = &type;

            ctx->p_current_function_declaration = p_declaration;

            /* _core: the body's trailing `//lint N` is checked below instead,
           after the fall-off-the-end exit diagnostics have been queued.
           See flow3_visit_compound_statement_core. */
            flow3_visit_compound_statement_core(ctx, p_declaration->function_body);

            if (!flow3_is_last_item_return(p_declaration->function_body))
            {
                flow3_exit_block_visit_defer_list(ctx, &p_declaration->function_body->defer_list, p_declaration->function_body->last_token);
                //flow3_check_params_at_function_exit(ctx, p_declaration);
                flow3_check_arena_objects_at_function_exit(ctx);
                const struct marker marker = {
                    .p_token_begin = p_declaration->function_body->last_token,
                    .p_token_end = p_declaration->function_body->last_token
                };
                flow3_check_file_scope_objects_at_function_exit(ctx, &marker);
                /* Falling off the end of the function is an exit point too --
               a _Clear/_Dtor parameter's contract must hold here just as
               much as at an explicit return
               (ctx->p_current_function_declaration is still set at this
               point, so this must run before it's cleared below).

               Must run BEFORE flow3_defer_list_set_end_of_lifetime -- see
               the identical reordering (and the false-negative it fixes)
               in flow3_check_function_exit, just above. */
                flow3_check_write_qualified_params_at_exit(ctx, &marker, p_declaration->function_body->last_token);
                flow3_defer_list_set_end_of_lifetime(ctx, &p_declaration->function_body->defer_list, p_declaration->function_body->last_token);
            }

            if (p_declaration->function_body->lint_token)
            {
                flow3_check_dianostic_suppression(ctx, p_declaration->function_body->lint_token);
            }

            ctx->p_current_function_declaration = NULL;
            type_destroy(&type);
            ctx->p_return_type = NULL;
        }

        if (p_declaration->lint_token)
        {
            flow3_check_dianostic_suppression(ctx, p_declaration->lint_token);
        }
    }
    catch
    {
    }
}

void flow3_start_visit_declaration(struct flow3_visit_ctx* ctx, struct declaration* p_declaration)
{
    ctx->labels_size = 0;
    flow3_predicate_cache_reset(ctx);
    ctx->collect_deferred_effects = false;
    ctx->deferred_effects_count = 0;

    flow3_allocated_object_arena_clear(&ctx->allocated_object_arena);
    flow3_map_arena_clear(&ctx->flow3_map_arena);
    ctx->p_current_flow3_map = flow3_map_arena_new(&ctx->flow3_map_arena, NULL, FLOW3_MAP_ROOT);
    if (ctx->p_current_flow3_map == NULL)
        return; /* no map to work with */

    #ifdef FLOW3_DEBUG_TIMING
    clock_t _dbg_t0 = clock();
    #endif

    flow3_visit_declaration(ctx, p_declaration);

    #ifdef FLOW3_DEBUG_TIMING
    clock_t _dbg_t1 = clock();
    double _dbg_ms = (double)(_dbg_t1 - _dbg_t0) * 1000.0 / 1000000.0;
    if (_dbg_ms > 5.0)
    {
        struct token* _dbg_tok = p_declaration->first_token;
        fprintf(stderr, "[DEBUG decl %.1fms line=%d]\n",
            _dbg_ms,
            _dbg_tok ? _dbg_tok->line : -1);
    }
    #endif

    flow3_allocated_object_arena_clear(&ctx->allocated_object_arena);
    flow3_map_arena_clear(&ctx->flow3_map_arena);
    ctx->p_current_flow3_map = NULL;
}

void flow3_visit_ctx_destroy(_Dtor struct flow3_visit_ctx* ctx)
{
    flow3_allocated_object_arena_clear(&ctx->allocated_object_arena);
    flow3_map_arena_clear(&ctx->flow3_map_arena);
    flow3_alt_pool_free_all(&g_flow3_alt_pool);
}
