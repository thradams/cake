/*
  N3958 "Making offsetof a core language feature"
  https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3958.pdf

  offsetof is a keyword, not a macro: no header is needed, the result is
  an integer constant expression, and the member-designator may be a
  path such as in.b[2], including members of anonymous structs/unions.
*/

struct foo
{
    int N;
    struct { int a; int b[4]; } in;
    union { char tag; short code; };
    char buf[];
};

static_assert(offsetof(struct foo, N) == 0);
static_assert(offsetof(struct foo, in.a) == 4);
static_assert(offsetof(struct foo, in.b[2]) == 16);
static_assert(offsetof(struct foo, code) == 24);

/* a comma inside the type-name is fine, it is real syntax not a macro */
static_assert(offsetof(struct { int x, y; }, y) == 4);

int main(int argc, char** argv)
{
    /* non-constant index: result is plain arithmetic */
    int i = argc;
    return offsetof(struct foo, in.b[i]);
}
