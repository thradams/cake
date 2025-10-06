extern int func(void);
static auto p_func1 = func;
auto p_func2 = func;
int main()
{
    p_func1 = 0;
    p_func2 = 0;
}