/* issue 521: the dead arm of if (0) does not reach the code after the if */

void f(const char* s);

void g(void)
{
    char* p;
    if (0) //lint 44
        ;
    else
        p = "";
    f(p);
}
