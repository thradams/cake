
char *__cdecl strdup(char * _String);
void __cdecl free(void * _Block);

int main(void)
{
    char * s1;
    char * s2;

    s1 = "String";
    s2 = strdup(s1);
    ;
    free(s2);
}

