/* Cake x86_msvc */

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


