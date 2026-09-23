 //Obsolete implicitly octal literals
 //https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3319.htm

static_assert(0o52 == 052);
static_assert(0O52 == 052);
static_assert(0O52 == 42);

int main()
{
    int i = 0o52;
}
