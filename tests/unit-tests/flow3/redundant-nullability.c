#pragma safety enable

/* writing the default nullability is redundant; inner levels are part of the type */

struct X
{
    int* _Nullable p; //lint 122
    int* _Nonnull q;
    int* _Nullable* pp;
};

int* _Nullable f(void); //lint 122
int* _Nonnull g(void);
void h(int* _Nonnull p); //lint 122
void k(int* _Nullable p);

void use(int* p)
{
    int* _Nullable a = p; //lint 122
    int* b = (int* _Nullable)p; //lint 122
    int* _Nonnull c = p;
}

/* a _Dtor object may be partly built: _Nullable on the pointed object adds nothing */
void x_destroy(_Nullable _Dtor struct X* p); //lint 122
void x_destroy2(_Dtor struct X* p);
