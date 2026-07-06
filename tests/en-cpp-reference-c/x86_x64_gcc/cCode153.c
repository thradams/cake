/* Cake x86_x64_gcc */
union pad {
    char c[5];
    float f;
};

union S {
    unsigned int u32;
    unsigned int short u16[2];
    unsigned char u8;
};


extern int printf(char * __format, ...);

int main(void)
{
    union S  s;
    union pad  p;

    s.u32 = 305419896;
    printf("Union S has size %zu and holds %x\n", 4L, s.u32);
    s.u16[0] = 17;
    ;
    p.c = 0;
    p.f = 1.23;
    printf("size of union of char[5] and float is %zu\n", 8L);
}


