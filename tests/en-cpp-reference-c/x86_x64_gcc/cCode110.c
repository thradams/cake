/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);

int main(void)
{
    {
        int x;

        x = 0;
        for (; x < 3; x++)
        {
            {
                int y;

                y = 0;
                for (; y < 3; y++)
                {
                    printf("(%d;%d)\n", x, y);
                    if (x + y >= 3)
                    {
                        goto endloop;
                    }
                }
            }
        }
    }
    endloop:
    ;
}


