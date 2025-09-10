#pragma safety enable


void destroy(_Owner int i);

int main()
{
    _Owner int i [[cake::leak]] = 0;
    int v = i;

    //warning: cannot assign a non-owner to owner
    [[cake::w25]]
    destroy(v);

}



