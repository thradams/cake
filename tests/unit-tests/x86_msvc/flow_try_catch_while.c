/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:29 */

struct X {
    char * name;
};

void *malloc(unsigned int size);

struct X *F(int i)
{
    struct X * p1;

    p1 = 0;
    if (1) /*try*/
    {
        if (i == 1)
        {
            p1 = malloc(4);
            while (0)
            {
            }
        }
        else
        {
            if (i == 3)
            {
                p1 = malloc(4);
            }
        }
    }
    else __L0: /*catch*/ 
    {
    }
    return p1;
}
