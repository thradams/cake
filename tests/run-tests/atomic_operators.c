#include <stdio.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

struct big { int a[5]; };

_Atomic int i;
_Atomic(long long) ll;
_Atomic(char) c;
_Atomic(short) s;
_Atomic(double) d;
_Atomic(int*) p;
_Atomic struct big b;
int arr[4];

struct with_member { _Atomic(int) m; };

int main(void)
{
    i = 5;
    CHECK(i == 5);
    CHECK(i++ == 5);
    CHECK(++i == 7);
    CHECK(i-- == 7);
    CHECK(--i == 5);
    i += 10;
    i -= 3;
    i *= 2;
    i /= 3;
    i %= 5;
    CHECK(i == 3);
    i <<= 4;
    i >>= 1;
    i |= 1;
    i &= 0x19;
    i ^= 0x10;
    CHECK(i == 9);
    CHECK((i = 11) == 11);

    ll = 1;
    ll <<= 40;
    CHECK(ll == (1LL << 40));

    c = 'a';
    c += 1;
    CHECK(c == 'b');

    s = 1000;
    s *= 3;
    CHECK(s == 3000);

    d = 1.5;
    d += 1.0;
    d *= 2;
    CHECK(d == 5.0);

    i = 0;
    i += 2.5; /* computed as double */
    CHECK(i == 2);

    p = arr;
    p++;
    p += 2;
    CHECK(p == arr + 3);
    CHECK(p-- == arr + 3);
    CHECK(p == arr + 2);
    *p = 42;
    CHECK(arr[2] == 42);

    struct big b0 = {{1, 2, 3, 4, 5}};
    b = b0;
    struct big b1 = b;
    CHECK(b1.a[4] == 5);

    struct with_member w;
    w.m = 1;
    w.m += 2;
    CHECK(w.m == 3);

    _Atomic int* pi = &i;
    *pi = 20;
    (*pi)++;
    CHECK(i == 21);

    _Atomic int local_array[3];
    local_array[1] = 4;
    local_array[1] *= 4;
    CHECK(local_array[1] == 16);

    return failures;
}
