/* Cake x86_x64_gcc */

extern unsigned int short **__ctype_b_loc(void);
extern int printf(char * __format, ...);

int main(void)
{
    {
        int ndx;

        ndx = 0;
        for (; 255 >= ndx; ++ndx)
        if (((*__ctype_b_loc())[(int)(ndx)] & 4096))
        {
            printf("%c", ndx);
        }
    }
    printf("\n");
}


