struct _iobuf {
    void * _Placeholder;
};


struct _iobuf *__cdecl fopen(char * _FileName, char * _Mode);
void __cdecl perror(char * _ErrorMessage);
int __cdecl fclose(struct _iobuf * _Stream);

int main(void)
{
    struct _iobuf * f;

    f = fopen("non_existent", "r");
    if (f == 0U)
    {
        perror("fopen() failed");
    }
    else
    {
        fclose(f);
    }
}

