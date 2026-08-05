#pragma safety enable

/*
   KNOWN BUG, root cause identified, not yet fixed. Traced from
   expressions.c's unary_expression(), the `is_lvalue` / `is_const` / ...
   traits handling:

       struct type* _Opt p_type = NULL;
       if (first_of_type_name_ahead(ctx))
           p_type = &new_expression->type_name->abstract_declarator->type;
       else
           p_type = &new_expression->right->type;
       runtime_assert(p_type != NULL);
       switch (traits_token->type) { ... uses p_type ... }  // false "possibly null"

   Root cause: `obj = calloc(...)` is an IMPLICIT void*->T* conversion (no
   EXPR_CAST node), so the fabricated pointee behind `obj` stays typed
   `void`. `object_get_member(void_obj, ...)` then always returns NULL,
   so `obj->a` (the `->` access) falls back to seeding an untyped ANY value
   instead of a REF to the member -- which means `&obj->a` (EXPR_UNARY_
   ADDRESSOF only recognizes REF inputs) produces NO alternatives at all,
   and `p = &obj->a;` copies that emptiness into `p`.

   A single branch with an empty/untracked `p` silently produces no
   warning (see one_branch_ok() below) -- but flow3_map_merge_arms(),
   merging the true and false arms of the `if`, treats "this arm's entry
   for p is empty" as "this arm never wrote p at all" and falls back to
   p's value from BEFORE the `if` (the `NULL` from its declaration) --
   producing the false "possibly null" warning.

   FIXED. The fix is the one previously tried and reverted: extend flow3.c's
   `->` on-demand pointee fabrication (which already re-fabricates when there
   is no pointee at all) to also re-fabricate when the existing pointee is
   void-typed but the base pointer's own static type isn't.

   It was reverted the first time because on cake's own sources it unlocked
   member resolution on many other void-fabricated pointees and inflated the
   warning count by ~350, mostly cascades that could not be audited in the
   time available. Those cascades were largely artefacts of separate defects
   that have since been fixed -- the ENDED origin filter on `->`, report-once
   guards on the assignment/return path and at scope exit, and aggregation of
   per-member "moved"/"uninitialized" reports -- plus an arena budget too small
   to keep pointees alive (FLOW3_ALLOCATED_OBJECT_ARENA_MAX_SIZE). With those
   in place the fix is readable and three samples that had been annotated for
   false positives now pass clean: this one, member-narrowing-after-output-param.c
   and deref-after-catch-reset-false-positive.c, each of which was documenting a
   guard that could not narrow because the member had no real object behind it.
*/

#define NULL ((void*)0)


void* _Owner _Opt _Clear calloc(int n, int s);
void free(void* _Owner _Opt p);

struct s { int a; int b; };

void use(int* p);

/* A single branch is fine on its own. */
void one_branch_ok(void)
{
    struct s* _Owner _Opt obj = calloc(1, sizeof * obj);
    if (obj == NULL) return;

    int* _Opt p = &obj->a;

    runtime_assert(p != NULL);
    use(p); /* ok: no warning */

    free(obj);
}

/* Merging the true and false branches -- each assigning `p` to a
   different member of the SAME _Owner object -- must not lose the
   non-null fact. */
void merged_branches_bug(int cond)
{
    struct s* _Owner _Opt obj = calloc(1, sizeof * obj);
    if (obj == NULL) return;

    int* _Opt p = NULL;

    if (cond)
        p = &obj->a;
    else
        p = &obj->b;

    runtime_assert(p != NULL);

    /* FIXED: no longer warns. obj->a and obj->b now resolve to real member
       objects, so taking their address produces a REF alternative and the
       branch merge keeps it instead of falling back to p's pre-if NULL. */
    use(p);

    free(obj);
}
