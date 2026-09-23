#pragma safety enable

/* `free((void* _Owner)s)` moves s into the cast result, no false "owner not moved" at exit (tokenizer.c) */

#define NULL ((void*)0)

char* _Owner _Opt get(void);
void free(void* _Owner _Opt p);

/* free through an _Owner cast moves s -> clean, no leak warning. */
void freed_via_cast(void)
{
    char* _Owner _Opt s = get();
    free((void* _Owner)s);
}

/* Direct free (no cast) already worked -- kept for contrast. */
void freed_direct(void)
{
    char* _Owner _Opt s = get();
    free(s);
}

/* documented: `free((void* _Owner)s); free(s);` warns that s is moved */

struct T { char* _Owner _Opt m; };

void freed_via_cast_through_pointer(void)
{
    struct T t = { .m = get() };
    struct T* p = &t;
    free((void* _Owner)p->m);
}

void freed_via_cast_through_deref(void)
{
    char* _Owner _Opt s = get();
    char* _Owner _Opt* ps = &s;
    free((void* _Owner)*ps);
}

void assigned_via_cast(void)
{
    char* _Owner _Opt s = get();
    void* _Owner _Opt x = NULL;
    x = (void* _Owner)s;
    free(x);
}

void* _Owner _Opt returned_via_cast(void)
{
    char* _Owner _Opt s = get();
    return (void* _Owner)s;
}
