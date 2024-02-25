//en.cppreference.com/w/c/language/array_initialization.html
int main(void)
{
    // The following four array declarations are the same
    short q1[4][3][2] = {
        { 1 },
        { 2, 3 },
        { 4, 5, 6 }
    };
 
    short q2[4][3][2] = {1, 0, 0, 0, 0, 0, 2, 3, 0, 0, 0, 0, 4, 5, 6};
 
    short q3[4][3][2] = {
        {
            { 1 },
        },
        {
            { 2, 3 },
        },
        {
            { 4, 5 },
            { 6 },
        }
    };
 
    short q4[4][3][2] = {1, [1]=2, 3, [2]=4, 5, 6};
 
 
    // Character names can be associated with enumeration constants
    // using arrays with designators:
    enum { RED, GREEN, BLUE };
    const char *nm[] = {
        [RED] = "red",
        [GREEN] = "green",
        [BLUE] = "blue",
    };
}