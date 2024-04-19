
int* _Owner  get();

void f() 
{
    int* _Owner p = 0;
    p = get();
}


#pragma cake diagnostic check "-Wmissing-destructor"

