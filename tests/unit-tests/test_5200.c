#pragma nullable enable
#pragma ownership enable

char * _Owner create();
void f(char * _Owner p);

int main()
{
    f(create());
}

