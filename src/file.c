//#include <stdio.h>

struct { bool success; int value; } parse(const char* s)
{
    if (s == 0)
        return (typeof(parse(0))) { false, 1 };

    return (typeof(parse(0))) { true, 1 };
};

int main()
{
    auto r = parse("1");
    if (r.success)
    {
       // printf("%d", r.value);
    }
}