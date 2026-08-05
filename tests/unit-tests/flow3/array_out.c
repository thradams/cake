#pragma safety enable


int do_work(int cond, char errmsg[], int size);

int do_work2(int cond, char *errmsg, int size);

int main(void) {
    char errmsg[200] = { 0 };
    int rc = do_work(-1, errmsg, sizeof(errmsg));

   // static_debug(errmsg);
    if (errmsg[0] != '\0') {                 
        rc = 1;
        //return 1; //cannot say unreachable
    }

    char errmsg2[200] = { 0 };
    int rc2 = do_work2(-1, errmsg2, sizeof(errmsg));

   // static_debug(errmsg);
    if (errmsg2[0] != '\0') {                 
        rc2 = 1;
        //return 1; //cannot say unreachable
    }

    return 0;
}
