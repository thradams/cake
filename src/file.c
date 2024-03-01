
void* _Owner malloc(unsigned long size);
void free(void* _Owner ptr);

int main()
{
    void* _Owner p = malloc(1);
    static_debug(p);
    free(p);
    static_debug(p);
}
