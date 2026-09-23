#pragma safety enable

/* 85 is off by default and shows the folded condition most directly */
#pragma cake diagnostic warning 85

struct S { int buf[4]; };

int arrow_member(struct S* p, int c)
{
    p->buf[1] = 7;
    p->buf[c] = 5;
    if (p->buf[1] == 5)
    {
        return 1;
    }
    return 0;
}

int parenthesized(struct S* p, int c)
{
    p->buf[1] = 7;
    (p->buf)[c] += 1;
    if (p->buf[1] == 8)
    {
        return 1;
    }
    return 0;
}

int increment(struct S* p, int c)
{
    p->buf[1] = 7;
    p->buf[c]++;
    if (p->buf[1] == 8)
    {
        return 1;
    }
    return 0;
}

/* the write lands in the pointee, p keeps its non-null value */
void pointer(int* _Opt p, int c)
{
    if (p == 0)
    {
        return;
    }
    p[c] = 5;
    *p = 1;
}

int pointer_to_object(int c)
{
    int x = 0;
    int* p = &x;
    p[c] = 5;
    if (x == 5)
    {
        return 1;
    }
    return 0;
}

/* p points at a[0]; p[c] can reach every element of a */
int pointer_into_array(int c)
{
    int a[4] = { 0 };
    int* p = a;
    p[c] = 5;
    if (a[1] == 5)
    {
        return 1;
    }
    return 0;
}

int pointer_into_array_element(int c)
{
    int a[4] = { 0 };
    int* p = &a[2];
    p[c] = 5;
    if (a[0] == 5)
    {
        return 1;
    }
    return 0;
}

int pointer_value_operand(int c)
{
    int a[4] = { 0 };
    int* p = a;
    (p + 1)[c] = 5;
    if (a[3] == 5)
    {
        return 1;
    }
    return 0;
}

int pointer_into_matrix(int c)
{
    int m[2][3] = { 0 };
    int* p = m[0];
    p[c] = 5;
    if (m[1][2] == 5)
    {
        return 1;
    }
    return 0;
}
