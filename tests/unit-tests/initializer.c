struct A { int x, y; };
struct B { struct A a; };

int main(void)
{
    struct A ia = { 1, 2 };
    struct B b = { .a = ia, .a.y = 42 };
}