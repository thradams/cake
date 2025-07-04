
void throw_break_point()
{
}

char *get_posix_error_message(int error)
{
    /*switch*/
    {
        register int _R0 = error;
        if (_R0 == 1) goto _CKL1; /*case 1*/
        if (_R0 == 2) goto _CKL2; /*case 2*/
        if (_R0 == 3) goto _CKL3; /*case 3*/
        if (_R0 == 4) goto _CKL4; /*case 4*/
        if (_R0 == 5) goto _CKL5; /*case 5*/
        if (_R0 == 6) goto _CKL6; /*case 6*/
        if (_R0 == 7) goto _CKL7; /*case 7*/
        if (_R0 == 8) goto _CKL8; /*case 8*/
        if (_R0 == 9) goto _CKL9; /*case 9*/
        if (_R0 == 10) goto _CKL10; /*case 10*/
        if (_R0 == 11) goto _CKL11; /*case 11*/
        if (_R0 == 12) goto _CKL12; /*case 12*/
        if (_R0 == 13) goto _CKL13; /*case 13*/
        if (_R0 == 14) goto _CKL14; /*case 14*/
        if (_R0 == 16) goto _CKL15; /*case 16*/
        if (_R0 == 17) goto _CKL16; /*case 17*/
        if (_R0 == 18) goto _CKL17; /*case 18*/
        if (_R0 == 19) goto _CKL18; /*case 19*/
        if (_R0 == 20) goto _CKL19; /*case 20*/
        if (_R0 == 21) goto _CKL20; /*case 21*/
        if (_R0 == 22) goto _CKL21; /*case 22*/
        if (_R0 == 23) goto _CKL22; /*case 23*/
        if (_R0 == 24) goto _CKL23; /*case 24*/
        if (_R0 == 25) goto _CKL24; /*case 25*/
        if (_R0 == 139) goto _CKL25; /*case 139*/
        if (_R0 == 27) goto _CKL26; /*case 27*/
        if (_R0 == 28) goto _CKL27; /*case 28*/
        if (_R0 == 29) goto _CKL28; /*case 29*/
        if (_R0 == 30) goto _CKL29; /*case 30*/
        if (_R0 == 31) goto _CKL30; /*case 31*/
        if (_R0 == 32) goto _CKL31; /*case 32*/
        if (_R0 == 33) goto _CKL32; /*case 33*/
        if (_R0 == 34) goto _CKL33; /*case 34*/
        if (_R0 == 36) goto _CKL34; /*case 36*/
        if (_R0 == 38) goto _CKL35; /*case 38*/
        if (_R0 == 39) goto _CKL36; /*case 39*/
        if (_R0 == 40) goto _CKL37; /*case 40*/
        if (_R0 == 41) goto _CKL38; /*case 41*/
        if (_R0 == 114) goto _CKL39; /*case 114*/
        if (_R0 == 122) goto _CKL40; /*case 122*/
        if (_R0 == 111) goto _CKL41; /*case 111*/
        if (_R0 == 125) goto _CKL42; /*case 125*/
        if (_R0 == 120) goto _CKL43; /*case 120*/
        if (_R0 == 137) goto _CKL44; /*case 137*/
        if (_R0 == 124) goto _CKL45; /*case 124*/
        if (_R0 == 121) goto _CKL46; /*case 121*/
        if (_R0 == 134) goto _CKL47; /*case 134*/
        if (_R0 == 104) goto _CKL48; /*case 104*/
        if (_R0 == 132) goto _CKL49; /*case 132*/
        if (_R0 == 42) goto _CKL50; /*case 42*/
        if (_R0 == 128) goto _CKL51; /*case 128*/
        if (_R0 == 109) goto _CKL52; /*case 109*/
        if (_R0 == 115) goto _CKL53; /*case 115*/
        if (_R0 == 136) goto _CKL54; /*case 136*/
        if (_R0 == 123) goto _CKL55; /*case 123*/
        if (_R0 == 135) goto _CKL56; /*case 135*/
        if (_R0 == 130) goto _CKL57; /*case 130*/
        if (_R0 == 102) goto _CKL58; /*case 102*/
        if (_R0 == 100) goto _CKL59; /*case 100*/
        if (_R0 == 101) goto _CKL60; /*case 101*/
        if (_R0 == 116) goto _CKL61; /*case 116*/
        if (_R0 == 118) goto _CKL62; /*case 118*/
        if (_R0 == 117) goto _CKL63; /*case 117*/
        if (_R0 == 106) goto _CKL64; /*case 106*/
        if (_R0 == 108) goto _CKL65; /*case 108*/
        if (_R0 == 119) goto _CKL66; /*case 119*/
        if (_R0 == 113) goto _CKL67; /*case 113*/
        if (_R0 == 126) goto _CKL68; /*case 126*/
        if (_R0 == 138) goto _CKL69; /*case 138*/
        if (_R0 == 107) goto _CKL70; /*case 107*/
        if (_R0 == 110) goto _CKL71; /*case 110*/
        if (_R0 == 103) goto _CKL72; /*case 103*/
        if (_R0 == 112) goto _CKL73; /*case 112*/
        goto /*default*/ _CKL74;

        {
            /*case 1*/ _CKL1:
            return "Operation not permitted";
            /*case 2*/ _CKL2:
            return "No such file or directory";
            /*case 3*/ _CKL3:
            return "No such process";
            /*case 4*/ _CKL4:
            return "Interrupted system call";
            /*case 5*/ _CKL5:
            return "I/O error";
            /*case 6*/ _CKL6:
            return "No such device or address";
            /*case 7*/ _CKL7:
            return "Arg list too long";
            /*case 8*/ _CKL8:
            return "Exec string_format error";
            /*case 9*/ _CKL9:
            return "Bad file number";
            /*case 10*/ _CKL10:
            return "No child processes";
            /*case 11*/ _CKL11:
            return "Try again";
            /*case 12*/ _CKL12:
            return "Out of memory";
            /*case 13*/ _CKL13:
            return "Permission denied";
            /*case 14*/ _CKL14:
            return "Bad address";
            /*case 16*/ _CKL15:
            return "Device or resource busy";
            /*case 17*/ _CKL16:
            return "File exists";
            /*case 18*/ _CKL17:
            return "Cross-device link";
            /*case 19*/ _CKL18:
            return "No such device";
            /*case 20*/ _CKL19:
            return "Not a directory";
            /*case 21*/ _CKL20:
            return "Is a directory";
            /*case 22*/ _CKL21:
            return "Invalid argument";
            /*case 23*/ _CKL22:
            return "File table overflow";
            /*case 24*/ _CKL23:
            return "Too many open files";
            /*case 25*/ _CKL24:
            return "Not a typewriter";
            /*case 139*/ _CKL25:
            return "Text file busy";
            /*case 27*/ _CKL26:
            return "File too large";
            /*case 28*/ _CKL27:
            return "No space left on device";
            /*case 29*/ _CKL28:
            return "Illegal seek";
            /*case 30*/ _CKL29:
            return "Read-only file system";
            /*case 31*/ _CKL30:
            return "Too many links";
            /*case 32*/ _CKL31:
            return "Broken pipe";
            /*case 33*/ _CKL32:
            return "Math argument out of domain of func";
            /*case 34*/ _CKL33:
            return "Math result not representable";
            /*case 36*/ _CKL34:
            return "Resource deadlock would occur";
            /*case 38*/ _CKL35:
            return "File name too long";
            /*case 39*/ _CKL36:
            return "No record locks available";
            /*case 40*/ _CKL37:
            return "Function not implemented";
            /*case 41*/ _CKL38:
            return "Directory not empty";
            /*case 114*/ _CKL39:
            return "Too many symbolic links encountered";
            /*case 122*/ _CKL40:
            return "No message of desired type";
            /*case 111*/ _CKL41:
            return "Identifier removed";
            /*case 125*/ _CKL42:
            return "Device not a stream";
            /*case 120*/ _CKL43:
            return "No data available";
            /*case 137*/ _CKL44:
            return "Timer expired";
            /*case 124*/ _CKL45:
            return "Out of streams resources";
            /*case 121*/ _CKL46:
            return "Link has been severed";
            /*case 134*/ _CKL47:
            return "Protocol error";
            /*case 104*/ _CKL48:
            return "Not a data message";
            /*case 132*/ _CKL49:
            return "Value too large for defined data type";
            /*case 42*/ _CKL50:
            return "Illegal byte sequence";
            /*case 128*/ _CKL51:
            return "Socket operation on non-socket";
            /*case 109*/ _CKL52:
            return "Destination address required";
            /*case 115*/ _CKL53:
            return "Message too long";
            /*case 136*/ _CKL54:
            return "Protocol wrong type for socket";
            /*case 123*/ _CKL55:
            return "Protocol not available";
            /*case 135*/ _CKL56:
            return "Protocol not supported";
            /*case 130*/ _CKL57:
            return "Operation not supported on transport endpoint";
            /*case 102*/ _CKL58:
            return "Address family not supported by protocol";
            /*case 100*/ _CKL59:
            return "Address already in use";
            /*case 101*/ _CKL60:
            return "Cannot assign requested address";
            /*case 116*/ _CKL61:
            return "Network is down";
            /*case 118*/ _CKL62:
            return "Network is unreachable";
            /*case 117*/ _CKL63:
            return "Network dropped connection because of reset";
            /*case 106*/ _CKL64:
            return "Software caused connection abort";
            /*case 108*/ _CKL65:
            return "Connection reset by peer";
            /*case 119*/ _CKL66:
            return "No buffer space available";
            /*case 113*/ _CKL67:
            return "Transport endpoint is already connected";
            /*case 126*/ _CKL68:
            return "Transport endpoint is not connected";
            /*case 138*/ _CKL69:
            return "Connection timed out";
            /*case 107*/ _CKL70:
            return "Connection refused";
            /*case 110*/ _CKL71:
            return "No route to host";
            /*case 103*/ _CKL72:
            return "Operation already in progress";
            /*case 112*/ _CKL73:
            return "Operation now in progress";
            /*default*/ _CKL74:
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
    return "Unknown";
}

int windows_error_to_posix(int i)
{
    /*switch*/
    {
        register int _R1 = i;
        if (_R1 == 5) goto _CKL1; /*case 5*/
        if (_R1 == 183) goto _CKL2; /*case 183*/
        if (_R1 == 20) goto _CKL3; /*case 20*/
        if (_R1 == 111) goto _CKL4; /*case 111*/
        if (_R1 == 170) goto _CKL5; /*case 170*/
        if (_R1 == 142) goto _CKL6; /*case 142*/
        if (_R1 == 82) goto _CKL7; /*case 82*/
        if (_R1 == 1011) goto _CKL8; /*case 1011*/
        if (_R1 == 1012) goto _CKL9; /*case 1012*/
        if (_R1 == 1013) goto _CKL10; /*case 1013*/
        if (_R1 == 16) goto _CKL11; /*case 16*/
        if (_R1 == 55) goto _CKL12; /*case 55*/
        if (_R1 == 2404) goto _CKL13; /*case 2404*/
        if (_R1 == 145) goto _CKL14; /*case 145*/
        if (_R1 == 267) goto _CKL15; /*case 267*/
        if (_R1 == 112) goto _CKL16; /*case 112*/
        if (_R1 == 80) goto _CKL17; /*case 80*/
        if (_R1 == 2) goto _CKL18; /*case 2*/
        if (_R1 == 39) goto _CKL19; /*case 39*/
        if (_R1 == 12) goto _CKL20; /*case 12*/
        if (_R1 == 15) goto _CKL21; /*case 15*/
        if (_R1 == 1) goto _CKL22; /*case 1*/
        if (_R1 == 6) goto _CKL23; /*case 6*/
        if (_R1 == 123) goto _CKL24; /*case 123*/
        if (_R1 == 33) goto _CKL25; /*case 33*/
        if (_R1 == 212) goto _CKL26; /*case 212*/
        if (_R1 == 131) goto _CKL27; /*case 131*/
        if (_R1 == 998) goto _CKL28; /*case 998*/
        if (_R1 == 8) goto _CKL29; /*case 8*/
        if (_R1 == 21) goto _CKL30; /*case 21*/
        if (_R1 == 17) goto _CKL31; /*case 17*/
        if (_R1 == 110) goto _CKL32; /*case 110*/
        if (_R1 == 2401) goto _CKL33; /*case 2401*/
        if (_R1 == 995) goto _CKL34; /*case 995*/
        if (_R1 == 14) goto _CKL35; /*case 14*/
        if (_R1 == 3) goto _CKL36; /*case 3*/
        if (_R1 == 30) goto _CKL37; /*case 30*/
        if (_R1 == 1237) goto _CKL38; /*case 1237*/
        if (_R1 == 25) goto _CKL39; /*case 25*/
        if (_R1 == 32) goto _CKL40; /*case 32*/
        if (_R1 == 4) goto _CKL41; /*case 4*/
        if (_R1 == 29) goto _CKL42; /*case 29*/
        if (_R1 == 19) goto _CKL43; /*case 19*/
        if (_R1 == 10013) goto _CKL44; /*case 10013*/
        if (_R1 == 10048) goto _CKL45; /*case 10048*/
        if (_R1 == 10049) goto _CKL46; /*case 10049*/
        if (_R1 == 10047) goto _CKL47; /*case 10047*/
        if (_R1 == 10037) goto _CKL48; /*case 10037*/
        if (_R1 == 10009) goto _CKL49; /*case 10009*/
        if (_R1 == 10053) goto _CKL50; /*case 10053*/
        if (_R1 == 10061) goto _CKL51; /*case 10061*/
        if (_R1 == 10054) goto _CKL52; /*case 10054*/
        if (_R1 == 10039) goto _CKL53; /*case 10039*/
        if (_R1 == 10014) goto _CKL54; /*case 10014*/
        if (_R1 == 10065) goto _CKL55; /*case 10065*/
        if (_R1 == 10036) goto _CKL56; /*case 10036*/
        if (_R1 == 10004) goto _CKL57; /*case 10004*/
        if (_R1 == 10022) goto _CKL58; /*case 10022*/
        if (_R1 == 10056) goto _CKL59; /*case 10056*/
        if (_R1 == 10024) goto _CKL60; /*case 10024*/
        if (_R1 == 10040) goto _CKL61; /*case 10040*/
        if (_R1 == 10050) goto _CKL62; /*case 10050*/
        if (_R1 == 10052) goto _CKL63; /*case 10052*/
        if (_R1 == 10051) goto _CKL64; /*case 10051*/
        if (_R1 == 10055) goto _CKL65; /*case 10055*/
        if (_R1 == 10042) goto _CKL66; /*case 10042*/
        if (_R1 == 10057) goto _CKL67; /*case 10057*/
        if (_R1 == 10038) goto _CKL68; /*case 10038*/
        if (_R1 == 10045) goto _CKL69; /*case 10045*/
        if (_R1 == 10043) goto _CKL70; /*case 10043*/
        if (_R1 == 10041) goto _CKL71; /*case 10041*/
        if (_R1 == 10060) goto _CKL72; /*case 10060*/
        if (_R1 == 10035) goto _CKL73; /*case 10035*/
        goto /*default*/ _CKL74;

        {
            /*case 5*/ _CKL1:
            return 13;
            /*case 183*/ _CKL2:
            return 17;
            /*case 20*/ _CKL3:
            return 19;
            /*case 111*/ _CKL4:
            return 38;
            /*case 170*/ _CKL5:
            return 16;
            /*case 142*/ _CKL6:
            return 16;
            /*case 82*/ _CKL7:
            return 13;
            /*case 1011*/ _CKL8:
            return 5;
            /*case 1012*/ _CKL9:
            return 5;
            /*case 1013*/ _CKL10:
            return 5;
            /*case 16*/ _CKL11:
            return 13;
            /*case 55*/ _CKL12:
            return 19;
            /*case 2404*/ _CKL13:
            return 16;
            /*case 145*/ _CKL14:
            return 41;
            /*case 267*/ _CKL15:
            return 22;
            /*case 112*/ _CKL16:
            return 28;
            /*case 80*/ _CKL17:
            return 17;
            /*case 2*/ _CKL18:
            return 2;
            /*case 39*/ _CKL19:
            return 28;
            /*case 12*/ _CKL20:
            return 13;
            /*case 15*/ _CKL21:
            return 19;
            /*case 1*/ _CKL22:
            return 40;
            /*case 6*/ _CKL23:
            return 22;
            /*case 123*/ _CKL24:
            return 22;
            /*case 33*/ _CKL25:
            return 39;
            /*case 212*/ _CKL26:
            return 39;
            /*case 131*/ _CKL27:
            return 22;
            /*case 998*/ _CKL28:
            return 13;
            /*case 8*/ _CKL29:
            return 12;
            /*case 21*/ _CKL30:
            return 11;
            /*case 17*/ _CKL31:
            return 18;
            /*case 110*/ _CKL32:
            return 5;
            /*case 2401*/ _CKL33:
            return 16;
            /*case 995*/ _CKL34:
            return 105;
            /*case 14*/ _CKL35:
            return 12;
            /*case 3*/ _CKL36:
            return 2;
            /*case 30*/ _CKL37:
            return 5;
            /*case 1237*/ _CKL38:
            return 11;
            /*case 25*/ _CKL39:
            return 5;
            /*case 32*/ _CKL40:
            return 13;
            /*case 4*/ _CKL41:
            return 24;
            /*case 29*/ _CKL42:
            return 5;
            /*case 19*/ _CKL43:
            return 13;
            /*case 10013*/ _CKL44:
            return 13;
            /*case 10048*/ _CKL45:
            return 100;
            /*case 10049*/ _CKL46:
            return 101;
            /*case 10047*/ _CKL47:
            return 102;
            /*case 10037*/ _CKL48:
            return 103;
            /*case 10009*/ _CKL49:
            return 9;
            /*case 10053*/ _CKL50:
            return 106;
            /*case 10061*/ _CKL51:
            return 107;
            /*case 10054*/ _CKL52:
            return 108;
            /*case 10039*/ _CKL53:
            return 109;
            /*case 10014*/ _CKL54:
            return 14;
            /*case 10065*/ _CKL55:
            return 110;
            /*case 10036*/ _CKL56:
            return 112;
            /*case 10004*/ _CKL57:
            return 4;
            /*case 10022*/ _CKL58:
            return 22;
            /*case 10056*/ _CKL59:
            return 113;
            /*case 10024*/ _CKL60:
            return 24;
            /*case 10040*/ _CKL61:
            return 115;
            /*case 10050*/ _CKL62:
            return 116;
            /*case 10052*/ _CKL63:
            return 117;
            /*case 10051*/ _CKL64:
            return 118;
            /*case 10055*/ _CKL65:
            return 119;
            /*case 10042*/ _CKL66:
            return 123;
            /*case 10057*/ _CKL67:
            return 126;
            /*case 10038*/ _CKL68:
            return 128;
            /*case 10045*/ _CKL69:
            return 130;
            /*case 10043*/ _CKL70:
            return 135;
            /*case 10041*/ _CKL71:
            return 136;
            /*case 10060*/ _CKL72:
            return 138;
            /*case 10035*/ _CKL73:
            return 140;
            /*default*/ _CKL74:
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
    return 1;
}

