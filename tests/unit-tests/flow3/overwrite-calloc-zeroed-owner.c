#pragma safety enable

/* `p->type = make_type()` overwrites the zeroed nested struct with an unknown value, so free(p) through void* correctly reports name_opt/next as possibly owning */

void* _Owner _Opt _Clear calloc(unsigned long n, unsigned long size);
void free(void* _Owner _Opt p);

struct type {
    char* _Owner _Opt name_opt;
    struct type* _Owner _Opt next;
};

struct expr {
    struct type type;
};

struct type make_type(void);
int more(void);

void f(void)
{
    try
    {
        struct expr* _Owner _Opt p = calloc(1, sizeof *p);
        if (p == 0) throw;

        p->type = make_type();

        free(p); //lint 29 29 owner object (.type.name_opt) not moved (opt-nonnull see line 25)
    }
    catch
    {
    }
}

void f_loop(void)
{
    try
    {
        while (more())
        {
            struct expr* _Owner _Opt p = calloc(1, sizeof *p);
            if (p == 0) throw;

            p->type = make_type();

            free(p); //lint 29 29 owner object (.type.name_opt) not moved (opt-nonnull see line 43)
        }
    }
    catch
    {
    }
}
