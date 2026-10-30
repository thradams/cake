#pragma safety enable


struct X { char  * _Nonnull _Owner owner_variable;   };
char * _Nonnull f(struct X *parameter) 
{
    return parameter->owner_variable;
}
