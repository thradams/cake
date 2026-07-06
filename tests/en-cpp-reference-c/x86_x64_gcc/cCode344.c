/* Cake x86_x64_gcc */

extern char *strstr(char * __haystack, char * __needle);
extern int printf(char * __format, ...);

void find_str(char * str, char * substr)
{
    char * pos;

    pos = strstr(str, substr);
    pos ? printf("found the string '%s' in '%s' at position %td\n", substr, str, pos - str) : printf("the string '%s' was not found in '%s'\n", substr, str);
}


int main(void)
{
    char * str;

    str = "one two three";
    find_str(str, "two");
    find_str(str, "");
    find_str(str, "nine");
    find_str(str, "n");
    return 0;
}


