#pragma safety enable

/*
   Repro for expressions.c:743 (character_constant_expression).

   `ctx->current` is declared `_Opt` in the struct. It is guarded with
   `if (ctx->current == NULL) throw;`, which correctly narrows it for the
   direct `ctx->current->lexeme` access right after (no warning there).

   That narrowed value is cast and stored into a local `_Opt` pointer
   `p`. After some pointer arithmetic (`p++`), `p` is passed to a
   non-nullable parameter and reassigned from the result:

       p = decode(p, &c);

   `p` was derived from an already narrowed, non-owner, non-null
   source, so no warning should be reported here.
*/

#define NULL ((void*)0)

struct token
{
    char* _Owner lexeme;
};

struct parser_ctx
{
    struct token* _Opt current;
};

const unsigned char* decode(const unsigned char* s, unsigned int* c);

void f(struct parser_ctx* ctx)
{
    try
    {
        if (ctx->current == NULL)
        {
            throw;
        }

        const unsigned char* _Opt p = (const unsigned char*)ctx->current->lexeme;

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
