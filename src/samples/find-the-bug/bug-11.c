#pragma safety enable

void* _Owner malloc(unsigned long size);

struct X{
  char * p_ch;    
};

struct X * _Owner f()
{
   char ch  = 0;
   struct X * _Owner p = malloc(sizeof(struct X));
   if (p)
   {
       p->p_ch =  &ch;
       return p;
   } 
   return nullptr;   
}
