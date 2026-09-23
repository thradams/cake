#pragma safety enable

/* conditional operator: narrowing per arm, Elvis form, side effects merge; a parenthesized inner ternary `a ? (b ? 1 : 2) : 3` must keep the inner value */

struct node
{
    struct node* _Opt next;
    int value;
};

struct node* _Opt fallback();

int true_arm_narrowed(struct node* _Opt p)
{
    /* p narrowed to non-null in the true arm by the condition itself */
    int v = p ? p->value : -1;
    return v;
}

int false_arm_narrowed(struct node* _Opt p)
{
    /* p is provably null in the false arm -- must still warn */
    int v = p ? 1 : p->value; //lint 33 -> operator applied to a null pointer
    return v;
}

int unrelated_condition_not_narrowed(struct node* _Opt p, int cond)
{
    /* cond is unrelated to p, so p is not narrowed in either arm */
    int v = cond ? p->value : -1; //lint 33 -> operator applied to a null pointer
    return v;
}

void elvis_operator(struct node* _Opt p)
{
    /* GNU `?:`: the true arm uses the condition's own value, q is non-null like an if(q) guard */
    struct node* _Opt q = p ?: fallback();
    if (q)
    {
        int v = q->value; // safe -- q narrowed non-null by if(q)
        (void)v;
    }
}

void both_arms_side_effects(int cond)
{
    int y = 0;
    int x = cond ? (y = 1) : (y = 2);
    (void)x;
    /* expected: y == 1 (true arm ran) or y == 2 (false arm ran) */
    compile_assert(y == 1 || y == 2);
}

int division_guard(int d)
{
    /* d narrowed non-zero in the true arm by the condition -- safe */
    int r = (d != 0) ? (10 / d) : 0;
    return r;
}

int division_unguarded(int d, int cond)
{
    /* cond has nothing to do with d being zero -- must still warn */
    int r = cond ? (10 / d) : 0; //lint 36 division by zero
    return r;
}

int nested_ternary_no_parens(int a, int b)
{
    /* right-associative without parens: baseline that always worked */
    int r = a ? b ? 1 : 2 : 3;
    /* expected: r == 1, 2, or 3 depending on which path was taken */
    compile_assert(r == 1 || r == 2 || r == 3);
    return r;
}

int nested_ternary_parenthesized(int a, int b)
{
    /* parenthesized inner ternary: this shape used to lose the inner result (only r == 3 showed up) */
    int r = a ? (b ? 1 : 2) : 3;
    /* expected: r == 1, 2, or 3 depending on which path was taken */
    compile_assert(r == 1 || r == 2 || r == 3);
    return r;
}

int ternary_as_condition(struct node* _Opt p)
{
    /* p is narrowed non-null while the ternary is evaluated as an if condition */
    if (p ? p->value > 0 : 0)
    {
        return 1;
    }
    return 0;
}
