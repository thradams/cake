#pragma safety enable

/*
   FIXED: narrowing of a struct member used to be discarded at the loop
   back-edge.

   `if (l->head == 0)` proves the member is null inside the branch, and a
   null member owns nothing -- so assigning to it cannot discard a resource.
   flow3 used that correctly in straight-line code (ok_no_loop) but dropped
   it once the identical body sat inside a loop, and reported warning 26.

   The cause was the second (diagnostic) pass of flow3_visit_for_statement
   starting from the first pass's exit state instead of the union over
   iterations; see loop-second-pass-narrows-first-iteration.c. warning 26 is
   gone.

   What bug_in_loop reports now -- warning 72 on l->head -- is CORRECT, and
   was previously masked by the false 26. p_item is moved into l->head on the
   first iteration; an owner cannot be moved twice, so on any later iteration
   `l->head = p_item` stores an already-consumed owner and the caller is left
   with l->head consumed.

   Reduced from src/parser.c balanced_token_sequence_opt, appending to the
   token list under `if (p_balanced_token_sequence->tail == NULL)`.
*/

struct item { int v; };

struct list { struct item* _Owner _Opt head; };

void ok_no_loop(struct list* l, struct item* _Owner p_item)
{
    if (l->head == 0)
        l->head = p_item; /* correct: no warning 26 */
} //lint 29 p_item is not moved when the branch is not taken

void bug_in_loop(struct list* l, struct item* _Owner p_item)
{
    for (;;)
    {
        if (l->head == 0)
            l->head = p_item; //lint 32 (its true, moved twice..)
    }
} //lint 29 72 p_item is not moved on every path; l->head left consumed (correct)
