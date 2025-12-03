/* Cake x86_msvc */

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


