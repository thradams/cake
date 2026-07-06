/* Cake x86_x64_gcc */
struct lconv {
    char * decimal_point;
    char * thousands_sep;
    char * grouping;
    char * int_curr_symbol;
    char * currency_symbol;
    char * mon_decimal_point;
    char * mon_thousands_sep;
    char * mon_grouping;
    char * positive_sign;
    char * negative_sign;
    char int_frac_digits;
    char frac_digits;
    char p_cs_precedes;
    char p_sep_by_space;
    char n_cs_precedes;
    char n_sep_by_space;
    char p_sign_posn;
    char n_sign_posn;
    char int_p_cs_precedes;
    char int_p_sep_by_space;
    char int_n_cs_precedes;
    char int_n_sep_by_space;
    char int_p_sign_posn;
    char int_n_sign_posn;
};


extern char *setlocale(int __category, char * __locale);
extern struct lconv *localeconv(void);
extern int printf(char * __format, ...);

int main(void)
{
    struct lconv * lc;

    setlocale(4, "en_IN.utf8");
    lc = localeconv();
    printf("Local Currency Symbol        : %s\n", lc->currency_symbol);
    printf("International Currency Symbol: %s\n", lc->int_curr_symbol);
}


