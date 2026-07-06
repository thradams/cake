/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);

void fa(int i)
{
    if (i == 2)
    {
        return;
    }
    printf("fa():   %d\n", i);
}


int fb(int i)
{
    if (i > 4)
    {
        return 4;
    }
    printf("fb():   %d\n", i);
    return 2;
}


int main(void)
{
    int i;

    fa(2);
    fa(1);
    i = fb(5);
    i = fb(i);
    printf("main(): %d\n", i);
}


