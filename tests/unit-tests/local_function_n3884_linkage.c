/* N3884 EXAMPLE 8 with a static file scope function */

#include <stdio.h>

static void func() { printf("external definition\n"); }

void demo1()
{
  extern void func();                         /* refers to file scope definition */
  func();                                     /* prints "external definition" */
  static void func() { printf("local 1\n"); } //lint 1020
}

void demo2()
{
  static void func();                         /* hides file scope definition */
  func();                                     /* prints "local 2" */
  static void func() { printf("local 2\n"); } /* valid, compatible definition in same scope */
}
