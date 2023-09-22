
#pragma once

//wint_t                wctrans_t                 wctype_t      WEOF

typedef int                           errno_t;
typedef unsigned short                wint_t;
typedef unsigned short                wctype_t;



 int iswalnum(wint_t wc);
 int iswalpha(wint_t wc);
 int iswblank(wint_t wc);
 int iswcntrl(wint_t wc);
 int iswdigit(wint_t wc);
 int iswgraph(wint_t wc);
 int iswlower(wint_t wc);
 int iswprint(wint_t wc);
 int iswpunct(wint_t wc);
 int iswspace(wint_t wc);
 int iswupper(wint_t wc);
 int iswxdigit(wint_t wc);
 int iswctype(wint_t wc, wctype_t desc);
 wctype_t wctype(const char *property);
 wint_t towlower(wint_t wc);
 wint_t towupper(wint_t wc);
 wint_t towctrans(wint_t wc, wctrans_t desc);
 //wctrans_t wctrans(const char *property);
