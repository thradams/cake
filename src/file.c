
/*  
  See also: http://thradams.com/cake/ownership.html
*/
void * _Owner malloc(int i);
void free(_Implicit void * _Owner p);

int main() {
   void * _Owner p = malloc(1);
   free(p);
}
