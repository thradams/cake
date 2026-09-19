#pragma safety enable

/* zero is a real state: using a zeroed non-_Opt pointer member (passing it, returning the aggregate) warns for both `= {0}` and calloc (_Clear); only the creation is clean */

typedef unsigned long size_t;
void* _Owner _Opt _Clear calloc(size_t n, size_t sz);
void free(void* _Owner _Opt p);

struct X { char* p_non_null; };

void f(char* p);

/* --- braced zero-init --- */

void braced_use(void)
{
    struct X x = { 0 };   /* allowed: creating a zeroed struct is fine */
    f(x.p_non_null);      /* warns: p_non_null is null */ //lint 35
}

struct X braced_return(void)
{
    struct X x = { 0 };   /* allowed */
    return x;             /* warns: x.p_non_null is null */ //lint 35
}

/* --- calloc: must behave identically --- */

void calloc_use(void)
{
    struct X* _Owner _Opt x = calloc(1, sizeof * x);   /* allowed */
    if (x == 0) return;
    f(x->p_non_null);     /* warns: zeroed by calloc -> null */ //lint 35
    free(x);
}

/* Assigning first is how you make it OK. */
void calloc_assign_then_use(char* src)
{
    struct X* _Owner _Opt x = calloc(1, sizeof * x);
    if (x == 0) return;
    x->p_non_null = src;
    f(x->p_non_null);     /* ok: assigned */
    free(x);
}

/* --- incomplete vs complete --- */

/* initialization happens in parts: creating and filling field by field is clean, returning or passing requires the object to be complete */

void take(struct X* x);

/* creating the storage is clean in both spellings (the split declare-then-assign form is how the parser is written) */
void create_is_clean(void)
{
    struct X* _Owner _Opt a = calloc(1, sizeof * a);   /* clean */
    free(a);

    struct X* _Owner _Opt b = 0;
    b = calloc(1, sizeof * b);                         /* clean */
    free(b);
}

/* Filling it in: clean. This is the "in parts" half of the rule. */
void completing_it_is_clean(char* src)
{
    struct X* _Owner _Opt x = calloc(1, sizeof * x);
    if (x == 0) return;
    x->p_non_null = src;  /* clean: this is completion */
    take(x);              /* clean: complete now */
    free(x);
}

/* passing it while incomplete warns: the parameter type promises a non-null member (parser.c after the calloc) */
void pass_while_incomplete_warns(void)
{
    struct X* _Owner _Opt x = calloc(1, sizeof * x);
    if (x == 0) return;
    take(x);              /* warns: incomplete */ //lint 35
    free(x);
}

/* Assigning null to a non-_Opt pointer: a direct use of the null value. */
void assign_null_to_non_opt(struct X* x)
{
    x->p_non_null = 0;    /* warns */ //lint 35
}

/* by value the aggregate carries its null member, so both zeroing forms report */
void takev(struct X x);

void by_value_still_warns(void)
{
    struct X* _Owner _Opt x = calloc(1, sizeof * x);
    if (x == 0) return;
    takev(*x);            /* warns: the null member is passed with the copy */ //lint 35
    free(x);
}
