#pragma safety enable

/* a _Clear implementation must release an _Owner member (else 26) and leave it exactly 0 at exit (else 69) */

struct X
{
    char* _Opt _Owner text;
};

void free(void* _Opt _Owner p);

void clear_x_correct(_Clear struct X* p)
{
    /* release first, then zero -- both checks are satisfied */
    free(p->text);
    p->text = 0;
}

void clear_x_leaks(_Clear struct X* p)
{
    /* overwrites text with 0 without releasing it */
    p->text = 0; //lint 26 assignment discards _Owner '.text' without releasing it first (see line 46)
}

void clear_x_forgets_entirely(_Clear struct X* p)
{
    /* does nothing, caught by the exit-is-zero check */
} //lint 69 _Clear parameter 'p' pointee (.text) is not zero at exit
