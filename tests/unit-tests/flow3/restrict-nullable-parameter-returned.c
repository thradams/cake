#pragma safety enable

int* first(int* _Nullable restrict p, int* q)
{
    if (p == 0)
    {
        return q;
    }
    return p; //lint 102
}
