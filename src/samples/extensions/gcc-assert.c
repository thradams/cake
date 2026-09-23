/* GCC's <assert.h> expands assert(expr) roughly like this, using
   __extension__ to silence pedantic warnings and __assert_fail as the
   failure handler. __func__ works as usual inside it. */

extern void __assert_fail(const char* __assertion, const char* __file,
    unsigned int __line, const char* __function)
    __attribute__((__noreturn__));

#define assert(expr) \
    ((void) sizeof ((expr) ? 1 : 0), __extension__ ({ \
        if (expr) \
            ; /* empty */ \
        else \
            __assert_fail(#expr, __FILE__, __LINE__, __func__); \
    }))

int main()
{
    int x = 1;
    assert(x == 1);
}
