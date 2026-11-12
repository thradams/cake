#pragma safety enable

/*
    A function definition, a local function (defined inside another function) and a
    function literal get the same checks, and what happens inside one (return, labels,
    break) never affects the function around it.
*/

struct S { int a; int b; };
void call(void (*f)(void));
void call_int(int (*f)(int));
void call_clear(void (*f)(_Clear struct S*));

/* ---- return inside the nested function does not end the outer function ---- */

int local_return(void)
{
    static int f(int a)
    {
        return a;
    }
    int i = f(1);
    int j = i;
    return j;
}

int literal_return(void)
{
    call((static void (void)) { return; });
    int j = 1;
    return j;
}

/* ---- control reaches end of non-void function ---- */

int definition_falls_off(int a)
{
    if (a > 0)
        return 1;
} //lint 98 control reaches end of non-void function

int local_falls_off(void)
{
    static int f(int a)
    {
        if (a > 0)
            return 1;
    } //lint 98 control reaches end of non-void function
    return f(1);
}

int literal_falls_off(void)
{
    call_int((static int (int a)) {
        if (a > 0)
            return 1;
    }); //lint 98 control reaches end of non-void function
    return 0;
}

/* ---- _Clear parameter must be cleared at every exit ---- */

void definition_clear(_Clear struct S* p)
{
    p->a = 0;
} //lint 69 _Clear parameter 'p' pointee (.b) is never set to zero

void local_clear(void)
{
    static void f(_Clear struct S* p)
    {
        p->a = 0;
    } //lint 69 _Clear parameter 'p' pointee (.b) is never set to zero
    struct S s = { 1, 2 };
    f(&s);
}

void literal_clear(void)
{
    call_clear((static void (_Clear struct S* p)) {
        p->a = 0;
    }); //lint 69 _Clear parameter 'p' pointee (.b) is never set to zero
}

/* ---- the outer function keeps its own parameters, return type and labels ---- */

int after_local(_Clear struct S* p)
{
    static void g(void)
    {
        goto done;
    done:
        return;
    }
    g();
    p->a = 0;
    p->b = 0;
    return 1;
}

int after_literal(_Clear struct S* p)
{
    call((static void (void)) {
        goto done;
    done:
        return;
    });
    p->a = 0;
    p->b = 0;
    return 1;
}
