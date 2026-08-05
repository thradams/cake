#pragma safety enable

/*
   "Zero" is a real state, and zero does NOT mean OK.

   Creating a zeroed aggregate is allowed. USING a zeroed non-_Opt pointer
   member is not: it is null, so passing it on (or returning the aggregate
   that contains it) must warn until it has been assigned.

   `= {0}` already behaved this way. calloc did not: its zeros were modeled as
   EMPTY entries and the old "empty operand holds vacuously" rule made
   `p->m == 0` true while `p->m` still looked usable -- so the bug below went
   unreported. calloc now carries `_Clear`, whose zeros are real state, and the
   two forms finally agree.

   Everything in this file WARNS by design, except the two creation cases at
   the bottom. It is a specification, not a regression: see the expected
   diagnostic on each line.

   Two narrower rules were tried here and reverted, both worth not retrying:

     - tagging the zero with its provenance ("this null came from calloc, not
       from an assignment"). It made calloc and `= {0}` disagree again -- a
       by-value pass of a braced-zero struct kept warning while the calloc'd one
       went quiet -- which is the exact divergence this file exists to prevent.

     - suppressing the report for members reached by following a POINTER into
       what it points at, on the argument that passing a pointer does not read
       the pointee. That is true of the pointee's CONTENTS being uninitialized,
       but not of this check: nullable-003.c pins `struct X x = {};
       receives_non_null(&x);` as a warning, and a pointer to a freshly calloc'd
       struct is the same shape. Handing a callee a pointer whose non-_Opt
       members are null is a finding however the storage got zeroed.

   What must NOT warn is the creation itself -- see under_construction_create.
   That was already true and needed no change.

*/

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

/*
   Initialization happens in PARTS. A non-_Opt pointer that is still zero means
   the object is INCOMPLETE, not that something invalid was done: the storage
   was created and the fields have not all been filled in yet.

   So creating it is clean, and so is filling it in field by field. But USING
   it -- returning it, or passing it as a parameter -- requires it to be
   COMPLETE, because those are the points where the type's promise (this member
   is non-null) is handed to someone else.
*/

void take(struct X* x);

/* Creating the storage: clean, in both spellings. The split declare-then-assign
   form is how cake's own parser is written, because the object must exist
   before a `try` for the `catch` to release it. */
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

/*
   Passing it while still incomplete: warns. The parameter type promises the
   member is non-null and it is not. Same shape as nullable-003.c's
   `receives_non_null(&x)` on a braced-zero local -- however the storage got
   zeroed, an incomplete object cannot be used as a complete one.

   cake's own parser does this one line after the calloc
   (`ctx->p_current_try_statement_opt = p_try_statement;`), and the report is
   correct there.
*/
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

/* By VALUE the aggregate carries its null member along, so both zeroing forms
   report -- braced_use/braced_return above, and calloc here. */
void takev(struct X x);

void by_value_still_warns(void)
{
    struct X* _Owner _Opt x = calloc(1, sizeof * x);
    if (x == 0) return;
    takev(*x);            /* warns: the null member is passed with the copy */ //lint 35
    free(x);
}
