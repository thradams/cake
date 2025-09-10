#pragma safety enable


void* _Owner _Opt malloc(int i);
void free(void* _Owner _Opt p);
int rand();

int main()
{
    char* _Owner _Opt s [[cake::leak]] = malloc(1);
    try
    {
        if (rand())
        {
            free(s);
        }
        else
        {
            //static_debug(s);
            throw;
        }
    }
    catch
    {
    }
}
