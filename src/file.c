struct X { int i; void* p; };
void f(struct X* p) {}

int main() 
{
    const struct X x;
    f(&x);
}

