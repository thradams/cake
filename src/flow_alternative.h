/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma once
#include "token.h"

enum
{
    ANY_VALUE = 0xCAFE,
    UNINITIALIZED_VALUE = 0xBAD,
    FLOW_BRANCH_INITIAL_BUCKETS = 8,
    FLOW_BRANCH_LOAD_FACTOR_PERCENT = 75,
};

enum flow_relation
{
    FLOW_RELATION_UNINITIALIZED = 0, /*  indeterminate representation */
    FLOW_RELATION_EQUAL,             /* == ref_value */
    FLOW_RELATION_NOT_EQUAL,         /* != ref_value */
    FLOW_RELATION_ANY,               /* unspecified (TYPE_MIN <= value <= TYPE_MAX) */
    FLOW_RELATION_GREATER,           /* > ref_value */
    FLOW_RELATION_GREATER_EQUAL,     /* >= ref_value */
    FLOW_RELATION_LESS,              /* < ref_value */
    FLOW_RELATION_LESS_EQUAL,        /* <= ref_value */
};

enum flow_value_kind
{
    FLOW_VALUE_KIND_SIGNED = 0, /* .i */
    FLOW_VALUE_KIND_UNSIGNED,   /* .u  */
    FLOW_VALUE_KIND_PTR,        /* .p  means pointer */
    FLOW_VALUE_KIND_REF,        /* .p  means reference - non null */
};

enum flow_imaginary
{
    FLOW_IMAGINARY_NONE = 0, /* none */
    FLOW_IMAGINARY_MOVED,    /* ownership moved */
    FLOW_IMAGINARY_ENDED     /* object's lifetime ended */
    /* see also FLOW_RELATION_UNINITIALIZED */
};

struct flow_alternative
{
    union
    {
        unsigned long long u;
        signed long long i;
        const struct object* _Opt p;
    } value;

    enum flow_value_kind value_kind;             /* tag of the union */
    enum flow_relation value_relation;           /* relation with the value*/
    enum flow_imaginary imaginary;               /* imaginary part: MOVED, ENDED, ABSENT, NONE */

    const struct flow_branch* _Opt p_origin_map;    /* which flow map arm set this value; */

    /* For a value kept by narrowing a condition: the origin of the value it was
       narrowed from (narrowing re-tags p_origin_map with the arm). Like
       p_origin_map, the value only exists on paths through it. NULL otherwise. */
    const struct flow_branch* _Opt p_narrowed_from;

    /* The narrowing contradicted the value it came from (`!= 0` reaching the
       false arm of a test): no path gets here with it. It is kept only so the
       entry is not empty; code that asks which paths a value exists on skips it. */
    bool contradicted;
    const struct token* _Opt p_origin_token;     /* Where this state was established. */
};

bool flow_value_is_same(const struct flow_alternative* a, const struct flow_alternative* b);
int flow_alternative_line(const struct flow_alternative* a);
bool flow_alternative_is_false(const struct flow_alternative* alt);
bool flow_alternative_is_true(const struct flow_alternative* alt);
bool flow_alternative_is_zero(const struct flow_alternative* alt);
bool flow_value_is_false(const struct flow_alternative* a);
bool flow_value_is_true(const struct flow_alternative* a);
bool flow_alternative_is_dead(const struct flow_alternative* alt);
void flow_alternative_sprint(struct osstream* ss, const struct flow_alternative* alt);
void flow_alternative_print(const struct flow_alternative* alt);

bool flow_alt_to_interval(const struct flow_alternative* alt, long long* lo, long long* hi);
bool flow_alt_set_interval(struct flow_alternative* alt, long long lo, long long hi);
bool flow_alt_negate(const struct flow_alternative* in, struct flow_alternative* out);
bool flow_alternative_can_be_zero(const struct flow_alternative* alt);

void print_object_ptr(struct osstream* ss, const struct object* _Opt p, bool is_parent);

struct flow_alternatives
{
    struct flow_alternative* _Owner _Opt* _Owner _Opt data;
    int size;
    int capacity;
};

void flow_alternatives_clear(_Clear struct flow_alternatives* vs);
void flow_alternatives_append(struct flow_alternatives* dst, const struct flow_alternatives* src);
void flow_alternatives_add(struct flow_alternatives* vs, const struct flow_alternative* p_alternative);
void flow_alternatives_print(const struct flow_alternatives* alternatives);

void flow_alternatives_pool_shutdown(void);

