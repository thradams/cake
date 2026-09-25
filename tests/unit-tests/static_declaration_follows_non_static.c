void compute(void);
static void compute(void) {} //lint 1020 static declaration of 'compute' follows non-static declaration

int main(void)
{
    compute();
    return 0;
}
