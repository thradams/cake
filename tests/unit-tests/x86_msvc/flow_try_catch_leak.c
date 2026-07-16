/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:29 */

void *malloc(int i);
int rand();
void free(void * p);

int main()
{
    char * s;

    s = malloc(1);
    if (1) /*try*/
    {
        if (rand())
        {
            free(s);
        }
        else
        {
            goto __L0; /* throw */
        }
    }
    else __L0: /*catch*/ 
    {
    }
}
