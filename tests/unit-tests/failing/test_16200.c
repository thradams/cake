//external var problem

#pragma safety enable



struct X {
    char* p;
};


int condition;

void f1()
{
    int i;
    try
    {
        if (condition) {
            i = 1;
            throw;
        }
        if (condition) {
            throw;
        }
    }
    catch
    {
        return;
    }

    assert_state(i ,"uninitialized");
}

void f2()
{
    int i;
    try
    {
        if (condition) {
            i = 1;
            throw;
        }
        if (condition) {
            throw;
        }
    }
    catch
    {        
    }

    assert_state(i ,"uninitialized | not-zero");
}

void f2()
{
    int i;
    try
    {
        if (condition) {
            i = 1;
            throw;
        }
        if (condition) {
            throw;
        }
        i = 0;
    }
    catch
    {        
    }

    assert_state(i ,"uninitialized | zero | not-zero");
}

void f3()
{
    int i;
    try
    {
        if (condition) {
            i = 1;
            throw;
        }
        if (condition) {
            throw;
        }
        i = 0;
        return;
    }
    catch
    {        
    }

    assert_state(i ,"uninitialized | not-zero");
}

void f4()
{
    int i;
    try
    {
        if (condition) {            
            throw;
        }
        if (condition) {
            throw;
        }       
    }
    catch
    {        
        i = 0;
    }

    assert_state(i ,"uninitialized | zero");
}


int main()
{

}
