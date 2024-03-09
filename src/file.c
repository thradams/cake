void* _Owner malloc(int i);
void free(void* _Owner _Opt p);
int rand();

int main()
{
    char* _Owner s = malloc(1);
    try
    {
        if (rand())
        {
            free(s);
        }
        else
        {
            static_debug(s);
            throw;
        }
    }
    catch
    {
    }    
}
#pragma cake diagnostic check "-Wmissing-destructor"
