#pragma safety enable

/*
    Every parameter form and every kind of exit (return with a value, return, falling off
    the end) gives the same result in a definition (d_), a local function (l_) and a
    function literal (x_).
*/

typedef unsigned long size_t;
_Uninitialized void* _Owner malloc(size_t sz);
void free(void* _Owner _Nullable p);

struct S { int a; int b; };
struct person { char* _Owner name; };

void call_s(void (*f)(struct S*));
void call_clear(void (*f)(_Clear struct S*, int));
void call_out(void (*f)(_Out struct S*));
void call_dtor(void (*f)(_Dtor struct person*));
void call_person(void (*f)(struct person*));
void call_int_ptr(int* (*f)(int*));
void call_ret_int(int (*f)(int));
char* _Owner call_owner(char* _Owner (*f)(void));

/* ---- _Clear: every member 0 at every exit ---- */

void d_clear_early_return(_Clear struct S* p, int c)
{
    p->a = 0;
    if (c) return; //lint 69
    p->b = 0;
}

void l_clear_early_return(void)
{
    static void f(_Clear struct S* p, int c)
    {
        p->a = 0;
        if (c) return; //lint 69
        p->b = 0;
    }
}

void x_clear_early_return(void)
{
    call_clear((static void (_Clear struct S* p, int c)) {
        p->a = 0;
        if (c) return; //lint 69
        p->b = 0;
    });
}

/* ---- _Out: every member assigned at every exit ---- */

void d_out_partial(_Out struct S* p)
{
    p->a = 1;
} //lint 71

void l_out_partial(void)
{
    static void f(_Out struct S* p)
    {
        p->a = 1;
    } //lint 71
}

void x_out_partial(void)
{
    call_out((static void (_Out struct S* p)) {
        p->a = 1;
    }); //lint 71
}

/* ---- _Dtor: the owner members are released ---- */

void d_dtor_ok(_Dtor struct person* p)
{
    free(p->name);
}

void l_dtor_ok(void)
{
    static void f(_Dtor struct person* p)
    {
        free(p->name);
    }
}

void x_dtor_ok(void)
{
    call_dtor((static void (_Dtor struct person* p)) {
        free(p->name);
    });
}

void d_dtor_forgets(_Dtor struct person* p)
{
} //lint 29

void l_dtor_forgets(void)
{
    static void f(_Dtor struct person* p)
    {
    } //lint 29
}

void x_dtor_forgets(void)
{
    call_dtor((static void (_Dtor struct person* p)) {
    }); //lint 29
}

/* ---- a plain pointer is a borrow: the owner member is not left moved ---- */

void d_borrow_leaves_moved(struct person* p)
{
    free(p->name);
} //lint 72

void l_borrow_leaves_moved(void)
{
    static void f(struct person* p)
    {
        free(p->name);
    } //lint 72
}

void x_borrow_leaves_moved(void)
{
    call_person((static void (struct person* p)) {
        free(p->name);
    }); //lint 72
}

/* ---- return: the final state of the returned object ---- */

int* d_return_local(void)
{
    int i = 0;
    return &i; //lint 31
}

void l_return_local(void)
{
    static int* f(void)
    {
        int i = 0;
        return &i; //lint 31
    }
}

void x_return_local(void)
{
    call_int_ptr((static int* (int* p)) {
        int i = 0;
        return &i; //lint 31
    });
}

char* _Owner d_return_owner_ok(void)
{
    char* _Owner p = malloc(1);
    return p; //lint 30
}

void l_return_owner_ok(void)
{
    static char* _Owner f(void)
    {
        char* _Owner p = malloc(1);
        return p; //lint 30
    }
}

void x_return_owner_ok(void)
{
    char* _Owner r = call_owner((static char* _Owner (void)) {
        char* _Owner p = malloc(1);
        return p; //lint 30
    });
    free(r);
}

char* _Owner d_owner_leaks(void)
{
    char* _Owner p = malloc(1);
    return 0; //lint 29
}

void l_owner_leaks(void)
{
    static char* _Owner f(void)
    {
        char* _Owner p = malloc(1);
        return 0; //lint 29
    }
}

void x_owner_leaks(void)
{
    char* _Owner r = call_owner((static char* _Owner (void)) {
        char* _Owner p = malloc(1);
        return 0; //lint 29
    });
    free(r);
}
