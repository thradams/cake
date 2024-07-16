//en.cppreference.com/w/c/language/union.html
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
 
int main(void)
{
    union S
    {
        uint32_t u32;
        uint16_t u16[2];
        uint8_t  u8;
    } s = {0x12345678}; // s.u32 is now the active member
    printf("Union S has size %zu and holds %x\n", sizeof s, s.u32);
    s.u16[0] = 0x0011;  // s.u16 is now the active member
    // reading from s.u32 or from s.u8 reinterprets the object representation
//  printf("s.u8 is now %x\n", s.u8); // unspecified, typically 11 or 00
//  printf("s.u32 is now %x\n", s.u32); // unspecified, typically 12340011 or 00115678
 
    // pointers to all members of a union compare equal to themselves and the union
    assert((uint8_t*)&s == &s.u8);
 
    // this union has 3 bytes of trailing padding
    union pad
    {
       char  c[5];   // occupies 5 bytes
       float f;      // occupies 4 bytes, imposes alignment 4
    } p = {.f = 1.23}; // the size is 8 to satisfy float's alignment
    printf("size of union of char[5] and float is %zu\n", sizeof p);
}