#pragma safety enable

/* h = h || c: h was 0, now it may be 1 */
int f(int c)
{
    bool h = false;
    h = h || c;
    if (!h)
        return 1;
    return 0;
}

/* h = h && c: h was 1, now it may be 0 */
int g(int c)
{
    bool h = true;
    h = h && c;
    if (h)
        return 1;
    return 0;
}
