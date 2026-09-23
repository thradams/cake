
void f1(){
    /*we cannot use local variables*/
    int i = 0;
    //(void(void)){ i = 1; }();
}

void f2(){
    /*we can use then at discarded expressions*/
    int i = 0;
    (static void (void)){ int k = sizeof(i); }();
}

int g;
void f3(){
    /*we can use variables from file scope*/
    (static void(void)){ int k = g; }();
}


void f4(){
    (static void(void)){ const char * s = __func__; }();
}
