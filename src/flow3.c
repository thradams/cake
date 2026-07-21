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
    FLOW3_NUM_OF_BUCKETS = 64,
};

enum flow3_relation
{
    FLOW3_RELATION_UNINITIALIZED = 0, // value == trash
    FLOW3_RELATION_EQUAL,             // == value
    FLOW3_RELATION_NOT_EQUAL,         // != value
    FLOW3_RELATION_ANY,               // value >= TYPE_MIN and value <= TYPE_MAX
    /* Relational (half-line) constraints against a constant. The variable
       is known to satisfy  (variable  OP  value)  for the OP below. These
       let a comparison such as `a > 0` narrow `a` in its true branch, so a
       later assert/compile_assert can prove the same fact. Consumers that
       don't understand these relations must treat them conservatively as
       "unknown" (never as a proof), which keeps the analysis sound. */
    FLOW3_RELATION_GREATER,           // >  value
    FLOW3_RELATION_GREATER_EQUAL,     // >= value
    FLOW3_RELATION_LESS,              // <  value
    FLOW3_RELATION_LESS_EQUAL,        // <= value
};

enum flow3_value_kind
{
    FLOW3_VALUE_KIND_SIGNED = 0, /* signed long long .i  (default, zero-init safe) */
    FLOW3_VALUE_KIND_UNSIGNED,    /* unsigned long long .u                           */
    FLOW3_VALUE_KIND_PTR,     /* struct object*   .p  (concrete arena pointer)  */
    FLOW3_VALUE_KIND_REF,     /* struct object*   .p  (reference — always non-null) */
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
    union
    {
        unsigned long long u;
        signed long long   i;
        const struct object* p;
    } value;
    enum flow3_value_kind value_kind;
    enum flow3_relation value_relation;
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
    bool is_dead;     /* branch proven unreachable by constant folding */

