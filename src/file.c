#pragma ownership enable

void* _Owner malloc(unsigned long size);
void free(void* _Owner ptr);

void f(char s[12]);

int main()
{
    char s[21];
    f(s);
    void* _Owner p = malloc(1);
    //return 0;
    //free(p);
}
