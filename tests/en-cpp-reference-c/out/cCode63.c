struct _iobuf {
    void * _Placeholder;
};


char str[20];
struct _iobuf *__cdecl fopen(char * _FileName, char * _Mode);
int __cdecl fputc(int _Character, struct _iobuf * _Stream);
int __cdecl fclose(struct _iobuf * _Stream);
unsigned int __cdecl fread(void * _Buffer, unsigned int _ElementSize, unsigned int _ElementCount, struct _iobuf * _Stream);
int __cdecl puts(char * _Buffer);
void __cdecl rewind(struct _iobuf * _Stream);

int main(void)
{
    struct _iobuf * f;
    char ch;

    f = fopen("file.txt", "w");
    for (ch = 48; ch <= 57; ch++)
    {
        fputc(ch, f);
    }
    fclose(f);
    f = fopen("file.txt", "r");
    fread(str, 1, 10, f);
    puts(str);
    rewind(f);
    fread(str, 1, 10, f);
    puts(str);
    fclose(f);
    return 0;
}

