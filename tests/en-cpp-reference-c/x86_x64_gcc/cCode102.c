/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);

int main(void)
{
    {
        int i;

        i = 0;
        for (; i < 10; i++)
        {
            if (i != 5)
            {
                continue;
            }
            printf("%d ", i);
        }
    }
    printf("\n");
    {
        int j;

        j = 0;
        for (; j < 2; j++)
        {
            {
                int k;

                k = 0;
                for (; k < 5; k++)
                {
                    if (k == 3)
                    {
                        continue;
                    }
                    printf("%d%d ", j, k);
                }
            }
        }
    }
}


