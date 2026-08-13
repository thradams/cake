#pragma safety enable

/*
   Real bug, found via cake self-analysis on parser.c:declaration_core
   (parser.c:2716 `return p_declaration;`, field `last_token`, see line
   2567 where `p_declaration` is calloc'd).

   `last_token` is declared non-optional, but one branch (`kind == 1`
   here, mirroring declaration_core's "static_assertion"/"pragma"/
   "expected declaration" branches) never assigns it, leaving it at the
   zero-initialized NULL calloc left behind. flow3 correctly tracks this
   as a real possible-null member on the initializing assignment `p =
   &d;` -- not a false positive.
*/

struct token { int x; };

struct declaration {
    struct token* first_token;
    struct token* last_token; /* non-nullable */
};

void make(int kind, struct token* t)
{
    struct declaration d = { .first_token = t };
    struct declaration* p = &d; //lint 35 warning: '&d->last_token' may be null

    if (kind == 1)
    {
        /* forgot to set p->last_token here -- same shape as the
           static_assertion/pragma_declaration/error-recovery branches
           in declaration_core that never touch p_declaration->last_token */
    }
    else
    {
        p->last_token = t;
    }
}
