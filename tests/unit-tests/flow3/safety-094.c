#pragma safety enable


int* _Nonnull f();
int main()
{
    int* p = f();


    if (p) // pointer is always not-null
        return 0;

}
