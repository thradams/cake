#pragma safety enable

/* an unresolved `v[i]` of scalar type is seeded ANY, so `s->current[0] == '\n'` does not fold and the else branch is reachable (tokenizer.c stream_match) */

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
