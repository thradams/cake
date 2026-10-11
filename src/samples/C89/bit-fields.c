/*
  Bit-fields

  By default cake keeps bit-fields in the generated code.

  With -no-bitfields, cake replaces them with plain integer members
  and does every read and write with shifts and masks. The layout
  (size, alignment, bit positions) is the one of the target, so the
  generated code does not depend on how the backend compiler lays out
  bit-fields.

  For this sample, -no-bitfields -target=x86_64-linux-gnu-gcc gives:

    struct X {
        unsigned int __bits0;
    };

    x.a = 5;    -> (x.__bits0 = (x.__bits0 & 0xfffffff8U) | 0x5U);
    x.b = 300;  -> (x.__bits0 = (x.__bits0 & 0xfffff007U) | 0x960U);
    x.a + x.b   -> ((int)(x.__bits0 & 0x7U)) + ((int)((x.__bits0 >> 3) & 0x1ffU))
*/

struct X {
    unsigned char a : 3;
    unsigned int b : 9;
};

#ifdef _MSC_VER
static_assert(sizeof(struct X) == 8);
static_assert(alignof(struct X) == 4);
#else
static_assert(sizeof(struct X) == 4);
static_assert(alignof(struct X) == 4);
#endif

int main()
{
    struct X x = { 0 };
    x.a = 5;
    x.b = 300;
    return x.a + x.b;
}
