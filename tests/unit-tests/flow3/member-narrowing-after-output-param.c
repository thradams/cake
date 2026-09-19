#pragma safety enable

/* FIXED: `d = calloc(...)` in assignment form retypes the void pointee like the initializer form, so `if (d->m)` narrows a real member (parser.c declaration_core) */

void* _Owner _Opt _Clear calloc(unsigned long n, unsigned long size);
void free(void* _Owner _Opt p);

struct token { int type; };
struct decl { struct token* _Opt lint_token; };

int set_it(struct token* _Opt* out);
void use(struct token* t);

/* The parser.c shape: declared null, assigned from calloc inside the body. */
void assigned_from_calloc(void)
{
    struct decl* _Owner _Opt d = 0;
    d = calloc(1, sizeof *d);
    if (d == 0) return;

    set_it(&d->lint_token);

    if (d->lint_token)
    {
        use(d->lint_token);
    }

    free(d);
}

/* control: the initializer form, must behave identically */
void initialized_from_calloc_ok(void)
{
    struct decl* _Owner _Opt d = calloc(1, sizeof *d);
    if (d == 0) return;

    set_it(&d->lint_token);

    if (d->lint_token)
    {
        use(d->lint_token);
    }

    free(d);
}
