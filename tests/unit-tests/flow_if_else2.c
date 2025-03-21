#pragma safety enable


int rand();
void free(void* _Owner ptr);

struct X { char* _Owner _Opt name; };

void x_destroy(_Dtor struct X*  p);
struct X f();

void f()
{
    {
        struct X x = {0};   
        
        if (rand())
        {            
            x = f();
        }
        else
        {         
            x = f();
        }
        x_destroy(&x);
    }
}
