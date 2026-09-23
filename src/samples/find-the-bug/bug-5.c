#pragma safety enable

void* _Owner _Opt calloc(unsigned int n, unsigned long size);
void free(void* _Owner _Opt ptr);

struct Y {
    int i; 
};

struct X {
    int i; 
    struct Y* _Opt pY;
};

int main() 
{
    struct X* _Owner _Opt pX = calloc(1, sizeof *pX);
    if (pX) 
    {
        struct Y* _Owner _Opt pY = calloc(1, sizeof *pY);
        if (pY) 
        {
            pX->pY = pY;
            struct X* _Opt p = pX;
            free(pY);            
            p->pY->i = 1;  // no warning            
        }
        free(pX);
    }
}
