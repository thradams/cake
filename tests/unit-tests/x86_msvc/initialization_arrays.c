/* Cake 0.12.26 x86_msvc */
struct __cX19 {
    int a;
    int b;
    int c;
};

struct __tag0 {
    int a[3];
    int b;
};

struct __cX17 {
    int a;
    int b;
    int c;
};

struct data {
    int nr;
    char * value;
};

struct __cX15 {
    int a;
    int b;
    int c;
};

struct __cX8 {
    int i;
};

struct __cY9 {
    int i;
    struct __cX8  x;
};

struct __cY6 {
    int a;
    int ar[3];
    int b;
};

struct X {
    int a;
    int b;
};

struct Y {
    int i;
    struct X  x;
    struct X  x2;
};


static void _cake_zmem(void *dest, unsigned int len)
{
  unsigned char *ptr;

  ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

static void _cake_memcpy(void * dest, const void * src, unsigned int n)
{
  char *csrc;
  char *cdest;
  unsigned int i; 

  csrc = (char *)src;
  cdest = (char *)dest;
  for (i = 0; i < n; i++) cdest[i] = csrc[i]; 
}

void T00()
{
    int i[2];
    int i2[2];
    int i3[2];
    int i5;
    int i4;

    i[0] = 1;
    i[1] = 2;
    i2[0] = 1;
    i2[1] = 0;
    i3[0] = 1;
    i3[1] = 0;
    i5 = 1;
    i4 = 1;
}


void T0()
{
    int i3;
    int i;
    int i2;

    i3 = 3;
    i = 1;
    i2 = 2;
}


void T1()
{
    struct Y  y;

    y.i = 1;
    y.x.a = 1;
    y.x.b = 2;
    y.x2.a = 3;
    y.x2.b = 0;
}


void T2()
{
    struct __cY6  y;

    y.a = 0;
    y.ar[0] = 0;
    y.ar[1] = 2;
    y.ar[2] = 3;
    y.b = 4;
}


void T3()
{
    struct __cX8  x;
    struct __cY9  y;

    _cake_zmem(&x, 4);
    y.i = 4;
    _cake_memcpy(&y.x, &x, 4);
}


void T4()
{
    short q4[4][3][2];
    short q1[4][3][2];
    short q2[4][3][2];
    short q3[4][3][2];

    q4[0][0][0] = 1;
    q4[0][0][1] = 0;
    q4[0][1][0] = 0;
    q4[0][1][1] = 0;
    q4[0][2][0] = 0;
    q4[0][2][1] = 0;
    q4[1][0][0] = 2;
    q4[1][0][1] = 3;
    q4[1][1][0] = 0;
    q4[1][1][1] = 0;
    q4[1][2][0] = 0;
    q4[1][2][1] = 0;
    q4[2][0][0] = 4;
    q4[2][0][1] = 5;
    q4[2][1][0] = 6;
    q4[2][1][1] = 0;
    q4[2][2][0] = 0;
    q4[2][2][1] = 0;
    q4[3][0][0] = 0;
    q4[3][0][1] = 0;
    q4[3][1][0] = 0;
    q4[3][1][1] = 0;
    q4[3][2][0] = 0;
    q4[3][2][1] = 0;
    q1[0][0][0] = 1;
    q1[0][0][1] = 0;
    q1[0][1][0] = 0;
    q1[0][1][1] = 0;
    q1[0][2][0] = 0;
    q1[0][2][1] = 0;
    q1[1][0][0] = 2;
    q1[1][0][1] = 3;
    q1[1][1][0] = 0;
    q1[1][1][1] = 0;
    q1[1][2][0] = 0;
    q1[1][2][1] = 0;
    q1[2][0][0] = 4;
    q1[2][0][1] = 5;
    q1[2][1][0] = 6;
    q1[2][1][1] = 0;
    q1[2][2][0] = 0;
    q1[2][2][1] = 0;
    q1[3][0][0] = 0;
    q1[3][0][1] = 0;
    q1[3][1][0] = 0;
    q1[3][1][1] = 0;
    q1[3][2][0] = 0;
    q1[3][2][1] = 0;
    q2[0][0][0] = 1;
    q2[0][0][1] = 0;
    q2[0][1][0] = 0;
    q2[0][1][1] = 0;
    q2[0][2][0] = 0;
    q2[0][2][1] = 0;
    q2[1][0][0] = 2;
    q2[1][0][1] = 3;
    q2[1][1][0] = 0;
    q2[1][1][1] = 0;
    q2[1][2][0] = 0;
    q2[1][2][1] = 0;
    q2[2][0][0] = 4;
    q2[2][0][1] = 5;
    q2[2][1][0] = 6;
    q2[2][1][1] = 0;
    q2[2][2][0] = 0;
    q2[2][2][1] = 0;
    q2[3][0][0] = 0;
    q2[3][0][1] = 0;
    q2[3][1][0] = 0;
    q2[3][1][1] = 0;
    q2[3][2][0] = 0;
    q2[3][2][1] = 0;
    q3[0][0][0] = 1;
    q3[0][0][1] = 0;
    q3[0][1][0] = 0;
    q3[0][1][1] = 0;
    q3[0][2][0] = 0;
    q3[0][2][1] = 0;
    q3[1][0][0] = 2;
    q3[1][0][1] = 3;
    q3[1][1][0] = 0;
    q3[1][1][1] = 0;
    q3[1][2][0] = 0;
    q3[1][2][1] = 0;
    q3[2][0][0] = 4;
    q3[2][0][1] = 5;
    q3[2][1][0] = 6;
    q3[2][1][1] = 0;
    q3[2][2][0] = 0;
    q3[2][2][1] = 0;
    q3[3][0][0] = 0;
    q3[3][0][1] = 0;
    q3[3][1][0] = 0;
    q3[3][1][1] = 0;
    q3[3][2][0] = 0;
    q3[3][2][1] = 0;
}


void T5()
{
    int a[2][3];

    a[0][0] = 1;
    a[0][1] = 2;
    a[0][2] = 3;
    a[1][0] = 4;
    a[1][1] = 5;
    a[1][2] = 6;
}


void T6()
{
    char s[4];
    char s2[4];

    _cake_memcpy(s, "123", 4);
    s2 = s;
}


void T7()
{
    struct __tag0 w[2];

    w[0].a[0] = 1;
    w[0].a[1] = 0;
    w[0].a[2] = 0;
    w[0].b = 0;
    w[1].a[0] = 2;
    w[1].a[1] = 0;
    w[1].a[2] = 0;
    w[1].b = 0;
}


void T8()
{
    struct data dat[4];

    dat[0].nr = 1;
    dat[0].value = "Foo";
    dat[1].nr = 2;
    dat[1].value = "Bar";
    dat[2].nr = 3;
    dat[2].value = "Hello";
    dat[3].nr = 4;
    dat[3].value = "World";
}


void T9()
{
    int x[3];
    int * p;
    int a[3];

    x[0] = 1;
    x[1] = 3;
    x[2] = 5;
    p = x;
    a[0] = 1;
    a[1] = 2;
    a[2] = 3;
}


void T10()
{
    int a[3];

    a[0] = 3;
    a[1] = 2;
    a[2] = 1;
}


void T11()
{
    struct __cX15  x;

    x.a = 0;
    x.b = 2;
    x.c = 3;
}


void T12()
{
    struct __cX17  x;

    x.a = 0;
    x.b = 0;
    x.c = 3;
}


void T13()
{
    struct __cX19  x;

    _cake_zmem(&x, 12);
}


