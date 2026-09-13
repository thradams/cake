// n3958 - Making offsetof a core language feature
struct S {
    char c;
    double d;
};

struct inner { int a; int b[4]; };
struct foo { int N; struct inner in; char buf[]; };
struct bf { int x : 3; int y : 5; };
struct anon { char c; union { int u; long long ll; }; };

int main(void)
{
    static_assert(__builtin_offsetof(struct S, c) == 0);
    static_assert(__builtin_offsetof(struct S, d) == 8);

    /* comma inside the type-name is fine: offsetof is an operator, not a macro */
    static_assert(offsetof(struct { int x, y; int buf[10]; }, buf) == 8);

    /* designator-list: . identifier and [ constant-expression ] */
    static_assert(offsetof(struct foo, in) == 4);
    static_assert(offsetof(struct foo, in.a) == 4);
    static_assert(offsetof(struct foo, in.b) == 8);
    static_assert(offsetof(struct foo, in.b[2]) == 16);
    static_assert(offsetof(struct foo, buf) == 24);
    static_assert(offsetof(struct foo, buf[3]) == 27);

    /* member inside anonymous union */
    static_assert(offsetof(struct anon, ll) == 8);

    /* non-constant index: not an integer constant expression */
    int i = 2;
    unsigned long long k = offsetof(struct foo, buf[i]);
    static_assert(offsetof(struct foo, buf[i]) == 26); //lint 960 error: expected constant expression
    static_assert(offsetof(struct foo, in.b[4]) == 24);   //lint 42 warning: past the end


    offsetof(struct bf, y);          //lint 860 error: bit-field
    offsetof(struct foo, nope);      //lint 720 error: member not found
    offsetof(struct foo, N.x);       //lint 730 error: not a struct
    offsetof(struct foo, N[0]);      //lint 700 error: not an array
    offsetof(struct foo, in.b[0].q); //lint 730 error: not a struct
}

/* union members all start at offset 0 */
union U { int u; long long ll; struct { char c; int x; } s; };
static_assert(offsetof(union U, ll) == 0);
static_assert(offsetof(union U, s.x) == 4);
