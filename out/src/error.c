
void throw_break_point()
{
}

char *get_posix_error_message(int error)
{
    switch (error)
    {
        case 1 :
        return "Operation not permitted";
        case 2 :
        return "No such file or directory";
        case 3 :
        return "No such process";
        case 4 :
        return "Interrupted system call";
        case 5 :
        return "I/O error";
        case 6 :
        return "No such device or address";
        case 7 :
        return "Arg list too long";
        case 8 :
        return "Exec string_format error";
        case 9 :
        return "Bad file number";
        case 10 :
        return "No child processes";
        case 11 :
        return "Try again";
        case 12 :
        return "Out of memory";
        case 13 :
        return "Permission denied";
        case 14 :
        return "Bad address";
        case 16 :
        return "Device or resource busy";
        case 17 :
        return "File exists";
        case 18 :
        return "Cross-device link";
        case 19 :
        return "No such device";
        case 20 :
        return "Not a directory";
        case 21 :
        return "Is a directory";
        case 22 :
        return "Invalid argument";
        case 23 :
        return "File table overflow";
        case 24 :
        return "Too many open files";
        case 25 :
        return "Not a typewriter";
        case 139 :
        return "Text file busy";
        case 27 :
        return "File too large";
        case 28 :
        return "No space left on device";
        case 29 :
        return "Illegal seek";
        case 30 :
        return "Read-only file system";
        case 31 :
        return "Too many links";
        case 32 :
        return "Broken pipe";
        case 33 :
        return "Math argument out of domain of func";
        case 34 :
        return "Math result not representable";
        case 36 :
        return "Resource deadlock would occur";
        case 38 :
        return "File name too long";
        case 39 :
        return "No record locks available";
        case 40 :
        return "Function not implemented";
        case 41 :
        return "Directory not empty";
        case 114 :
        return "Too many symbolic links encountered";
        case 122 :
        return "No message of desired type";
        case 111 :
        return "Identifier removed";
        case 125 :
        return "Device not a stream";
        case 120 :
        return "No data available";
        case 137 :
        return "Timer expired";
        case 124 :
        return "Out of streams resources";
        case 121 :
        return "Link has been severed";
        case 134 :
        return "Protocol error";
        case 104 :
        return "Not a data message";
        case 132 :
        return "Value too large for defined data type";
        case 42 :
        return "Illegal byte sequence";
        case 128 :
        return "Socket operation on non-socket";
        case 109 :
        return "Destination address required";
        case 115 :
        return "Message too long";
        case 136 :
        return "Protocol wrong type for socket";
        case 123 :
        return "Protocol not available";
        case 135 :
        return "Protocol not supported";
        case 130 :
        return "Operation not supported on transport endpoint";
        case 102 :
        return "Address family not supported by protocol";
        case 100 :
        return "Address already in use";
        case 101 :
        return "Cannot assign requested address";
        case 116 :
        return "Network is down";
        case 118 :
        return "Network is unreachable";
        case 117 :
        return "Network dropped connection because of reset";
        case 106 :
        return "Software caused connection abort";
        case 108 :
        return "Connection reset by peer";
        case 119 :
        return "No buffer space available";
        case 113 :
        return "Transport endpoint is already connected";
        case 126 :
        return "Transport endpoint is not connected";
        case 138 :
        return "Connection timed out";
        case 107 :
        return "Connection refused";
        case 110 :
        return "No route to host";
        case 103 :
        return "Operation already in progress";
        case 112 :
        return "Operation now in progress";
        default:
        break;
    }
    return "Unknown";
}

int windows_error_to_posix(int i)
{
    switch (i)
    {
        case 5L :
        return 13;
        case 183L :
        return 17;
        case 20L :
        return 19;
        case 111L :
        return 38;
        case 170L :
        return 16;
        case 142L :
        return 16;
        case 82L :
        return 13;
        case 1011L :
        return 5;
        case 1012L :
        return 5;
        case 1013L :
        return 5;
        case 16L :
        return 13;
        case 55L :
        return 19;
        case 2404L :
        return 16;
        case 145L :
        return 41;
        case 267L :
        return 22;
        case 112L :
        return 28;
        case 80L :
        return 17;
        case 2L :
        return 2;
        case 39L :
        return 28;
        case 12L :
        return 13;
        case 15L :
        return 19;
        case 1L :
        return 40;
        case 6L :
        return 22;
        case 123L :
        return 22;
        case 33L :
        return 39;
        case 212L :
        return 39;
        case 131L :
        return 22;
        case 998L :
        return 13;
        case 8L :
        return 12;
        case 21L :
        return 11;
        case 17L :
        return 18;
        case 110L :
        return 5;
        case 2401L :
        return 16;
        case 995L :
        return 105;
        case 14L :
        return 12;
        case 3L :
        return 2;
        case 30L :
        return 5;
        case 1237L :
        return 11;
        case 25L :
        return 5;
        case 32L :
        return 13;
        case 4L :
        return 24;
        case 29L :
        return 5;
        case 19L :
        return 13;
        case 10013L :
        return 13;
        case 10048L :
        return 100;
        case 10049L :
        return 101;
        case 10047L :
        return 102;
        case 10037L :
        return 103;
        case 10009L :
        return 9;
        case 10053L :
        return 106;
        case 10061L :
        return 107;
        case 10054L :
        return 108;
        case 10039L :
        return 109;
        case 10014L :
        return 14;
        case 10065L :
        return 110;
        case 10036L :
        return 112;
        case 10004L :
        return 4;
        case 10022L :
        return 22;
        case 10056L :
        return 113;
        case 10024L :
        return 24;
        case 10040L :
        return 115;
        case 10050L :
        return 116;
        case 10052L :
        return 117;
        case 10051L :
        return 118;
        case 10055L :
        return 119;
        case 10042L :
        return 123;
        case 10057L :
        return 126;
        case 10038L :
        return 128;
        case 10045L :
        return 130;
        case 10043L :
        return 135;
        case 10041L :
        return 136;
        case 10060L :
        return 138;
        case 10035L :
        return 140;
        default:
        break;
    }
    return 1;
}

