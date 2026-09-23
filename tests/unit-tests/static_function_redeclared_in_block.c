/* no warning: func and func2 are used through the block scope redeclarations */

static void func() {  }
static void func2() {  }
static void func3() {  }

void f()
{
  func3();
}

int main()
{
  void func();
  func();
  extern void func2();
  func2();

  /* local functions, no linkage */
  static void local() {  }
  local();
  static void func3() {  }
  func3();
}
