#pragma safety enable

/* `bool e = (a || b);` is initialized even when the operator leaves its result unseeded: the assignment records ANY instead of returning early (tokenizer.c:4830) */

#define NULL ((void*)0)

struct T { int type; };
struct L { struct T* _Opt head; };

/* The reduced tokenizer.c shape: right operand's value depends on the path. */
_Bool or_is_boolean(struct L r4)
{
    const _Bool e = (r4.head == NULL || r4.head->type == 1);
    return e;
}

/* Same for &&. */
_Bool and_is_boolean(struct L r4)
{
    const _Bool e = (r4.head != NULL && r4.head->type == 1);
    return e;
}

/* control 1: a single comparison is never left unseeded */
_Bool single_comparison_ok(struct L r4)
{
    const _Bool e = (r4.head == NULL);
    return e;
}

/* control 2: if/else assigning concrete values */
_Bool spelled_out_ok(struct L r4)
{
    _Bool e;
    if (r4.head == NULL)
        e = 1;
    else
        e = (r4.head->type == 1);
    return e;
}
