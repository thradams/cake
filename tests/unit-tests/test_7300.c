#pragma nullable enable
#pragma ownership enable

void* _Owner _Opt malloc(int i);
void free(void* _Owner _Opt p);
int rand();

int main()
{
    char* _Owner _Opt s = malloc(1);
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
