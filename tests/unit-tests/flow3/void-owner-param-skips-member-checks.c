#pragma safety enable

/* `x_destroy(p); free(p);` must not report (*p).i ended: a void* destination cannot access members, only p itself is consumed */

struct X { int i; };
void free(void* _Owner _Opt p);
void x_destroy(_Dtor struct X* p);

void f(struct X* _Owner p)
{
    x_destroy(p);
    free(p); /* ok: void* can't access *p's members, so nothing to flag */
}

/* contrast: passing the ended object to a parameter that can access members is still flagged */
void read_x(struct X* p);

void g(struct X* _Owner p)
{
    x_destroy(p);
    read_x(p); //lint 30 possible uninitialized object '*p' (i is uninitialized after x_destroy)
} //lint 29 owner object (p) not moved (root see line 48)
