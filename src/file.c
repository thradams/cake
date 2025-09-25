typedef unsigned short word;
#define __near
#define derefw(x) (*((__near word*)(x)))
void main()
{
    derefw(0xe0e0) = (0x30);
}