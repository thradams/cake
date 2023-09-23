struct X{
int i;
};
void f(struct X* const x){}
int main()
{
    struct X x;
    f(&x);
}


