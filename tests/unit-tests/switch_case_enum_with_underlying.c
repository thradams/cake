enum color : unsigned char { RED, GREEN, BLUE };

int main()
{
    enum color c = RED;
    switch (c)
    {
        case RED: break;
        case GREEN: break;
        case BLUE: break;

        case 300: break;
    } //lint 4
}
