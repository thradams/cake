/* N3884: labels, case/default and break do not cross a local function */

int main(int argc, char* argv[])
{
    switch (argc)
    {
      case 1:
        static void local()
        {
           L2:
           goto L1; //lint 1430
           default: //lint 750
           break;   //lint 760
        } //lint 55
        local();
        break;
      case 2:
        goto L2;    //lint 1430
    }
    L1:
} //lint 55
