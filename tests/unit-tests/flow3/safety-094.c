#pragma safety enable


int* f();
int main()
{
    int* _Opt p = f();


    if (p) // pointer is always not-null
        return 0;

}
