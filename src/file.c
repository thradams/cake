char((a1));
char b1;
static_assert((typeof(a1)) == (typeof(b1)));

char((a2))[2];
char b2[2];
static_assert((typeof(a2)) == (typeof(b2)));

char((a3))(int(a));
char (b3)(int a);
static_assert((typeof(a3)) == (typeof(b3)));
