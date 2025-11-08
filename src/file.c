#define arraysize(A) (sizeof(A) / sizeof((A))[0])

int f(const unsigned int v)
{
    int array[3];
    static_assert(arraysize(array) == 3);
}