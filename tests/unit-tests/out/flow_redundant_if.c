struct Y {
    int type;
};

struct X {
    struct Y * current;
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

