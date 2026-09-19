#pragma safety enable

/* `!x` is always an initialized 0 or 1, even for a non-constant operand (parser.c color_enabled) */

struct options { int color_disabled; int flag; };

void use(int a, int b);

/* bool from a negated member: c is initialized. */
void from_not(struct options* o)
{
    const bool c = !o->color_disabled;
    use(o->flag, c);              /* clean */
}

/* int result of ! is a boolean too. */
void int_not(struct options* o)
{
    const int c = !o->color_disabled;
    use(o->flag, c);              /* clean */
}

/* Narrowing through `!` still works. */
void narrow(int* _Opt p)
{
    if (!(p == 0))
        *p = 1;                  /* ok: p narrowed non-null */
}
