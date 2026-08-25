#pragma safety enable

/*
   Same root cause as loop-second-pass-narrows-first-iteration.c (the loop
   body is visited twice, and the diagnostic pass starts from the FIRST
   pass's exit state instead of the union over iterations) but manifesting
   AFTER the loop instead of inside it.

   `found` starts NULL. The loop conditionally assigns it once. On the
   second (diagnostic) pass `found` already holds the first pass's
   post-iteration value (non-null), so by the time flow3 reaches the code
   after the loop it reports `found` as always non-null -- warning 28 -- even
   though the loop can run zero iterations (head == NULL) or never take the
   `found == 0` branch inside a single iteration in a way that leaves it
   unset for the caller's purposes. The reported state has to be the union
   of "loop ran zero times" / "loop ran and never matched" with "loop ran and
   matched", not just the second pass's state.

   Reduced from src/tokenizer.c control_line's `#line` handling: p_filename
   is set at most once while scanning the directive's tokens, the following
   `if (p_filename != NULL)` gets reported as unreachable/redundant
   (warning 28, and inside it a spurious warning 33 null-dereference on the
   branch flow3 thinks can't be taken), and the dereference it guards is a
   real bug when the guard is trusted instead.

   Current state: FIXED -- `found` is no longer folded after the loop.

   Two things to know when reading the history above. Warning 28
   (W_FLOW_NON_NULL) is no longer emitted anywhere in the compiler, so the
   symptom as originally written cannot reproduce either way; the probe that
   replaces it is warning 85 ("condition is always false"), which is what
   would fire today if the loop-exit state still claimed `found` non-null.
   Neither fires, and the body below is not reported unreachable, which is
   what this file now locks in.

   Tried: generalizing flow_widen_loop_variant_objects (currently numeric
   values only) to also widen a pointer that names a different concrete
   identity after pass 1 vs pass 2 -- same treatment as a moving counter.
   This DOES fix this file (confirmed by rebuilding cake and rerunning), but
   it reproduces the exact regression loop-second-pass-narrows-while-and-do.c
   already warns about for pointers: run over src/tokenizer.c itself, it adds
   4 false "lifetime ended while still owning its resource" (warning 29) on
   the ordinary free-and-advance owner idiom

       struct include_dir* _Owner _Opt p = list->head;
       while (p) {
           struct include_dir* _Owner _Opt next = p->next;
           free(p);
           p = next;
       }

   Widening treats `p`'s changing identity across passes as just another
   variant to erase to ANY, which throws away the ownership-lifetime tracking
   that idiom depends on. Reverted rather than trade one false positive for a
   worse, more common one. A real fix needs to widen pointer NULLABILITY
   (null vs non-null) without erasing ownership/lifetime facts about the
   pointee -- i.e. it needs the same "join only the narrowing state, leave
   owner/lifetime facts out" split already called for in
   loop-second-pass-narrows-while-and-do.c, not a blanket identity-widen.
*/

struct node
{
    int value;
    struct node* _Opt next;
};

void after_loop_wrongly_non_null(struct node* _Opt head)
{
    struct node* _Opt found = 0;

    for (struct node* _Opt p = head; p; p = p->next)
    {
        if (found == 0)
        {
            found = p;
        }
    }

    if (found == 0) /* reachable: head could be NULL, or every node already matched */
    {
        int reached = 1; /* reported "unreachable code" if the test folds */
        (void)reached;
    }
}
