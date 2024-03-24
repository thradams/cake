void* _Owner malloc(unsigned long size);
void free(void* _Owner ptr);

struct Z {
    char* _Owner nameZ;
};

struct Y {
    char* _Owner nameY;
    struct Z *pZ;
};

struct X {
    char* _Owner nameX;
    struct Y *pY;
};

void f(struct X * p)
{
  static_debug(p);
}
int main()
{
    
}
