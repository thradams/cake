#pragma safety enable

/* FIXED: the merge cleanup no longer frees arm entries (an arm that never branched IS the parent, walking up wiped live ancestors), so later `||` operands still see `ctx->current`'s widened state (expressions.c is_first_of_unary_expression) */


struct token { int type; };
struct ctx { struct token* _Opt current; };

int fpost(struct ctx* ctx);

int f(struct ctx* ctx)
{
    if (ctx->current == 0)
        return 0;

    return fpost(ctx) ||
        ctx->current->type == 1 ||
        ctx->current->type == 2 ||
        ctx->current->type == 3;
}
