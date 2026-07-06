/* Cake x86_x64_gcc */

extern void srand(unsigned int __seed);
extern int long time(int long * __timer);
extern int rand(void);
extern int printf(char * __format, ...);

int main(void)
{
    int random_variable;

    srand(time(0));
    random_variable = rand();
    printf("Random value on [0,%d]: %d\n", 2147483647, random_variable);
    {
        int n;

        n = 0;
        for (; n != 20; ++n)
        {
            int x;

            x = 7;
            while (x > 6)
            x = 1 + rand() / 357913941;
            printf("%d ", x);
        }
    }
}


