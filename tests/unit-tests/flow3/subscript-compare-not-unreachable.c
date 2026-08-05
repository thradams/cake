#pragma safety enable

/*
   Comparing an array/pointer subscript of scalar element type against a
   constant must NOT fold to a constant condition.

   flow3 left the result object of an unresolved `v[i]` subscript EMPTY (no
   value alternatives). The equality evaluator treats an empty operand as
   "holds vacuously", so `s->current[0] == '\n'` folded to always-true and the
   else branch was reported as unreachable code. `*p` (dereference) already
   seeded an ANY value; subscript did not.

   Fix: seed an unresolved integer-element subscript as an ANY value of its
   element type, so the comparison is unknown and both branches stay live.

   (Reproduced from tokenizer.c stream_match: `if (s->current[0] == '\n')`.)
*/

struct stream { const char* current; int line; int col; };

void stream_step(struct stream* s)
{
    if (s->current[0] == '\n')
    {
        s->line++;
        s->col = 1;
    }
    else
    {
        s->col++;   /* reachable -- must NOT warn "unreachable code" */
    }
}

/* Also fine for a plain char-array index compared to a constant. */
int has_marker(const char buf[8])
{
    if (buf[2] == 'x')
        return 1;   /* reachable */
    else
        return 0;   /* reachable -- must NOT warn "unreachable code" */
}
