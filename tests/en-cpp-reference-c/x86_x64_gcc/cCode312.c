/* Cake x86_x64_gcc */

extern unsigned int short **__ctype_b_loc(void);
extern int printf(char * __format, ...);

int main(void)
{
    {
        int ndx;

        ndx = 0;
        for (; ndx <= 255; ndx++)
        if (((*__ctype_b_loc())[(int)(ndx)] & 1))
        {
            printf("0x%02x\n", ndx);
        }
    }
}


