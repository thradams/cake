/* Cake x86_x64_gcc */

extern void srand(unsigned int __seed);
extern int long time(int long * __timer);
extern int printf(char * __format, ...);
extern int rand(void);

int main(void)
{
    srand(time(0));
    printf("RAND_MAX: %i\n", 2147483647);
    printf("INT_MAX: %i\n", 2147483647);
    printf("Random value on [0,1]: %f\n", (double)rand() / 2147483647);
}


