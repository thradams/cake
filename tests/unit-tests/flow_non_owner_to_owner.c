#pragma safety enable



void* f();
int main()
{
    //cannot assign a non-owner to owner
    [[cake::w25]]
    void* _Owner p [[cake::leak]] = f();
}


