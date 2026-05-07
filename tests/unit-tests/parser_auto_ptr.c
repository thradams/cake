int main()
{

    int x = 1;

    //error: 'auto' requires a plain identifier

    auto* p = &x; //lint 1290

    *p = 0;
}

