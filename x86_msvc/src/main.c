// Cake 0.12.05 target=x86_msvc
struct report {
    int no_files;
    double cpu_time_used_sec;
    int error_count;
    int warnings_count;
    int info_count;
    unsigned char test_mode;
    int test_failed;
    int test_succeeded;
    unsigned char ignore_this_report;
};

struct _iobuf {
    void * _Placeholder;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};


static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

__declspec(selectany) int _Avx2WmemEnabledWeakValue = 0;
unsigned char enable_vt_mode(void);
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __Ck0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
void print_help();
int compile(int argc, char ** argv, struct report * error);
static void print_report(struct report * report, unsigned char no_colors);

int main(int argc, char ** argv)
{
    struct report  report;

    enable_vt_mode();
    printf("Cake 0.12.05 ");
    printf("(MSVC ");
    printf("x86");
    printf(")");
    printf("\n");
    if (argc < 2)
    {
        print_help();
        return 1;
    }
    _cake_zmem(&report, 48);
    compile(argc, (char **)argv, &report);
    if (!report.ignore_this_report)
    {
        print_report(&report, 1);
    }
    if (report.test_mode)
    {
        return report.test_failed ? 1 : 0;
    }
    return report.error_count > 0 ? 1 : 0;
}



static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &__Ck0__OptionsStorage;
}

static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);
}

static int __cdecl printf(char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4U));
    _Result = _vfprintf_l((__acrt_iob_func(1)), _Format, 0U, _ArgList);
    ((void)(_ArgList = 0U));
    return _Result;
}

static void print_report(struct report * report, unsigned char no_colors)
{
    printf("\n");
    printf(" %d files in %.2f seconds\n", report->no_files, report->cpu_time_used_sec);
    if (no_colors)
    {
        printf(" %d errors ", report->error_count);
        printf("%d warnings ", report->warnings_count);
        printf("%d notes ", report->info_count);
    }
    else
    {
        printf(" %d\x1b[91m errors \x1b[0m", report->error_count);
        printf("%d\x1b[95m warnings \x1b[0m", report->warnings_count);
        printf("%d\x1b[36;1m notes \x1b[0m", report->info_count);
    }
    if (report->test_mode)
    {
        printf("\n");
        printf(" %d tests of %d failed\n", report->test_failed, (report->test_failed + report->test_succeeded));
    }
    printf("\n");
}
