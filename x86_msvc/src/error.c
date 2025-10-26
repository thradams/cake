/* Cake 0.12.26 x86_msvc */

void throw_break_point()
{
}


char *get_posix_error_message(int error)
{
    /*switch*/
    {
        int __v0 = error;
        if (__v0 == 1) goto __L1; /*case 1*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 3) goto __L3; /*case 3*/
        if (__v0 == 4) goto __L4; /*case 4*/
        if (__v0 == 5) goto __L5; /*case 5*/
        if (__v0 == 6) goto __L6; /*case 6*/
        if (__v0 == 7) goto __L7; /*case 7*/
        if (__v0 == 8) goto __L8; /*case 8*/
        if (__v0 == 9) goto __L9; /*case 9*/
        if (__v0 == 10) goto __L10; /*case 10*/
        if (__v0 == 11) goto __L11; /*case 11*/
        if (__v0 == 12) goto __L12; /*case 12*/
        if (__v0 == 13) goto __L13; /*case 13*/
        if (__v0 == 14) goto __L14; /*case 14*/
        if (__v0 == 16) goto __L15; /*case 16*/
        if (__v0 == 17) goto __L16; /*case 17*/
        if (__v0 == 18) goto __L17; /*case 18*/
        if (__v0 == 19) goto __L18; /*case 19*/
        if (__v0 == 20) goto __L19; /*case 20*/
        if (__v0 == 21) goto __L20; /*case 21*/
        if (__v0 == 22) goto __L21; /*case 22*/
        if (__v0 == 23) goto __L22; /*case 23*/
        if (__v0 == 24) goto __L23; /*case 24*/
        if (__v0 == 25) goto __L24; /*case 25*/
        if (__v0 == 139) goto __L25; /*case 139*/
        if (__v0 == 27) goto __L26; /*case 27*/
        if (__v0 == 28) goto __L27; /*case 28*/
        if (__v0 == 29) goto __L28; /*case 29*/
        if (__v0 == 30) goto __L29; /*case 30*/
        if (__v0 == 31) goto __L30; /*case 31*/
        if (__v0 == 32) goto __L31; /*case 32*/
        if (__v0 == 33) goto __L32; /*case 33*/
        if (__v0 == 34) goto __L33; /*case 34*/
        if (__v0 == 36) goto __L34; /*case 36*/
        if (__v0 == 38) goto __L35; /*case 38*/
        if (__v0 == 39) goto __L36; /*case 39*/
        if (__v0 == 40) goto __L37; /*case 40*/
        if (__v0 == 41) goto __L38; /*case 41*/
        if (__v0 == 114) goto __L39; /*case 114*/
        if (__v0 == 122) goto __L40; /*case 122*/
        if (__v0 == 111) goto __L41; /*case 111*/
        if (__v0 == 125) goto __L42; /*case 125*/
        if (__v0 == 120) goto __L43; /*case 120*/
        if (__v0 == 137) goto __L44; /*case 137*/
        if (__v0 == 124) goto __L45; /*case 124*/
        if (__v0 == 121) goto __L46; /*case 121*/
        if (__v0 == 134) goto __L47; /*case 134*/
        if (__v0 == 104) goto __L48; /*case 104*/
        if (__v0 == 132) goto __L49; /*case 132*/
        if (__v0 == 42) goto __L50; /*case 42*/
        if (__v0 == 128) goto __L51; /*case 128*/
        if (__v0 == 109) goto __L52; /*case 109*/
        if (__v0 == 115) goto __L53; /*case 115*/
        if (__v0 == 136) goto __L54; /*case 136*/
        if (__v0 == 123) goto __L55; /*case 123*/
        if (__v0 == 135) goto __L56; /*case 135*/
        if (__v0 == 130) goto __L57; /*case 130*/
        if (__v0 == 102) goto __L58; /*case 102*/
        if (__v0 == 100) goto __L59; /*case 100*/
        if (__v0 == 101) goto __L60; /*case 101*/
        if (__v0 == 116) goto __L61; /*case 116*/
        if (__v0 == 118) goto __L62; /*case 118*/
        if (__v0 == 117) goto __L63; /*case 117*/
        if (__v0 == 106) goto __L64; /*case 106*/
        if (__v0 == 108) goto __L65; /*case 108*/
        if (__v0 == 119) goto __L66; /*case 119*/
        if (__v0 == 113) goto __L67; /*case 113*/
        if (__v0 == 126) goto __L68; /*case 126*/
        if (__v0 == 138) goto __L69; /*case 138*/
        if (__v0 == 107) goto __L70; /*case 107*/
        if (__v0 == 110) goto __L71; /*case 110*/
        if (__v0 == 103) goto __L72; /*case 103*/
        if (__v0 == 112) goto __L73; /*case 112*/
        goto __L74; /* default */

        {
            __L1: /*case 1*/ 
            return "Operation not permitted";
            __L2: /*case 2*/ 
            return "No such file or directory";
            __L3: /*case 3*/ 
            return "No such process";
            __L4: /*case 4*/ 
            return "Interrupted system call";
            __L5: /*case 5*/ 
            return "I/O error";
            __L6: /*case 6*/ 
            return "No such device or address";
            __L7: /*case 7*/ 
            return "Arg list too long";
            __L8: /*case 8*/ 
            return "Exec string_format error";
            __L9: /*case 9*/ 
            return "Bad file number";
            __L10: /*case 10*/ 
            return "No child processes";
            __L11: /*case 11*/ 
            return "Try again";
            __L12: /*case 12*/ 
            return "Out of memory";
            __L13: /*case 13*/ 
            return "Permission denied";
            __L14: /*case 14*/ 
            return "Bad address";
            __L15: /*case 16*/ 
            return "Device or resource busy";
            __L16: /*case 17*/ 
            return "File exists";
            __L17: /*case 18*/ 
            return "Cross-device link";
            __L18: /*case 19*/ 
            return "No such device";
            __L19: /*case 20*/ 
            return "Not a directory";
            __L20: /*case 21*/ 
            return "Is a directory";
            __L21: /*case 22*/ 
            return "Invalid argument";
            __L22: /*case 23*/ 
            return "File table overflow";
            __L23: /*case 24*/ 
            return "Too many open files";
            __L24: /*case 25*/ 
            return "Not a typewriter";
            __L25: /*case 139*/ 
            return "Text file busy";
            __L26: /*case 27*/ 
            return "File too large";
            __L27: /*case 28*/ 
            return "No space left on device";
            __L28: /*case 29*/ 
            return "Illegal seek";
            __L29: /*case 30*/ 
            return "Read-only file system";
            __L30: /*case 31*/ 
            return "Too many links";
            __L31: /*case 32*/ 
            return "Broken pipe";
            __L32: /*case 33*/ 
            return "Math argument out of domain of func";
            __L33: /*case 34*/ 
            return "Math result not representable";
            __L34: /*case 36*/ 
            return "Resource deadlock would occur";
            __L35: /*case 38*/ 
            return "File name too long";
            __L36: /*case 39*/ 
            return "No record locks available";
            __L37: /*case 40*/ 
            return "Function not implemented";
            __L38: /*case 41*/ 
            return "Directory not empty";
            __L39: /*case 114*/ 
            return "Too many symbolic links encountered";
            __L40: /*case 122*/ 
            return "No message of desired type";
            __L41: /*case 111*/ 
            return "Identifier removed";
            __L42: /*case 125*/ 
            return "Device not a stream";
            __L43: /*case 120*/ 
            return "No data available";
            __L44: /*case 137*/ 
            return "Timer expired";
            __L45: /*case 124*/ 
            return "Out of streams resources";
            __L46: /*case 121*/ 
            return "Link has been severed";
            __L47: /*case 134*/ 
            return "Protocol error";
            __L48: /*case 104*/ 
            return "Not a data message";
            __L49: /*case 132*/ 
            return "Value too large for defined data type";
            __L50: /*case 42*/ 
            return "Illegal byte sequence";
            __L51: /*case 128*/ 
            return "Socket operation on non-socket";
            __L52: /*case 109*/ 
            return "Destination address required";
            __L53: /*case 115*/ 
            return "Message too long";
            __L54: /*case 136*/ 
            return "Protocol wrong type for socket";
            __L55: /*case 123*/ 
            return "Protocol not available";
            __L56: /*case 135*/ 
            return "Protocol not supported";
            __L57: /*case 130*/ 
            return "Operation not supported on transport endpoint";
            __L58: /*case 102*/ 
            return "Address family not supported by protocol";
            __L59: /*case 100*/ 
            return "Address already in use";
            __L60: /*case 101*/ 
            return "Cannot assign requested address";
            __L61: /*case 116*/ 
            return "Network is down";
            __L62: /*case 118*/ 
            return "Network is unreachable";
            __L63: /*case 117*/ 
            return "Network dropped connection because of reset";
            __L64: /*case 106*/ 
            return "Software caused connection abort";
            __L65: /*case 108*/ 
            return "Connection reset by peer";
            __L66: /*case 119*/ 
            return "No buffer space available";
            __L67: /*case 113*/ 
            return "Transport endpoint is already connected";
            __L68: /*case 126*/ 
            return "Transport endpoint is not connected";
            __L69: /*case 138*/ 
            return "Connection timed out";
            __L70: /*case 107*/ 
            return "Connection refused";
            __L71: /*case 110*/ 
            return "No route to host";
            __L72: /*case 103*/ 
            return "Operation already in progress";
            __L73: /*case 112*/ 
            return "Operation now in progress";
            __L74: /* default */ 
            goto __L0; /* break */

        }
        __L0:;
    }
    return "Unknown";
}


