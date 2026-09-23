#pragma safety enable

void* _Owner _Opt malloc(unsigned long size);

struct X{
  char * p_ch;    
};

struct X * _Owner _Opt f()
{
   char ch  = 0;
   struct X * _Owner _Opt p = malloc(sizeof(struct X));
   if (p)
   {
       p->p_ch =  &ch;
       return p;
   } 
   return nullptr;   
}
