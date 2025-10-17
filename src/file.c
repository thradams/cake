struct X { int i; char s[32]; };
    struct X x0 = { .i = 1, .s = "ad" };
int main()
{
    bool b = true;

    struct X x = {};
    struct X x2 = { .i = 1, .s = "ad" };
    x0;
    b;
}