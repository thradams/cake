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
