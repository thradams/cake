#pragma safety enable

struct token { int type; };
struct ctx { struct token* current; };

int fpost(struct ctx* ctx);

int f(struct ctx* a, struct ctx* b, int c)
{
    struct ctx* p = c ? a : b;
    fpost(a);
    fpost(b);
    /* no warning: unknown after the calls, on both targets */
    return p->current->type == 1;
}

int g(struct ctx* a)
{
    struct ctx* p = a;
    fpost(a);
    /* no warning: same with one target */
    return p->current->type == 1;
}