int windows_error_to_posix(int i)
{
    /*switch*/
    {
        int __v0 = i;
        if (__v0 == 5) goto __L1; /*case 5*/
        if (__v0 == 183) goto __L2; /*case 183*/
        if (__v0 == 20) goto __L3; /*case 20*/
        if (__v0 == 111) goto __L4; /*case 111*/
        if (__v0 == 170) goto __L5; /*case 170*/
        if (__v0 == 142) goto __L6; /*case 142*/
        if (__v0 == 82) goto __L7; /*case 82*/
        if (__v0 == 1011) goto __L8; /*case 1011*/
        if (__v0 == 1012) goto __L9; /*case 1012*/
        if (__v0 == 1013) goto __L10; /*case 1013*/
        if (__v0 == 16) goto __L11; /*case 16*/
        if (__v0 == 55) goto __L12; /*case 55*/
        if (__v0 == 2404) goto __L13; /*case 2404*/
        if (__v0 == 145) goto __L14; /*case 145*/
        if (__v0 == 267) goto __L15; /*case 267*/
        if (__v0 == 112) goto __L16; /*case 112*/
        if (__v0 == 80) goto __L17; /*case 80*/
        if (__v0 == 2) goto __L18; /*case 2*/
        if (__v0 == 39) goto __L19; /*case 39*/
        if (__v0 == 12) goto __L20; /*case 12*/
        if (__v0 == 15) goto __L21; /*case 15*/
        if (__v0 == 1) goto __L22; /*case 1*/
        if (__v0 == 6) goto __L23; /*case 6*/
        if (__v0 == 123) goto __L24; /*case 123*/
        if (__v0 == 33) goto __L25; /*case 33*/
        if (__v0 == 212) goto __L26; /*case 212*/
        if (__v0 == 131) goto __L27; /*case 131*/
        if (__v0 == 998) goto __L28; /*case 998*/
        if (__v0 == 8) goto __L29; /*case 8*/
        if (__v0 == 21) goto __L30; /*case 21*/
        if (__v0 == 17) goto __L31; /*case 17*/
        if (__v0 == 110) goto __L32; /*case 110*/
        if (__v0 == 2401) goto __L33; /*case 2401*/
        if (__v0 == 995) goto __L34; /*case 995*/
        if (__v0 == 14) goto __L35; /*case 14*/
        if (__v0 == 3) goto __L36; /*case 3*/
        if (__v0 == 30) goto __L37; /*case 30*/
        if (__v0 == 1237) goto __L38; /*case 1237*/
        if (__v0 == 25) goto __L39; /*case 25*/
        if (__v0 == 32) goto __L40; /*case 32*/
        if (__v0 == 4) goto __L41; /*case 4*/
        if (__v0 == 29) goto __L42; /*case 29*/
        if (__v0 == 19) goto __L43; /*case 19*/
        if (__v0 == 10013) goto __L44; /*case 10013*/
        if (__v0 == 10048) goto __L45; /*case 10048*/
        if (__v0 == 10049) goto __L46; /*case 10049*/
        if (__v0 == 10047) goto __L47; /*case 10047*/
        if (__v0 == 10037) goto __L48; /*case 10037*/
        if (__v0 == 10009) goto __L49; /*case 10009*/
        if (__v0 == 10053) goto __L50; /*case 10053*/
        if (__v0 == 10061) goto __L51; /*case 10061*/
        if (__v0 == 10054) goto __L52; /*case 10054*/
        if (__v0 == 10039) goto __L53; /*case 10039*/
        if (__v0 == 10014) goto __L54; /*case 10014*/
        if (__v0 == 10065) goto __L55; /*case 10065*/
        if (__v0 == 10036) goto __L56; /*case 10036*/
        if (__v0 == 10004) goto __L57; /*case 10004*/
        if (__v0 == 10022) goto __L58; /*case 10022*/
        if (__v0 == 10056) goto __L59; /*case 10056*/
        if (__v0 == 10024) goto __L60; /*case 10024*/
        if (__v0 == 10040) goto __L61; /*case 10040*/
        if (__v0 == 10050) goto __L62; /*case 10050*/
        if (__v0 == 10052) goto __L63; /*case 10052*/
        if (__v0 == 10051) goto __L64; /*case 10051*/
        if (__v0 == 10055) goto __L65; /*case 10055*/
        if (__v0 == 10042) goto __L66; /*case 10042*/
        if (__v0 == 10057) goto __L67; /*case 10057*/
        if (__v0 == 10038) goto __L68; /*case 10038*/
        if (__v0 == 10045) goto __L69; /*case 10045*/
        if (__v0 == 10043) goto __L70; /*case 10043*/
        if (__v0 == 10041) goto __L71; /*case 10041*/
        if (__v0 == 10060) goto __L72; /*case 10060*/
        if (__v0 == 10035) goto __L73; /*case 10035*/
        goto __L74; /* default */

        {
            __L1: /*case 5*/ 
            return 13;
            __L2: /*case 183*/ 
            return 17;
            __L3: /*case 20*/ 
            return 19;
            __L4: /*case 111*/ 
            return 38;
            __L5: /*case 170*/ 
            return 16;
            __L6: /*case 142*/ 
            return 16;
            __L7: /*case 82*/ 
            return 13;
            __L8: /*case 1011*/ 
            return 5;
            __L9: /*case 1012*/ 
            return 5;
            __L10: /*case 1013*/ 
            return 5;
            __L11: /*case 16*/ 
            return 13;
            __L12: /*case 55*/ 
            return 19;
            __L13: /*case 2404*/ 
            return 16;
            __L14: /*case 145*/ 
            return 41;
            __L15: /*case 267*/ 
            return 22;
            __L16: /*case 112*/ 
            return 28;
            __L17: /*case 80*/ 
            return 17;
            __L18: /*case 2*/ 
            return 2;
            __L19: /*case 39*/ 
            return 28;
            __L20: /*case 12*/ 
            return 13;
            __L21: /*case 15*/ 
            return 19;
            __L22: /*case 1*/ 
            return 40;
            __L23: /*case 6*/ 
            return 22;
            __L24: /*case 123*/ 
            return 22;
            __L25: /*case 33*/ 
            return 39;
            __L26: /*case 212*/ 
            return 39;
            __L27: /*case 131*/ 
            return 22;
            __L28: /*case 998*/ 
            return 13;
            __L29: /*case 8*/ 
            return 12;
            __L30: /*case 21*/ 
            return 11;
            __L31: /*case 17*/ 
            return 18;
            __L32: /*case 110*/ 
            return 5;
            __L33: /*case 2401*/ 
            return 16;
            __L34: /*case 995*/ 
            return 105;
            __L35: /*case 14*/ 
            return 12;
            __L36: /*case 3*/ 
            return 2;
            __L37: /*case 30*/ 
            return 5;
            __L38: /*case 1237*/ 
            return 11;
            __L39: /*case 25*/ 
            return 5;
            __L40: /*case 32*/ 
            return 13;
            __L41: /*case 4*/ 
            return 24;
            __L42: /*case 29*/ 
            return 5;
            __L43: /*case 19*/ 
            return 13;
            __L44: /*case 10013*/ 
            return 13;
            __L45: /*case 10048*/ 
            return 100;
            __L46: /*case 10049*/ 
            return 101;
            __L47: /*case 10047*/ 
            return 102;
            __L48: /*case 10037*/ 
            return 103;
            __L49: /*case 10009*/ 
            return 9;
            __L50: /*case 10053*/ 
            return 106;
            __L51: /*case 10061*/ 
            return 107;
            __L52: /*case 10054*/ 
            return 108;
            __L53: /*case 10039*/ 
            return 109;
            __L54: /*case 10014*/ 
            return 14;
            __L55: /*case 10065*/ 
            return 110;
            __L56: /*case 10036*/ 
            return 112;
            __L57: /*case 10004*/ 
            return 4;
            __L58: /*case 10022*/ 
            return 22;
            __L59: /*case 10056*/ 
            return 113;
            __L60: /*case 10024*/ 
            return 24;
            __L61: /*case 10040*/ 
            return 115;
            __L62: /*case 10050*/ 
            return 116;
            __L63: /*case 10052*/ 
            return 117;
            __L64: /*case 10051*/ 
            return 118;
            __L65: /*case 10055*/ 
            return 119;
            __L66: /*case 10042*/ 
            return 123;
            __L67: /*case 10057*/ 
            return 126;
            __L68: /*case 10038*/ 
            return 128;
            __L69: /*case 10045*/ 
            return 130;
            __L70: /*case 10043*/ 
            return 135;
            __L71: /*case 10041*/ 
            return 136;
            __L72: /*case 10060*/ 
            return 138;
            __L73: /*case 10035*/ 
            return 140;
            __L74: /* default */ 
            goto __L0; /* break */

        }
        __L0:;
    }
    return 1;
}


