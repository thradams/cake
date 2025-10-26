/* Cake 0.12.26 x86_msvc */
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


