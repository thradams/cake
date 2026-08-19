

struct A { int x; };
struct B { int y; };

void func_int_ptr(int *p) {}
void func_const_int_ptr(const int *p) {}
void func_double_ptr(double *p) {}

void take_const(const int *p) {}
void write(int *p) {}

/* top-level parameter qualifiers are dropped from the function type */
void func_plain_int(int p) {}
void func_const_int(const int p) {}
void func_volatile_int(volatile int p) {}
void func_ptr(int *p) {}
void func_const_ptr_param(int * const p) {}
void func_const_pointee(const int *p) {}
void func_volatile_pointee(volatile int *p) {}
void func_ptr_ptr(int **p) {}
void func_const_ptr_ptr(const int **p) {}

void t(void)
{
    int i = 0;
    const int ci = 0;
    long l;
    float f = 0;
    double d;

    int *p1 = &i;
    const int *p2 = &i;
    unsigned int *pui;
    float *pf = &f;
    struct A a = { 0 };
    struct A *pa = &a;
    struct B *pb;

    /* --- compatible: allowed without diagnostics --- */

    l = i;
    d = f;

    /* adding const to the pointee of a plain pointer assignment is fine */
    p2 = p1;

    /* any object pointer converts to/from void* */
    void *pv = p1;
    p1 = pv;
    const void *pcv = p2;
    (void)pcv;

    /* matching function pointer signatures */
    void (*fp1)(int *) = func_int_ptr;
    void (*fp2)(const int *) = func_const_int_ptr;
    (void)fp1; (void)fp2;

    /* top-level parameter qualifiers don't affect function-type compatibility */
    void (*fq1)(int) = func_plain_int;
    fq1 = func_const_int;
    fq1 = func_volatile_int;

    void (*fq2)(int *) = func_ptr;
    fq2 = func_const_ptr_param;
    (void)fq1; (void)fq2;

    /* passing a non-const pointer where const is expected is fine */
    take_const(p1);
    write(p1);

    /* --- incompatible: each of these must be diagnosed --- */

    /* unrelated pointee types */
    pui = p1; //lint 54 incompatible types
    p1 = pf; //lint 54 incompatible types

    /* unrelated struct pointer types */
    pb = pa; //lint 54 incompatible types

    /* discards const */
    p1 = p2; //lint 15 discarding const qualifier

    /* int** -> const int** is not allowed: it would let const data be
       written through the non-const alias */
    int **pp1 = &p1;
    const int **pp2 = &p2;
    pp2 = pp1; //lint 15 discarding const qualifier

    /* function pointer with mismatched parameter type */
    void (*fp3)(double *) = func_int_ptr; //lint 54 incompatible types

    /* function pointer with mismatched parameter qualifier */
    void (*fp4)(const int *) = func_int_ptr; //lint 54 incompatible types

    /* passing address of a const object where a non-const pointer is expected */
    write(&ci); //lint 15 discarding const qualifier at argument

    /* pointee-level parameter qualifiers must match exactly (unlike the
       top-level qualifiers above) */
    void (*fq3)(int *) = func_ptr;
    fq3 = func_const_pointee; //lint 54 incompatible types

    void (*fq4)(const int *) = func_const_pointee;
    fq4 = func_volatile_pointee; //lint 54 incompatible types

    void (*fq5)(int **) = func_ptr_ptr;
    fq5 = func_const_ptr_ptr; //lint 54 incompatible types

    (void)pui; (void)pb; (void)fp3; (void)fp4; (void)fq3; (void)fq4; (void)fq5;
}
