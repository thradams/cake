#pragma nullable enable
#pragma ownership enable

struct X { char  * /*_Owner*/ _Opt owner_variable;   };
struct X global;
char * f() 
{
    return global.owner_variable;
}
