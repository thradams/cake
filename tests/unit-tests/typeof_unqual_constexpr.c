/*
   constexpr is a storage class of the operand of typeof, not part of its
   type: typeof(K) is 'const int' and typeof_unqual(K) is a modifiable 'int'.
*/

constexpr int K = 7;

static_assert(_Generic(typeof(K), const int: 1, default: 0));
static_assert(_Generic(typeof_unqual(K), int: 1, default: 0));

int f(void)
{
    typeof_unqual(K) a = 1;
    a++;
    a = 3;

    typeof(K) b = 1;
    b++; //lint 920 error: increment of read-only object

    return a + b;
}
