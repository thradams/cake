#pragma safety enable

int strcmp(const char* s1, const char* s2);
char* f();

int main()
{
    const char* s = f();
    if (s && strcmp(s, "a") == 0)
    {
    }
    else if (s)
    {
    }
}
