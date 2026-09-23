/*
   Cake is a cross-compiling compiler, but unlike traditional compilers
   that produce machine executables directly, Cake generates C code as
   its output. This C code is tailored for the target platform, taking
   into account differences in architecture, data sizes, and calling conventions.
*/

#ifdef _WIN64
    // 64-bit Windows  -target=x64_msvc

    //long is 4 bytes on windows
    static_assert(sizeof(1L) == 4);
    static_assert(sizeof(void*) == 8);
    static_assert(sizeof(L""[0]) == 2);

#elif defined _WIN32

static_assert(sizeof(bool) == 1);
static_assert(alignof(bool) == 1);

static_assert(sizeof(char) == 1);
static_assert(alignof(char) == 1);

static_assert(sizeof(short) == 2);
static_assert(alignof(short) == 2);

static_assert(sizeof(L' ') == 2);
static_assert(sizeof(u' ') == 2);
static_assert(sizeof(U' ') == 4);

static_assert(sizeof(int) == 4);
static_assert(alignof(int) == 4);

static_assert(sizeof(long) == 4);
static_assert(alignof(long) == 4);

static_assert(sizeof(long long) == 8);
static_assert(alignof(long long) == 8);

static_assert(sizeof(float) == 4);
static_assert(alignof(float) == 4);

static_assert(sizeof(double) == 8);
static_assert(alignof(double) == 8);

static_assert(sizeof(long double) == 8);
static_assert(alignof(long double) == 8);

static_assert(sizeof(void*) == 4);
static_assert(alignof(void*) == 4);

static_assert(sizeof(nullptr) == 4);
static_assert(alignof(nullptr) == 4);

static_assert(sizeof(sizeof(0)) == 4);
static_assert(alignof(sizeof(0)) == 4);

static_assert(sizeof((char*)0-1) == 4);
static_assert(alignof((char*)0-1) == 4);



#endif

#ifdef __x86_64__

// linux -target=x86_x64_gcc

_Static_assert(sizeof(bool) == 1, "");
_Static_assert(alignof(bool) == 1, "");

_Static_assert(sizeof(char) == 1, "");
_Static_assert(alignof(char) == 1, "");

_Static_assert(sizeof(short) == 2, "");
_Static_assert(alignof(short) == 2, "");

_Static_assert(sizeof(L' ') == 4, "");
_Static_assert(sizeof(u' ') == 2, "");
_Static_assert(sizeof(U' ') == 4, "");

_Static_assert(sizeof(int) == 4, "");
_Static_assert(alignof(int) == 4, "");

_Static_assert(sizeof(long) == 8, "");
_Static_assert(alignof(long) == 8, "");

_Static_assert(sizeof(long long) == 8, "");
_Static_assert(alignof(long long) == 8, "");

_Static_assert(sizeof(float) == 4, "");
_Static_assert(alignof(float) == 4, "");

_Static_assert(sizeof(double) == 8, "");
_Static_assert(alignof(double) == 8, "");

_Static_assert(sizeof(long double) == 16, "");
_Static_assert(alignof(long double) == 16, "");

_Static_assert(sizeof(void*) == 8, "");
_Static_assert(alignof(void*) == 8, "");

_Static_assert(sizeof(nullptr) == 8, "");
_Static_assert(alignof(nullptr) == 8, "");

_Static_assert(sizeof(sizeof(0)) == 8, "");
_Static_assert(alignof(sizeof(0)) == 8, "");

_Static_assert(sizeof((char*)0 - 1) == 8, "");
_Static_assert(alignof((char*)0 - 1) == 8, "");


#endif
