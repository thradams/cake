#pragma ownership enable
#pragma nullable enable

void* _Owner _Opt malloc();
int main() {
   void * _Owner p = malloc();
}
