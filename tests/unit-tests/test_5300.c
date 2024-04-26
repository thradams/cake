#pragma safety enable


void destroy(char* _Owner x);
char   * _Owner  get();

int main()
{
  destroy(get());
}
