int main()
{
    goto a;
    a:;

    //error: duplicated label 'a'
    [[cake::e1440]]
    a:;
}


