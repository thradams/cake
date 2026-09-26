const int f1(void);          //lint 92 warning: type qualifier on return type has no effect
volatile int f2(void);       //lint 92 warning: type qualifier on return type has no effect
int* const f3(void);         //lint 92 warning: type qualifier on return type has no effect
const int f4(void) { return 1; } //lint 92 warning: type qualifier on return type has no effect

/* pointer to const is fine */
const int* f5(void);
int f6(void);
