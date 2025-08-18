void myprintf(const char* fmt, ...) __attribute__((format(printf, 1, 2)));


__attribute__((deprecated))
void old_function() {}

__attribute__((always_inline)) inline int add(int a, int b)
{
    return a + b;
}


struct __attribute__((aligned(16))) Aligned
{
    int x;
    char y;
    int a __attribute__((aligned(16)));
};

typedef int int16_t __attribute__((mode(HI))); // 16-bit integer type
typedef int* int_ptr __attribute__((nonnull));

void foo() __attribute__((noreturn, hot, aligned(16)));

int f1(int x) __attribute__((pure));        // on function declarator
__attribute__((deprecated)) int f2(int x);  // on type declarator

struct __attribute__((packed)) Packed
{
    char a;
    int b;
};

struct __attribute__((packed, aligned(8))) Data
{
    int id __attribute__((aligned(4)));
    char flag;
};


int main()
{
    int unused_var __attribute__((unused)) = 42;
    return 0;
}
