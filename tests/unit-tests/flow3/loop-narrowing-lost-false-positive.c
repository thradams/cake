#pragma safety enable

/* FIXED: `if (l->head == 0)` narrowing survives the loop back-edge (no 26); the remaining 72 is correct, p_item cannot be moved twice (parser.c balanced_token_sequence_opt) */

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
