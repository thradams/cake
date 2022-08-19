var sample = {};

sample["C11 _Generic"] =
    `
#include <math.h>

#define cbrt(X) _Generic((X), \\
                  double: cbrtl, \\
                  float: cbrtf ,\\
                  default: cbrtl  \\
              )(X)

#pragma expand cbrt


int main(void)
{
    cbrt(1.0);
}

`;

sample["C11 _Static_assert"] =
`
int main()
{
    _Static_assert(1 == 1, "error");    
}
`;


sample["C11 u8 literals"] =
    `
/*
  source code character set is always utf8
*/

char * s1 = u8"maçã";
char * s2 = u8"maca";
char * s3 = "maçã";
char * s4 = "maca";
`;


sample["C23 Digit Separator"] =
`
int main()
{
    int a = 1000'00;
    _Static_assert(1000'00 == 100000);
}
`;

sample["C23 Binary Literal"] =
`

/* pp-numbers starting with 0b or 0B also are changed */
#define X  0b1010

int main()
{
    int a = X;
    int b = 0B1010;
}
`;


sample["C23 static_assert"] =
`
int main()
{    
    static_assert(1 == 1, "error");
    static_assert(1 == 1);
}
`;


sample["C23 #elifdef  #elifndef"] =
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


sample["C23 __VA_OPT__"] =
`
 #define F(...)  f(0 __VA_OPT__(,) __VA_ARGS__)
 F(a, b, c);

 #define F2(...)  f(0 __VA_OPT__(,) __VA_ARGS__)
 F2()
`;

sample["C23 _has_include"] =
`
#if __has_include(<stdio.h>)
#warning  YES
#endif

#if __has_include(<any.h>)
#warning  YES
#else
#warning  NO
#endif
`;

sample["C23 #embed"] =
`
#include <stdio.h>

int main()
{
  static const char file_txt[] = {
   #embed "stdio.h"
   ,0
  };

  printf("%s\\n", file_txt);

}
`;

sample["C23 #warning"] =
    `
#include <stdio.h>

int main()
{
  #warning TODO ..missing code  
}
`;

sample["C23 empty initializer"] =
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


sample["Extension _Hashof"] =
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

   

sample["Extension typeid"] =
`
int a[10];
static_assert(typeid(a) == typeid(double [10]), "types are diferent");

`;

sample["Extension try catch throw"] =
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


sample["Extension try catch throw II"] =
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

sample["Extension defer I"] =
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

sample["Extension defer II"] =
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

sample["Extension defer with breaks III"] =
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


sample["Extension defer with breaks IV"] =
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


sample["Extension defer with return V"] =
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


sample["Extension defer goto VI"] =
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

sample["Extension Like C++17 if with initialization"] =
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

sample["Extension repeat"] =
`
int main()
{
   repeat {
     break;
   }
}
`;

sample["Extension Literal function (lambdas)"] =
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

sample["Hello, World!"] =
`
#include <stdio.h>

int main()
{
  printf("Hello, 世界");
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

