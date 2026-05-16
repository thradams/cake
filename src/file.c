
typedef unsigned long size_t;
#define offsetof(type, ident) ((size_t) (unsigned long) &((type *)0)->ident)

struct X { 
  int i; 
};
int main(){
static_assert(&((struct X *)0)->i));
}//static_assert(offsetof(struct X, i));
//static_assert(offsetof(struct X, i));
