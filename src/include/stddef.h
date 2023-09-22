#pragma once

#define __STDC_VERSION_STDDEF_H__ 202311L

typedef typeof(nullptr) nullptr_t;


typedef long int ptrdiff_t;
typedef long unsigned int size_t;
typedef int wchar_t;

typedef struct {
  long long __max_align_ll;
  long double __max_align_ld;
} max_align_t;
  
