void* owner make();
void free(implicit void* owner p);

void f(condition)
{
    void* owner p = make();


    switch (condition)
    {
        case 1:
        {
            free(p);
        }
        break;
        case 2:
        {
            free(p);
        }
        break;

        default:
            free(p);
            break;
    }        
}