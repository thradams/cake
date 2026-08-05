#pragma safety enable

/*
   p->type starts zeroed (calloc is _Clear), so name_opt/next are fine at
   that point. `p->type = make_type()` then overwrites the whole nested
   struct with make_type()'s return -- an untracked value flow3 must treat
   as unknown/ANY, since make_type has no contract describing what it
   hands back. free(p) discards *p (including *p's nested .type) through
   void*, without ever explicitly moving or releasing p->type.name_opt/next
   -- so the analyser correctly flags both as still possibly owning
   whatever make_type() produced. This is a consequence of fixing void*
   erasure to check member state (see void-owner-erasure-leaks-member.c)
   rather than unconditionally trusting the whole aggregate is clean.
*/

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
