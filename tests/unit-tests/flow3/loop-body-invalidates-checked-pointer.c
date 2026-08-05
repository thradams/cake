#pragma safety enable

/*
   Regression test: flow3 keeps a top-of-loop null guard LIVE when the loop
   body invalidates the checked pointer.

   A pointer member is narrowed non-null OUTSIDE a loop. The loop body then
   calls a non-const function that mutates that member (advancing a token
   cursor), so on the 2nd+ iteration the member can be null again. flow3's
   two-pass loop analysis warms up state on the first (suppressed) pass -- the
   body's advance() invalidates the cursor -- so on the second pass the
   top-of-loop guard is correctly seen as reachable, NOT dead code.

   (Contrast parser.c:2390, where a SECOND null guard at the bottom of the body
   re-proves the cursor non-null before the back-edge and the loop condition is
   const, so that top guard genuinely is unreachable -- a true positive, not a
   flow3 bug. This sample pins the false-positive-free case.)

   This sample must be clean.
*/

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
