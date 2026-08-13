#pragma safety enable

/*
   Narrowing of a struct member is discarded at the loop back-edge.

   `if (l->head == 0)` proves the member is null inside the branch, and a
   null member owns nothing -- so assigning to it cannot discard a resource.
   flow3 uses that correctly in straight-line code (ok_no_loop below emits
   no warning 26), but when the identical body is placed inside a loop the
   fact is dropped when the back-edge is merged and warning 26 is reported.

   The only difference between the two functions is the enclosing loop.
   Writing the narrowing as _Assert(l->head == 0) instead of `if` does not
   help: the assert is honoured in straight-line code and lost in a loop in
   exactly the same way.

   Reduced from src/parser.c balanced_token_sequence_opt, appending to the
   token list under `if (p_balanced_token_sequence->tail == NULL)`; adding
   `_Assert(p_balanced_token_sequence->head == NULL)` did not silence it.
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
            l->head = p_item; //lint 26 32 TODO FALSE WARNING (26)
    }
} //lint 29 p_item is not moved on every path
