#pragma safety enable

/*
   A plain (non-owner) POINTER member set in a braced initializer takes its
   initialized / null state from the initializer expression -- just like an
   integer member takes its value relation. Without this, a struct built as
   `{ .field = p->x }` had its pointer members treated as uninitialized and was
   wrongly flagged when the aggregate was used (e.g. returned).

   (Owner members are intentionally left to their own move/init tracking.)
*/

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

/* The original false positive: members are initialized from member accesses,
   so returning the aggregate must NOT report "uninitialized". */
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

/*
   ...and a possibly-null source is still caught (documented, since it warns):

       void null_source(struct token* _Opt t) {
           struct marker m = { t, t };
           m.p_token_begin->line = 1;   // warns: -> on a possibly-null pointer
       }
*/
