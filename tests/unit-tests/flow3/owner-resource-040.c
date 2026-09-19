#pragma safety enable



void* _Owner _Opt _Uninitialized malloc(unsigned size);
void free(void* _Owner _Opt  ptr);

int main() {
   void * _Owner _Opt p = malloc(1);
   do{
      /* FIXED: no extra "(*p) lifetime has ended", free takes void*; only "p is moved" is real */
      free(p); //lint 32 object 'p' is moved (see line 9)
   }
   while(0);   
}
