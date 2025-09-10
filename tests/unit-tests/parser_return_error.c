
void f()
{
    //error: void function 'f' should not return a value 
    [[cake::e1120]]
    return 1;
}

int f2()
{
    //error: non void function 'f2' should return a value
    [[cake::e1121]]
    return;
}
