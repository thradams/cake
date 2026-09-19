/* https://github.com/thradams/cake/issues/353 */

int f(int i)
{
    int i; //lint 1020 redeclaration
    return 0;
}

int g(int i)
{
    {
        int i = 1; //lint 7 /* nested block may shadow */
        (void)i;
    }
    return i;
}
