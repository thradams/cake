#pragma nullable enable
#pragma ownership enable


void * /*_Owner*/ /*_Opt*/ calloc(int n , int i);

char* /*_Owner*/ /*_Opt*/ f(int i)
{
    char* /*_Owner*/ /*_Opt*/ p = calloc(1,2);        
    switch (i) {
        case 1: break;
    }
    return p;
}