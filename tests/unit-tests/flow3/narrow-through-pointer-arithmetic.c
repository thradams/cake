#pragma safety enable

/*
   A null check on a struct member must keep narrowing that member when the
   value is then used in POINTER ARITHMETIC -- including after the containing
   struct has been passed to a function.

   Passing `st` to reserve() invalidates the tracked state of st->c_str, which
   is correct. The `st->c_str == 0` check afterwards re-establishes it, so the
   `st->c_str + st->size` that follows is a non-null pointer.

   The three functions below isolate the ingredients: only the last one needs
   both the invalidating call AND the arithmetic. Regression for osstream.c
   (ss_vafprintf / ss_vasprintf) and compile.c:565, where correct, guarded
   code reported "may be null, but the destination does not allow null".
*/

struct osstream
{
    char* _Opt c_str;
    int size;
};

int reserve(struct osstream* st, int n);
int take(char* p);

/* No invalidating call before the guard. */
int arithmetic_only(struct osstream* st)
{
    if (st->c_str == 0)
    {
        return -1;
    }

    return take(st->c_str + st->size);
}

/* Invalidating call before the guard, but no arithmetic in the use. */
int call_only(struct osstream* st)
{
    reserve(st, 1);

    if (st->c_str == 0)
    {
        return -1;
    }

    return take(st->c_str);
}

/* Both: the guard must still narrow through the arithmetic. */
int call_then_arithmetic(struct osstream* st)
{
    reserve(st, 1);

    if (st->c_str == 0)
    {
        return -1;
    }

    return take(st->c_str + st->size);
}

/* The same shape guarded inside a single expression with &&, as compile.c
   writes it. */
int guarded_in_same_expression(struct osstream* st, int n)
{
    reserve(st, 1);

    if (st->c_str && take(st->c_str + n) != 0)
    {
        return 1;
    }

    return 0;
}

/* Contrast: with no check at all the warning is still expected. */
int unguarded(struct osstream* st)
{
    return take(st->c_str + st->size); //lint 35
}
