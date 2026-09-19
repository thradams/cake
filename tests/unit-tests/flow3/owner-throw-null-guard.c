#pragma safety enable

/* `if (temp == NULL) throw; p->name_opt->lexeme = temp;` accounts for temp on every path, no "owner not moved" (codegen.c:161) */

#define NULL ((void*)0)

void free(void* _Owner _Opt p);
char* _Owner _Opt strdup(const char* s);

struct m { struct m* _Opt next; const char* key; };
struct m* _Opt find(const char* s);
int cmp(const char* a, const char* b);

struct name { char* _Owner _Opt lexeme; };
struct decl { struct name* _Opt name_opt; int renamed; };

int rename_if_necessary(struct decl* p, const char* nn)
{
    try
    {
        if (p->name_opt == NULL) throw;

        struct m* _Opt it = find(nn);
        while (it)
        {
            if (cmp(nn, it->key) == 0)
            {
                for (int i = 2; i < 1000000; )
                {
                    struct m* _Opt it2 = find(nn);
                    if (it2 == NULL)
                    {
                        p->renamed = 1;
                        if (p->name_opt)
                        {
                            free(p->name_opt->lexeme);
                            char* _Opt _Owner temp = strdup(nn);
                            if (temp == NULL) throw;
                            p->name_opt->lexeme = temp;
                        }
                        return 1;
                    }

                    if (i > 10000)
                        i += 1000;
                    else if (i > 1000)
                        i += 100;
                    else
                        i++;
                }
                _Assert(false);
            }
            it = it->next;
        }
        return 1;
    }
    catch
    {
    }
    return 0;
}
