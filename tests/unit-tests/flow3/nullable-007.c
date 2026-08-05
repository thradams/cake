#pragma nullable enable

char* _Opt make();

char * returns_non_null()
{
    char* _Opt p1 = make();
    return p1; //lint 35 passing a possible null pointer 'p1' to non-nullable pointer parameter (see line 7)

}

