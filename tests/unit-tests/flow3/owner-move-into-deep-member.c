#pragma safety enable

/* moving an owner into a deep member `p->b->name_opt->lexeme = temp` records the move on temp even when the destination pointee was not synthesized (codegen.c rename_file_scope_declarator, MSVC) */

#define NULL ((void*)0)

void free(void* _Owner _Opt p);
char* _Owner strdup(const char* s);

struct name { char* _Owner lexeme; };
struct decl { struct name* name_opt; };
struct outer { struct decl* d; };

int move_into_deep_member(struct outer* p, const char* s)
{
    try
    {
        if (p->d == NULL) throw;
        if (p->d->name_opt == NULL) throw;

        free(p->d->name_opt->lexeme);
        char* _Owner temp = strdup(s);
        if (temp == NULL) throw;
        p->d->name_opt->lexeme = temp;   /* temp moved into the deep member */

        return 1;
    }
    catch
    {
    }
    return 0;
}

/* single-hop baseline; the strdup failure path nulls n->lexeme so the borrowed parameter is left valid (72 otherwise) */
int move_into_shallow_member(struct name* n, const char* s)
{
    free(n->lexeme);
    n->lexeme = 0;
    char* _Owner temp = strdup(s);
    if (temp == NULL) return 0;
    n->lexeme = temp;
    return 1;
}
