#pragma safety enable

/*
   `*p` (EXPR_UNARY_CONTENT) never checked for a lifetime-ended pointee --
   only `p->member` (EXPR_POSTFIX_ARROW) did. So after an _Owner pointer
   parameter's pointee was consumed by a callee that takes ownership,
   `*p = ...` went completely unchecked, even though `p->member = ...`
   on the same pointee was already caught.

   User-reported repro:

       void consume(int *_Owner p);
       void test(int *_Owner x) {
           int *y = x;
           int *z = y;
           consume(x);
           *y = 0;    // missing: lifetime of *y ended
       }

   Fixed by adding the same flow3_object_leaves_in_state_2(FLOW3_LEAF_ENDED)
   check EXPR_POSTFIX_ARROW already does, to EXPR_UNARY_CONTENT's REF
   resolution loop -- checking the whole pointee (there is no member index
   for `*p`) instead of one member.

   Note: a SECOND-level alias (z, an alias of y rather than of x directly)
   is not yet caught -- the fix only follows one REF hop, matching the
   user's repro, which declares z but never dereferences it. See z below.
*/

void consume(int* _Owner p);

/* Direct case: no aliasing, dereference the same pointer that was consumed. */
void direct(int* _Owner x)
{
    consume(x);
    *x = 0; //lint 31 dereference of '*x': pointed object lifetime has ended
}

/* Aliased case, exactly as reported: y aliases x directly; z aliases y (not
   dereferenced, same as the original repro -- see the note above). */
void aliased(int* _Owner x)
{
    int* y = x;
    int* z = y;
    consume(x);
    *y = 0; //lint 31 dereference of '*y': pointed object lifetime has ended
}

/* Contrast: consumed through a DIFFERENT, unrelated pointer -- must not be
   flagged. */
void unrelated_pointer_not_flagged(int* _Owner x, int* _Owner other)
{
    consume(other);
    *x = 0;
    consume(x);
}
