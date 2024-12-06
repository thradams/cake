constexpr char s[] = "abc";
static_assert(s[0] == 'a');
static_assert(s[1] == 'b');
static_assert(s[2] == 'c');
static_assert(s[3] == '\0');



constexpr char s2[] = {"abc"};
static_assert(s2[0] == 'a');
static_assert(s2[1] == 'b');
static_assert(s2[2] == 'c');
static_assert(s2[3] == '\0');


