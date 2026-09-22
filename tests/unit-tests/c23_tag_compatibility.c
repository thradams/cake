/* C23 N3037: same tag + same content = same type (issue #187) */

struct X { int a; int b; };
struct X { int a; int b; };

struct A { int i; struct { int m; } n; union { int u; float f; }; int bits : 3; };

void f(struct X);
void fa(struct A);

int main()
{
    {
        struct X { int a; int b; };
        struct X x = {1, 2};
        f(x);

        struct A { int i; struct { int m; } n; union { int u; float f; }; int bits : 3; };
        struct A a = {0};
        fa(a);
    }

    {
        struct X { int a; long b; };
        struct X x = {1, 2};
        f(x); //lint 950
    }

    {
        struct X { int a; int c; };
        struct X x = {1, 2};
        f(x); //lint 950
    }

    {
        struct X { int a; int b : 4; };
        struct X x = {1, 2};
        f(x); //lint 950
    }

    {
        struct A { int i; struct { long m; } n; union { int u; float f; }; int bits : 3; };
        struct A a = {0};
        fa(a); //lint 950
    }

    {
        union X { int a; int b; };
        union X x = {1};
        f(x); //lint 950
    }
}

/* definition in a parameter list (prototype scope) is the same type too */
#define VEC(T) struct vec_##T { T* data; int size; }
void push(VEC(int)* v, int x);
void use_vec(void)
{
    VEC(int) v = {0};
    push(&v, 1);
}

/* a self-referential member compares the struct with itself: still the same type */
struct N { int v; struct N* next; };
void walk(struct N*);
void use_list(void)
{
    struct N { int v; struct N* next; };
    struct N n = {0};
    walk(&n);
}

/* enums: same tag, same underlying type, same enumerators and values = same type */
enum E : short { E1 = 1, E2 = 2 };
void fe(enum E);
void use_enum(void)
{
    {
        enum E : short { E1 = 1, E2 = 2 };
        enum E e = E1;
        fe(e);
    }
    {
        enum E : int { E1 = 1, E2 = 2 };
        enum E e = E1;
        fe(e); //lint 40
    }
    {
        enum E : short { E1 = 1, E2 = 3 };
        enum E e = E1;
        fe(e); //lint 40
    }
}
