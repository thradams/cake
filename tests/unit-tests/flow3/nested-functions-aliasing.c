#pragma safety enable

/*
    Aliasing between parameters, restrict, a file scope object the function uses and the
    restrict check of a call give the same result in a definition (d_), a local function
    (l_) and a function literal (x_).
*/

#define NULL ((void*)0)

struct Y { int i; };
struct X { struct Y* pY; };

int g;

void call_pp(void (*f)(struct X*, struct X*, struct Y*));
void call_pp_restrict(void (*f)(struct X* restrict, struct X*, struct Y*));
void call_pg(void (*f)(int*));
void call_p1(void (*f)(struct Y*));
void call_p0(void (*f)(void));
void call_pp_args(void (*f)(struct Y*, const struct Y*));
void copy(struct Y* restrict dest, const struct Y* src);

/* ---- p1 and p2 may be the same object ---- */

void d_alias(struct X* p1, struct X* p2, struct Y* y)
{
    p2->pY = y;
    p1->pY = NULL;
    p2->pY->i = 1; //lint 100
}

void l_alias(void)
{
    static void f(struct X* p1, struct X* p2, struct Y* y)
    {
        p2->pY = y;
        p1->pY = NULL;
        p2->pY->i = 1; //lint 100
    }
}

void x_alias(void)
{
    call_pp((static void (struct X* p1, struct X* p2, struct Y* y)) {
        p2->pY = y;
        p1->pY = NULL;
        p2->pY->i = 1; //lint 100
    });
}

/* ---- restrict: they are not ---- */

void d_restrict(struct X* restrict p1, struct X* p2, struct Y* y)
{
    p2->pY = y;
    p1->pY = NULL;
    p2->pY->i = 1;
}

void l_restrict(void)
{
    static void f(struct X* restrict p1, struct X* p2, struct Y* y)
    {
        p2->pY = y;
        p1->pY = NULL;
        p2->pY->i = 1;
    }
}

void x_restrict(void)
{
    call_pp_restrict((static void (struct X* restrict p1, struct X* p2, struct Y* y)) {
        p2->pY = y;
        p1->pY = NULL;
        p2->pY->i = 1;
    });
}

/* ---- a parameter may point to a file scope object the function uses ---- */

void d_global(int* p)
{
    *p = 1;
    g = 2;
    compile_assert(*p == 1); //lint 67
}

void l_global(void)
{
    static void f(int* p)
    {
        *p = 1;
        g = 2;
        compile_assert(*p == 1); //lint 67
    }
}

void x_global(void)
{
    call_pg((static void (int* p)) {
        *p = 1;
        g = 2;
        compile_assert(*p == 1); //lint 67
    });
}

/* ---- the restrict check of a call ---- */

void d_call_same(struct Y* a)
{
    copy(a, a); //lint 101
}

void l_call_same(void)
{
    static void f(struct Y* a)
    {
        copy(a, a); //lint 101
    }
}

void x_call_same(void)
{
    call_p1((static void (struct Y* a)) {
        copy(a, a); //lint 101
    });
}

void d_call_maybe(struct Y* a, const struct Y* b)
{
    copy(a, b); //lint 100
}

void l_call_maybe(void)
{
    static void f(struct Y* a, const struct Y* b)
    {
        copy(a, b); //lint 100
    }
}

void x_call_maybe(void)
{
    call_pp_args((static void (struct Y* a, const struct Y* b)) {
        copy(a, b); //lint 100
    });
}

/* ---- the state of the enclosing function is not seen: a nested function starts as any function ---- */

void d_state(void)
{
    g = 5;
    compile_assert(g == 5);
}

void d_state_outer(void)
{
    g = 5;
    static void f(void)
    {
        compile_assert(g == 5); //lint 67
    }
    f();
}

void x_state_outer(void)
{
    g = 5;
    call_p0((static void (void)) {
        compile_assert(g == 5); //lint 67
    });
}

/* ---- only static objects of the enclosing function are visible, and nothing is known about them on entry ---- */

void d_static_outer(void)
{
    static int s;
    s = 5;
    static void f(void)
    {
        compile_assert(s == 5); //lint 67
    }
    f();
}

void x_static_outer(void)
{
    static int s;
    s = 5;
    call_p0((static void (void)) {
        compile_assert(s == 5); //lint 67
    });
}
