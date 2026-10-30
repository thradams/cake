#pragma safety enable
#pragma flow enable

char* make();

char * _Nonnull returns_non_null()
{
    char* p1 = make();
    return p1; //lint 35 passing a possible null pointer 'p1' to non-nullable pointer parameter (see line 7)

}

