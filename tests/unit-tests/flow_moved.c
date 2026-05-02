#pragma safety enable

void free(void *_Owner _Opt p);

void f(int* _Opt _Owner p)
{
   int * _Opt _Owner p2 = p;   
   assert_state(p, "null, moved");
   if (p)
   {
     assert_state(p, "moved");
   }
   free(p2);
}
