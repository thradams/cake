#pragma safety enable
int f(int condition)
{
    int * /*_Owner*/ /*_Opt*/ p = 0;
    if (1) /*try*/
    {
        int * /*_Owner*/ /*_Opt*/ p2 = p;

        //p is still null here because null pointers are not moved.
        

        if (condition) goto _catch_label_1;
        p = 0;
    }
    else _catch_label_1:
    {
    }
    
    
}
