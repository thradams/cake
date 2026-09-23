
#include <stdio.h>
int main(void)
{
    printf("value=%ld", 2147483647L + 10);

    #ifdef _WIN32
    //use option : -target=x86_msvc
    static_assert(-2147483639 == 2147483647L + 10, "");
    #else
    //use option : -target=x86_x64_gcc
    static_assert(2147483657LL == 2147483647L + 10, "");
    #endif

    auto i32_max = 2'147'483'647;
    auto u32_max = 4'294'967'295;
    auto i64_max = 9'223'372'036'854'775'807;
    auto u64_max = 18'446'744'073'709'551'615;

}
