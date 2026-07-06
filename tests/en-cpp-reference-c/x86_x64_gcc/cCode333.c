/* Cake x86_x64_gcc */

extern unsigned int long strcspn(char * __s, char * __reject);
extern unsigned int long strlen(char * __s);
extern int printf(char * __format, ...);

int main(void)
{
    char * string;
    char * invalid;
    unsigned int long valid_len;

    string = "abcde312$#@";
    invalid = "*$#";
    valid_len = strcspn(string, invalid);
    if (valid_len != strlen(string))
    {
        printf("'%s' contains invalid chars starting at position %zu\n", string, valid_len);
    }
}


