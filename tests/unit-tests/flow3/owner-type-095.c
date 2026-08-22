#pragma safety enable



void* f();
int main()
{
    //cannot assign a non-owner to owner
    void* _Owner p  = f(); //lint 78 cannot assign a non-owner to owner
} //lint 29 owner object (p) not moved (root see line 9)
