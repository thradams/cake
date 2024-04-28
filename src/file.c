#pragma safety enable

void* _Owner _Opt malloc(unsigned long size);
void free(void* _Owner _Opt ptr);

int main() {
   void * _Opt p = malloc(1);
   free(p);
}


