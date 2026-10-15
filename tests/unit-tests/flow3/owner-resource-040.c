#pragma safety enable



void* _Owner _Opt _Uninitialized malloc(unsigned size);
void free(void* _Owner _Opt  ptr);

int main() {
   void * _Owner _Opt p = malloc(1);
   do{
      free(p);
   }
   while(0);   
}
