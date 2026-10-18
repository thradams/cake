/* static const keeps its value: the false arm of K ? a : b is dead */

static const int K = 1;
void f(const char* s);

void g(void)
{
    char* p;
    char* q = K ? "" : p;
    f(q);
}
