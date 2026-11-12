#pragma safety enable

/* issue #531: by flow state, the return type must declare a pointee that is uninitialized on the returned path */

struct X { struct X* p; int i; };

_Uninitialized void * _Nonnull _Owner malloc(unsigned int size);
struct X * _Owner make_x(void); /* an initialized object with unknown member values */
void x_delete(_Dtor struct X* _Owner p);

int * _Owner direct() { return malloc(sizeof(int)); } //lint 30
_Uninitialized int * _Owner annotated() { return malloc(sizeof(int)); }

int * _Owner written()
{
    int * _Owner p = malloc(sizeof(int));
    *p = 1;
    return p;
}

int * _Owner written_on_one_path(int c)
{
    int * _Owner p = malloc(sizeof(int));
    if (c) { *p = 1; }
    return p; //lint 30
}

/* flow.md A9.2 */
_Uninitialized void * _Owner malloc_opt(unsigned int size);

int * _Owner written_on_one_path_after_null_check(int c)
{
    int * _Owner p = malloc_opt(sizeof(int));
    if (p == 0) { return 0; }
    if (c) { *p = 1; }
    return p; //lint 30
}

/* the arm of the delete left the loop: e is set only on the other path */
struct Y { struct Y* _Owner left; };
int more(void);
int bad(void);
void y_delete(_Dtor struct Y* _Owner p);
_Clear struct Y* _Nonnull _Owner y_new(void);

struct Y * _Owner set_after_break(struct Y * _Owner e)
{
    while (more())
    {
        struct Y * _Owner n = y_new();
        if (bad())
        {
            y_delete(n);
            n = nullptr;
            break;
        }
        n->left = e;
        e = n;
    }
    return e;
}

struct Y * _Owner set_after_throw(struct Y * _Owner e)
{
    try
    {
        while (more())
        {
            struct Y * _Owner n = y_new();
            if (bad())
            {
                y_delete(n);
                n = nullptr;
                throw;
            }
            n->left = e;
            e = n;
        }
    }
    catch
    {
    }
    return e;
}

/* null on the path where the pointee was released: the pointee is not checked there */
struct X * _Owner null_after_delete(int c)
{
    struct X * _Owner p = make_x();
    try
    {
        if (p == nullptr)
            throw;
        if (c)
            throw;
    }
    catch
    {
        x_delete(p);
        p = nullptr;
    }

    if (p && p->i)
    {
        if (c)
        {
            x_delete(p);
            p = nullptr;
        }
    }
    return p;
}
