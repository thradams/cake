#pragma safety enable

void* _Owner calloc(unsigned int n, unsigned long size);
void free(void* _Owner _Nullable ptr);

struct Y {
    int i; 
};

struct X {
    int i; 
    struct Y* pY;
};

int main() 
{
    struct X* _Owner pX = calloc(1, sizeof *pX);
    if (pX) 
    {
        struct Y* _Owner pY = calloc(1, sizeof *pY);
        if (pY) 
        {
            pX->pY = pY;
            struct X* p = pX;
            free(pY);            
            p->pY->i = 1;  // no warning            
        }
        free(pX);
    }
}
