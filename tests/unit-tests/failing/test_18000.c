
#pragma safety enable 

void* _Owner _Opt malloc(int size);
void free(void* _Owner _Opt ptr);

struct Y {
    int i;
};

struct X {
  struct Y y;  
};

struct X* _Opt _Owner create(struct Y* pY)
{
    struct X* _Opt  _Owner p  = malloc(sizeof * p);
    if (p)  {
        p->y = *pY;
    }
    return p;
}

