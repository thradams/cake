/* after the call the conditions that use _result hold */
int g(int a) compile_assert(_result > 0);

void h(void)
{
    int r = g(1);
    compile_assert(r > 0);
}
