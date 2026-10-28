// an array parameter is adjusted to a pointer (6.7.6.3p7), so it is compatible with that pointer

void f1(int a[const 2]);
void f1(int * const a);

void f2(int a[const][2]);
void f2(int (* const a)[2]);

void f3(int a[static 2]);
void f3(int * a);

void f3b(int a[restrict]);
void f3b(int * restrict a);

void f4(int a[2]);
void f4(int a[3]); //lint 52 'f4' declared with array parameter of different size

void f5(const int a[2]);
void f5(int * a); //lint 1020

void f6(int a[2][3]);
void f6(int (*a)[4]); //lint 1020
