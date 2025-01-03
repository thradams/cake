#pragma safety enable



void* _Owner make();
void free( void* _Owner p);

void f(int condition)
{
    void* _Owner p = make();


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