
struct X {
  union {
    struct {
      int *pSelect;
    } view;
  }u;
};

int main()
{
    struct X t;
    t.u.view.pSelect = 0;
}
