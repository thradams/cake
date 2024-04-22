#pragma nullable enable
#pragma ownership enable

void destroy(char* _Owner x);
char   * _Owner  get();

int main()
{
  destroy(get());
}
