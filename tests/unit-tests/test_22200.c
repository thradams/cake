﻿
#pragma safety enable

struct Y {
    int type;
};

struct X {
    struct Y* _Opt current;
};

void f(struct X* ctx)
{
    if (ctx->current == 0)
        return;

    if (ctx->current != 0)
    {

    }
    else
    {
        //still not null, so no warning
        ctx->current->type = 1;
    }

}