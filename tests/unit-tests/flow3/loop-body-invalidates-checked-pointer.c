#pragma safety enable

/* a top-of-loop null guard stays live when the body invalidates the checked member: the second pass sees it reachable, no unreachable-code warning */

struct token;

struct ctx {
    struct token* _Opt current;
};

/* const receiver: flow3 must NOT invalidate through this call */
_Bool cond(const struct ctx* c);

/* non-const receiver: mutates c->current (may become null) */
void advance(struct ctx* c);

void on_end(struct ctx* c);

void f(struct ctx* ctx)
{
    if (ctx->current == 0)
        return;

    while (cond(ctx))
    {
        if (ctx->current == 0)   /* live on 2nd+ iteration */
        {
            on_end(ctx);
            return;
        }
        advance(ctx);               /* invalidates ctx->current */
    }
}
