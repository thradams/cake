/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);

int main(void)
{
    unsigned int a;
    unsigned int short mask;

    a = 305419896;
    mask = 240;
    printf("Promoted mask:\t%#010x\nValue:\t\t%#x\nSetting bits:\t%#x\nClearing bits:\t%#x\nSelecting bits:\t%#010x\n", mask, a, a | mask, a & ~mask, a & mask);
}


