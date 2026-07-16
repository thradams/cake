/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct Y;

struct X {
    struct Y * current;
};

struct Y {
    int type;
};

void f(struct X * ctx)
{
    if (ctx->current == 0)
    {
        return;
    }
    if (ctx->current != 0)
    {
    }
    else
    {
        ctx->current->type = 1;
    }
}
