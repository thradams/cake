/* Cake x64_msvc */

struct __tag0 {
    int x;
    int y;
};

struct Named {
    int id;
    char text[8];
};

union Vec2 {
    struct __tag0 __m0;
    int v[2];
};

union Data {
    int i;
    float f;
    unsigned char bytes[4];
};

struct Inner {
    int val;
};

struct Outer {
    struct Inner inner;
    int x;
};

struct Pair {
    int a;
    int b;
};

struct RGB {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

struct Point {
    int x;
    int y;
};

struct __cS26 {
    int x;
};

struct Pt {
    int x;
    int y;
};

struct S {
    int a;
    int b;
    int c;
};

struct Rect {
    int w;
    int h;
};

struct Msg {
    int id;
    char text[4];
};

static void memcpy(void * dest, const void * src, unsigned long long n);
static void memset(void *dest, int ch, unsigned long long count);

void test_struct_basic(void)
{
    struct Point p;
    struct Point q;

    p.x = 10;
    p.y = 20;
    q.x = 0;
    q.y = 5;
}

void test_struct_out_of_order(void)
{
    struct RGB c;

    c.r = 128;
    c.g = 0;
    c.b = 255;
}

void test_struct_redesignation(void)
{
    struct Pair p;

    p.a = 99;
    p.b = 2;
}

void test_array_basic(void)
{
    int a[5];

    a[0] = 0;
    a[1] = 0;
    a[2] = 42;
    a[3] = 0;
    a[4] = 0;
}

void test_array_last_index(void)
{
    int a[4];

    a[0] = 0;
    a[1] = 0;
    a[2] = 0;
    a[3] = 7;
}

void test_array_continuation(void)
{
    int a[6];

    a[0] = 0;
    a[1] = 0;
    a[2] = 10;
    a[3] = 20;
    a[4] = 30;
    a[5] = 0;
}

void test_array_redesignation(void)
{
    int a[4];

    a[0] = 0;
    a[1] = 99;
    a[2] = 0;
    a[3] = 0;
}

void test_array_size_inferred(void)
{
    int a[5];

    a[0] = 0;
    a[1] = 0;
    a[2] = 0;
    a[3] = 0;
    a[4] = 1;
}

void test_nested_struct(void)
{
    struct Outer o;

    o.inner.val = 7;
    o.x = 3;
}

void test_struct_with_char_array(void)
{
    struct Named n;

    n.id = 1;
    memcpy(n.text, "hello", 6);
}

void test_union_designation(void)
{
    union Data d;
    union Data e;

    d.i = 42;
    e.i = 0;
    e.f = 0;
    e.bytes = 0;
}

void test_anonymous_struct_in_union(void)
{
    union Vec2 u;

    u. = 0;
    u.v = 0;
}

void test_constexpr_struct(void)
{
    struct Rect r;

    r.w = 640;
    r.h = 480;
}

void test_constexpr_array(void)
{
    int lut[4];

    lut[0] = 1;
    lut[1] = 2;
    lut[2] = 0;
    lut[3] = 8;
}

void test_mixed_designated_positional(void)
{
    struct S s;

    s.a = 1;
    s.b = 10;
    s.c = 20;
}

void test_array_of_structs(void)
{
    struct Pt pts[4];

    pts[0].x = 1;
    pts[0].y = 2;
    pts[1].x = 0;
    pts[1].y = 0;
    pts[2].x = 0;
    pts[2].y = 0;
    pts[3].x = 7;
    pts[3].y = 8;
}

void test_array_index_zero(void)
{
    int a[3];

    a[0] = 99;
    a[1] = 0;
    a[2] = 0;
}

void test_error_unknown_member(void)
{
    struct __cS26 s;

    memset(&s, 0, 4);
}

void test_error_array_index_oob(void)
{
    int a[3];

    memset(&a, 0, 12);
}

void test_error_negative_index(void)
{
    int a[3];

    memset(&a, 0, 12);
}

void test_warning_excess_after_designator(void)
{
    int a[2];

    a[0] = 0;
    a[1] = 1;
}

void test_warning_string_too_long_designated(void)
{
    struct Msg m;

    m.id = 0;
    memcpy(m.text, "toolong", 8);
}

static void memset(void *dest, int ch, unsigned long long count)
{
    unsigned char *ptr;

    ptr = (unsigned char*)dest;
    while (count-- > 0)
    {
       *ptr++ = 0;
    }
}


static void memcpy(void * dest, const void * src, unsigned long long n)
{
    char *csrc;
    char *cdest;
    unsigned long long i; 

    csrc = (char *)src;
    cdest = (char *)dest;
    for (i = 0; i < n; i++)
    {
       cdest[i] = csrc[i]; 
    }
}

