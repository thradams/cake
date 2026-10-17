#pragma safety enable

_Uninitialized void* _Owner _Opt malloc(unsigned long size);
void free(void* _Owner ptr);

int main() {
   void * _Owner  _Opt p = malloc(1);
   if (p)
   {
     free(p);
   }
   static_debug(p);
}
