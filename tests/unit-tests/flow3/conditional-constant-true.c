/* the false arm of 1 ? a : b is dead */

void f(const char* s);

void g(void)
{
    char* p;
    char* q = 1 ? "" : p;
    f(q);
}
