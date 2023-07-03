void * _Owner malloc(int i);
void free(_Implicit void * _Owner p);

int main() {
   void * _Owner p = malloc(1);
   void  * p2;
   p = 0;
}
