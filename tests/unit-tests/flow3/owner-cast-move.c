#pragma safety enable

/*
   Casting an owner to an _Owner target transfers ownership: the source is
   moved into the cast result. The common case is `free((void* _Owner)s)` --
   the cast to `void* _Owner` moves s, and free consumes it.

   flow3 used to cast only the VALUE and not transfer ownership, so after
   `free((void* _Owner)s)` the original s still looked un-moved and a false
   "owner object 's' not moved" (leak) warning was reported at function exit.

   (Reproduced from tokenizer.c: `free((void* _Owner)s);`)
*/

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

/*
   Use-after-move is still caught (documented, since it warns):

       void double_free(void) {
           char* _Owner _Opt s = get();
           free((void* _Owner)s);   // moves s
           free(s);                 // warns: object 's' is moved
       }
*/
