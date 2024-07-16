#pragma safety enable


int* /*_Owner*/  get();

void f()
{
    int* /*_Owner*/ /*_Opt*/ p = 0;
    p = get();
}


#pragma cake diagnostic check "-Wmissing-destructor"
