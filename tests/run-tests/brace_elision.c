/*
   Brace elision (6.7.11): an initializer without braces initializes the
   first scalar inside the next subobject, going down through arrays and
   structs in any order; a string literal initializes an array of characters
   as a whole, and a pointer when the array is an array of pointers.
*/

#include <stdio.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

struct member
{
    int b;
    char s[4];
};

struct array_then_structs
{
    int a[2];
    struct member m[2];
};

struct scalar_then_structs
{
    int a;
    struct member m[2];
};

struct pointers
{
    const char* names[2];
    int k;
};

struct inner_text
{
    struct
    {
        char s[4];
    } inner;
    int k;
};

struct matrix
{
    int a[2][2];
    int z;
};

static struct array_then_structs file_x = { 1, 2, 3, "abc", 4 };
static struct pointers file_p = { "x", "y", 3 };
static const char* file_names[2][2] = { "a", "b", "c", "d" };

static int id(int x)
{
    return x;
}

static void static_storage(void)
{
    CHECK(file_x.a[0] == 1 && file_x.a[1] == 2);
    CHECK(file_x.m[0].b == 3 && file_x.m[0].s[0] == 'a' && file_x.m[0].s[2] == 'c' && file_x.m[0].s[3] == 0);
    CHECK(file_x.m[1].b == 4 && file_x.m[1].s[0] == 0);
    CHECK(file_p.names[0][0] == 'x' && file_p.names[1][0] == 'y' && file_p.k == 3);
    CHECK(file_names[0][1][0] == 'b' && file_names[1][0][0] == 'c' && file_names[1][1][0] == 'd');
}

static void automatic_storage(void)
{
    struct array_then_structs x = { 1, id(2), 3, "abc", id(4) };
    struct scalar_then_structs y = { 1, 3, "ab", 4, "cd" };
    struct pointers p = { "x", "y", id(3) };
    struct inner_text t = { "abc", 4 };
    struct matrix q = { 1, 2, 3, 4, id(5) };
    struct member grid[2][2] = { 1, "a", 2, "b", 3, "c", id(4), "d" };
    const char* names[2][2] = { "a", "b", "c", "d" };

    CHECK(x.a[0] == 1 && x.a[1] == 2);
    CHECK(x.m[0].b == 3 && x.m[0].s[0] == 'a' && x.m[0].s[2] == 'c' && x.m[0].s[3] == 0);
    CHECK(x.m[1].b == 4 && x.m[1].s[0] == 0);

    CHECK(y.a == 1 && y.m[0].b == 3 && y.m[0].s[1] == 'b' && y.m[1].b == 4 && y.m[1].s[0] == 'c');

    CHECK(p.names[0][0] == 'x' && p.names[1][0] == 'y' && p.k == 3);
    CHECK(t.inner.s[0] == 'a' && t.inner.s[3] == 0 && t.k == 4);
    CHECK(q.a[0][1] == 2 && q.a[1][0] == 3 && q.a[1][1] == 4 && q.z == 5);

    CHECK(grid[0][0].b == 1 && grid[0][1].b == 2 && grid[1][0].b == 3 && grid[1][1].b == 4);
    CHECK(grid[0][1].s[0] == 'b' && grid[1][1].s[0] == 'd' && grid[1][1].s[1] == 0);

    CHECK(names[0][0][0] == 'a' && names[0][1][0] == 'b' && names[1][0][0] == 'c' && names[1][1][0] == 'd');
}

int main(void)
{
    static_storage();
    automatic_storage();

    return failures;
}
