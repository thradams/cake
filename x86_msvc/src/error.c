// Cake 0.12.05 target=x86_msvc

void throw_break_point()
{
}


char *get_posix_error_message(int error)
{
    /*switch*/
    {
        register int __Ck0_temp = error;
        if (__Ck0_temp == 1) goto _CKL1; /*case 1*/
        if (__Ck0_temp == 2) goto _CKL2; /*case 2*/
        if (__Ck0_temp == 3) goto _CKL3; /*case 3*/
        if (__Ck0_temp == 4) goto _CKL4; /*case 4*/
        if (__Ck0_temp == 5) goto _CKL5; /*case 5*/
        if (__Ck0_temp == 6) goto _CKL6; /*case 6*/
        if (__Ck0_temp == 7) goto _CKL7; /*case 7*/
        if (__Ck0_temp == 8) goto _CKL8; /*case 8*/
        if (__Ck0_temp == 9) goto _CKL9; /*case 9*/
        if (__Ck0_temp == 10) goto _CKL10; /*case 10*/
        if (__Ck0_temp == 11) goto _CKL11; /*case 11*/
        if (__Ck0_temp == 12) goto _CKL12; /*case 12*/
        if (__Ck0_temp == 13) goto _CKL13; /*case 13*/
        if (__Ck0_temp == 14) goto _CKL14; /*case 14*/
        if (__Ck0_temp == 16) goto _CKL15; /*case 16*/
        if (__Ck0_temp == 17) goto _CKL16; /*case 17*/
        if (__Ck0_temp == 18) goto _CKL17; /*case 18*/
        if (__Ck0_temp == 19) goto _CKL18; /*case 19*/
        if (__Ck0_temp == 20) goto _CKL19; /*case 20*/
        if (__Ck0_temp == 21) goto _CKL20; /*case 21*/
        if (__Ck0_temp == 22) goto _CKL21; /*case 22*/
        if (__Ck0_temp == 23) goto _CKL22; /*case 23*/
        if (__Ck0_temp == 24) goto _CKL23; /*case 24*/
        if (__Ck0_temp == 25) goto _CKL24; /*case 25*/
        if (__Ck0_temp == 139) goto _CKL25; /*case 139*/
        if (__Ck0_temp == 27) goto _CKL26; /*case 27*/
        if (__Ck0_temp == 28) goto _CKL27; /*case 28*/
        if (__Ck0_temp == 29) goto _CKL28; /*case 29*/
        if (__Ck0_temp == 30) goto _CKL29; /*case 30*/
        if (__Ck0_temp == 31) goto _CKL30; /*case 31*/
        if (__Ck0_temp == 32) goto _CKL31; /*case 32*/
        if (__Ck0_temp == 33) goto _CKL32; /*case 33*/
        if (__Ck0_temp == 34) goto _CKL33; /*case 34*/
        if (__Ck0_temp == 36) goto _CKL34; /*case 36*/
        if (__Ck0_temp == 38) goto _CKL35; /*case 38*/
        if (__Ck0_temp == 39) goto _CKL36; /*case 39*/
        if (__Ck0_temp == 40) goto _CKL37; /*case 40*/
        if (__Ck0_temp == 41) goto _CKL38; /*case 41*/
        if (__Ck0_temp == 114) goto _CKL39; /*case 114*/
        if (__Ck0_temp == 122) goto _CKL40; /*case 122*/
        if (__Ck0_temp == 111) goto _CKL41; /*case 111*/
        if (__Ck0_temp == 125) goto _CKL42; /*case 125*/
        if (__Ck0_temp == 120) goto _CKL43; /*case 120*/
        if (__Ck0_temp == 137) goto _CKL44; /*case 137*/
        if (__Ck0_temp == 124) goto _CKL45; /*case 124*/
        if (__Ck0_temp == 121) goto _CKL46; /*case 121*/
        if (__Ck0_temp == 134) goto _CKL47; /*case 134*/
        if (__Ck0_temp == 104) goto _CKL48; /*case 104*/
        if (__Ck0_temp == 132) goto _CKL49; /*case 132*/
        if (__Ck0_temp == 42) goto _CKL50; /*case 42*/
        if (__Ck0_temp == 128) goto _CKL51; /*case 128*/
        if (__Ck0_temp == 109) goto _CKL52; /*case 109*/
        if (__Ck0_temp == 115) goto _CKL53; /*case 115*/
        if (__Ck0_temp == 136) goto _CKL54; /*case 136*/
        if (__Ck0_temp == 123) goto _CKL55; /*case 123*/
        if (__Ck0_temp == 135) goto _CKL56; /*case 135*/
        if (__Ck0_temp == 130) goto _CKL57; /*case 130*/
        if (__Ck0_temp == 102) goto _CKL58; /*case 102*/
        if (__Ck0_temp == 100) goto _CKL59; /*case 100*/
        if (__Ck0_temp == 101) goto _CKL60; /*case 101*/
        if (__Ck0_temp == 116) goto _CKL61; /*case 116*/
        if (__Ck0_temp == 118) goto _CKL62; /*case 118*/
        if (__Ck0_temp == 117) goto _CKL63; /*case 117*/
        if (__Ck0_temp == 106) goto _CKL64; /*case 106*/
        if (__Ck0_temp == 108) goto _CKL65; /*case 108*/
        if (__Ck0_temp == 119) goto _CKL66; /*case 119*/
        if (__Ck0_temp == 113) goto _CKL67; /*case 113*/
        if (__Ck0_temp == 126) goto _CKL68; /*case 126*/
        if (__Ck0_temp == 138) goto _CKL69; /*case 138*/
        if (__Ck0_temp == 107) goto _CKL70; /*case 107*/
        if (__Ck0_temp == 110) goto _CKL71; /*case 110*/
        if (__Ck0_temp == 103) goto _CKL72; /*case 103*/
        if (__Ck0_temp == 112) goto _CKL73; /*case 112*/
        goto _CKL74;/*default*/

        {
            _CKL1: /*case 1*/ 
            return "Operation not permitted";
            _CKL2: /*case 2*/ 
            return "No such file or directory";
            _CKL3: /*case 3*/ 
            return "No such process";
            _CKL4: /*case 4*/ 
            return "Interrupted system call";
            _CKL5: /*case 5*/ 
            return "I/O error";
            _CKL6: /*case 6*/ 
            return "No such device or address";
            _CKL7: /*case 7*/ 
            return "Arg list too long";
            _CKL8: /*case 8*/ 
            return "Exec string_format error";
            _CKL9: /*case 9*/ 
            return "Bad file number";
            _CKL10: /*case 10*/ 
            return "No child processes";
            _CKL11: /*case 11*/ 
            return "Try again";
            _CKL12: /*case 12*/ 
            return "Out of memory";
            _CKL13: /*case 13*/ 
            return "Permission denied";
            _CKL14: /*case 14*/ 
            return "Bad address";
            _CKL15: /*case 16*/ 
            return "Device or resource busy";
            _CKL16: /*case 17*/ 
            return "File exists";
            _CKL17: /*case 18*/ 
            return "Cross-device link";
            _CKL18: /*case 19*/ 
            return "No such device";
            _CKL19: /*case 20*/ 
            return "Not a directory";
            _CKL20: /*case 21*/ 
            return "Is a directory";
            _CKL21: /*case 22*/ 
            return "Invalid argument";
            _CKL22: /*case 23*/ 
            return "File table overflow";
            _CKL23: /*case 24*/ 
            return "Too many open files";
            _CKL24: /*case 25*/ 
            return "Not a typewriter";
            _CKL25: /*case 139*/ 
            return "Text file busy";
            _CKL26: /*case 27*/ 
            return "File too large";
            _CKL27: /*case 28*/ 
            return "No space left on device";
            _CKL28: /*case 29*/ 
            return "Illegal seek";
            _CKL29: /*case 30*/ 
            return "Read-only file system";
            _CKL30: /*case 31*/ 
            return "Too many links";
            _CKL31: /*case 32*/ 
            return "Broken pipe";
            _CKL32: /*case 33*/ 
            return "Math argument out of domain of func";
            _CKL33: /*case 34*/ 
            return "Math result not representable";
            _CKL34: /*case 36*/ 
            return "Resource deadlock would occur";
            _CKL35: /*case 38*/ 
            return "File name too long";
            _CKL36: /*case 39*/ 
            return "No record locks available";
            _CKL37: /*case 40*/ 
            return "Function not implemented";
            _CKL38: /*case 41*/ 
            return "Directory not empty";
            _CKL39: /*case 114*/ 
            return "Too many symbolic links encountered";
            _CKL40: /*case 122*/ 
            return "No message of desired type";
            _CKL41: /*case 111*/ 
            return "Identifier removed";
            _CKL42: /*case 125*/ 
            return "Device not a stream";
            _CKL43: /*case 120*/ 
            return "No data available";
            _CKL44: /*case 137*/ 
            return "Timer expired";
            _CKL45: /*case 124*/ 
            return "Out of streams resources";
            _CKL46: /*case 121*/ 
            return "Link has been severed";
            _CKL47: /*case 134*/ 
            return "Protocol error";
            _CKL48: /*case 104*/ 
            return "Not a data message";
            _CKL49: /*case 132*/ 
            return "Value too large for defined data type";
            _CKL50: /*case 42*/ 
            return "Illegal byte sequence";
            _CKL51: /*case 128*/ 
            return "Socket operation on non-socket";
            _CKL52: /*case 109*/ 
            return "Destination address required";
            _CKL53: /*case 115*/ 
            return "Message too long";
            _CKL54: /*case 136*/ 
            return "Protocol wrong type for socket";
            _CKL55: /*case 123*/ 
            return "Protocol not available";
            _CKL56: /*case 135*/ 
            return "Protocol not supported";
            _CKL57: /*case 130*/ 
            return "Operation not supported on transport endpoint";
            _CKL58: /*case 102*/ 
            return "Address family not supported by protocol";
            _CKL59: /*case 100*/ 
            return "Address already in use";
            _CKL60: /*case 101*/ 
            return "Cannot assign requested address";
            _CKL61: /*case 116*/ 
            return "Network is down";
            _CKL62: /*case 118*/ 
            return "Network is unreachable";
            _CKL63: /*case 117*/ 
            return "Network dropped connection because of reset";
            _CKL64: /*case 106*/ 
            return "Software caused connection abort";
            _CKL65: /*case 108*/ 
            return "Connection reset by peer";
            _CKL66: /*case 119*/ 
            return "No buffer space available";
            _CKL67: /*case 113*/ 
            return "Transport endpoint is already connected";
            _CKL68: /*case 126*/ 
            return "Transport endpoint is not connected";
            _CKL69: /*case 138*/ 
            return "Connection timed out";
            _CKL70: /*case 107*/ 
            return "Connection refused";
            _CKL71: /*case 110*/ 
            return "No route to host";
            _CKL72: /*case 103*/ 
            return "Operation already in progress";
            _CKL73: /*case 112*/ 
            return "Operation now in progress";
            _CKL74: /*default*/ 
            goto _CKL0; /*break*/

        }
        _CKL0:;
    }
    return "Unknown";
}


