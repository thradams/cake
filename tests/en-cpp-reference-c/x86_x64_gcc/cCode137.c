/* Cake x86_x64_gcc */
struct s {
    int x;
};


extern int printf(char * __format, ...);

int main(void)
{
    struct s  s;
    struct s * p;

    s.x = 1;
    p = &s;
    p->x = 7;
    printf("%d\n", p->x);
}


