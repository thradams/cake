struct s { int a; };
struct incomplete;
int a = __builtin_offsetof(struct incomplete, a); //lint 740 offsetof: incomplete type 'struct incomplete'
