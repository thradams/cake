// #530: in a parameter, int a[const n] is int* const a; the element is not const

void f1(int n, int a[const n])
{
    a++; //lint 920 increment of read-only object 'a'
    a[0] = 1;
}

void f2(int * const a)
{
    a++; //lint 920 increment of read-only object 'a'
    a[0] = 1;
}

void f3(int a[static const 5])
{
    a[0] = 1;
}

void f4(int a[volatile 2], int b[restrict 2])
{
    a[0] = 1;
    b[0] = 1;
}

void f4b(int a[restrict 2], int b[const restrict static 2])
{
    a++;
    a[0] = b[0];
    b[0] = 1;
}

void f5(int a[const][2])
{
    a[0][0] = 1;
}

// const on the element type is still const

void g1(int n, const int a[n])
{
    a[0] = 1; //lint 920 assignment of read-only object of type 'const int'
}

void g2(int n, const int a[const n])
{
    a[0] = 1; //lint 920 assignment of read-only object of type 'const int'
}

typedef int A[3];
void g3(const A a)
{
    a[0] = 1; //lint 920 assignment of read-only object of type 'const int'
}

struct X { int arr[2]; };
void g4(const struct X* p)
{
    p->arr[0] = 1; //lint 920 assignment of read-only object of type 'const int'
}

void g5(void)
{
    const int b[3] = { 0 };
    b[0] = 1; //lint 920 assignment of read-only object of type 'const int'
    const A c = { 0 };
    c[0] = 1; //lint 920 assignment of read-only object of type 'const int'
}
