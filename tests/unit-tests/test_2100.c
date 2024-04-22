void (*f[2][3])(int i);
int main()
{
    static_assert(sizeof(void (*[2])(int i)) == sizeof(void *) * 2);
    static_assert(sizeof(f) == sizeof(void (*[2])(int i)) * 3);
}
