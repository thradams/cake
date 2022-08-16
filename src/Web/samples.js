var sample = {};



sample["Digit Separator C23"] =
`
int main()
{
    int a = 1000'00;
    _Static_assert(1000'00 == 100000);
}
`;

sample["Binary Literal C23"] =
`

/* pp-numbers starting with 0b or 0B also are changed */
#define X  0b1010

int main()
{
    int a = X;
    int b = 0B1010;
}
`;


sample["_Static_assert static_assert"] =
    `
int main()
{
    _Static_assert(1 == 1, "error");
    static_assert(1 == 1, "error");
    static_assert(1 == 1);
}
`;


sample["elifdef  elifndef C23"] =
`
/*
  C23 preprocessing directives elifdef and elifndef N2645
*/
#define Y

#ifdef X
#define VERSION 1
#elifdef  Y
#define VERSION 2
#else
#define VERSION 3
#endif

_Static_assert(VERSION == 2, "");

`;





sample["empty initializer C23"] =
    `
int main()
{
    struct X {
        int i;
    } x = {};

    x = (struct X) {};

    struct Y
    {
        struct X x;
    } y = { {} };
}

`;

sample["C23 typeof"] =
`
#define SWAP(a, b) \\
  do {\\
    typeof(a) temp = a; a = b; b = temp; \\
  } while (0)

#pragma expand SWAP

int main()
{
    int a = 1;
    typeof(a) b = 1;
    typeof(int*) p1, p2;
    SWAP(a, b);

    struct { int i; } x;
    typeof(x) x2;
    typeof(x) x3;
}

`;




sample["little of semantics analysis"] =
    `
int main()
{
    int a = 1;
    *a = 2; //error

    struct X { int i; }x;
    x.j = 1;
   
}

`;

sample["u8 literals"] =
`
/*
  source code character set is always utf8
*/

char * s1 = u8"maçã";
char * s2 = u8"maca";
char * s3 = "maçã";
char * s4 = "maca";
`;

sample["Hello, World!"] =
`
#include <stdio.h>

int main()
{
  printf("Hello, 世界");
}
`;


sample["C23 bool true false"] =
 `
int main()
{
  bool b = true;
  b = false;
  static_assert(1 == true);
  static_assert(0 == false);
}

`;

sample["C23 nullptr (NULL as extension)"] =
`
#if NULL == 0
int main()
{
  void * p = NULL;
  static_assert(nullptr == 0);
}
#endif
`;


sample["_Hashof (extension)"] =
`
struct X {
    int a[10];
  
    /*uncomment the next line*/
    //char * text;
};

void x_destroy(struct X* p);

int main()
{
    struct X x = {};
    x_destroy(&x);
}










void x_destroy(struct X* p)
{
    /*In a far away galaxy*/
    static_assert(_Hashof(p) == 283780300, "check this function x_destroy");
}


`
;

   

sample["typeid (extension)"] =
`
int a[10];
static_assert(typeid(a) == typeid(double [10]), "types are diferent");

`;

sample["try catch (extension)"] =
`
#include <stdio.h>

int main()
{
  FILE * f = NULL;
  try
  {
     f = fopen("file.txt", "r");
     if (f == NULL) throw;

    /*success here*/
  }
  catch
  {
     /*some error*/
  }

  if (f)
    fclose(f);
}

`;


sample["try catch II(extension)"] =
`
#include <stdio.h>

/*not sure if usefull , but this is allowed*/

int main()
{
  try
  {
      FILE * f = NULL;
      try {
         FILE *f = fopen("file.txt", "r");
         if (f == NULL) throw;
         /*more*/
      }
      catch {
         if (f)
          fclose(f);
         throw;
      }
  }
  catch
  {
  }
}

`;

sample["1 defer (extension)"] =
    `
#include <stdio.h>

int main()
{

  try
  {
     FILE* f = fopen("file.txt", "r");
     if (f == NULL) throw;
     defer fclose(f);

    /*success here*/
  }
  catch
  {
     /*some error*/
  }


}

`;

sample["2 defer (extension)"] =
    `
#include <stdio.h>

int main()
{

  try
  {
     FILE* f = fopen("in.txt", "r");
     if (f == NULL) throw;
     defer fclose(f);

     FILE* f2 = fopen("out.txt", "w");
     if (f2 == NULL) throw;
     defer fclose(f2);

     //...

    /*success here*/
  }
  catch
  {
     /*some error*/
  }


}

`;

sample["defer with breaks (extension)"] =
    `

#include <stdio.h>

int main()
{

  do
  {
     FILE* f = fopen("in.txt", "r");
     if (f == NULL) break;
     defer fclose(f);

     FILE* f2 = fopen("out.txt", "w");
     if (f2 == NULL) break;
     defer fclose(f2);

     //...

    /*success here*/
  }
  while(0);


}

`;


sample["defer with breaks II (extension)"] =
    `

#include <stdio.h>

int main()
{
  FILE* f = NULL;
  defer if (f) fclose(f);

  do
  {
     f = fopen("in.txt", "r");
     if (f == NULL) break;     
  }
  while(0);

}

`;


sample["defer with return (extension)"] =
    `

#include <stdio.h>

int main()
{
  FILE* f = fopen("in.txt", "r");
  if (f == NULL) return 1;
  defer fclose(f);

  FILE* f2 = fopen("out.txt", "w");
  if (f2 == NULL) return 1;
  defer fclose(f2);

  return 0;
}


`;


sample["defer goto (extension)"] =
    `

#include <stdio.h>

int main()
{
  FILE* f = fopen("in.txt", "r");
  if (f != NULL)
  {
     defer fclose(f);

     FILE* f2 = fopen("out.txt", "w");
     if (f2 == NULL) goto LEND;
     defer fclose(f2);
  }
  LEND:
  return 0;
}

`;

sample["Like C++17 if with initialization (extension)"] =
    `
#include <stdio.h>

int main()
{
   int size = 10;
   if (FILE* f = fopen("file.txt", "r"); f)
   {
     /*...*/
     fclose(f);
   }
}
`;

sample["repeat (extension)"] =
`
int main()
{
   /*5.7.1 The repeat Statement
   repeat merely executes the given statement forever unless a
   break statement is encountered, or a goto passes control to a
   statement outside the loop. The statement in a repeat statement
   is almost invariably compound. continue and break statements are valid
   inside a repeat.
   */

   repeat
   {
     break;
   }
}
`;

sample["Literal function (lambdas) (extension)"] =
    `
#include <stdio.h>
#include <stdlib.h>
extern char* strdup(const char* s);

void async(void* capture, int sz, void (*F)(void* data))
{
  /*
    The real function would copy capture and the function pointer
    to a queue and then execute at some thread pool
  */
  F(capture);
}

void dispatch(void* capture, int sz, void (*F)(void* data))
{
  /*
    The real function would copy capture and the function pointer
    to a queue and then execute sequencially.
  */
  F(capture);
}


void create_app(const char* appname)
{
  printf("main thread\\n");
 
  struct capture {
     char * name;
  } capture = { .name = strdup(appname) };

  async(&capture, sizeof capture, (void (void* p))
  {
    struct capture* capture = p;
    printf("this is running at any thread (name=%s)\\n", capture->name);

    dispatch(capture, sizeof *capture, (void (void* p) )
    {
      struct capture* capture = p;
      printf("back to main thread\\n");
      free(capture->name);
    });
  });
}

int main()
{
  create_app("string");
  return 0;
}

`;
