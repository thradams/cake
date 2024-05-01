#pragma safety enable

int strcmp(const char* s1, const char* s2);
char* _Opt f();

int main()
{
    const char* _Opt s = f();
    if (s && strcmp(s, "a") == 0)
    {
    }
    else if (s)
    {
    }
}

