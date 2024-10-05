#ifdef NDEBUG
#define assert(...) ((void)0)
#else
#define assert(...) assert(__VA_ARGS__)
#endif
