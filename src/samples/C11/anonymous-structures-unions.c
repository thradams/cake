struct point { float x, y, z; };

struct location {
    char* name;
    struct point;
};

int main()
{
    struct location location = {};
    location.x = 1;
    location.y = 2;
}
