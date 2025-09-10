int main()
{

    int x = 1;

    //error: 'auto' requires a plain identifier
    [[cake::e1290]]
    auto* p = &x;

    *p = 0;
}

