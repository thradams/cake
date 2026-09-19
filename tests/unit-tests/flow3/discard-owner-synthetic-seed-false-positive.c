#pragma safety enable
#define NULL ((void*)0)

/* overwriting an _Owner member reached through a plain pointer parameter warns even when the move is legitimate: unknown is treated as possibly live, not as safe */

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
        /* warns by design: lexeme's incoming state is unknown, see the file comment */
        p_init_declarator->p_declarator->name_opt->lexeme = temp; //lint 26 assignment discards _Owner '.lexeme' without releasing it first
    }
    catch
    {
        return 1;
    }
    return 0;
}

/* a real local owner overwritten without freeing is flagged */
void real_local_leak(void)
{
    struct token t;
    t.lexeme = strdup("hello"); //lint 35 passing a possible null pointer 'strdup("hello")' to non-nullable pointer parameter (see
    t.lexeme = strdup("world"); //lint 26 35 assignment discards _Owner 't.lexeme' without releasing it first (see line 80)
    free(t.lexeme);
}
