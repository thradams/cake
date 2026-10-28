// [static] and [const] are allowed only in the outermost array of a parameter (6.7.6.2p1)

int a[const]; //lint 1000 static or type qualifiers are not allowed in non-parameter array declarator
int b[const 2]; //lint 1000
typedef int T[const 2]; //lint 1000
int (*p)[const 2]; //lint 1000
int (*f1(void))[const 2]; //lint 1000

int r[restrict 2]; //lint 1000

struct S {
    int m[const 2]; //lint 1000
    int n[restrict 2]; //lint 1000
};

void f2(void)
{
    int c[const 2] = { 0 }; //lint 1000
    extern int d[static 2]; //lint 1000
    (void)sizeof(int[const 2]); //lint 1000
    (void)c;
}

void f3(int a[2][const 2]); //lint 1000
void f4(int (*a)[const 2]); //lint 1000

// ok: outermost array of a parameter
void f5(int a[const 2], int b[static 2], int c[const][2], int d[restrict volatile 2]);
void f6(void (*fp)(int a[const 2]));
