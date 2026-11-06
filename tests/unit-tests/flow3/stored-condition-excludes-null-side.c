/* `ok` true says which side of `p != 0` this path is on: the null of p, narrowed from the other side, is not a value here */
#pragma safety enable

struct E { int size; };
struct E* find(int);
void g(int);

void f(void)
{
    struct E* p = find(1);
    struct E* q = find(2);
    bool ok = p != 0 && q != 0;
    if (ok && p != 0 && q != 0 && 1 < p->size)
        g(1);
}

void not_tested(int c)
{
    struct E* p = find(1);
    if (c)
        g(p->size); //lint 33
}
