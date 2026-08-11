#pragma safety enable

/*
   Repro model for the codegen.c:161 report:

     char* _Opt _Owner temp = strdup(new_name);
     if (temp == NULL) throw;              // eq-true branch aborts
     p->name_opt->lexeme = temp;           // temp moved here

   The claim under test: at the end of the `if (name_opt)` block, is the
   owner `temp` correctly accounted for on every path?
     - eq-false (temp != NULL): temp is moved into ->lexeme  -> OK
     - eq-true  (temp == NULL): `throw` leaves the block; temp is null,
       so there is nothing to move -> OK
   So NO "owner not moved" diagnostic is expected. If flow3 reports one and
   attributes it to the `eq-true` branch, that path is being treated as if
   it reached the scope exit with a live (non-null, unmoved) owner, which
   would be the bug to fix.

   The nesting (try / while / for / if) mirrors the real function so the
   reachability and throw-unwinding match.
*/

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
