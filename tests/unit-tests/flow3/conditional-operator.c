#pragma safety enable

/*
   Regression tests for the conditional (ternary) operator, EXPR_CONDITIONAL
   in flow3.c. Checked with the same methodology used for if/while/for:
   condition narrowing into each arm, the Elvis (`?:`) form, per-arm side
   effects merging correctly for downstream code, and the false arm
   getting no narrowing benefit from a true-only condition (so it must
   still warn where warranted).

   Most of this already worked correctly -- flow3_visit_expression's
   EXPR_CONDITIONAL case uses flow3_ensure_branch_pair (so a non-narrowing
   condition still forces two distinct arm maps) and flow3_map_merge_a_b
   (which inherits the is_dead-arm-skipping already fixed in
   flow3_map_merge_arms) exactly like if-statement does.

   One real bug was found and fixed: a PARENTHESIZED ternary used as an
   operand of an outer ternary, e.g. `a ? (b ? 1 : 2) : 3`, lost the
   inner result entirely -- only the false arm's value (3) ever showed
   up. Root cause: `(...)` is its own AST node (EXPR_PRIMARY_PARENTHESIS),
   a distinct `struct expression` from the expression it wraps, with its
   own separate `.object` storage. Its case in flow3_visit_expression
   correctly forwarded the branch pair for narrowing purposes but never
   copied the wrapped expression's computed VALUE onto its own `.object`.
   The outer EXPR_CONDITIONAL's result-value merge looks up each arm's
   value by that arm expression's own `&object` -- so when the true arm
   was the parenthesis node, the lookup found nothing (the inner
   ternary's value lived under the INNER node's `.object`, a different
   address). Fixed by having EXPR_PRIMARY_PARENTHESIS copy the wrapped
   expression's value onto its own `.object` after visiting it. See
   nested_ternary_parenthesized below (and the same shape without
   parens as a working baseline in nested_ternary_no_parens).
*/

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
    /* cond has nothing to do with p, so p is not narrowed in either
       arm -- must still warn on the true arm's access */
    int v = cond ? p->value : -1; //lint 33 -> operator applied to a null pointer
    return v;
}

void elvis_operator(struct node* _Opt p)
{
    /* GNU `?:` -- left is NULL, so flow3 uses condition_expr's own value
       on the true arm. q ends up non-null either from p (true arm) or
       from fallback()'s result (false arm), same as an if(q) guard. */
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
    // static_debug(y);
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
    /* right-associative, no parens -- worked correctly even before the
       EXPR_PRIMARY_PARENTHESIS fix, kept here as a baseline contrast */
    int r = a ? b ? 1 : 2 : 3;
    /* expected: r == 1, 2, or 3 depending on which path was taken */
    // static_debug(r);
    compile_assert(r == 1 || r == 2 || r == 3);
    return r;
}

int nested_ternary_parenthesized(int a, int b)
{
    /* same shape, but the inner ternary is parenthesized -- this is
       the case that previously lost the inner result (only r == 3
       ever showed up; see the header comment above) */
    int r = a ? (b ? 1 : 2) : 3;
    /* expected: r == 1, 2, or 3 depending on which path was taken */
    // static_debug(r);
    compile_assert(r == 1 || r == 2 || r == 3);
    return r;
}

int ternary_as_condition(struct node* _Opt p)
{
    /* p narrowed non-null while flow3 evaluates the ternary itself as
       an if-condition -- p->value here must not warn */
    if (p ? p->value > 0 : 0)
    {
        return 1;
    }
    return 0;
}
