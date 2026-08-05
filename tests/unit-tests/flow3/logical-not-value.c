#pragma safety enable

/*
   The logical-NOT operator `!x` yields a boolean (0 or 1) and is always
   INITIALIZED. flow3 only seeded the result when the operand was a compile-time
   constant; for a non-constant operand it left the result with no value, so
   `bool c = !x;` reported c as "possibly uninitialized" when c was passed on.

   (Reproduced from parser.c: `const bool color_enabled = !ctx->options.color_disabled;`)
*/

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
