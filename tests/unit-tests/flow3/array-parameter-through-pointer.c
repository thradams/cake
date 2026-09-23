#pragma safety enable

/* 85 is off by default and shows the folded condition most directly */
#pragma cake diagnostic warning 85

struct S { int buf[2]; };

void f(int a[2]);
void f_const(const int a[2]);

int identifier(void)
{
    int a[2] = { 1, 1 };
    f(a);
    if (a[0] == 2)
    {
        return 1;
    }
    return 0;
}

int arrow_member(struct S* p)
{
    p->buf[0] = 1;
    f(p->buf);
    if (p->buf[0] == 2)
    {
        return 1;
    }
    return 0;
}

int through_local_pointer(void)
{
    struct S s = { 0 };
    struct S* p = &s;
    f(p->buf);
    if (s.buf[0] == 2)
    {
        return 1;
    }
    return 0;
}

int parenthesized(struct S* p)
{
    p->buf[0] = 1;
    f((p->buf));
    if (p->buf[0] == 2)
    {
        return 1;
    }
    return 0;
}

/* a const array parameter does not write, the value is kept */
void const_parameter(struct S* p)
{
    p->buf[0] = 1;
    f_const(p->buf);
    compile_assert(p->buf[0] == 1);
}

/* a struct passed by value is a copy: its array member is not written */
struct T { struct S s; int x; };
void g(struct S s);
void h(struct T t);

void struct_by_value(void)
{
    struct S s = { 0 };
    g(s);
    compile_assert(s.buf[0] == 0);
}

void struct_by_value_through_pointer(struct S* p)
{
    p->buf[0] = 1;
    g(*p);
    compile_assert(p->buf[0] == 1);
}

void nested_struct_by_value(void)
{
    struct T t = { 0 };
    h(t);
    compile_assert(t.s.buf[1] == 0);
}

int member_array_argument(void)
{
    struct S s = { 0 };
    f(s.buf);
    if (s.buf[0] == 2)
    {
        return 1;
    }
    return 0;
}

void compound_literal_argument(void)
{
    f((int[2]){ 1, 2 });
}
