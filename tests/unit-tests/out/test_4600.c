#pragma nullable enable
#pragma ownership enable

char * /*_Owner*/ _Opt f() {
    char * /*_Owner*/ _Opt p = 0;
    return p; /*implicit move*/
}