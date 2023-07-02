

void free([[cake::implicit]] void* _owner ptr);
[[nodiscard]] void* _owner malloc(int size);

struct _owner X
{
    int i;
};

int main() 
{
    struct X * _owner p = malloc(sizeof (struct X));
    free(p);
}


