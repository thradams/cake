#pragma safety enable

/*
   Regression test for two related false positives reported directly
   against real code:

     int errcode = mkdir(to, 0700);
     if (errcode != 0)
     {
         return errcode; // "errcode" possibly uninitialized -- false positive
     }

     struct X x = f();   // or: struct X x; x = f();
     use(x.a);            // "x.a" possibly uninitialized -- false positive

   Root cause: EXPR_POSTFIX_FUNCTION_CALL only ever seeded
   p_expression->object (the call's own result) when the return type was
   a pointer -- both the _Opt and non-_Opt pointer branches wrote a
   proper alternative into the flow map. For every OTHER return type
   (plain scalar, or struct/union returned by value) nothing was seeded
   at all: the call's result object was left completely untracked.

   For a bare `return errcode;`-style DIRECT read this doesn't matter
   immediately after the call (the call itself has no prior state to
   misreport) -- but it matters enormously for the DESTINATION of an
   assignment or initialization FROM that call:
   flow3_check_object_init_assigment looks up the source's own map
   entry, and if there isn't one (exactly this case), it returns early
   WITHOUT ever touching the destination's existing state. So `errcode`
   kept whatever UNINITIALIZED state its own declaration gave it, and
   reading it moments later (directly, not combined with arithmetic)
   still reported "possibly uninitialized" -- even though it had
   plainly just been assigned from a function call.

   The same gap applied to every member of a struct/union return value:
   nothing seeded them as initialized either, so `struct X x = f();`
   left every one of x's members UNINITIALIZED in the flow map, only
   masked in casual testing because reading members combined with
   arithmetic (`x.a + x.b`) happens not to consult this per-object check
   at all (a separate, narrower limitation) -- a DIRECT member read
   (`return x.a;`, or assigning it on to something else) exposed the
   real gap.

   Fixed by extending EXPR_POSTFIX_FUNCTION_CALL to seed non-pointer
   return values too, reusing flow3_parameter_object_init (already used
   for parameters) -- which recurses through struct/union members,
   marking non-optional pointer members non-null (same rule as
   parameters) and every other member/scalar as ANY (initialized, value
   unknown), never UNINITIALIZED.
*/

int mkdir(const char* path, int mode);

int copy_folder(const char* from, const char* to)
{
    int errcode = mkdir(to, 0700);
    if (errcode != 0)
    {
        return errcode; /* ok: errcode was assigned from mkdir's return value */
    }
    return 0;
}

struct X
{
    int a;
    int b;
    int* p;
};

struct X f(void);

int use_declaration_form(void)
{
    struct X x = f();
    int* q = x.p;
    return x.a + x.b + *q; /* ok: every member of a returned struct is
                               assumed initialized, and its non-optional
                               pointer member assumed non-null */
}

int use_assignment_form(void)
{
    struct X x;
    x = f();
    int* q = x.p;
    return x.a + x.b + *q; /* ok: same rule applies to plain assignment,
                               not just declaration-with-initializer */
}

/* Contrast: a real uninitialized read (no call involved at all) is
   still correctly flagged. */
int real_uninitialized(void)
{
    struct X x;
    return x.a; //lint 30 passing a possible uninitialized object 'x.a' (see line 95)
}
