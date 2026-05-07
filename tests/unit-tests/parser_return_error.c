
void f()
{
    return 1; //lint 1120 error: void function 'f' should not return a value 
}

int f2()
{
    return; //lint 1121 error: non void function 'f2' should return a value
}
