#pragma nullable enable
#pragma ownership enable

int rand();
void free(void* _Owner ptr);

struct X { char* _Owner name; };

void x_destroy(struct X* _Obj_owner p);
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
