#pragma safety enable


char * _Nonnull _Owner create();
void f(char * _Owner p);

int main()
{
    f(create());
}
