#pragma safety enable

struct X {
    int* /*_Opt*/ pi;
};

int* f();
int main(struct X* /*_Opt*/ pX)
{
    if (pX)
    {
        
    }
    else
    {
        
    }

    if (!pX)
    {
        
    }
    else
    {
        
    }

    if (pX && pX->pi)
    {
        
        
    }
    else
    {
        
        
        
    }

    if (pX == 0 || (pX->pi = f()) == 0)
    {
        
    }
    else
    {
        
    }

    
}