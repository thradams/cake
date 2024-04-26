#pragma safety enable


int strlen(const char* s);

int main()
{
    strlen(0);
#pragma cake diagnostic check "-Wnullable-to-non-nullable"
    char * /*_Opt*/ p = 0;
    strlen(p);
#pragma cake diagnostic check "-Wnullable-to-non-nullable"
}

