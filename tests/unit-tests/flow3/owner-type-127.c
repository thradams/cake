#pragma safety enable


void destroy(char* _Owner x);
char   * _Nonnull _Owner  get();

int main()
{
  destroy(get());
}
