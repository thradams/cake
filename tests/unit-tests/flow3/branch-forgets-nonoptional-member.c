#pragma safety enable

/* real bug (parser.c declaration_core): one branch never sets the non-optional member, the calloc null is reported on `p = &d;` */

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
        /* forgot to set p->last_token here */
    }
    else
    {
        p->last_token = t;
    }
}
