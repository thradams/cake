#pragma safety enable

/* _Out overwrites the pointee: like `*x = t;`, owners alive in it are lost (issue #484). */

struct X { char * _Owner name; };

void* _Nonnull memcpy(_Out void* dest, const void* src, int count);
char * _Owner strdup(const char* s);
void free(void* _Owner _Opt p);

void f1(struct X * x)
{
    struct X t = {0};
    memcpy(x, &t, sizeof t); //lint 26
}

void f2(struct X * x)
{
    struct X t = {0};
    *x = t; //lint 26
}

void f3(struct X * x)
{
    /* no warning: name was released */
    free(x->name);
    struct X t = {0};
    memcpy(x, &t, sizeof t);
}

/* Same with a typed _Out pointee. */
void init_x(_Out struct X* p);

void f4(struct X * x)
{
    init_x(x); //lint 26
}

void f5(struct X * x)
{
    /* no warning: name was released */
    free(x->name);
    init_x(x);
}
