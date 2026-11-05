#pragma safety enable

/* a cast keeps the annotations of its operand: writing one is an error */

typedef void* pvoid;

void f(int* p)
{
    void* a = (void* _Owner)p; //lint 2400
    void* b = (void* _Nullable)p; //lint 2400
    void* c = (void* _Nonnull)p; //lint 2400
    void* d = (void*)p; /* ok */
    const char** h = (const char**)p; /* ok: inner levels are nullable by default, nothing is written */
    void** i = (pvoid*)p; /* ok: typedef */
    (void)h;
    (void)i;
    void* e = (int* _Owner*)p; //lint 2400
    void* f = (int* _Nullable*)p; //lint 2400
    void* g = (int* _Nonnull*)p; //lint 2400
    (void)e;
    (void)f;
    (void)g;
    (void)a;
    (void)b;
    (void)c;
    (void)d;
}