    /* Branch identity for join correlation. A true/false map pair created for
       one condition share a stable branch_id (>0) with opposite branch_side.
       A value's origin chain thus records the branch decisions taken to reach
       it; two values conflict iff some branch_id appears on both chains with
       opposite sides. 0 = not a branch map. Survives joins because ids don't
       depend on map lifetime. */
    int  branch_id;
    bool branch_side;
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
    enum sizeof_result r = type_get_sizeof(target_type, &width, ctx->ctx->options.target);
    if (r != SIZEOF_RESULT_OK)
        return 0;

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

static inline bool flow3_value_is_same(const struct flow3_alternative* a, const struct flow3_alternative* b)
{
    if (a->value_kind != b->value_kind)
    {
        return false;
    }
    switch (a->value_kind)
    {
    case FLOW3_VALUE_KIND_SIGNED:  return a->value.i == b->value.i;
    case FLOW3_VALUE_KIND_UNSIGNED: return a->value.u == b->value.u;
    case FLOW3_VALUE_KIND_PTR:  return a->value.p == b->value.p;
    case FLOW3_VALUE_KIND_REF:  return a->value.p == b->value.p;
    }
    return false;
}

static bool flow3_value_is_true(const struct flow3_alternative* a)
{
    switch (a->value_kind)
    {
    case FLOW3_VALUE_KIND_SIGNED:  return a->value.i != 0;
    case FLOW3_VALUE_KIND_UNSIGNED: return a->value.u != 0;
    case FLOW3_VALUE_KIND_PTR:  return a->value.p != 0;
    case FLOW3_VALUE_KIND_REF:
        //assert(false);
        break;
    }
    return false;
}

static bool flow3_value_is_false(const struct flow3_alternative* a)
{
    switch (a->value_kind)
    {
    case FLOW3_VALUE_KIND_SIGNED:  return a->value.i == 0;
    case FLOW3_VALUE_KIND_UNSIGNED: return a->value.u == 0;
    case FLOW3_VALUE_KIND_PTR:  return a->value.p == 0;
    case FLOW3_VALUE_KIND_REF:
        //r/eturn false;
        break;
    }
    return false;
}

static unsigned int flow3_hash_key(const struct object* obj)
{
    uintptr_t p = (uintptr_t)obj;
    return (unsigned int)((p ^ (p >> 5)) % FLOW3_NUM_OF_BUCKETS);
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

/*
   A "dead" map represents a branch proven unreachable by constant
   folding (e.g. the false arm of `if (x == 3)` when x is known to be 3).
   flow3_map_merge_arms skips dead arms entirely, even if the (dead)
   branch body wrote entries into them.
*/
static struct flow3_map* _Opt flow3_map_arena_new_dead(struct flow3_map_arena* a, struct flow3_map* parent, const char* name)
{
    struct flow3_map* _Opt m = flow3_map_arena_new(a, parent, name);
    if (m)
    {
        m->is_dead = true;
    }
    return m;
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
#define FLOW3_ALLOCATED_OBJECT_ARENA_MAX_SIZE 100

struct object* flow3_allocated_object_arena_new(struct flow3_allocated_object_arena* a)
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

static void flow3_alternatives_add(struct flow3_alternatives* vs, const struct flow3_alternative* p_alternative)
{
    try
    {
        for (int i = 0; i < vs->size; i++)
        {
            if (flow3_value_is_same(&vs->data[i], p_alternative) &&
                vs->data[i].value_relation == p_alternative->value_relation &&
                vs->data[i].imaginary == p_alternative->imaginary &&
                vs->data[i].origin == p_alternative->origin)
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
        vs->data[vs->size] = *p_alternative;
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
        vs->data[vs->size].value_relation = FLOW3_RELATION_EQUAL;
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
        flow3_alternatives_add(dst, &src->data[i]);
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
            m->num_of_buckets = FLOW3_NUM_OF_BUCKETS;
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
    {
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
}

static void flow3_map_set_object_any_n(struct flow3_map* m, const struct object* obj, int line, bool nullable_enabled)
{
    if (obj->members.head)
    {
        for (const struct object* member = obj->members.head; member; member = member->next)
        {
            flow3_map_set_object_any_n(m, member, line, nullable_enabled);
        }
        return;
    }
    struct flow3_key_alternatives* p_flow3_key_alternatives = flow3_map_find_add(m, obj);
    if (p_flow3_key_alternatives == NULL)
    {
        return;
    }
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

    {
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
    }


    for (int i = 0; i < e->alternatives.size; i++)
    {
        /*
           Preserve MOVED: a moved owner reaching the end of its scope is
           fine, and overwriting the flag would make a later exit pass
           (e.g. the enclosing compound's defer list after a return already
           checked the object) report a false "owner not moved".
        */
        if (e->alternatives.data[i].imaginary != FLOW3_IMAGINARY_MOVED)
        {
            e->alternatives.data[i].imaginary = FLOW3_IMAGINARY_ENDED;
        }
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

static void flow3_map_delete(struct flow3_map* _Owner _Opt m)
{
    if (m == NULL)
        return;

    flow3_map_clear(m);
    free(m);
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
    struct flow3_map temp = { .p_parent_map = parent, .name = "merge-temp" };

    struct object_set objs = { 0 };

    for (int i = 0; i < num_arms; i++)
    {
        if (arms[i] == NULL || arms[i]->is_dead)
        {
            continue;
        }
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
        if (p_temp_entry == NULL)
        {
            continue;
        }
        flow3_alternatives_clear(&p_temp_entry->alternatives);

        for (int j = 0; j < num_arms; j++)
        {
            if (arms[j] == NULL || arms[j]->is_dead)
            {
                continue;
            }

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
            else
            {
                /* Arm is reachable but didn't write this variable:
                   inherit the pre-branch value from parent, but re-tag each
                   alternative's origin as arms[j] (this arm's map) instead of
                   keeping the ancestor's origin. This lets later logic (e.g.
                   flow3_map_transfer_true_false_set) tell values that arrived
                   via this arm's path apart from an ancestor's original
                   value, without mutating the ancestor's own entry. */
                struct flow3_key_alternatives* p_pre_entry = flow3_map_search_up(parent, obj);
                if (p_pre_entry)
                {
                    for (int k = 0; k < p_pre_entry->alternatives.size; k++)
                    {
                        const struct flow3_alternative* a2 = &p_pre_entry->alternatives.data[k];
                        {
                            struct flow3_alternative tagged = *a2;
                            tagged.origin = arms[j];
                            tagged.line = a2->line;
                            flow3_alternatives_add(&p_temp_entry->alternatives, &tagged);
                        }
                    }
                }
            }
        }
    }

    object_set_destroy(&objs);

    /* Move all merged results from temp into parent, then clean up temp. */
    flow3_map_move_entries(parent, &temp);
    flow3_map_clear(&temp);
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
static void flow3_map_accumulate_into_join(struct flow3_map* p_join, struct flow3_map* p_src)
{
    struct object_set objs = { 0 };

    for (const struct flow3_map* cur = p_src; cur; cur = cur->p_parent_map)
    {
        if (cur->buckets == NULL)
        {
            continue;
        }
        for (int i = 0; i < cur->num_of_buckets; i++)
        {
            for (struct flow3_key_alternatives* e = cur->buckets[i]; e; e = e->next)
            {
                object_set_add(&objs, e->p_obj_key);
            }
        }
    }

    for (int i = 0; i < objs.size; i++)
    {
        const struct object* obj = objs.items[i];
        const struct flow3_key_alternatives* p_src_entry = flow3_map_search_up(p_src, obj);
        if (p_src_entry == NULL || p_src_entry->alternatives.size == 0)
        {
            continue;
        }
        struct flow3_key_alternatives* p_join_entry = flow3_map_find_add(p_join, obj);
        if (p_join_entry == NULL)
        {
            continue;
        }
        flow3_alternatives_append(&p_join_entry->alternatives, &p_src_entry->alternatives);
    }

    object_set_destroy(&objs);
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
    switch (alt->value_relation)
    {
    case FLOW3_RELATION_EQUAL:         ss_fprintf(ss, "== ");     break;
    case FLOW3_RELATION_NOT_EQUAL:     ss_fprintf(ss, "!= ");     break;
    case FLOW3_RELATION_ANY:           ss_fprintf(ss, "ANY ");    break;
    case FLOW3_RELATION_UNINITIALIZED: ss_fprintf(ss, "UNINIT "); break;
    case FLOW3_RELATION_GREATER:       ss_fprintf(ss, "> ");      break;
    case FLOW3_RELATION_GREATER_EQUAL: ss_fprintf(ss, ">= ");     break;
    case FLOW3_RELATION_LESS:          ss_fprintf(ss, "< ");      break;
    case FLOW3_RELATION_LESS_EQUAL:    ss_fprintf(ss, "<= ");     break;
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
    long long v;
    if (alt->value_kind == FLOW3_VALUE_KIND_SIGNED)
        v = alt->value.i;
    else if (alt->value_kind == FLOW3_VALUE_KIND_UNSIGNED)
        v = (long long)alt->value.u;
    else
        return false;

    switch (alt->value_relation)
    {
    case FLOW3_RELATION_EQUAL:         *lo = v; *hi = v; return true;
    case FLOW3_RELATION_GREATER:       if (v == LLONG_MAX) return false; *lo = v + 1; *hi = LLONG_MAX; return true;
    case FLOW3_RELATION_GREATER_EQUAL: *lo = v;           *hi = LLONG_MAX; return true;
    case FLOW3_RELATION_LESS:          if (v == LLONG_MIN) return false; *lo = LLONG_MIN; *hi = v - 1; return true;
    case FLOW3_RELATION_LESS_EQUAL:    *lo = LLONG_MIN;    *hi = v;         return true;
    default:                           return false;
    }
}

/* Is x OP rhs true for a single scalar x? */
static bool flow3_scalar_relation_holds(long long x, enum expression_type op, long long rhs)
{
    switch (op)
    {
    case EXPR_RELATIONAL_BIGGER_THAN:          return x > rhs;
    case EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN: return x >= rhs;
    case EXPR_RELATIONAL_LESS_THAN:            return x < rhs;
    case EXPR_RELATIONAL_LESS_OR_EQUAL_THAN:   return x <= rhs;
    default:                                   return false;
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
        long long lo, hi;
        if (flow3_alt_to_interval(alt, &lo, &hi) &&
            alt->value_relation != FLOW3_RELATION_EQUAL)
        {
            return lo <= 0 && 0 <= hi;
        }
    }
    return false;
}

static void flow3_narrow_map_into(struct flow3_map* p_dest, struct flow3_map* p_before, const struct object* p_obj_key, bool true_branch)
{
    struct flow3_key_alternatives* p_existing = flow3_map_search_up(p_before, p_obj_key);
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
        struct flow3_key_alternatives* p_dest_entry0 = flow3_map_find_add(p_dest, p_obj_key);
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
    struct flow3_key_alternatives* p_dest_entry = flow3_map_find_add(p_dest, p_obj_key);
    flow3_alternatives_clear(&p_dest_entry->alternatives);

    for (int k = 0; k < p_existing->alternatives.size; k++)
    {
        struct flow3_alternative* alt = &p_existing->alternatives.data[k];

        if (alt->value_relation == FLOW3_RELATION_ANY)
        {
            if (true_branch)
            {
                {
                    struct flow3_alternative a = {
                        .value_kind = FLOW3_VALUE_KIND_SIGNED,
                        .value = {.i = 0},
                        .value_relation = FLOW3_RELATION_NOT_EQUAL,
                        .imaginary = FLOW3_IMAGINARY_NONE,
                        .origin = p_dest,
                        .line = alt->line
                    };
                    flow3_alternatives_add(&p_dest_entry->alternatives, &a);
                }
            }
            else
            {
                {
                    struct flow3_alternative a = {
                        .value_kind = FLOW3_VALUE_KIND_SIGNED,
                        .value = {.i = 0},
                        .value_relation = FLOW3_RELATION_EQUAL,
                        .imaginary = FLOW3_IMAGINARY_NONE,
                        .origin = p_dest,
                        .line = alt->line
                    };
                    flow3_alternatives_add(&p_dest_entry->alternatives, &a);
                }
            }
            continue;
        }

        if (alt->value_relation == FLOW3_RELATION_NOT_EQUAL && flow3_value_is_false(alt))
        {
            if (true_branch)
            {
                {
                    struct flow3_alternative a = {
                        .value_kind = FLOW3_VALUE_KIND_SIGNED,
                        .value = {.i = 0},
                        .value_relation = FLOW3_RELATION_NOT_EQUAL,
                        .imaginary = FLOW3_IMAGINARY_NONE,
                        .origin = p_dest,
                        .line = alt->line
                    };
                    flow3_alternatives_add(&p_dest_entry->alternatives, &a);
                }
            }
            else
            {
                {
                    struct flow3_alternative a = {
                        .value_kind = FLOW3_VALUE_KIND_SIGNED,
                        .value = {.i = 0},
                        .value_relation = FLOW3_RELATION_EQUAL,
                        .imaginary = FLOW3_IMAGINARY_NONE,
                        .origin = p_dest,
                        .line = alt->line
                    };
                    flow3_alternatives_add(&p_dest_entry->alternatives, &a);
                }
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
                    .line = alt->line
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
                    .line = alt->line
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
                    .line = alt->line
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
            if (p_existing->alternatives.data[k].value_relation == FLOW3_RELATION_UNINITIALIZED)
            {
                flow3_alternatives_add(&p_dest_entry->alternatives, &p_existing->alternatives.data[k]);
            }
        }
    }

}

static struct flow3_map* flow3_narrow_map(struct flow3_map_arena* arena, struct flow3_map* p_before, const struct object* p_obj_key, bool true_branch, const char* name)
{
    struct flow3_map* p_dest = flow3_map_arena_new(arena, p_before, name);
    flow3_narrow_map_into(p_dest, p_before, p_obj_key, true_branch);
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
static void flow3_tag_branch_pair(struct flow3_map* p_true, struct flow3_map* p_false)
{
    if (p_true == NULL || p_false == NULL || p_true == p_false)
        return;   /* folded/degenerate pair -- no real branch */
    int id = ++g_flow3_next_branch_id;
    p_true->branch_id  = id; p_true->branch_side  = true;
    p_false->branch_id = id; p_false->branch_side = false;
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
static void flow3_predicate_invalidate(struct flow3_visit_ctx* ctx, const struct object* p_obj)
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
static bool flow3_predicate_key(const struct expression* p_cond,
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

    const struct object* l = object_get_referenced(&p_cond->left->object);
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
        const struct object* r = object_get_referenced(&p_cond->right->object);
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
    enum expression_type op;
    const struct object* lo;
    const struct object* ro;
    long long rc;
    if (!flow3_predicate_key(p_cond, &op, &lo, &ro, &rc))
        return fresh_id;

    for (int i = 0; i < ctx->predicate_cache_size; i++)
    {
        if (ctx->predicate_cache[i].op == op &&
            ctx->predicate_cache[i].left_obj == lo &&
            ctx->predicate_cache[i].right_obj == ro &&
            ctx->predicate_cache[i].right_const == rc)
        {
            return ctx->predicate_cache[i].branch_id;   /* reuse */
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

static struct flow3_branch_pair flow3_ensure_branch_pair(struct flow3_visit_ctx* ctx,
    struct flow3_map* p_fallback,
    struct flow3_branch_pair pair,
    const char* true_name,
    const char* false_name)
{
    if (pair.p_true == pair.p_false)
    {
        struct flow3_map* base = pair.p_true ? pair.p_true : p_fallback;
        pair.p_true = flow3_map_arena_new(&ctx->flow3_map_arena, base, true_name);
        pair.p_false = flow3_map_arena_new(&ctx->flow3_map_arena, base, false_name);
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
       most importantly, marking it is_dead on an unconditional
       return/break/continue/goto/throw -- it corrupts p_fallback itself
       (an ancestor shared with the OTHER arm and with whatever comes
       after this whole construct), which can make unrelated,
       still-reachable code get misreported as unreachable. Wrap any
       side that aliases p_fallback in its own fresh child map so branch
       bodies can only ever mutate their own arm.
    */
    if (pair.p_true == p_fallback)
    {
        struct flow3_map* _Opt fresh = flow3_map_arena_new(&ctx->flow3_map_arena, pair.p_true, true_name);
        if (fresh != NULL)
        {
            pair.p_true = fresh;
        }
    }
    if (pair.p_false == p_fallback)
    {
        struct flow3_map* _Opt fresh = flow3_map_arena_new(&ctx->flow3_map_arena, pair.p_false, false_name);
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
        {
            struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, p_object);
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
        }

        p_object = p_object->members.head;
        for (; p_object; p_object = p_object->next)
        {
            flow3_object_init(ctx, p_object, &p_object->type, line);
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
        struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, p_object);
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
#define FLOW3_PARAMETER_OBJECT_INIT_MAX_DEPTH 4

static void flow3_parameter_object_init_r(struct flow3_visit_ctx* ctx, struct object* p_object, const struct type* p_type, int line, int depth);

static void flow3_parameter_object_init(struct flow3_visit_ctx* ctx, struct object* p_object, const struct type* p_type, int line)
{
    flow3_parameter_object_init_r(ctx, p_object, p_type, line, 0);
}

static void flow3_parameter_object_init_r(struct flow3_visit_ctx* ctx, struct object* p_object, const struct type* p_type, int line, int depth)
{
    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    if (p_object->members.head)
    {
        {
            struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, p_object);
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
        }

        p_object = p_object->members.head;
        for (; p_object; p_object = p_object->next)
        {
            flow3_parameter_object_init_r(ctx, p_object, &p_object->type, line, depth);
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
        !type_is_opt(p_type, nullable_enabled))
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
        struct object* p_pointed = flow3_allocated_object_arena_new(&ctx->allocated_object_arena);
        if (p_pointed != NULL)
        {
            struct type pointed_type = type_is_array(p_type)
                ? get_array_item_type(p_type)
                : type_remove_pointer(p_type);
            make_object(&pointed_type, p_pointed, MAKE_STATE_ANY, ctx->ctx->options.target);
            type_destroy(&pointed_type);
        }

        struct flow3_key_alternatives* ep = flow3_map_find_add(ctx->p_current_flow3_map, p_object);
        flow3_alternatives_clear(&ep->alternatives);
        /* Single EQUAL alternative carrying the concrete arena pointer.
         * The parameter is non-null on entry and points exactly to this object. */
        if (p_pointed != NULL)
        {
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
        }
        else
        {
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
                        {
                            struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, m);
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
                    /* Parent node: NOT_APPLICABLE (aggregate, state lives in members). */
                    //flow3_map_replace_alternatives_i(ctx->p_current_flow3_map,
                    //    p_pointed, NOT_APPLICABLE_VALUE, FLOW3_NOT_APPLICABLE, line);
                }
                else
                {
                    {
                        struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, p_pointed);
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
            }
            else if (depth < FLOW3_PARAMETER_OBJECT_INIT_MAX_DEPTH)
            {
                /* Seed the pointed object as ANY. */
                flow3_parameter_object_init_r(ctx, p_pointed, NULL, line, depth + 1);
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
        }
        else
        {
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
                {
                    struct flow3_alternative a = {
                        .value_kind = FLOW3_VALUE_KIND_SIGNED,
                        .value = {.i = ANY_VALUE},
                        .value_relation = FLOW3_RELATION_ANY,
                        .imaginary = FLOW3_IMAGINARY_NONE,
                        .origin = p_nonnull_map,
                        .line = line
                    };
                    flow3_alternatives_add(&eo->alternatives, &a);
                }

                /* Null arm: object does not exist when pointer is null. */
                flow3_alternatives_add_does_not_exist(&eo->alternatives, p_null_map, line);
            }
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
        struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, p_object);
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
        for (struct object* m = p_object->members.head; m != NULL; m = m->next)
            flow3_seed_aggregate_from_init_exprs(ctx, m);
        return;
    }

    /* Leaf INTEGER member set by a non-constant initializer expression.
       Restricted to integer members on purpose: pointer/owner members have
       their own move / init / null tracking, and re-running the init-assignment
       machinery on them would perturb that. Integers only carry a value
       relation, which is exactly what we want to propagate here. */
    if (p_object->p_init_expression != NULL &&
        type_is_integer(&p_object->type) &&
        p_object->state != CONSTANT_VALUE_STATE_CONSTANT &&
        p_object->state != CONSTANT_VALUE_EQUAL)
    {
        flow3_check_object_init_assigment(ctx,
            p_object->p_init_expression,
            p_object,
            &p_object->p_init_expression->object,
            INIT_OBJ);
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

    /* Non-narrowing conditions return the identity pair (or none at all);
       force two distinct arm maps so branch-body writes cannot leak
       unconditionally into the state after the if. */
    cond_pair = flow3_ensure_branch_pair(ctx, p_before, cond_pair, "if-true", "if-false");

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
            cond_pair.p_true->branch_id  = shared; cond_pair.p_true->branch_side  = true;
            cond_pair.p_false->branch_id = shared; cond_pair.p_false->branch_side = false;
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

    struct flow3_map* p_before = ctx->p_current_flow3_map;

    /* throw paths are accumulated into this map */
    struct flow3_map* p_throw_join = flow3_map_arena_new(&ctx->flow3_map_arena, p_before, "throw join");

    ctx->p_throw_join_map = p_throw_join;

    /* --- visit the try body --- */
    struct flow3_map* p_try_branch = flow3_map_arena_new(&ctx->flow3_map_arena, p_before, "try branch");
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
    /* ctx->p_throw_join_map was already restored before the catch body
       was visited, above. */
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
   True when `m` is `ancestor` or a descendant of it (walking the
   flow3_map parent chain). Used to check whether a fact recorded with
   origin `ancestor` is actually visible/applicable from the branch
   that produced `m` -- i.e. whether `ancestor` lies on the path from
   the root down to `m`, as opposed to being an unrelated sibling
   branch's own map.
*/
static bool flow3_map_is_ancestor_or_self(const struct flow3_map* ancestor, const struct flow3_map* m)
{
    if (ancestor == NULL)
        return true; /* no filter: everything is visible */

    for (const struct flow3_map* cur = m; cur; cur = cur->p_parent_map)
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
static void flow3_check_object_access(struct flow3_visit_ctx* ctx,
    const char* parent_expression_str,
    struct expression* p_expression,
    const struct object* p_object_src,
    bool check_unitialized,
    const struct flow3_map* p_origin_filter)
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
            flow3_check_object_access(ctx, ss.c_str, p_expression, member, check_unitialized, p_origin_filter);
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

        if (!flow3_map_is_ancestor_or_self(p_alternative->origin, p_origin_filter))
        {
            /* This alternative was established on a branch that isn't
               an ancestor of the branch we're checking from -- it
               belongs to a sibling arm and never actually coexists with
               the specific pointer alternative that led here. Skip it;
               see the function comment above. */
            continue;
        }

        if (p_alternative->imaginary == FLOW3_IMAGINARY_ENDED)
        {
            diagnostic(W_FLOW_LIFETIME_ENDED,
                ctx->ctx, NULL, &marker,
                "object '%s' lifetime has ended (see line %d)",
                parent_expression_str, p_alternative->line);

            continue; //no need to print other errors
        }

        if (p_alternative->value_kind == FLOW3_VALUE_KIND_REF)
        {
            if (p_alternative->value.p != p_object_src)
            {
                flow3_check_object_access(ctx,
                    parent_expression_str,
                                                    p_expression,
                                                    p_alternative->value.p, check_unitialized, p_origin_filter);

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

        if (p_alternative->imaginary != FLOW3_IMAGINARY_ENDED && p_alternative->value_relation == FLOW3_RELATION_UNINITIALIZED)
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
    if (p_object_src == NULL)
    {
        return;
    }

    if (p_object_dest == NULL)
    {
        return;
    }
    struct marker marker = expression_to_marker(p_expression);

    if (p_object_src->members.head && p_object_dest->members.head)
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
    if (p_object_src->members.head && !p_object_dest->members.head)
    {
        /*
           p_object_src has members but p_object_dest does NOT (e.g.
           `char* n = "a";`, or passing a file-scope array like
           `char file_scope_buf[200];` to a `char*` parameter): this is
           an array-to-pointer decay, not a member-wise aggregate copy,
           so the member-copy loop above (which requires both sides to
           have matching members) does not apply.

           Previously this case fell all the way through that loop doing
           nothing (its `member_dest` was NULL from the start), then
           `return`ed without ever touching p_object_dest -- silently
           keeping whatever UNINITIALIZED state flow3_object_init gave
           it, later reported as "passing a possible uninitialized
           object" the first time the pointer was read.

           The fix is NOT to fall through to the generic scalar lookup
           below and read p_object_src's own top-level key: that key
           holds whatever *stray* scalar alternative some other seeding
           path happened to leave there for the array object itself
           (e.g. file-scope objects are seeded with a generic
           "ANY value" SIGNED placeholder never meant to be read as a
           pointer), not a description of the decayed pointer value --
           doing so mistakenly flagged `parse2(file_scope_buf)` as
           "passing a possible null pointer" (found via this file's own
           ctor-array-and-pointer-parameter.c regression sample going
           from 0 warnings to 1 once the plain "return" above was
           naively changed to "fall through").

           An array's decayed address is a compile-time fact: it is
           always non-null and always "initialized" as a pointer value,
           regardless of whether the array's *contents* are themselves
           initialized (a separate, per-element concern the member-copy
           loop above already handles when both sides are aggregates).
           So just synthesize that fact directly.
        */
        if (init_type == INIT_OBJ)
        {
            struct flow3_key_alternatives* p_dest_key_alternatives =
                flow3_map_find_add(ctx->p_current_flow3_map, p_object_dest);
            flow3_alternatives_clear(&p_dest_key_alternatives->alternatives);
            struct flow3_alternative a = {
                .value_kind = FLOW3_VALUE_KIND_PTR,
                .value = {.p = (void*)p_object_src},
                .value_relation = FLOW3_RELATION_NOT_EQUAL, /* != NULL */
                .imaginary = FLOW3_IMAGINARY_NONE,
                .origin = ctx->p_current_flow3_map,
                .line = p_expression->first_token->line
            };
            flow3_alternatives_add(&p_dest_key_alternatives->alternatives, &a);
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

        if (p_src_alternative->value_kind == FLOW3_VALUE_KIND_REF)
        {
            if (p_src_alternative->value.p != NULL &&
                (uintptr_t)p_src_alternative->value.p < 0x100000)
            {
                /* Defensive: a REF alternative is documented to always carry
                   a real struct object* alias, but this address is far too
                   low to be a valid heap/arena pointer -- found via
                   dogfooding flow3 on cake's own sources (token.c,
                   tokenizer.c both hit this deterministically at the same
                   address, well below any real allocation). Root cause not
                   fully isolated; treating it as a corrupted/mistagged
                   alternative and skipping is safer than dereferencing it. */
                continue;
            }

            if (p_src_alternative->value.p != p_object_src)
            {
                flow3_check_object_init_assigment(ctx,
                                                    p_expression,
                                                    p_object_dest, //uninitialized alawys
                                                    p_src_alternative->value.p,
                                                    init_type);


                if (type_is_owner(&p_object_dest->type))
                {
                    flow3_map_set_object_moved(ctx->p_current_flow3_map, p_src_alternative->value.p, p_expression->first_token->line);
                }
                continue;
            }
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
            /*
               Skip this whole "check the pointee's own state" step when
               the DESTINATION parameter's pointer type is void* (e.g.
               `void free(void* _Owner _Opt p)`): a void* is type-erased,
               so the callee has no way to read/access any member of
               whatever *p used to be, and passing p through it is
               nothing more than moving p ITSELF (already handled above
               via flow3_map_set_object_moved for owner destinations) --
               not an access to (*p)'s structure. Without this guard, a
               _Dtor call that had already correctly ended a struct's
               members' lifetimes (e.g. `x_destroy(p);`), immediately
               followed by `free(p);` to release p itself, incorrectly
               warned "object '(*p).i' lifetime has ended" for every
               member -- even though free (being void*) never actually
               looks at any of them. */
            if (!type_is_void_ptr(&p_object_dest->type))
            {
                const bool check_unitialized = !type_is_pointed_ctor(&p_object_dest->type);
                /*
                   Pass THIS alternative's own origin as the correlation
                   filter: p_src_alternative->value.p (the pointee) may
                   have OTHER alternatives left over from a sibling
                   branch where the pointer itself held a different
                   value (e.g. null, after being reset in a catch block
                   that also freed the pointee) -- those never coexist
                   with p_src_alternative and must not be considered
                   here. See flow3_check_object_access's comment.
                */
                flow3_check_object_access(ctx, ss2.c_str, p_expression, p_src_alternative->value.p, check_unitialized, p_src_alternative->origin);
            }


            if (init_type == INIT_PARAMETER)
            {
                bool pointee_write_qualifier_handled = false;

                if (type_is_pointed_clear(&p_object_dest->type))
                {
                    flow3_map_set_object_zero(ctx->p_current_flow3_map,
                  p_src_alternative->value.p, p_expression->first_token->line);
                    pointee_write_qualifier_handled = true;
                }

                if (type_is_pointed_dtor(&p_object_dest->type))
                {
                    flow3_map_set_object_lifetime_ended(ctx->p_current_flow3_map,
                  p_src_alternative->value.p, p_expression->first_token->line);
                    pointee_write_qualifier_handled = true;
                }

                if (type_is_pointed_ctor(&p_object_dest->type))
                {
                    flow3_map_set_object_any_n(ctx->p_current_flow3_map,
                  p_src_alternative->value.p, p_expression->first_token->line,
                  ctx->ctx->options.null_checks_enabled);
                    pointee_write_qualifier_handled = true;
                }

                if (type_is_owner(&p_object_dest->type))
                {
                    flow3_map_set_object_lifetime_ended(ctx->p_current_flow3_map,
                  p_src_alternative->value.p, p_expression->first_token->line);
                    pointee_write_qualifier_handled = true;
                }

                if (!pointee_write_qualifier_handled &&
                    !type_is_pointed_const(&p_object_dest->type) &&
                    p_src_alternative->value.p != &p_expression->object)
                {
                    /* Plain mutable pointer parameter, no _Clear/_Dtor/_Ctor/_Owner
                       qualifier and not pointing to const: the callee is free to
                       write anything through it, so nothing the caller knew about
                       the pointee before the call can be trusted afterward.
                       Without this, flow3 would silently keep treating the old
                       (possibly stale) alternatives as still valid post-call.

                       The `value.p != &p_expression->object` guard excludes array
                       arguments: an array decays to a pointer whose "pointee" IS
                       the array's own object (there's no separate allocation the
                       way there is for a real pointer variable). Invalidating it
                       here clobbered the SAME entry that also carries the array's
                       own "definitely non-null" tracking, so passing an array to
                       one plain mutable-pointer parameter and then to another
                       falsely started warning "passing a possible null pointer"
                       on the second call. Found via a user-reported false
                       positive on real code (generate_file_scope_new_name's
                       `char new_name[]` passed to snprintf then hashmap_find). */
                    flow3_map_set_object_any_n(ctx->p_current_flow3_map,
                  p_src_alternative->value.p, p_expression->first_token->line,
                  ctx->ctx->options.null_checks_enabled);
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

        if (p_src_alternative->value_relation == FLOW3_RELATION_UNINITIALIZED)
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
static void flow3_check_discarding_owner_before_overwrite(struct flow3_visit_ctx* ctx,
                                                           const struct object* p_object_dest,
                                                           const struct marker* marker)
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
        for (const struct object* member = p_object_dest->members.head; member; member = member->next)
        {
            flow3_check_discarding_owner_before_overwrite(ctx, member, marker);
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

    const struct flow3_key_alternatives* e = flow3_map_search_up(ctx->p_current_flow3_map, p_object_dest);
    if (e == NULL)
    {
        return;
    }

    for (int i = 0; i < e->alternatives.size; i++)
    {
        const struct flow3_alternative* p_alternative = &e->alternatives.data[i];

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

        const char* object_name = p_object_dest->member_designator ? p_object_dest->member_designator : "";
        diagnostic(W_OWNERSHIP_DISCARDING_OWNER,
            ctx->ctx,
            NULL,
            marker,
            "assignment discards _Owner '%s' without releasing it first (see line %d)",
            object_name,
            p_alternative->line);
    }
}

static void flow3_check_assigment(struct flow3_visit_ctx* ctx,
    struct expression* p_expression_dest,
    struct expression* p_expression_src)
{
    const struct flow3_key_alternatives* p_expression_dest_key_alternatives =
        flow3_map_search_up(ctx->p_current_flow3_map, &p_expression_dest->object);

    for (int i = 0;
         p_expression_dest_key_alternatives != NULL &&
         i < p_expression_dest_key_alternatives->alternatives.size; i++)
    {
        const struct flow3_alternative* p_expression_dest_alternative =
            &p_expression_dest_key_alternatives->alternatives.data[i];

        if (p_expression_dest_alternative->value_kind == FLOW3_VALUE_KIND_REF)
        {
            const struct object* p_object_dest = p_expression_dest_alternative->value.p;

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
            flow3_check_discarding_owner_before_overwrite(ctx, p_object_dest, &marker);

            flow3_check_object_init_assigment(ctx,
                p_expression_src,
                p_object_dest,
                &p_expression_src->object,
                INIT_OBJ);
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
        const struct flow3_key_alternatives* p_src =
            flow3_map_search_up(ctx->p_current_flow3_map, &p_expression_src->object);
        for (int i = 0; p_src != NULL && i < p_src->alternatives.size; i++)
        {
            const struct flow3_alternative* a = &p_src->alternatives.data[i];
            if (a->value_kind == FLOW3_VALUE_KIND_REF && a->value.p != NULL)
                flow3_map_set_object_moved(ctx->p_current_flow3_map, a->value.p, p_expression_dest->first_token->line);
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
        long long val;
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
                    {
                        struct flow3_alternative tagged = *alt;
                        tagged.origin = origin;
                        tagged.line = line;
                        flow3_alternatives_add(false_alts, &tagged);
                    }
                }
                else
                {
                    /* != c is true, keep NOT_EQUAL c in true branch only */
                    {
                        struct flow3_alternative tagged = *alt;
                        tagged.origin = origin;
                        tagged.line = line;
                        flow3_alternatives_add(true_alts, &tagged);
                    }
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
                    {
                        struct flow3_alternative tagged = *alt;
                        tagged.origin = origin;
                        tagged.line = line;
                        flow3_alternatives_add(false_alts, &tagged);
                    }
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
    case EXPR_RELATIONAL_BIGGER_THAN:          return EXPR_RELATIONAL_LESS_THAN;
    case EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN: return EXPR_RELATIONAL_LESS_OR_EQUAL_THAN;
    case EXPR_RELATIONAL_LESS_THAN:            return EXPR_RELATIONAL_BIGGER_THAN;
    case EXPR_RELATIONAL_LESS_OR_EQUAL_THAN:   return EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN;
    default:                                   return op;
    }
}

/* Narrow `src` (a variable's alternatives) for the condition `var OP c`,
   producing refined alternatives for the true and false branches.
   - concrete EQUAL values are routed to the branch they satisfy;
   - an unknown scalar (ANY) becomes a half-line constraint on each branch;
   - everything else is carried unchanged into both branches (conservative). */
static void narrow_by_relational(const struct flow3_alternatives* src,
                                 long long c,
                                 enum expression_type op,   /* var OP c */
                                 struct flow3_alternatives* true_alts,
                                 struct flow3_alternatives* false_alts,
                                 const struct flow3_map* origin,
                                 int line)
{
    for (int i = 0; i < src->size; i++)
    {
        const struct flow3_alternative* alt = &src->data[i];
        if (alt->value_relation == FLOW3_RELATION_UNINITIALIZED)
            continue;

        long long lo, hi;
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

        /* NOT_EQUAL, an existing half-line constraint, pointers, or
           MOVED/ENDED state: keep unchanged in both branches. */
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
    const struct flow3_key_alternatives* entry =
        flow3_map_search_up(ctx->p_current_flow3_map, &p_expr->object);
    if (entry == NULL)
        return false;

    bool found = false;
    long long val = 0;

    for (int i = 0; i < entry->alternatives.size; i++)
    {
        const struct flow3_alternative* alt = &entry->alternatives.data[i];
        bool is_ref = (alt->value_kind == FLOW3_VALUE_KIND_REF && alt->value.p != NULL);
        const struct flow3_key_alternatives* resolved = entry;
        if (is_ref)
        {
            resolved = flow3_map_search_up(ctx->p_current_flow3_map, alt->value.p);
            if (resolved == NULL) return false;
        }
        int count = is_ref ? resolved->alternatives.size : 1;
        for (int j = 0; j < count; j++)
        {
            const struct flow3_alternative* v = is_ref ? &resolved->alternatives.data[j] : alt;
            if (v->value_relation != FLOW3_RELATION_EQUAL)
                return false;
            long long c;
            if (v->value_kind == FLOW3_VALUE_KIND_SIGNED)        c = v->value.i;
            else if (v->value_kind == FLOW3_VALUE_KIND_UNSIGNED) c = (long long)v->value.u;
            else if (v->value_kind == FLOW3_VALUE_KIND_PTR)      c = (long long)(uintptr_t)v->value.p;
            else return false;
            if (found && c != val)
                return false;   /* more than one distinct value */
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
    const struct flow3_key_alternatives* entry =
        flow3_map_search_up(ctx->p_current_flow3_map, &p_expr->object);
    if (entry == NULL)
        return;

    for (int i = 0; i < entry->alternatives.size; i++)
    {
        const struct flow3_alternative* alt = &entry->alternatives.data[i];
        if (alt->value_kind != FLOW3_VALUE_KIND_REF || alt->value.p == NULL)
            continue;

        const struct object* obj = alt->value.p;
        const struct flow3_key_alternatives* obj_entry =
            flow3_map_search_up(ctx->p_current_flow3_map, obj);
        if (obj_entry == NULL)
            continue;

        struct flow3_alternatives true_alts = { 0 }, false_alts = { 0 };
        narrow_by_relational(&obj_entry->alternatives, c, op,
                             &true_alts, &false_alts, p_true, line);
        /* Tag each branch's alternatives with ITS OWN map so join
           correlation can tell them apart (true values belong to p_true,
           false values to p_false -- not both to p_true). */
        for (int k = 0; k < true_alts.size; k++)  true_alts.data[k].origin = p_true;
        for (int k = 0; k < false_alts.size; k++) false_alts.data[k].origin = p_false;
        if (true_alts.size > 0)
        {
            struct flow3_key_alternatives* e = flow3_map_find_add(p_true, obj);
            flow3_alternatives_clear(&e->alternatives);
            e->alternatives = true_alts;
        }
        if (false_alts.size > 0)
        {
            struct flow3_key_alternatives* e = flow3_map_find_add(p_false, obj);
            flow3_alternatives_clear(&e->alternatives);
            e->alternatives = false_alts;
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
        if (alt->value_kind == FLOW3_VALUE_KIND_PTR)
        {
            if (c == 0)
            {
                if (alt->value_relation == FLOW3_RELATION_EQUAL)
                {
                    result = is_equal ? (alt->value.p == NULL) : (alt->value.p != NULL);
                    known = true;
                }
                else if (alt->value_relation == FLOW3_RELATION_NOT_EQUAL && alt->value.p == NULL)
                {
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
    for (const struct flow3_map* p = m; p != NULL; p = p->p_parent_map)
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
static bool flow3_origins_compatible(const struct flow3_map* o1, const struct flow3_map* o2)
{
    if (o1 == NULL || o2 == NULL || o1 == o2)
        return true;
    for (const struct flow3_map* a = o1; a != NULL; a = a->p_parent_map)
    {
        if (a->branch_id == 0)
            continue;
        for (const struct flow3_map* b = o2; b != NULL; b = b->p_parent_map)
        {
            if (b->branch_id == a->branch_id && b->branch_side != a->branch_side)
                return false;
        }
    }
    return true;
}

/* Given two COMPATIBLE origins (one an ancestor-or-equal of the other),
   return the more specific (descendant) one -- the narrower branch the
   combined value belongs to. This keeps a computed result correlated to the
   branch that produced it (e.g. 1+2 -> 3 stays tagged to the `then` branch). */
static const struct flow3_map* flow3_origin_more_specific(const struct flow3_map* o1,
                                                          const struct flow3_map* o2)
{
    if (o1 == NULL) return o2;
    if (o2 == NULL) return o1;
    if (o1 == o2)   return o1;
    if (flow3_map_is_ancestor_or_equal(o1, o2)) return o2;  /* o1 is ancestor -> o2 deeper */
    return o1;                                              /* o2 is ancestor -> o1 deeper */
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
    case FLOW3_VALUE_KIND_SIGNED:   *out = alt->value.i;                     return true;
    case FLOW3_VALUE_KIND_UNSIGNED: *out = (long long)alt->value.u;          return true;
    case FLOW3_VALUE_KIND_PTR:      *out = (long long)(uintptr_t)alt->value.p; return true;
    default:                        return false;
    }
}

static int flow3_pair_equality(const struct flow3_alternative* lval,
                               const struct flow3_alternative* rval,
                               bool is_equal)
{
    long long lc, rc;
    bool lk = flow3_alt_concrete_int(lval, &lc);
    bool rk = flow3_alt_concrete_int(rval, &rc);

    if (lk && rk)
    {
        bool eq = (lc == rc);
        return (is_equal ? eq : !eq) ? 1 : 0;
    }
    if (rk)
    {
        struct flow3_alternatives one = { (struct flow3_alternative*)lval, 1, 1 };
        return flow3_evaluate_equality_against_constant(&one, rc, is_equal);
    }
    if (lk)
    {
        struct flow3_alternatives one = { (struct flow3_alternative*)rval, 1, 1 };
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
    const struct flow3_key_alternatives* left_entry =
        flow3_map_search_up(ctx->p_current_flow3_map, &p_left->object);
    const struct flow3_key_alternatives* right_entry =
        flow3_map_search_up(ctx->p_current_flow3_map, &p_right->object);
    if (left_entry == NULL || right_entry == NULL)
        return -1;

    bool all_true = true, all_false = true, any_pair = false, any_skipped = false;

    for (int li = 0; li < left_entry->alternatives.size; li++)
    {
        const struct flow3_alternative* lalt = &left_entry->alternatives.data[li];
        bool l_is_ref = (lalt->value_kind == FLOW3_VALUE_KIND_REF && lalt->value.p != NULL);
        const struct flow3_key_alternatives* left_resolved = left_entry;
        if (l_is_ref)
        {
            left_resolved = flow3_map_search_up(ctx->p_current_flow3_map, lalt->value.p);
            if (left_resolved == NULL) return -1;
        }
        int l_count = l_is_ref ? left_resolved->alternatives.size : 1;

        for (int lvi = 0; lvi < l_count; lvi++)
        {
            const struct flow3_alternative* lval = l_is_ref ? &left_resolved->alternatives.data[lvi] : lalt;

            for (int ri = 0; ri < right_entry->alternatives.size; ri++)
            {
                const struct flow3_alternative* ralt = &right_entry->alternatives.data[ri];
                bool r_is_ref = (ralt->value_kind == FLOW3_VALUE_KIND_REF && ralt->value.p != NULL);
                const struct flow3_key_alternatives* right_resolved = right_entry;
                if (r_is_ref)
                {
                    right_resolved = flow3_map_search_up(ctx->p_current_flow3_map, ralt->value.p);
                    if (right_resolved == NULL) return -1;
                }
                int r_count = r_is_ref ? right_resolved->alternatives.size : 1;

                for (int rvi = 0; rvi < r_count; rvi++)
                {
                    const struct flow3_alternative* rval = r_is_ref ? &right_resolved->alternatives.data[rvi] : ralt;
                    if (!flow3_origins_compatible(lval->origin, rval->origin))
                    {
                        any_skipped = true;   /* correlated join: cross-branch pair */
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
    const struct flow3_key_alternatives* entry =
        flow3_map_search_up(ctx->p_current_flow3_map, &p_expr->object);
    if (entry == NULL)
        return;

    for (int i = 0; i < entry->alternatives.size; i++)
    {
        const struct flow3_alternative* alt = &entry->alternatives.data[i];
        if (alt->value_kind != FLOW3_VALUE_KIND_REF || alt->value.p == NULL)
            continue;

        const struct object* obj = alt->value.p;
        const struct flow3_key_alternatives* obj_entry =
            flow3_map_search_up(ctx->p_current_flow3_map, obj);
        if (obj_entry == NULL)
            continue;

        struct flow3_alternatives true_alts = { 0 }, false_alts = { 0 };
        narrow_by_constant(&obj_entry->alternatives, c, is_equal,
                           &true_alts, &false_alts, p_true, line);
        /* Tag each branch's alternatives with its own map (see the relational
           narrow) so join correlation can distinguish them. */
        for (int k = 0; k < true_alts.size; k++)  true_alts.data[k].origin = p_true;
        for (int k = 0; k < false_alts.size; k++) false_alts.data[k].origin = p_false;
        if (true_alts.size > 0)
        {
            struct flow3_key_alternatives* e = flow3_map_find_add(p_true, obj);
            flow3_alternatives_clear(&e->alternatives);
            e->alternatives = true_alts;
        }
        if (false_alts.size > 0)
        {
            struct flow3_key_alternatives* e = flow3_map_find_add(p_false, obj);
            flow3_alternatives_clear(&e->alternatives);
            e->alternatives = false_alts;
        }
    }
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
    switch (alt->value_kind)
    {
    case FLOW3_VALUE_KIND_SIGNED:   printf("%lld", alt->value.i); break;
    case FLOW3_VALUE_KIND_UNSIGNED: printf("%llu", alt->value.u); break;
    case FLOW3_VALUE_KIND_PTR:      printf("%s", obj_display(alt->value.p)); break;
    case FLOW3_VALUE_KIND_REF:      printf("REF->%s", obj_display(alt->value.p)); break;
    default:                   printf("?");
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
    long long worst_l, worst_r, best_l, best_r;
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
    const struct flow3_key_alternatives* left_entry =
        flow3_map_search_up(ctx->p_current_flow3_map, &p_left->object);
    const struct flow3_key_alternatives* right_entry =
        flow3_map_search_up(ctx->p_current_flow3_map, &p_right->object);
    if (left_entry == NULL || right_entry == NULL)
        return -1;

    bool all_true = true, all_false = true, any = false;

    for (int li = 0; li < left_entry->alternatives.size; li++)
    {
        const struct flow3_alternative* lalt = &left_entry->alternatives.data[li];
        bool l_is_ref = (lalt->value_kind == FLOW3_VALUE_KIND_REF && lalt->value.p != NULL);
        const struct flow3_key_alternatives* left_resolved = left_entry;
        if (l_is_ref)
        {
            left_resolved = flow3_map_search_up(ctx->p_current_flow3_map, lalt->value.p);
            if (left_resolved == NULL) return -1;
        }
        int l_count = l_is_ref ? left_resolved->alternatives.size : 1;

        for (int lvi = 0; lvi < l_count; lvi++)
        {
            const struct flow3_alternative* lval = l_is_ref ? &left_resolved->alternatives.data[lvi] : lalt;
            long long llo, lhi;
            if (!flow3_alt_to_interval(lval, &llo, &lhi))
                return -1;

            for (int ri = 0; ri < right_entry->alternatives.size; ri++)
            {
                const struct flow3_alternative* ralt = &right_entry->alternatives.data[ri];
                bool r_is_ref = (ralt->value_kind == FLOW3_VALUE_KIND_REF && ralt->value.p != NULL);
                const struct flow3_key_alternatives* right_resolved = right_entry;
                if (r_is_ref)
                {
                    right_resolved = flow3_map_search_up(ctx->p_current_flow3_map, ralt->value.p);
                    if (right_resolved == NULL) return -1;
                }
                int r_count = r_is_ref ? right_resolved->alternatives.size : 1;

                for (int rvi = 0; rvi < r_count; rvi++)
                {
                    const struct flow3_alternative* rval = r_is_ref ? &right_resolved->alternatives.data[rvi] : ralt;
                    if (!flow3_origins_compatible(lval->origin, rval->origin))
                        continue;   /* correlated join: skip cross-branch pairs */
                    long long rlo, rhi;
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
    case EXPR_EQUALITY_EQUAL:     return flow3_pair_equality(lval, rval, true);
    case EXPR_EQUALITY_NOT_EQUAL: return flow3_pair_equality(lval, rval, false);
    case EXPR_RELATIONAL_BIGGER_THAN:
    case EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN:
    case EXPR_RELATIONAL_LESS_THAN:
    case EXPR_RELATIONAL_LESS_OR_EQUAL_THAN:
    {
        long long llo, lhi, rlo, rhi;
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
    const struct flow3_key_alternatives* left_entry =
        flow3_map_search_up(ctx->p_current_flow3_map, &p_left->object);
    const struct flow3_key_alternatives* right_entry =
        flow3_map_search_up(ctx->p_current_flow3_map, &p_right->object);
    if (left_entry == NULL || right_entry == NULL)
        return false;

    bool any = false;

    for (int li = 0; li < left_entry->alternatives.size; li++)
    {
        const struct flow3_alternative* lalt = &left_entry->alternatives.data[li];
        bool l_is_ref = (lalt->value_kind == FLOW3_VALUE_KIND_REF && lalt->value.p != NULL);
        const struct flow3_key_alternatives* left_resolved = left_entry;
        if (l_is_ref)
        {
            left_resolved = flow3_map_search_up(ctx->p_current_flow3_map, lalt->value.p);
            if (left_resolved == NULL) { flow3_alternatives_clear(out); return false; }
        }
        int l_count = l_is_ref ? left_resolved->alternatives.size : 1;

        for (int lvi = 0; lvi < l_count; lvi++)
        {
            const struct flow3_alternative* lval = l_is_ref ? &left_resolved->alternatives.data[lvi] : lalt;

            for (int ri = 0; ri < right_entry->alternatives.size; ri++)
            {
                const struct flow3_alternative* ralt = &right_entry->alternatives.data[ri];
                bool r_is_ref = (ralt->value_kind == FLOW3_VALUE_KIND_REF && ralt->value.p != NULL);
                const struct flow3_key_alternatives* right_resolved = right_entry;
                if (r_is_ref)
                {
                    right_resolved = flow3_map_search_up(ctx->p_current_flow3_map, ralt->value.p);
                    if (right_resolved == NULL) { flow3_alternatives_clear(out); return false; }
                }
                int r_count = r_is_ref ? right_resolved->alternatives.size : 1;

                for (int rvi = 0; rvi < r_count; rvi++)
                {
                    const struct flow3_alternative* rval = r_is_ref ? &right_resolved->alternatives.data[rvi] : ralt;
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
    struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
    flow3_alternatives_clear(&e->alternatives);

    struct flow3_alternatives per_path = { 0 };
    if (flow3_comparison_result_alts(ctx, p_expression->left, p_expression->right,
                                     p_expression->expression_type, &per_path,
                                     p_expression->first_token->line))
    {
        e->alternatives = per_path;   /* move */
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
    const struct flow3_key_alternatives* left_entry =
        flow3_map_search_up(ctx->p_current_flow3_map, &p_left->object);
    const struct flow3_key_alternatives* right_entry =
        flow3_map_search_up(ctx->p_current_flow3_map, &p_right->object);

    if (!left_entry || !right_entry)
    {
        {
            struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, &p_result->object);
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
        }
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
        const struct flow3_alternative* lalt = &left_entry->alternatives.data[li];
        if (lalt->imaginary == FLOW3_IMAGINARY_ABSENT) continue;

        // Resolve left REF if present
        const struct flow3_key_alternatives* left_resolved = left_entry;
        if (lalt->value_kind == FLOW3_VALUE_KIND_REF && lalt->value.p != NULL)
        {
            left_resolved = flow3_map_search_up(ctx->p_current_flow3_map, lalt->value.p);
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
                if (ralt->value_kind == FLOW3_VALUE_KIND_REF && ralt->value.p != NULL)
                {
                    right_resolved = flow3_map_search_up(ctx->p_current_flow3_map, ralt->value.p);
                    if (!right_resolved) continue;
                }

                // ---- Loop over resolved right values ----
                for (int rvi = 0; rvi < right_resolved->alternatives.size; rvi++)
                {
                    const struct flow3_alternative* rval = &right_resolved->alternatives.data[rvi];
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
                            case '<':   /* << */
                            case '>':   /* >> */
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
                        bool lptr = (lval->value_kind == FLOW3_VALUE_KIND_PTR);
                        bool rptr = (rval->value_kind == FLOW3_VALUE_KIND_PTR);
                        if (lptr && !rptr && (op == '+' || op == '-'))
                        {
                            flow3_alternatives_add(&result_alts, lval);
                            continue;
                        }
                        if (rptr && !lptr && op == '+')
                        {
                            flow3_alternatives_add(&result_alts, rval);
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
    struct flow3_key_alternatives* dst =
        flow3_map_find_add(ctx->p_current_flow3_map, &p_result->object);
    if (dst)
    {
        flow3_alternatives_clear(&dst->alternatives);
        if (result_alts.size > 0)
            dst->alternatives = result_alts;   // move
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
static void flow3_seed_constant_result(struct flow3_visit_ctx* ctx, struct expression* p_expression)
{
    if (!object_has_known_value(&p_expression->object))
        return;

    struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
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

static bool flow3_cast_one_value(struct flow3_visit_ctx* ctx,
                                 const struct flow3_alternative* alt,
                                 const struct type* p_target_type,
                                 struct flow3_alternatives* out,
                                 const struct flow3_map* origin,
                                 int line)
{
    if (alt->imaginary == FLOW3_IMAGINARY_ABSENT ||
        alt->value_relation != FLOW3_RELATION_EQUAL)
        return false;

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
        return false;   /* nonzero int->ptr, or int->other */
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
        /* An enumerator is a compile-time constant (the parser folded its
           value into the expression object). Seed it so it can be used in
           flow-checked comparisons, like a numeric literal. Enum values may be
           negative, so seed it as signed. */
        if (object_has_known_value(&p_expression->object))
        {
            struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
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
                struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, p_obj);
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

            struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, p_obj);
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

        assert(p_expression->declarator != NULL);

        {
            struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
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
        struct flow3_map* p_true = flow3_narrow_map(&ctx->flow3_map_arena, ctx->p_current_flow3_map, p_obj2, true, "var-true");
        struct flow3_map* p_false = flow3_narrow_map(&ctx->flow3_map_arena, ctx->p_current_flow3_map, p_obj2, false, "var-false");
        flow3_tag_branch_pair(p_true, p_false);
        return (struct flow3_branch_pair) { p_true, p_false };
    }

    case EXPR_PRIMARY_PARENTHESIS:
    {
        assert(p_expression->right != NULL);
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
        const struct flow3_key_alternatives* p_inner_entry =
            flow3_map_search_up(ctx->p_current_flow3_map, &p_inner->object);
        if (p_inner_entry)
        {
            struct flow3_key_alternatives* p_entry =
                flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
            if (p_entry)
            {
                flow3_alternatives_clear(&p_entry->alternatives);
                flow3_alternatives_append(&p_entry->alternatives, &p_inner_entry->alternatives);
            }
        }

        return paren_pair;
    }

    case EXPR_PRIMARY_STATEMENT_EXPRESSION:
        assert(p_expression->compound_statement != NULL);
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
           `default: assert(0); return 0;` branch, which in an NDEBUG
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
        struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
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
            struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
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
        flow3_tag_branch_pair(p_true, p_false);

        for (int i = 0; p_left_alternatives != NULL && i < p_left_alternatives->alternatives.size; i++)
        {
            const struct flow3_alternative* p_left_alternative = &p_left_alternatives->alternatives.data[i];

            if (p_left_alternative->value_relation == FLOW3_RELATION_EQUAL &&
                    p_left_alternative->value_kind == FLOW3_VALUE_KIND_REF &&
                    p_left_alternative->value.p != NULL)
            {
                struct object* p_member = object_get_member(p_left_alternative->value.p, member_index);

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
        flow3_tag_branch_pair(p_true, p_false);

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
                        const struct flow3_alternative* p_pointer_alt = &p_pointer_alts->alternatives.data[k];

                        if (p_pointer_alt->imaginary == FLOW3_IMAGINARY_ABSENT)
                            continue;

                        /* Null check (skipped in unevaluated contexts like
                           sizeof/_Alignof: the -> is never applied at runtime). */
                        if (p_pointer_alt->value_relation == FLOW3_RELATION_EQUAL &&
                            p_pointer_alt->value_kind == FLOW3_VALUE_KIND_PTR &&
                            p_pointer_alt->value.p == NULL)
                        {
                            if (!ctx->expression_is_not_evaluated)
                                diagnostic(W_FLOW_NULL_DEREFERENCE, ctx->ctx, NULL, &marker,
                                       "-> operator applied to a null pointer");
                            continue;
                        }

                        if (p_pointer_alt->value_kind != FLOW3_VALUE_KIND_PTR || p_pointer_alt->value.p == NULL)
                            continue;

                        const struct object* p_pointed_obj = p_pointer_alt->value.p;

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

                        flow3_narrow_map_into(p_true, ctx->p_current_flow3_map, member_obj, true);
                        flow3_narrow_map_into(p_false, ctx->p_current_flow3_map, member_obj, false);

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
                        if (!ctx->expression_is_not_evaluated)
                            diagnostic(W_FLOW_NULL_DEREFERENCE, ctx->ctx, NULL, &marker,
                                   "-> operator applied to a null pointer");
                        continue;
                    }

                    if (ptr_alt->value_kind != FLOW3_VALUE_KIND_PTR || ptr_alt->value.p == NULL)
                        continue;

                    const struct object* p_pointed_obj = ptr_alt->value.p;

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
                const struct flow3_key_alternatives* idx_alts =
                    flow3_map_search_up(ctx->p_current_flow3_map, &p_expression->right->object);
                struct marker idx_marker = expression_to_marker(p_expression);
                bool warned = false;
                for (int i = 0; !warned && idx_alts != NULL && i < idx_alts->alternatives.size; i++)
                {
                    const struct flow3_alternative* idx = &idx_alts->alternatives.data[i];

                    /* The index expression usually resolves to a REF to the
                       variable object; follow it to the value alternatives. */
                    const struct flow3_key_alternatives* value_alts = NULL;
                    if (idx->value_kind == FLOW3_VALUE_KIND_REF && idx->value.p != NULL)
                        value_alts = flow3_map_search_up(ctx->p_current_flow3_map, idx->value.p);

                    const struct flow3_alternative* vlist_one = idx;
                    int vcount = 1;
                    const struct flow3_alternative* vlist_many = NULL;
                    if (value_alts != NULL)
                    {
                        vlist_many = value_alts->alternatives.data;
                        vcount = value_alts->alternatives.size;
                    }

                    for (int j = 0; !warned && j < vcount; j++)
                    {
                        const struct flow3_alternative* v = vlist_many ? &vlist_many[j] : vlist_one;
                        long long lo, hi;
                        if (!flow3_alt_to_interval(v, &lo, &hi))
                            continue;
                        if (lo >= N)
                        {
                            diagnostic(W_OUT_OF_BOUNDS, ctx->ctx, NULL, &idx_marker,
                                "array index is past the end of the array (size %lld)", N);
                            warned = true;
                        }
                        else if (hi < 0)
                        {
                            diagnostic(W_OUT_OF_BOUNDS, ctx->ctx, NULL, &idx_marker,
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

            struct flow3_key_alternatives* result_entry = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
            flow3_alternatives_clear(&result_entry->alternatives);

            const struct flow3_key_alternatives* p_left_alternatives =
                flow3_map_search_up(ctx->p_current_flow3_map, &skip_parenthesis(p_expression->left)->object);

            struct flow3_map* p_true = flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, "arr-true");
            struct flow3_map* p_false = flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, "arr-false");
            flow3_tag_branch_pair(p_true, p_false);

            bool any_resolved = false;
            for (int i = 0; index >= 0 && p_left_alternatives != NULL && i < p_left_alternatives->alternatives.size; i++)
            {
                const struct flow3_alternative* p_left_alternative = &p_left_alternatives->alternatives.data[i];

                if (p_left_alternative->value_relation == FLOW3_RELATION_EQUAL &&
                    p_left_alternative->value_kind == FLOW3_VALUE_KIND_REF &&
                    p_left_alternative->value.p != NULL)
                {
                    struct object* p_element = object_get_member(p_left_alternative->value.p, (size_t)index);
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

                    flow3_narrow_map_into(p_true, ctx->p_current_flow3_map, p_element, true);
                    flow3_narrow_map_into(p_false, ctx->p_current_flow3_map, p_element, false);
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

        /* Array element used as bool (unknown index or unresolved). */
        const struct object* p_obj = &p_expression->object;
        struct flow3_map* p_true = flow3_narrow_map(&ctx->flow3_map_arena, ctx->p_current_flow3_map, p_obj, true, "arr-true");
        struct flow3_map* p_false = flow3_narrow_map(&ctx->flow3_map_arena, ctx->p_current_flow3_map, p_obj, false, "arr-false");
        flow3_tag_branch_pair(p_true, p_false);
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

            struct flow3_map* p_nonnull_map = flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, "call-opt-nonnull");

            struct object* p_pointed = flow3_allocated_object_arena_new(&ctx->allocated_object_arena);
            if (p_pointed != NULL)
            {
                struct type pointed_type = type_remove_pointer(p_ret_type);
                make_object(&pointed_type, p_pointed, MAKE_STATE_ANY, ctx->ctx->options.target);
                struct flow3_map* old = ctx->p_current_flow3_map;
                ctx->p_current_flow3_map = p_nonnull_map;
                flow3_object_init(ctx, p_pointed, &pointed_type, call_line);
                ctx->p_current_flow3_map = old;
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
            struct flow3_alternative v = {
                .value_kind = FLOW3_VALUE_KIND_PTR,
                .value = {.p = NULL }
            };
            {
                struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                flow3_alternatives_clear(&e->alternatives);
                struct flow3_alternative a = {
                    .value_kind = v.value_kind,
                    .value = v.value,
                    .value_relation = FLOW3_RELATION_NOT_EQUAL,
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
        {
            struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
            flow3_alternatives_clear(&e->alternatives);
            struct flow3_alternative a = {
                .value_kind = FLOW3_VALUE_KIND_SIGNED,
                .value = {.i = ANY_VALUE},
                .value_relation = FLOW3_RELATION_ANY,
                .imaginary = FLOW3_IMAGINARY_NONE,
                .origin = ctx->p_current_flow3_map,
                .line = line
            };
            flow3_alternatives_add(&e->alternatives, &a);
        }
        break;
    }

    case EXPR_UNARY_STATIC_ASSERTION:
        assert(p_expression->static_assertion);
        flow3_visit_static_assertion(ctx, p_expression->static_assertion);
        break;

    case EXPR_UNARY_ALIGNOF_EXPRESSION:
    {
        assert(p_expression->right);
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
    {
        assert(p_expression->right);

        const bool t2 = ctx->expression_is_not_evaluated;
        ctx->expression_is_not_evaluated = true;
        flow3_visit_expression(ctx, p_expression->right);
        ctx->expression_is_not_evaluated = t2;
        flow3_seed_constant_result(ctx, p_expression);
        break;
    }

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
            {
                struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
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
            /* Operand value unknown — result is also unknown. */
            {
                struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
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
            {
                struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
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
                struct flow3_map* p_dead = flow3_map_arena_new_dead(&ctx->flow3_map_arena, p_before, "not-dead-false");
                return (struct flow3_branch_pair) { ctx->p_current_flow3_map, p_dead };
            }
            else
            {
                struct flow3_map* p_dead = flow3_map_arena_new_dead(&ctx->flow3_map_arena, p_before, "not-dead-true");
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

        /* Mutating the operand invalidates any predicate over it. */
        flow3_predicate_invalidate(ctx, object_get_referenced(&p_operand->object));

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
            {
                struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, p_obj);
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
                struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
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
            const struct flow3_alternative* ref = &p_entry->alternatives.data[ri];
            if (ref->value_kind != FLOW3_VALUE_KIND_REF || ref->value.p == NULL)
                continue;
            advanced_any = true;

            const struct object* p_actual_obj = ref->value.p;
            const struct flow3_key_alternatives* p_resolved =
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
                const struct flow3_alternative* alt = &p_resolved->alternatives.data[i];
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
                    /* advancing a pointer keeps its null/pointee identity */
                    flow3_alternatives_add(&new_var_alts, alt);
                    flow3_alternatives_add(&new_result_alts, alt);
                }
                else
                {
                    struct flow3_alternative a = { .value_kind = FLOW3_VALUE_KIND_SIGNED, .value = {.i = ANY_VALUE}, .value_relation = FLOW3_RELATION_ANY, .imaginary = FLOW3_IMAGINARY_NONE, .origin = org, .line = p_expression->first_token->line };
                    flow3_alternatives_add(&new_var_alts, &a);
                    flow3_alternatives_add(&new_result_alts, &a);
                }
            }

            struct flow3_key_alternatives* p_var_entry = flow3_map_find_add(ctx->p_current_flow3_map, p_actual_obj);
            if (p_var_entry) { flow3_alternatives_clear(&p_var_entry->alternatives); p_var_entry->alternatives = new_var_alts; }
            else flow3_alternatives_clear(&new_var_alts);
        }

        if (!advanced_any)
        {
            struct flow3_alternative a = { .value_kind = FLOW3_VALUE_KIND_SIGNED, .value = {.i = ANY_VALUE}, .value_relation = FLOW3_RELATION_ANY, .imaginary = FLOW3_IMAGINARY_NONE, .origin = ctx->p_current_flow3_map, .line = p_expression->first_token->line };
            flow3_alternatives_add(&new_result_alts, &a);
        }

        {
            struct flow3_key_alternatives* p_result_entry = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
            if (p_result_entry) { flow3_alternatives_clear(&p_result_entry->alternatives); p_result_entry->alternatives = new_result_alts; }
            else flow3_alternatives_clear(&new_result_alts);
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
            {
                struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
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
                struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
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

        struct flow3_key_alternatives* result_entry = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
        flow3_alternatives_clear(&result_entry->alternatives);

        const struct flow3_key_alternatives* p_right_alternatives = flow3_map_search_up(ctx->p_current_flow3_map, &p_expression->right->object);
        if (p_right_alternatives)
        {


            for (int i = 0; i < p_right_alternatives->alternatives.size; i++)
            {
                const struct flow3_alternative* p_right_alternative = &p_right_alternatives->alternatives.data[i];
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
        assert(p_expression->right != NULL);

        flow3_visit_expression(ctx, p_expression->right);

        struct flow3_key_alternatives* result_entry = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
        flow3_alternatives_clear(&result_entry->alternatives);
        struct marker marker = expression_to_marker(p_expression);

        const struct flow3_key_alternatives* p_right_alternatives = flow3_map_search_up(ctx->p_current_flow3_map, &p_expression->right->object);

        for (int i = 0; p_right_alternatives != NULL && i < p_right_alternatives->alternatives.size; i++)
        {
            const struct flow3_alternative* p_right_alt = &p_right_alternatives->alternatives.data[i];

            if (p_right_alt->imaginary == FLOW3_IMAGINARY_ABSENT)
                continue;
            if (p_right_alt->value_kind == FLOW3_VALUE_KIND_REF)
            {
                const struct flow3_key_alternatives* p_right_alternatives2 = flow3_map_search_up(ctx->p_current_flow3_map,
                    p_right_alt->value.p);

                if (p_right_alternatives2 == NULL)
                    continue;

                for (int j = 0; j < p_right_alternatives2->alternatives.size; j++)
                {
                    const struct flow3_alternative* p_right_alt2 = &p_right_alternatives2->alternatives.data[j];

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
                        diagnostic(W_FLOW_NULL_DEREFERENCE, ctx->ctx, NULL, &marker,
                                                        "possible null pointer dereference");

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
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

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
            const struct flow3_key_alternatives* p_dest_alts =
                flow3_map_search_up(ctx->p_current_flow3_map, p_dest_obj);
            if (p_dest_alts &&
                p_dest_alts->alternatives.size == 1 &&
                p_dest_alts->alternatives.data[0].value_relation == FLOW3_RELATION_EQUAL &&
                p_dest_alts->alternatives.data[0].value_kind == FLOW3_VALUE_KIND_REF &&
                p_dest_alts->alternatives.data[0].value.p != NULL)
            {
                p_dest_obj = p_dest_alts->alternatives.data[0].value.p;
            }

            struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
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
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);



        struct flow3_branch_pair lhs_pair2 = flow3_visit_expression(ctx, p_expression->left);
        flow3_visit_expression(ctx, p_expression->right);

        const struct object* p_left_obj = object_get_referenced(&p_expression->left->object);
        struct flow3_key_alternatives* p_lhs_entry = flow3_map_search_up(ctx->p_current_flow3_map, p_left_obj);

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
            const struct flow3_alternative* la = &p_lhs_entry->alternatives.data[i];

            if (la->value_kind == FLOW3_VALUE_KIND_PTR)
            {
                struct flow3_alternative a = *la;      /* keep pointer alternative */
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
                case EXPR_ASSIGNMENT_PLUS_ASSIGN:        result = lv + rv;                break;
                case EXPR_ASSIGNMENT_MINUS_ASSIGN:       result = lv - rv;                break;
                case EXPR_ASSIGNMENT_MULTI_ASSIGN:       result = lv * rv;                break;
                case EXPR_ASSIGNMENT_DIV_ASSIGN:         result = rv != 0 ? lv / rv : lv; break;
                case EXPR_ASSIGNMENT_MOD_ASSIGN:         result = rv != 0 ? lv % rv : lv; break;
                case EXPR_ASSIGNMENT_SHIFT_LEFT_ASSIGN:  result = lv << rv;               break;
                case EXPR_ASSIGNMENT_SHIFT_RIGHT_ASSIGN: result = lv >> rv;               break;
                case EXPR_ASSIGNMENT_AND_ASSIGN:         result = lv & rv;                break;
                case EXPR_ASSIGNMENT_OR_ASSIGN:          result = lv | rv;                break;
                case EXPR_ASSIGNMENT_NOT_ASSIGN:         result = lv ^ rv;                break;
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
            struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, p_left_obj);
            flow3_alternatives_clear(&e->alternatives);
            if (all_handled && new_alts.size > 0)
            {
                e->alternatives = new_alts;            /* move */
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
            {
                struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
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
            const struct flow3_alternative* src_alt = &p_src_entry->alternatives.data[i];
            if (src_alt->value_kind == FLOW3_VALUE_KIND_REF && src_alt->value.p != NULL)
            {
                const struct flow3_key_alternatives* resolved =
                    flow3_map_search_up(ctx->p_current_flow3_map, src_alt->value.p);
                if (resolved == NULL) { all_handled = false; break; }
                for (int j = 0; j < resolved->alternatives.size; j++)
                {
                    const struct flow3_alternative* v = &resolved->alternatives.data[j];
                    if (!flow3_cast_one_value(ctx, v, p_target_type, &new_alts,
                                              flow3_origin_more_specific(v->origin, src_alt->origin),
                                              p_expression->first_token->line))
                    { all_handled = false; break; }
                }
            }
            else
            {
                if (!flow3_cast_one_value(ctx, src_alt, p_target_type, &new_alts,
                                          src_alt->origin, p_expression->first_token->line))
                { all_handled = false; break; }
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
            {
                struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
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

        flow3_map_remove(ctx->p_current_flow3_map, &p_expression->left->object);
        break;
    }
    break;

    case EXPR_SHIFT_RIGHT:
    case EXPR_SHIFT_LEFT:
    {
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);

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
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

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
                struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
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
            struct flow3_map* p_dead = flow3_map_arena_new_dead(&ctx->flow3_map_arena,
                                                           ctx->p_current_flow3_map,
                                                           "rel-dead");
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
           EXPR_UNARY_ASSERT true-branch merge, `assert(a > 0)` -- record the
           half-line fact so a later compile_assert(a > 0) can prove it. */
        {
            long long cst = 0;
            const struct expression* p_var_expr = NULL;
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
                struct flow3_map* p_true = flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, "rel-true");
                struct flow3_map* p_false = flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, "rel-false");
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
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        flow3_visit_expression(ctx, p_expression->left);
        flow3_visit_expression(ctx, p_expression->right);

        struct flow3_key_alternatives* p_result_alternatives =
            flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
        flow3_alternatives_clear(&p_result_alternatives->alternatives);

        const bool is_equal_op = (p_expression->expression_type == EXPR_EQUALITY_EQUAL);

        /* Fold across ALL alternatives of both operands. A constant is simply
           an operand with a single alternative -- no special case. */
        int fold = flow3_evaluate_equality_multi(ctx, p_expression->left, p_expression->right, is_equal_op);
        if (fold != -1)
        {
            {
                struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
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
            struct flow3_map* p_dead = flow3_map_arena_new_dead(&ctx->flow3_map_arena,
                                                           ctx->p_current_flow3_map,
                                                           "eq-dead");
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
        const struct expression* p_var_expr = NULL;
        if (flow3_operand_is_single_constant(ctx, p_expression->right, &cst))
            p_var_expr = p_expression->left;
        else if (flow3_operand_is_single_constant(ctx, p_expression->left, &cst))
            p_var_expr = p_expression->right;

        if (p_var_expr)
        {
            struct flow3_map* p_true = flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, "eq-true");
            struct flow3_map* p_false = flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, "eq-false");
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
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);


        if (object_has_constant_value(&p_expression->left->object) &&
            object_has_constant_value(&p_expression->right->object))
        {
            const long long result = (object_to_signed_long_long(&p_expression->left->object) ||
                                      object_to_signed_long_long(&p_expression->right->object)) ? 1 : 0;
            {
                struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
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
                struct flow3_map* p_dead = flow3_map_arena_new_dead(&ctx->flow3_map_arena, p_before, "or-dead-false");
                return (struct flow3_branch_pair) { ctx->p_current_flow3_map, p_dead };
            }
            else
            {
                struct flow3_map* p_dead = flow3_map_arena_new_dead(&ctx->flow3_map_arena, p_before, "or-dead-true");
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

        /* Seed this OR's per-path boolean value. For each path (identified by
           origin), `L || R` is true if L is true there, else R's value there.
           L was evaluated on p_before; R on left's false map. Only applied
           when both sides are clean per-path booleans -- otherwise the result
           is left unseeded (previous behavior). This lets compile_assert see
           a 0 exactly on a path where neither disjunct holds. */
        {
            const struct flow3_key_alternatives* L =
                flow3_map_search_up(p_before, &p_expression->left->object);
            const struct flow3_key_alternatives* R =
                flow3_map_search_up(left_pair.p_false, &p_expression->right->object);

            struct flow3_alternatives out = { 0 };
            bool ok = (L != NULL && R != NULL);

            for (int i = 0; ok && i < L->alternatives.size; i++)
            {
                const struct flow3_alternative* l = &L->alternatives.data[i];
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
                        const struct flow3_alternative* r = &R->alternatives.data[j];
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
                    ok = false;   /* L not a clean boolean on this path */
                }
            }

            if (ok && out.size > 0)
            {
                struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                flow3_alternatives_clear(&e->alternatives);
                e->alternatives = out;   /* move */
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
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);


        if (object_has_constant_value(&p_expression->left->object) &&
            object_has_constant_value(&p_expression->right->object))
        {
            const long long result = (object_to_signed_long_long(&p_expression->left->object) &&
                                      object_to_signed_long_long(&p_expression->right->object)) ? 1 : 0;
            {
                struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
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
                struct flow3_map* p_dead = flow3_map_arena_new_dead(&ctx->flow3_map_arena, p_before, "and-dead-false");
                return (struct flow3_branch_pair) { ctx->p_current_flow3_map, p_dead };
            }
            else
            {
                struct flow3_map* p_dead = flow3_map_arena_new_dead(&ctx->flow3_map_arena, p_before, "and-dead-true");
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

        /* Seed this AND's per-path boolean value (dual of ||): for each path,
           `L && R` is 0 if L is false there, else R's value there. L was
           evaluated on p_before; R on left's true map. Only when both sides
           are clean per-path booleans; otherwise leave unseeded (previous
           behavior). Lets compile_assert see a 0 where either side fails. */
        {
            const struct flow3_key_alternatives* L =
                flow3_map_search_up(p_before, &p_expression->left->object);
            const struct flow3_key_alternatives* R =
                flow3_map_search_up(left_pair.p_true, &p_expression->right->object);

            struct flow3_alternatives out = { 0 };
            bool ok = (L != NULL && R != NULL);

            for (int i = 0; ok && i < L->alternatives.size; i++)
            {
                const struct flow3_alternative* l = &L->alternatives.data[i];
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
                        const struct flow3_alternative* r = &R->alternatives.data[j];
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
                struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
                flow3_alternatives_clear(&e->alternatives);
                e->alternatives = out;   /* move */
            }
            else
            {
                flow3_alternatives_clear(&out);
            }
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
            {
                struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
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
            struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
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
            {
                struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
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
            struct flow3_key_alternatives* e = flow3_map_find_add(ctx->p_current_flow3_map, &p_expression->object);
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
        cond_pair = flow3_ensure_branch_pair(ctx, ctx->p_current_flow3_map, cond_pair, "cond-true", "cond-false");

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

    /*
       If every path through the first pass's body unconditionally
       transferred control away, ctx->p_current_flow3_map is now marked
       is_dead (see flow3_visit_jump_statement); there is no sound
       "condition after one iteration" state to compute, and running the
       second pass on top of it would just build on dead/dead-code-
       polluted state (see the identical reasoning in
       flow3_visit_while_statement).
    */
    const bool body_falls_through =
        !(ctx->p_current_flow3_map != NULL && ctx->p_current_flow3_map->is_dead);

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
            flow3_map_arena_new(&ctx->flow3_map_arena, p_before, "do-while body (diagnostics only)");
        flow3_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
    }

    struct flow3_map* p_after_body = ctx->p_current_flow3_map;
    struct flow3_branch_pair do_cond_pair2 = { 0 };
    if (body_falls_through && p_iteration_statement->expression1)
    {
        do_cond_pair2 = flow3_visit_full_expression(ctx, p_iteration_statement->expression1);
    }

    flow3_exit_block_visit_defer_list(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);

    struct flow3_map* p_false_branch_dw = do_cond_pair2.p_false ? do_cond_pair2.p_false
        : flow3_map_arena_new(&ctx->flow3_map_arena, p_after_body, "do-while false branch");

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

    struct flow3_branch_pair w_pair1 = flow3_visit_full_expression(ctx, p_iteration_statement->expression1);
    w_pair1 = flow3_ensure_branch_pair(ctx, p_before, w_pair1, "while-true-1", "while-false-1");
    ctx->p_current_flow3_map = w_pair1.p_true;

    flow3_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

    /* Second pass — warnings on */
    diagnostic_stack_pop(&ctx->ctx->options.diagnostic_stack);

    /*
       If every path through the first pass's body unconditionally
       transferred control away (return/break/continue/goto/throw),
       ctx->p_current_flow3_map is now marked is_dead by
       flow3_visit_jump_statement, and there is no sound "after one
       iteration, re-check the condition" state to compute: the second
       pass would just build on top of dead (possibly dead-code-polluted)
       state. Skip the second pass entirely in that case, exactly the
       way merge_arms already skips is_dead arms elsewhere.
    */
    const bool body_falls_through =
        !(ctx->p_current_flow3_map != NULL && ctx->p_current_flow3_map->is_dead);

    struct flow3_branch_pair w_pair2 = { 0 };
    if (body_falls_through)
    {
        w_pair2 = flow3_visit_full_expression(ctx, p_iteration_statement->expression1);
        w_pair2 = flow3_ensure_branch_pair(ctx, ctx->p_current_flow3_map, w_pair2, "while-true-2", "while-false-2");

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
        w_pair1_diag = flow3_ensure_branch_pair(ctx, p_before, w_pair1_diag, "while-true-1b", "while-false-1b");
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

    /* First pass — suppress warnings */
    diagnostic_stack_push_empty(&ctx->ctx->options.diagnostic_stack);

    struct flow3_branch_pair for_pair1 = { 0 };
    if (p_condition)
    {
        for_pair1 = flow3_visit_full_expression(ctx, p_condition);
        for_pair1 = flow3_ensure_branch_pair(ctx, ctx->p_current_flow3_map, for_pair1, "for-true-1", "for-false-1");
        ctx->p_current_flow3_map = for_pair1.p_true;
    }

    flow3_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

    /*
       If every path through the first pass's body unconditionally
       transferred control away, ctx->p_current_flow3_map is now marked
       is_dead (see flow3_visit_jump_statement). The loop's increment
       expression (p_next) and a second pass would then just build on
       top of dead/dead-code-polluted state — skip both, mirroring
       flow3_visit_while_statement / flow3_visit_do_while_statement.
    */
    const bool body_falls_through =
        !(ctx->p_current_flow3_map != NULL && ctx->p_current_flow3_map->is_dead);

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
        for_pair2 = flow3_ensure_branch_pair(ctx, ctx->p_current_flow3_map, for_pair2, "for-true-2", "for-false-2");
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
            flow3_map_arena_new(&ctx->flow3_map_arena, p_before, "for body (diagnostics only)");
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
        for (const struct object* member = p_obj->members.head; member; member = member->next)
        {
            flow3_check_clear_object_is_zero_at_exit(ctx, member, param_name, marker, p_exit_token);
        }
        return;
    }

    const struct flow3_key_alternatives* e = flow3_map_search_up(ctx->p_current_flow3_map, p_obj);
    if (e == NULL || e->alternatives.size == 0)
    {
        /* Never touched at all -- the _Clear contract requires the
           callee to actively zero every member, so no evidence of a
           write is exactly as much a violation as a tracked non-zero
           value would be (unlike, say, an untouched local, there is no
           "inherits whatever it already was" story for a member the
           analysis has no information about here). */
        const char* member_name = p_obj->member_designator ? p_obj->member_designator : "";
        if (diagnostic(W_FLOW_CLEAR_NOT_ZERO_AT_EXIT,
            ctx->ctx,
            NULL,
            marker,
            "_Clear parameter '%s' pointee (%s) is never set to zero",
            param_name,
            member_name))
        {
            diagnostic(W_INFO, ctx->ctx, p_exit_token, NULL, "exit point");
        }
        return;
    }

    for (int i = 0; i < e->alternatives.size; i++)
    {
        const struct flow3_alternative* p_alternative = &e->alternatives.data[i];

        if (p_alternative->imaginary == FLOW3_IMAGINARY_ABSENT)
        {
            continue;
        }

        if (!flow3_alternative_is_zero(p_alternative))
        {
            const char* member_name = p_obj->member_designator ? p_obj->member_designator : "";
            if (diagnostic(W_FLOW_CLEAR_NOT_ZERO_AT_EXIT,
                ctx->ctx,
                NULL,
                marker,
                "_Clear parameter '%s' pointee (%s) is not zero at exit (see line %d)",
                param_name,
                member_name,
                p_alternative->line))
            {
                diagnostic(W_INFO, ctx->ctx, p_exit_token, NULL, "exit point");
            }
        }
    }
}

static void flow3_check_clear_params_at_exit(struct flow3_visit_ctx* ctx, const struct marker* marker, const struct token* p_exit_token)
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

        if (!type_is_pointer(p_param_type) || !type_is_pointed_clear(p_param_type))
        {
            continue;
        }

        /* Find the concrete pointee: flow3_parameter_object_init wired
           this parameter's own alternative to an arena object at
           function entry (see the "non-optional pointer" case there). */
        const struct flow3_key_alternatives* e =
            flow3_map_search_up(ctx->p_current_flow3_map, &p_param->declarator->object);

        if (e == NULL)
        {
            continue;
        }

        const char* param_name = p_param->declarator->name_opt ? p_param->declarator->name_opt->lexeme : "";

        for (int i = 0; i < e->alternatives.size; i++)
        {
            const struct flow3_alternative* p_alternative = &e->alternatives.data[i];

            if (p_alternative->value_kind == FLOW3_VALUE_KIND_PTR && p_alternative->value.p != NULL)
            {
                flow3_check_clear_object_is_zero_at_exit(ctx, p_alternative->value.p, param_name, marker, p_exit_token);
            }
        }
    }
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
    flow3_check_clear_params_at_exit(ctx, &marker, p_jump_statement->first_token);
}

static void flow3_visit_jump_statement(struct flow3_visit_ctx* ctx, struct jump_statement* p_jump_statement)
{
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
               (used by if/try/while/do/for) already skips is_dead arms
               regardless of the syntax-based "ends with jump" check the
               caller used to select which arms to pass in.
            */
            if (ctx->p_current_flow3_map != NULL)
            {
                ctx->p_current_flow3_map->is_dead = true;
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
                flow3_check_object_init_assigment(ctx, p_jump_statement->expression_opt, &param_object, &p_jump_statement->expression_opt->object, INIT_RETURN);
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
                ctx->p_current_flow3_map->is_dead = true;
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
                ctx->p_current_flow3_map->is_dead = true;
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
                ctx->p_current_flow3_map->is_dead = true;
            }
        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_GOTO)
        {
            assert(p_jump_statement->label != NULL);

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
                struct flow3_map* p_label_map =
                    flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_current_flow3_map, p_jump_statement->label->lexeme);
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
                ctx->p_current_flow3_map->is_dead = true;
            }
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
                flow3_map_arena_new(&ctx->flow3_map_arena, ctx->p_initial_map, "default");
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

            {
                struct flow3_key_alternatives* e = flow3_map_find_add(p_case_map, ctx->p_switch_obj_key);
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
       marked is_dead by flow3_visit_jump_statement right when an
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
            ctx->p_current_flow3_map->is_dead)
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
            !ctx->p_current_flow3_map->is_dead)
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
        if (alt->value_kind == FLOW3_VALUE_KIND_REF)
        {
            check_object_true(ctx, alt->value.p, p_position_token);
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

        if (p_alternative->value_kind == FLOW3_VALUE_KIND_REF)
        {
            //IS THIS POSSIBLE?
            if (p_alternative->value.p != p_obj)
            {
                flow3_check_object_at_exit(ctx,
                                            p_type,
                                            p_alternative->value.p,
                                            marker,
                                            p_exit_token);
                continue;
            }
        }

        if (p_alternative->imaginary == FLOW3_IMAGINARY_ABSENT)
            continue;

        if (is_owner &&
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
            if (flow3_alternative_is_zero(p_alternative))
            {

            }
            else
            {
                const char* object_name = p_obj->member_designator ? p_obj->member_designator : "";
                const char* origin_name =
                    (p_alternative->origin && p_alternative->origin->name) ? p_alternative->origin->name : "";
                if (diagnostic(W_FLOW_LIFETIME_ENDED,
                    ctx->ctx,
                    NULL,
                    marker,
                    "owner object (%s) not moved (%s see line %d)",
                    object_name,
                    origin_name,
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
static void flow3_check_write_qualifier_parameters(struct flow3_visit_ctx* ctx, struct declarator* p_declarator)
{
    const struct param_list* _Opt p_param_list = type_get_func_or_func_ptr_params(&p_declarator->type);
    if (p_param_list == NULL)
    {
        return;
    }

    struct token* _Opt p_token = p_declarator->first_token_opt ? p_declarator->first_token_opt : p_declarator->name_opt;

    for (struct param* _Opt p_current_parameter_type = p_param_list->head;
         p_current_parameter_type;
         p_current_parameter_type = p_current_parameter_type->next)
    {
        const struct type* p_param_type = &p_current_parameter_type->type;

        if (!type_is_pointer(p_param_type))
        {
            /* _Clear/_Dtor/_Ctor applied directly (no pointer to be the
               "pointed object") -- these flags land straight on the
               parameter's own type in that case. */
            if (type_is_clear(p_param_type))
            {
                diagnostic(C_ERROR_FLOW_WRITE_QUALIFIER_MUST_QUALIFY_POINTEE, ctx->ctx, p_token, NULL,
                    "_Clear must be used only at the pointed object");
            }
            else if (type_is_dtor(p_param_type))
            {
                diagnostic(C_ERROR_FLOW_WRITE_QUALIFIER_MUST_QUALIFY_POINTEE, ctx->ctx, p_token, NULL,
                    "_Dtor must be used only at the pointed object");
            }
            else if (type_is_ctor(p_param_type))
            {
                diagnostic(C_ERROR_FLOW_WRITE_QUALIFIER_MUST_QUALIFY_POINTEE, ctx->ctx, p_token, NULL,
                    "_Ctor must be used only at the pointed object");
            }
            continue;
        }

        if (!type_is_pointed_const(p_param_type))
        {
            continue;
        }

        if (type_is_pointed_clear(p_param_type))
        {
            diagnostic(C_ERROR_FLOW_WRITE_QUALIFIER_CANNOT_BE_CONST, ctx->ctx, p_token, NULL,
                "_Clear pointee cannot also be const");
        }
        else if (type_is_pointed_dtor(p_param_type))
        {
            diagnostic(C_ERROR_FLOW_WRITE_QUALIFIER_CANNOT_BE_CONST, ctx->ctx, p_token, NULL,
                "_Dtor pointee cannot also be const");
        }
        else if (type_is_pointed_ctor(p_param_type))
        {
            diagnostic(C_ERROR_FLOW_WRITE_QUALIFIER_CANNOT_BE_CONST, ctx->ctx, p_token, NULL,
                "_Ctor pointee cannot also be const");
        }
    }
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
        flow3_check_write_qualifier_parameters(ctx, p_declaration->init_declarator_list.head->p_declarator);
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
            /* Falling off the end of the function is an exit point too --
               a _Clear parameter's contract must hold here just as much
               as at an explicit return (ctx->p_current_function_declaration
               is still set at this point, so this must run before it's
               cleared below). */
            flow3_check_clear_params_at_exit(ctx, &marker, p_declaration->function_body->last_token);
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

void flow3_start_visit_declaration(struct flow3_visit_ctx* ctx, struct declaration* p_declaration)
{
    ctx->labels_size = 0;
    flow3_predicate_cache_reset(ctx);

    flow3_allocated_object_arena_clear(&ctx->allocated_object_arena);
    flow3_map_arena_clear(&ctx->flow3_map_arena);
    ctx->p_current_flow3_map = flow3_map_arena_new(&ctx->flow3_map_arena, NULL, "root");

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
}