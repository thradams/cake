#pragma safety enable

void f(int* p)
{

    if (p != 0) //lint 28 pointer is always non-null   
    {
    } 

    //pointer is always non-null

    if (0 != p)  //lint 28 pointer is always non-null   
    {}

    //pointer is always non-null

    if (p == 0)  //lint 28 pointer is always non-null   
    {}


    if (0 == p)  //lint 28 pointer is always non-null   
    {}
}
