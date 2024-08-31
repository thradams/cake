#pragma safety enable

void f(_Out char s[2]) 
{
}

void test1()
{
    char s[2];
    f(s);
}

void f2(_Out char *s) 
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