#pragma safety enable
#pragma flow enable

char* make();
void receives_non_null(char* p);

int main()
{
    char* p1 = make();
    receives_non_null(p1); //lint 35 passing a possible null pointer 'p1' to non-nullable pointer parameter (see line 8)
}

