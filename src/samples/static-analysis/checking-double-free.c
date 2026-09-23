#pragma safety enable

void free(void * _Owner p);

struct X {
    char * _Owner naasdasdme;
    char * _Owner nasdasame;
    char * _Owner naasdme;
    char * _Owner namasde;
    char * _Owner namasade;
};

void x_destroy(_Dtor struct X * p)
{
    free(p->naasdasdme);
    free(p->nasdasame);
    free(p->naasdme);
    free(p->namasde);
    free(p->namasade);
    free(p->namasde);
}
