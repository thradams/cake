#pragma nullable enable
#pragma ownership enable

struct X { char  * /*_Owner*/ /*_Opt*/ owner_variable;   };
struct X global;
char * /*_Opt*/ f() 
{
    return global.owner_variable;
}
