
#pragma default_nonnull
#pragma check_annotations enable
#pragma flow enable

void f(int i)
{

    int a;
    switch(i)
    {
        case 1:
            a = 1;
        break;

        case 2:
            a = 2;
        break;
        
        default:
            a = 3;
    }
    compile_assert(a == 1 || a == 2 || a == 3);
}


