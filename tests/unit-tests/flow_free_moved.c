﻿#pragma safety enable

void* _Owner _Opt calloc(unsigned long n, unsigned long size);
void free(void* _Owner _Opt ptr);


int main() {
   int * _Owner _Opt p = calloc(1, sizeof(int));
   int *_Owner _Opt p2 = p;    //MOVED
   free(p2);   
   free(p); //MOVED
#pragma cake diagnostic check "-Wusing-moved-object"
}