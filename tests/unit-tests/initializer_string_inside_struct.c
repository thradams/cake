struct X
{
     int i;
     char text[4];
};

constexpr struct X x = {1, "abc"};
static_assert(x.text[0] == 'a');
static_assert(x.text[1] == 'b');
static_assert(x.text[2] == 'c');
static_assert(x.text[3] == '\0');
