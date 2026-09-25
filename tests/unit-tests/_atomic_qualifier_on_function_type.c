// Test 5: _Atomic qualifier on function type

typedef void F(void);
_Atomic F g; //lint 2100 _Atomic qualifier cannot be applied to function types

_Atomic int f(void) { return 0; } /* _Atomic return type: ok */
