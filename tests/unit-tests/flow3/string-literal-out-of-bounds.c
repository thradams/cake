void f(int c)
{
    const char* s = "abc";
    if (c)
    {
        s = "abcdefgh";
    }
    char c1 = s[2];
    char c2 = s[6]; //lint 70 /* past "abc" only */
    char c3 = s[9]; //lint 70
}
