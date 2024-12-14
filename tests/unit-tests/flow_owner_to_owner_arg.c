#pragma safety enable


char * _Owner create();
void f(char * _Owner p);

int main()
{
    f(create());
}

