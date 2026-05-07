
#pragma safety enable

void* _Owner _Opt malloc(unsigned long size);
void free(void* _Owner ptr);

int main()
{
    void* _Owner  _Opt p = malloc(1);
    if (p)
    {
       static_debug(p);
    }

    static_debug(p);
}

