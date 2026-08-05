#pragma safety enable
#define NULL ((void*)0)

/*
   History / current behavior for a shape found in cake's own source
   (parser.c, symbol renaming):

     char* _Opt _Owner temp = strdup(new_name);
     if (temp == NULL) throw;
     p_init_declarator->p_declarator->name_opt->lexeme = temp;

   flow3_check_discarding_owner_before_overwrite (added to catch
   `owner = new_value;` without freeing the old value first) flags this
   line: "assignment discards _Owner '.lexeme' without releasing it
   first". temp IS legitimately being moved into lexeme -- nothing is
   actually leaked HERE -- but lexeme is reached through a plain,
   non-optional pointer parameter (name_opt), so its incoming value is
   only flow3's fabricated "must be valid because the type says non-
   _Opt" placeholder, never an actually-observed value. flow3 cannot
   tell "definitely already null/the caller's problem" apart from
   "genuinely still holds a live resource the caller forgot to free" in
   this situation.

   This WAS silently skipped (treating "unknown" as "assume safe"), but
   that suppression was deliberately removed: it also silently missed
   real leaks of the exact same shape (see
   owner-moved-into-struct-member.c's *_via_param cases, where the
   member genuinely does still hold a live resource). Between "warn on
   this one legitimate move too" and "silently miss real leaks written
   the same way", the warning was judged the better default. A caller
   who knows a specific field is always safe to overwrite this way can
   suppress that one diagnostic; flow3 has no sound way to infer it
   automatically.
*/

char* _Owner _Opt strdup(const char* s);
void free(void* _Owner _Opt p);

struct token
{
    char* _Owner lexeme;
};

struct declarator
{
    struct token* name_opt;
};

struct init_declarator
{
    struct declarator* p_declarator;
};

int rename_identifier(struct init_declarator* p_init_declarator, const char* new_name)
{
    try
    {
        char* _Opt _Owner temp = strdup(new_name);
        if (temp == NULL) throw;
        /* Now warns: "assignment discards _Owner '.lexeme' without
           releasing it first". temp really is just being moved in here
           -- this specific case has no leak -- but lexeme's incoming
           state is unknown (reached through a plain pointer parameter),
           and flow3 now treats "unknown" the same as "possibly live"
           rather than "assume safe". See the file comment above. */
        p_init_declarator->p_declarator->name_opt->lexeme = temp; //lint 26 assignment discards _Owner '.lexeme' without releasing it first
    }
    catch
    {
        return 1;
    }
    return 0;
}

/* A REAL local owner being overwritten without freeing first is, and
   always was, correctly flagged. */
void real_local_leak(void)
{
    struct token t;
    t.lexeme = strdup("hello"); //lint 35 passing a possible null pointer 'strdup("hello")' to non-nullable pointer parameter (see
    t.lexeme = strdup("world"); //lint 26 35 assignment discards _Owner 't.lexeme' without releasing it first (see line 80)
    free(t.lexeme);
}
