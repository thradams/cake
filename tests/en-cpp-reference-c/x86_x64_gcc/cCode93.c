/* Cake x86_x64_gcc */
struct S {
    unsigned int b;
};


extern int printf(char * __format, ...);

int main(void)
{
    struct S  s;

    s.b = 7;
    ++s.b;
    printf("%d\n", s.b);
}


