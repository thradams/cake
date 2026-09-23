#pragma safety enable

enum {
    mtx_plain ,
    mtx_timed,
    mtx_plain,
    mtx_timed,
};

enum {

    thrd_success , /* unspecified */
    thrd_nomem , /* unspecified */
    thrd_timedout , /* unspecified */
    thrd_busy , /* unspecified */
    thrd_error /* unspecified */
};

typedef struct { _Owner int dummy; } mtx_t;
int mtx_init(_Out mtx_t *mtx, int type);
void mtx_destroy( _Dtor mtx_t * mutex );

int main()
{
    mtx_t mtx;
    if (mtx_init(&mtx, mtx_plain) != thrd_success)
    {
        return 1;
    }
    mtx_destroy(&mtx);
}
