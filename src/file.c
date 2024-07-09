
#pragma safety enable


void* _Owner _Opt malloc(int i);
void free(void* _Owner _Opt);

struct X {
    char* _Owner _Opt name;
};

#define XY p->name
int main()
{
    struct X* _Owner _Opt p = malloc(sizeof(struct X));
    if (p)
    {
        if (*XY)
        {

        }
    }

}