/* Cake x86_x64_gcc */
int __v0[2] = {2,4};
float __v1[3] = {1.0,10.0,100.0};
struct point {
    double x;
    double y;
};


int * p = __v0;
float * pc = __v1;
void drawline1(struct point  from, struct point  to);
void drawline2(struct point * from, struct point * to);

int main(void)
{
    int n;
    int * p;
    int __v0[2];
    struct point  __v1;
    struct point  __v2;
    struct point  __v3;
    struct point  __v4;

    n = 2;
    p = &n;
    __v0[0] = *p;
    __v0[1] = 0;
    p = __v0;
    __v1.x = 1;
    __v1.y = 1;
    __v2.x = 3;
    __v2.y = 4;
    drawline1(__v1, __v2);
    __v3.x = 1;
    __v3.y = 1;
    __v4.x = 3;
    __v4.y = 4;
    drawline2(&__v3, &__v4);
}


extern int printf(char * __format, ...);

void drawline1(struct point  from, struct point  to)
{
    printf("drawline1: `from` @ %p {%.2f, %.2f}, `to` @ %p {%.2f, %.2f}\n", (void *)&from, from.x, from.y, (void *)&to, to.x, to.y);
}


void drawline2(struct point * from, struct point * to)
{
    printf("drawline2: `from` @ %p {%.2f, %.2f}, `to` @ %p {%.2f, %.2f}\n", (void *)from, from->x, from->y, (void *)to, to->x, to->y);
}


