/* `h->x` asks the same question as `*h`: a nullable file scope pointer holds any value of its type (A14.2) */
#pragma safety enable

struct X { int x; };
struct X* h;

void f(void)
{
    h->x = 1; //lint 33
}
