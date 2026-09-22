/* C23 feature smoke test (no headers, so it works with any header set) */

#if __STDC_VERSION__ < 202311L
#error "expected C23"
#endif

#if !defined(__STDC_EMBED_NOT_FOUND__) || !defined(__STDC_EMBED_FOUND__) || !defined(__STDC_EMBED_EMPTY__)
#error "__STDC_EMBED_* macros"
#endif

/* #elifdef / #elifndef */
#define FOO
#ifdef NOTDEF
#error "a"
#elifdef FOO
constexpr int elifdef_ok = 1;
#elifndef FOO
#error "b"
#endif

/* __VA_OPT__ */
#define CALL(a, ...) va_f(a __VA_OPT__(,) __VA_ARGS__)
int va_f(int a, ...) { return a; }

/* keywords */
static_assert(true == 1 && false == 0);
static_assert(_Generic(true, bool: 1, default: 0));
static_assert(_Generic(nullptr, typeof(nullptr): 1, default: 0));
static_assert(_Generic(int, int: 1, default: 0));

thread_local int tls;
constexpr int N = 4;
static_assert(N == 4);

/* literals */
static_assert(0b1010 == 10 && 0B11 == 3);
static_assert(1'000'000 == 1000000 && 0x1'F == 31);
static_assert(u8'a' == 'a');
static_assert(5wb == 5 && 255uwb == 255);

/* enums */
enum E : unsigned char { EA = 1, EB = 200 };
static_assert(sizeof(enum E) == 1);
enum FWD : int;
enum FWD : int { FA };
enum Big { V = 0xFFFFFFFFFFFFULL };
static_assert(sizeof(V) >= 8);

/* attributes */
[[nodiscard]] int nd(void) { return 1; }
[[maybe_unused]] static int mu(void) { return 2; }
[[deprecated("old")]] void dep(void) {}
[[noreturn]] void die(void);
[[unsequenced]] int sq(int x) { return x * x; }
[[reproducible]] int id(int x) { return x; }
[[gnu::unused]] static int vendor;
[[nonexistent::attr]] static int unknown_vendor;

/* unnamed parameters, () means (void) */
int unnamed(int, char*) { return 0; }
int empty_parens() { return 0; }

/* typeof, auto, alignas/alignof, compound literal storage class */
typeof(int(*)(void)) fp = 0;

struct S { int a; double b; };
struct FLEX { int n; int a[]; };

int main(void)
{
    int a = 1;
    typeof(a) b = a;
    typeof_unqual(const int) c = b;
    auto d = 10;
    auto e = 2.5;
    alignas(16) int al = 0;
    (void)alignof(int);

    /* empty initializer */
    struct S s = {};
    int arr[3] = {};
    struct FLEX fx = { .n = 0 };

    /* labels before declarations and at end of block */
    goto l;
l:
    int y = 1;
    if (y == 2) goto done;
    { done: }

    /* declaration right after case */
    switch (a) {
    case 1:
        int z = 1;
        a += z;
        [[fallthrough]];
    case 2:
        break;
    }

    /* pointer to VM type is mandatory */
    int n = 3;
    int (*pvm)[n] = 0;

    /* storage-class compound literals */
    int* ps = &(static int){ 5 };
    const int* pc = &(constexpr int){ 7 };

    int r = nd();
    [[maybe_unused]] int unused = 0;
    int* np = nullptr;

    return (int)(c + d + e + al + s.a + (int)s.b + arr[2] + fx.n + y + r + *ps + *pc
        + CALL(0) + CALL(0, 1) + sq(0) + id(0) + unnamed(0, "") + empty_parens()
        + tls + elifdef_ok + (np == nullptr) + (pvm == 0) + EA + FA) * 0;
}
