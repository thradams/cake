
/*
  Type of wchar_t changes on windows/linux 
*/
typedef typeof(L'.') wchar_t;

wchar_t s[] = L"abcd";
static_assert(sizeof(s) == sizeof(wchar_t)*5);
static_assert(sizeof(L"abcd") == sizeof(L'.')*5);

 