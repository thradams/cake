#pragma safety enable

/* p derived from a narrowed non-null `ctx->current`, then `p++` and `p = decode(p, &c)`: no warning (expressions.c:743) */

#define NULL ((void*)0)

struct token
{
    char* _Nonnull _Owner lexeme;
};

struct parser_ctx
{
    struct token* current;
};

const unsigned char* _Nonnull decode(const unsigned char* s, unsigned int* c);

void f(const struct parser_ctx* ctx)
{
    try
    {
        if (ctx->current == NULL)
        {
            throw;
        }

        const unsigned char* p = (const unsigned char*)ctx->current->lexeme;

        if (p[0] == 'u' && p[1] == '8')
        {
            p++;
            p++;
            p++;

            unsigned int c = 0;
            p = decode(p, &c);
        }
    }
    catch
    {
    }
}
