#pragma safety enable

void* _Owner malloc(unsigned long n);
void free(void* _Owner _Opt p);

/* issue 487: the state of case 1 falls through into case 2 */
void f(int c)
{
    char* _Owner p = 0;
    switch (c)
    {
        case 1:
            p = malloc(1);
            [[fallthrough]];
        case 2:
            p = malloc(2); //lint 26 -> malloc(1) is lost when c == 1
            break;
    }
    free(p);
}

/* no fall-through: no warning */
void g(int c)
{
    char* _Owner p = 0;
    switch (c)
    {
        case 1:
            p = malloc(1);
            break;
        case 2:
            p = malloc(2);
            break;
        default:
            p = malloc(3);
            break;
    }
    free(p);
}

/* if/else where both arms return: no fall-through into case 2 */
void h(int c, int d)
{
    char* _Owner p = 0;
    switch (c)
    {
        case 1:
            p = malloc(1);
            if (d) { free(p); return; }
            else { free(p); return; }
        case 2:
            p = malloc(2);
            break;
    }
    free(p);
}
