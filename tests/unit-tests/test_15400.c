typedef unsigned short wchar_t;
wchar_t s[] = L"abcd";
static_assert(sizeof(s) == sizeof(wchar_t)*5);