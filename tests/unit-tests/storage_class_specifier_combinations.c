/* C23 6.7.1p2: storage-class specifiers that may be combined */

void f(void)
{
    static constexpr int a = 0;
    auto constexpr int b = 0;
    register constexpr int c = 0;
    auto static int d = 0;
    static thread_local int e;
}

extern constexpr int bad1 = 0; //lint 2080
static extern int bad2; //lint 2080
