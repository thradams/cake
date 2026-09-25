_Atomic(long) b;
static_assert(_Generic(b, long: 1, default: 0));
static_assert(_Generic(&b, _Atomic long*: 1, default: 0));
static_assert(sizeof(b) == sizeof(long));

const _Atomic(int*) p = 0;
static_assert(_Generic(&p, int* const _Atomic*: 1, default: 0));

_Atomic(int)* q;
static_assert(_Generic(q, _Atomic int*: 1, default: 0));

typedef _Atomic(unsigned) au;
au u;
static_assert(_Generic(&u, _Atomic unsigned*: 1, default: 0));

struct S { _Atomic(short) m; };
static_assert(_Generic(&((struct S*)0)->m, _Atomic short*: 1, default: 0));

_Atomic(int) arr[3];
static_assert(_Generic(&arr[0], _Atomic int*: 1, default: 0));

int main(void)
{
    b++;
    b += 2;
    long v = b;
    return (int)v;
}
