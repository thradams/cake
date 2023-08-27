void* owner malloc(int i);
void free(void* owner p);

int main()
{
    int* owner p1 = 0;
    int* owner p2 = malloc(1);

    try
    {
        if (p2 == 0)
        {
            throw;
        }
        else
        {
            p1 = p2;
        }
    }
    catch
    {
    }
    static_debug(p2);

    free(p1);
    return 0;
}