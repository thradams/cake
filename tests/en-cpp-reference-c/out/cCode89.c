
void f(unsigned char   cond1, unsigned char   cond2)
{
    unsigned char   b;

    b = !!(cond1 && cond2);
    ;
}

int main(void)
{
    f(1, 1);
}

