#pragma safety enable

typedef int FILE;
FILE* _Owner _Opt fopen(const char* restrict filename, const char* restrict mode);
int fclose(FILE* _Owner stream);

int main()
{
   if (FILE* _Owner _Opt f = fopen("file.txt", "r"))
   {    
     /*...*/
     fclose(f);
   }
}

