#pragma safety enable

/* `st->c_str == 0` after passing st to a function re-narrows the member, so `st->c_str + st->size` is non-null (osstream.c, compile.c) */

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

/* same shape guarded with && in one expression, as compile.c writes it */
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
