
#pragma safety enable 

void* _Owner _Nullable malloc(int size);
void free(void* _Owner _Nullable ptr);

struct Y {
    int i;
};

struct X {
  struct Y y;  
};

struct X* _Nullable _Owner create(struct Y* pY)
{
    struct X* _Nullable  _Owner p  = malloc(sizeof * p);
    if (p)  {
        p->y = *pY;
    }
    return p;
}

