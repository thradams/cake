#pragma safety enable

/* `*p` must check for an ended pointee like `p->member` does after an _Owner was consumed; a second-level alias (z) is not followed yet */

void consume(int* _Owner p);

/* Direct case: no aliasing, dereference the same pointer that was consumed. */
void direct(int* _Owner x)
{
    consume(x);
    *x = 0; //lint 31 '*x': pointed object lifetime has ended
}

/* aliased case as reported: y aliases x, z aliases y (not dereferenced) */
void aliased(int* _Owner x)
{
    int* y = x;
    int* z = y;
    consume(x);
    *y = 0; //lint 31 '*y': pointed object lifetime has ended
}

/* contrast: consumed through an unrelated pointer, must not be flagged */
void unrelated_pointer_not_flagged(int* _Owner x, int* _Owner other)
{
    consume(other);
    *x = 0;
    consume(x);
}
