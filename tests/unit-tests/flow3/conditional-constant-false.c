/* the true arm of 0 ? a : b is dead */

void f(const char* s);

void g(void)
{
    char* p;
    char* q = 0 ? p : "";
    f(q);
}
