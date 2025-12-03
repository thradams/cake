/* Cake x86_msvc */

int f(int condition)
{
    int * p;

    p = 0;
    if (1) /*try*/
    {
        int * p2;

        p2 = p;
        if (condition)
        {
            goto __L0; /* throw */
        }
        p = 0;
    }
    else __L0: /*catch*/ 
    {
    }
}


