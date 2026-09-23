#pragma safety enable

struct S { int x; };
void set(int* p);

int arrow(const struct S* s)
{
    int i = 0;
    int* _Opt p = 0;
    if (s->x > 2) { p = &i; }
    if (s->x > 2) { i = *p; }
    return i;
}

int deref(const int* s)
{
    int i = 0;
    int* _Opt p = 0;
    if (*s > 2) { p = &i; }
    if (*s > 2) { i = *p; }
    return i;
}

int pointer_changed(const struct S* s, const struct S* t)
{
    int i = 0;
    int* _Opt p = 0;
    if (s->x > 2) { p = &i; }
    s = t;
    if (s->x > 2) { i = *p; } //lint 33 possible null pointer dereference
    return i;
}

int member_assigned(struct S* s)
{
    int i = 0;
    int* _Opt p = 0;
    if (s->x > 2) { p = &i; }
    s->x = 0;
    s->x = 5;
    if (s->x > 2) { i = *p; } //lint 33 possible null pointer dereference
    return i;
}

int member_incremented(struct S* s)
{
    int i = 0;
    int* _Opt p = 0;
    if (s->x > 2) { p = &i; }
    s->x++;
    if (s->x > 2) { i = *p; } //lint 33 possible null pointer dereference
    return i;
}

int compound_assigned(int* s)
{
    int i = 0;
    int* _Opt p = 0;
    if (*s > 2) { p = &i; }
    *s += 1;
    if (*s > 2) { i = *p; } //lint 33 possible null pointer dereference
    return i;
}

/* the call may change c */
int written_by_call(int c)
{
    int i = 0;
    int* _Opt p = 0;
    if (c > 2) { p = &i; }
    set(&c);
    if (c > 2) { i = *p; } //lint 33 possible null pointer dereference
    return i;
}

int written_through_alias(int c)
{
    int i = 0;
    int* _Opt p = 0;
    int* pc = &c;
    if (c > 2) { p = &i; }
    *pc = 5;
    if (c > 2) { i = *p; } //lint 33 possible null pointer dereference
    return i;
}

/* written_typedef_name in expressions.c: w is only set inside the loop, and the
   pointee member p->name is only tracked there, so nothing says which side ran */
struct decl { const char* _Opt name; };
const struct decl* _Opt first(void);
const struct decl* _Opt next(const struct decl* p);
const char* _Opt lookup(const char* name);
int strcmp(const char* a, const char* b);
void out(const char* s);

void set_on_some_paths(int is_pointer)
{
    const char* _Opt written = 0;
    const char* _Opt matched = 0;
    if (!is_pointer)
    {
        const struct decl* _Opt p = first();
        for (int level = 0; p != 0 && level < 8; level++)
        {
            if (p->name != 0)
            {
                if (written == 0)
                {
                    written = p->name;
                }
                if (lookup(p->name) != 0)
                {
                    matched = p->name;
                    break;
                }
            }
            p = next(p);
        }
    }
    if (written != 0 && matched != 0 && strcmp(written, matched) != 0)
    {
        out("a");
    }
    else if (written != 0)
    {
        out("b");
    }
    else
    {
        out("c");
    }
}
