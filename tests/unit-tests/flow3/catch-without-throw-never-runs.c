#pragma safety enable

/* a catch runs only after a throw of its try: with no throw it is not an outcome of the statement */

int get(void);

void catch_without_throw(void)
{
    int n = 0;
    try
    {
        n = 1;
    }
    catch
    {
        n = 2; //lint 68
    }
    compile_assert(n == 1);
}

void catch_with_throw(void)
{
    int n = 0;
    try
    {
        if (get())
            throw;
        n = 1;
    }
    catch
    {
        n = 2;
    }
    compile_assert(n == 1 || n == 2);
    compile_assert(n == 1); //lint 67
}

void throw_of_inner_try_is_not_of_the_outer(void)
{
    int n = 0;
    try
    {
        try
        {
            if (get())
                throw;
        }
        catch
        {
        }
        n = 1;
    }
    catch
    {
        n = 2; //lint 68
    }
    compile_assert(n == 1);
}

void catch_without_throw_and_try_returns(void)
{
    int n = 0;
    try
    {
        n = 1;
        return;
    }
    catch
    {
        n = 2; //lint 68
    }
    n = 3; //lint 68
}