int windows_error_to_posix(int i)
{
    /*switch*/
    {
        register int __Ck1_temp = i;
        if (__Ck1_temp == 5) goto _CKL1; /*case 5*/
        if (__Ck1_temp == 183) goto _CKL2; /*case 183*/
        if (__Ck1_temp == 20) goto _CKL3; /*case 20*/
        if (__Ck1_temp == 111) goto _CKL4; /*case 111*/
        if (__Ck1_temp == 170) goto _CKL5; /*case 170*/
        if (__Ck1_temp == 142) goto _CKL6; /*case 142*/
        if (__Ck1_temp == 82) goto _CKL7; /*case 82*/
        if (__Ck1_temp == 1011) goto _CKL8; /*case 1011*/
        if (__Ck1_temp == 1012) goto _CKL9; /*case 1012*/
        if (__Ck1_temp == 1013) goto _CKL10; /*case 1013*/
        if (__Ck1_temp == 16) goto _CKL11; /*case 16*/
        if (__Ck1_temp == 55) goto _CKL12; /*case 55*/
        if (__Ck1_temp == 2404) goto _CKL13; /*case 2404*/
        if (__Ck1_temp == 145) goto _CKL14; /*case 145*/
        if (__Ck1_temp == 267) goto _CKL15; /*case 267*/
        if (__Ck1_temp == 112) goto _CKL16; /*case 112*/
        if (__Ck1_temp == 80) goto _CKL17; /*case 80*/
        if (__Ck1_temp == 2) goto _CKL18; /*case 2*/
        if (__Ck1_temp == 39) goto _CKL19; /*case 39*/
        if (__Ck1_temp == 12) goto _CKL20; /*case 12*/
        if (__Ck1_temp == 15) goto _CKL21; /*case 15*/
        if (__Ck1_temp == 1) goto _CKL22; /*case 1*/
        if (__Ck1_temp == 6) goto _CKL23; /*case 6*/
        if (__Ck1_temp == 123) goto _CKL24; /*case 123*/
        if (__Ck1_temp == 33) goto _CKL25; /*case 33*/
        if (__Ck1_temp == 212) goto _CKL26; /*case 212*/
        if (__Ck1_temp == 131) goto _CKL27; /*case 131*/
        if (__Ck1_temp == 998) goto _CKL28; /*case 998*/
        if (__Ck1_temp == 8) goto _CKL29; /*case 8*/
        if (__Ck1_temp == 21) goto _CKL30; /*case 21*/
        if (__Ck1_temp == 17) goto _CKL31; /*case 17*/
        if (__Ck1_temp == 110) goto _CKL32; /*case 110*/
        if (__Ck1_temp == 2401) goto _CKL33; /*case 2401*/
        if (__Ck1_temp == 995) goto _CKL34; /*case 995*/
        if (__Ck1_temp == 14) goto _CKL35; /*case 14*/
        if (__Ck1_temp == 3) goto _CKL36; /*case 3*/
        if (__Ck1_temp == 30) goto _CKL37; /*case 30*/
        if (__Ck1_temp == 1237) goto _CKL38; /*case 1237*/
        if (__Ck1_temp == 25) goto _CKL39; /*case 25*/
        if (__Ck1_temp == 32) goto _CKL40; /*case 32*/
        if (__Ck1_temp == 4) goto _CKL41; /*case 4*/
        if (__Ck1_temp == 29) goto _CKL42; /*case 29*/
        if (__Ck1_temp == 19) goto _CKL43; /*case 19*/
        if (__Ck1_temp == 10013) goto _CKL44; /*case 10013*/
        if (__Ck1_temp == 10048) goto _CKL45; /*case 10048*/
        if (__Ck1_temp == 10049) goto _CKL46; /*case 10049*/
        if (__Ck1_temp == 10047) goto _CKL47; /*case 10047*/
        if (__Ck1_temp == 10037) goto _CKL48; /*case 10037*/
        if (__Ck1_temp == 10009) goto _CKL49; /*case 10009*/
        if (__Ck1_temp == 10053) goto _CKL50; /*case 10053*/
        if (__Ck1_temp == 10061) goto _CKL51; /*case 10061*/
        if (__Ck1_temp == 10054) goto _CKL52; /*case 10054*/
        if (__Ck1_temp == 10039) goto _CKL53; /*case 10039*/
        if (__Ck1_temp == 10014) goto _CKL54; /*case 10014*/
        if (__Ck1_temp == 10065) goto _CKL55; /*case 10065*/
        if (__Ck1_temp == 10036) goto _CKL56; /*case 10036*/
        if (__Ck1_temp == 10004) goto _CKL57; /*case 10004*/
        if (__Ck1_temp == 10022) goto _CKL58; /*case 10022*/
        if (__Ck1_temp == 10056) goto _CKL59; /*case 10056*/
        if (__Ck1_temp == 10024) goto _CKL60; /*case 10024*/
        if (__Ck1_temp == 10040) goto _CKL61; /*case 10040*/
        if (__Ck1_temp == 10050) goto _CKL62; /*case 10050*/
        if (__Ck1_temp == 10052) goto _CKL63; /*case 10052*/
        if (__Ck1_temp == 10051) goto _CKL64; /*case 10051*/
        if (__Ck1_temp == 10055) goto _CKL65; /*case 10055*/
        if (__Ck1_temp == 10042) goto _CKL66; /*case 10042*/
        if (__Ck1_temp == 10057) goto _CKL67; /*case 10057*/
        if (__Ck1_temp == 10038) goto _CKL68; /*case 10038*/
        if (__Ck1_temp == 10045) goto _CKL69; /*case 10045*/
        if (__Ck1_temp == 10043) goto _CKL70; /*case 10043*/
        if (__Ck1_temp == 10041) goto _CKL71; /*case 10041*/
        if (__Ck1_temp == 10060) goto _CKL72; /*case 10060*/
        if (__Ck1_temp == 10035) goto _CKL73; /*case 10035*/
        goto _CKL74;/*default*/

        {
            _CKL1: /*case 5*/ 
            return 13;
            _CKL2: /*case 183*/ 
            return 17;
            _CKL3: /*case 20*/ 
            return 19;
            _CKL4: /*case 111*/ 
            return 38;
            _CKL5: /*case 170*/ 
            return 16;
            _CKL6: /*case 142*/ 
            return 16;
            _CKL7: /*case 82*/ 
            return 13;
            _CKL8: /*case 1011*/ 
            return 5;
            _CKL9: /*case 1012*/ 
            return 5;
            _CKL10: /*case 1013*/ 
            return 5;
            _CKL11: /*case 16*/ 
            return 13;
            _CKL12: /*case 55*/ 
            return 19;
            _CKL13: /*case 2404*/ 
            return 16;
            _CKL14: /*case 145*/ 
            return 41;
            _CKL15: /*case 267*/ 
            return 22;
            _CKL16: /*case 112*/ 
            return 28;
            _CKL17: /*case 80*/ 
            return 17;
            _CKL18: /*case 2*/ 
            return 2;
            _CKL19: /*case 39*/ 
            return 28;
            _CKL20: /*case 12*/ 
            return 13;
            _CKL21: /*case 15*/ 
            return 19;
            _CKL22: /*case 1*/ 
            return 40;
            _CKL23: /*case 6*/ 
            return 22;
            _CKL24: /*case 123*/ 
            return 22;
            _CKL25: /*case 33*/ 
            return 39;
            _CKL26: /*case 212*/ 
            return 39;
            _CKL27: /*case 131*/ 
            return 22;
            _CKL28: /*case 998*/ 
            return 13;
            _CKL29: /*case 8*/ 
            return 12;
            _CKL30: /*case 21*/ 
            return 11;
            _CKL31: /*case 17*/ 
            return 18;
            _CKL32: /*case 110*/ 
            return 5;
            _CKL33: /*case 2401*/ 
            return 16;
            _CKL34: /*case 995*/ 
            return 105;
            _CKL35: /*case 14*/ 
            return 12;
            _CKL36: /*case 3*/ 
            return 2;
            _CKL37: /*case 30*/ 
            return 5;
            _CKL38: /*case 1237*/ 
            return 11;
            _CKL39: /*case 25*/ 
            return 5;
            _CKL40: /*case 32*/ 
            return 13;
            _CKL41: /*case 4*/ 
            return 24;
            _CKL42: /*case 29*/ 
            return 5;
            _CKL43: /*case 19*/ 
            return 13;
            _CKL44: /*case 10013*/ 
            return 13;
            _CKL45: /*case 10048*/ 
            return 100;
            _CKL46: /*case 10049*/ 
            return 101;
            _CKL47: /*case 10047*/ 
            return 102;
            _CKL48: /*case 10037*/ 
            return 103;
            _CKL49: /*case 10009*/ 
            return 9;
            _CKL50: /*case 10053*/ 
            return 106;
            _CKL51: /*case 10061*/ 
            return 107;
            _CKL52: /*case 10054*/ 
            return 108;
            _CKL53: /*case 10039*/ 
            return 109;
            _CKL54: /*case 10014*/ 
            return 14;
            _CKL55: /*case 10065*/ 
            return 110;
            _CKL56: /*case 10036*/ 
            return 112;
            _CKL57: /*case 10004*/ 
            return 4;
            _CKL58: /*case 10022*/ 
            return 22;
            _CKL59: /*case 10056*/ 
            return 113;
            _CKL60: /*case 10024*/ 
            return 24;
            _CKL61: /*case 10040*/ 
            return 115;
            _CKL62: /*case 10050*/ 
            return 116;
            _CKL63: /*case 10052*/ 
            return 117;
            _CKL64: /*case 10051*/ 
            return 118;
            _CKL65: /*case 10055*/ 
            return 119;
            _CKL66: /*case 10042*/ 
            return 123;
            _CKL67: /*case 10057*/ 
            return 126;
            _CKL68: /*case 10038*/ 
            return 128;
            _CKL69: /*case 10045*/ 
            return 130;
            _CKL70: /*case 10043*/ 
            return 135;
            _CKL71: /*case 10041*/ 
            return 136;
            _CKL72: /*case 10060*/ 
            return 138;
            _CKL73: /*case 10035*/ 
            return 140;
            _CKL74: /*default*/ 
            goto _CKL0; /*break*/

        }
        _CKL0:;
    }
    return 1;
}


