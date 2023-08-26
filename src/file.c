void * owner malloc(int i);
int main()
{
    int * p;
    p = (int* owner) malloc(1);
}