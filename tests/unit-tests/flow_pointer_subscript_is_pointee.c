#pragma safety enable

// p[0] and *p are the same object, and so are p[0].m and p->m: a write
// through one is read through the other. When p points to a[j], p[k] is
// a[j + k].

void free(void* _Owner _Nullable p);

void pointer_parameter(int* p)
{
    *p = 1;
    compile_assert(p[0] == 1);
    p[0] = 2;
    compile_assert(*p == 2);
}

void array_parameter(int a[])
{
    a[0] = 1;
    compile_assert(*a == 1);
}

void pointer_to_array_element(void)
{
    int b[3] = { 0 };
    b[0] = 1;
    b[2] = 3;
    int* p = b;
    compile_assert(*p == 1);
    compile_assert(p[0] == 1);
    compile_assert(p[2] == 3);
    p[1] = 5;
    compile_assert(b[1] == 5);
}

void two_targets(int c)
{
    int x = 1;
    int y = 2;
    int* p = &x;
    if (c)
        p = &y;
    p[0] = 7;
    compile_assert(*p == 7);
    compile_assert(x == 7 || y == 7);
}

struct X { int i; };

void member(struct X* p)
{
    p->i = 1;
    compile_assert(p[0].i == 1);
    p[0].i = 2;
    compile_assert(p->i == 2);
}

struct Y { char* _Owner t; };

void moved_twice(struct Y* p)
{
    free(p[0].t);
    free(p->t); //lint 32
    p->t = 0;
}

// an object invented by the analysis (the pointee of a parameter or of a
// result) has an unknown size: its index is not checked. An array parameter
// with a size, `int a[3]`, is a concrete object of 3 elements.

int* get(void);

void invented_size_is_unknown(int* p, int a[], int** pp)
{
    p[5] = 0;
    a[5] = 0;
    if (pp[3])
        pp[3][7] = 0;
    int* r = get();
    if (r)
        r[9] = 0;
}

void string_literal_is_concrete(void)
{
    const char* s = "abc";
    char c = s[5]; //lint 70
}

void sized_array_parameter(int a[3])
{
    *a = 1;
    compile_assert(a[0] == 1);
    a[0] = 2;
    compile_assert(*a == 2);
    a[5] = 0; //lint 42
}

void local_array(void)
{
    int b[3] = { 0 };
    *b = 1;
    compile_assert(b[0] == 1);
}
