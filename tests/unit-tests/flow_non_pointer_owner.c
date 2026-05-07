#pragma safety enable


void destroy(_Owner int i);

int main()
{
    _Owner int i = 0;
    int v = i;

    //warning: cannot assign a non-owner to owner

    destroy(v); //lint 25

} //lint 29



