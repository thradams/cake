
#define X2(...) __VA_OPT__()
X2(__COUNTER__)
__COUNTER__ // 4
