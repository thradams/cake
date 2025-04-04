#pragma safety enable

void* _Owner _Opt malloc(unsigned long size);
void free(void* _Owner _Opt ptr);

const char * get()
{
   char ch = 0;
   return &ch;
}
#pragma cake diagnostic check "-Wlifetime-ended"

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
#pragma cake diagnostic check "-Wlifetime-ended"

   }
   return nullptr;
}
