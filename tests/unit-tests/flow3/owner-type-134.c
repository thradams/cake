#pragma safety enable


struct X { char  * _Owner owner_variable;   };
struct X global;
char * f() 
{
    return global.owner_variable;
}
