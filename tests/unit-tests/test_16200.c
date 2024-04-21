#pragma nullable enable
#pragma ownership enable


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

    static_state(i ,"uninitialized");
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

    static_state(i ,"uninitialized or not-zero");
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

    static_state(i ,"uninitialized or zero or not-zero");
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

    static_state(i ,"uninitialized or not-zero");
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

    static_state(i ,"uninitialized or zero");
}


int main()
{

}
