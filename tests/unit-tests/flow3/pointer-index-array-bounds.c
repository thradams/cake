#pragma safety enable
/* p points to a[k]: p[i] is a[k + i], checked against the array */
void constant_index(void)
{
    int a[3] = { 0 };
    int *p = a + 0;
    p[2] = 1; /* ok */
    p[3] = 1; //lint 70
    int *q = &a[1];
    q[1] = 1; /* ok */
    q[2] = 1; //lint 70
    q[-1] = 1; /* ok: a[0] */
    q[-2] = 1; //lint 70
}

void flow_index(int c)
{
    int a[3] = { 0 };
    int *p = &a[1];
    int i = 2;
    p[i] = 1; //lint 70
    if (c) i = 0; else i = 5;
    p[i] = 1; //lint 70 the i == 5 path
    if (c > 0 && c < 2) p[c] = 1; /* ok: a[2] */
}

void moved(void)
{
    int a[3] = { 0 };
    int *p = &a[0];
    p++;
    p[1] = 1; /* ok: a[2] */
    p[2] = 1; //lint 70
}

void unknown(int *p, int i)
{
    p[i] = 1; /* p is not known to be in an array */
}
