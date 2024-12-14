#pragma safety enable

void free(void *_Owner _Opt p);

void f(int* _Opt _Owner p)
{
   int * _Opt _Owner p2 = p;   
   static_state(p, "null, moved");
   if (p)
   {
     static_state(p, "moved");
   }
   free(p2);
}
