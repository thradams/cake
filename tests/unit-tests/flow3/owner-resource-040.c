#pragma safety enable



_Uninitialized void* _Owner malloc(unsigned size);
void free(void* _Owner _Opt  ptr);

int main() {
   void * _Owner p = malloc(1);
   do{
      free(p);
   }
   while(0);   
}
