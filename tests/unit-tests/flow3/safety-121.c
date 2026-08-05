#pragma safety enable

void f(_Ctor char s[2]) 
{
}

void test1()
{
    char s[2];
    f(s); // ok: f's parameter is _Ctor, so it constructs s (passing uninitialized is correct)
}

void f2(_Ctor char *s) 
{
    *s= '\0';
}

void test2()
{
    char s[2];
    f2(s);
}

int main()
{
}
