
#pragma safety enable

void* _Owner _Opt calloc(int n , unsigned long size);
void free(void* _Owner _Opt ptr);

struct X
{
    int * _Owner _Opt data;
};

void x_delete(struct X * _Owner _Opt p)
{
    if (p) {
        free(p->data);
        free(p);
    }
}

struct X *_Opt _Owner f(int c) {
   struct X* _Owner _Opt p = calloc(1, sizeof * p);
   try
   {
       if (c) throw; 
   } 
   catch
   {
     x_delete(p);
     p = 0;     
   }
   static_debug(*p);
   return p;
}
