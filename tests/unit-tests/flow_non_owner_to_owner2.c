#pragma safety enable

void* f();

int main()
{
    //cannot assign a non-owner to owner

    void* _Owner p = f(); //lint 25
} //lint 29


