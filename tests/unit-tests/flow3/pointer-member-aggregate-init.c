#pragma safety enable

/* a plain pointer member set in a braced initializer takes its initialized/null state from the expression, so `{ .field = p->x }` is not uninitialized */

struct token { int line; };

struct marker
{
    struct token* p_token_begin;
    struct token* p_token_end;
};

struct expression
{
    struct token* first_token;
    struct token* last_token;
};

/* the original false positive: returning the aggregate must not report "uninitialized" */
struct marker expression_to_marker(const struct expression* p)
{
    struct marker m = {
        .p_token_begin = p->first_token,
        .p_token_end = p->last_token
    };
    return m;                       /* clean: both members are initialized */
}

/* Null state flows through the init too: a non-null source is fine... */
void non_null_source(struct token* t)
{
    struct marker m = { t, t };
    (void)m;
}

/* documented: a possibly-null source member still warns on dereference */
