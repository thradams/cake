/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

int f();

int main()
{
    int i;

    if (1) /*try*/
    {
        if (f())
        {
            i = 1;
            goto __L0; /* throw */
        }
        i = 0;
    }
    else __L0: /*catch*/ 
    {
    }
}
