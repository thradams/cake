enum color { RED, GREEN, BLUE };

int main()
{
    enum color c;
    switch (c)
    {
        case RED: break;
        case GREEN: break;
        case BLUE: break;

        case 100: break;
    } //lint 4
}
