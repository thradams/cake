/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/


#pragma safety enable

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include "error.h"
#include <errno.h>

#ifdef _WIN32
#include <winerror.h>
//#include <winsock2.h>
#endif

void throw_break_point()
{
    /*
      put a break point here to stop when throw is called
    */
}

const char* get_posix_error_message(int error)
{
    switch (error)
    {
    case EPERM:
        return "Operation not permitted";
    case  ENOENT:
        return "No such file or directory";
    case  ESRCH:
        return "No such process";
    case  EINTR:
        return "Interrupted system call";
    case  EIO:
        return "I/O error";
    case  ENXIO:
        return "No such device or address";
    case  E2BIG:
        return "Arg list too long";
    case  ENOEXEC:
        return "Exec string_format error";
    case  EBADF:
        return "Bad file number";
    case  ECHILD:
        return "No child processes";
    case  EAGAIN:
        return "Try again";
    case  ENOMEM:
        return "Out of memory";
    case  EACCES:
        return "Permission denied";
    case  EFAULT:
        return "Bad address";
    case  EBUSY:
        return "Device or resource busy";
    case  EEXIST:
        return "File exists";
    case  EXDEV:
        return "Cross-device link";
    case  ENODEV:
        return "No such device";
    case  ENOTDIR:
        return "Not a directory";
    case  EISDIR:
        return "Is a directory";
    case  EINVAL:
        return "Invalid argument";
    case  ENFILE:
        return "File table overflow";
    case  EMFILE:
        return "Too many open files";
    case  ENOTTY:
        return "Not a typewriter";
    case  ETXTBSY:
        return "Text file busy";
    case  EFBIG:
        return "File too large";
    case  ENOSPC:
        return "No space left on device";
    case  ESPIPE:
        return "Illegal seek";
    case  EROFS:
        return "Read-only file system";
    case  EMLINK:
        return "Too many links";
    case  EPIPE:
        return "Broken pipe";
    case  EDOM:
        return "Math argument out of domain of func";
    case  ERANGE:
        return "Math result not representable";
    case  EDEADLK:
        return "Resource deadlock would occur";
    case  ENAMETOOLONG:
        return "File name too long";
    case  ENOLCK:
        return "No record locks available";
    case  ENOSYS:
        return "Function not implemented";
    case  ENOTEMPTY:
        return "Directory not empty";
    case  ELOOP:
        return "Too many symbolic links encountered";
        //case  EWOULDBLOCK:
        //case EAGAIN:  return "Operation would block";
    case  ENOMSG:
        return "No message of desired type";
    case  EIDRM:
        return "Identifier removed";
    case  ENOSTR:
        return "Device not a stream";
    case  ENODATA:
        return "No data available";
    case  ETIME:
        return "Timer expired";
    case  ENOSR:
        return "Out of streams resources";

    case  ENOLINK:
        return "Link has been severed";
    case  EPROTO:
        return "Protocol error";
    case  EBADMSG:
        return "Not a data message";
    case  EOVERFLOW:
        return "Value too large for defined data type";
    case  EILSEQ:
        return "Illegal byte sequence";
    case  ENOTSOCK:
        return "Socket operation on non-socket";
    case  EDESTADDRREQ:
        return "Destination address required";
    case  EMSGSIZE:
        return "Message too long";
    case  EPROTOTYPE:
        return "Protocol wrong type for socket";
    case  ENOPROTOOPT:
        return "Protocol not available";
    case  EPROTONOSUPPORT:
        return "Protocol not supported";

    case  EOPNOTSUPP:
        return "Operation not supported on transport endpoint";

    case  EAFNOSUPPORT:
        return "Address family not supported by protocol";
    case  EADDRINUSE:
        return "Address already in use";
    case  EADDRNOTAVAIL:
        return "Cannot assign requested address";
    case  ENETDOWN:
        return "Network is down";
    case  ENETUNREACH:
        return "Network is unreachable";
    case  ENETRESET:
        return "Network dropped connection because of reset";
    case  ECONNABORTED:
        return "Software caused connection abort";
    case  ECONNRESET:
        return "Connection reset by peer";
    case  ENOBUFS:
        return "No buffer space available";
    case  EISCONN:
        return "Transport endpoint is already connected";
    case  ENOTCONN:
        return "Transport endpoint is not connected";

    case  ETIMEDOUT:
        return "Connection timed out";
    case  ECONNREFUSED:
        return "Connection refused";

    case  EHOSTUNREACH:
        return "No route to host";
    case  EALREADY:
        return "Operation already in progress";
    case  EINPROGRESS:
        return "Operation now in progress";
#ifndef _WIN32
    case  ENOTBLK:
        return "Block device required";
    case  ECHRNG:
        return "Channel number out of range";
    case  EL2NSYNC:
        return "Level 2 not synchronized";
    case  EL3HLT:
        return "Level 3 halted";
    case  EL3RST:
        return "Level 3 reset";
    case  ELNRNG:
        return "Link number out of range";
    case  EUNATCH:
        return "Protocol driver not attached";
    case  ENOCSI:
        return "No CSI structure available";
    case  EL2HLT:
        return "Level 2 halted";
    case  EBADE:
        return "Invalid ex   ";
    case  EBADR:
        return "Invalid request descriptor";
    case  EXFULL:
        return "Exchange full";
    case  ENOANO:
        return "No anode";
    case  EBADRQC:
        return "Invalid request code";
    case  EBADSLT:
        return "Invalid slot";

        //case  EDEADLOCK:
        //case EDEADLK:

    case  EBFONT:
        return "Bad font file string_format";
    case  ENONET:
        return "Machine is not on the network";
    case  ENOPKG:
        return "Package not installed";
    case  EREMOTE:
        return "Object is remote";

    case  EMULTIHOP:
        return "Multihop attempted";
    case  EDOTDOT:
        return "RFS specific error";
    case  EADV:
        return "Advertise error";
    case  ESRMNT:
        return "Srmount error";
    case  ECOMM:
        return "Communication error on send";
    case  ERESTART:
        return "Interrupted system call should be restarted";
    case  ESTRPIPE:
        return "Streams pipe error";
    case  EUSERS:
        return "Too many users";
    case  ENOTUNIQ:
        return "Email not unique on network";
    case  EBADFD:
        return "File descriptor in bad state";
    case  EREMCHG:
        return "Remote address changed";
    case  ELIBACC:
        return "Can not access a needed shared library";
    case  ELIBBAD:
        return "Accessing a corrupted shared library";
    case  ELIBSCN:
        return ".lib section in a.out corrupted";
    case  ELIBMAX:
        return "Attempting to link in too many shared libraries";
    case  ELIBEXEC:
        return "Cannot exec a shared library directly";
    case  ESOCKTNOSUPPORT:
        return "Socket type not supported";
    case  EPFNOSUPPORT:
        return "Protocol family not supported";
    case  EHOSTDOWN:
        return "Host is down";
    case  ESHUTDOWN:
        return "Cannot send after transport endpoint shutdown";
    case  ETOOMANYREFS:
        return "Too many references: cannot splice";
    case  ESTALE:
        return "Stale NFS file handle";
    case  EUCLEAN:
        return "Structure needs cleaning";
    case  ENOTNAM:
        return "Not a XENIX named type file";
    case  ENAVAIL:
        return "No XENIX semaphores available";
    case  EISNAM:
        return "Is a named type file";
    case  EREMOTEIO:
        return "Remote I/O error";
    case  EDQUOT:
        return "Quota exceeded";
    case  ENOMEDIUM:
        return "No medium found";
    case  EMEDIUMTYPE:
        return "Wrong medium type";
#endif
    default:
        break;

    }

    return "Unknown";
}
#ifdef _WIN32


int windows_error_to_posix(int i)
{
    switch (i)
    {
    case ERROR_ACCESS_DENIED:
        return EACCES;
    case ERROR_ALREADY_EXISTS:
        return EEXIST;
    case ERROR_BAD_UNIT:
        return ENODEV;
    case ERROR_BUFFER_OVERFLOW:
        return ENAMETOOLONG;
    case ERROR_BUSY:
        return EBUSY;
    case ERROR_BUSY_DRIVE:
        return EBUSY;
    case ERROR_CANNOT_MAKE:
        return EACCES;
    case ERROR_CANTOPEN:
        return EIO;
    case ERROR_CANTREAD:
        return EIO;
    case ERROR_CANTWRITE:
        return EIO;
    case ERROR_CURRENT_DIRECTORY:
        return EACCES;
    case ERROR_DEV_NOT_EXIST:
        return ENODEV;
    case ERROR_DEVICE_IN_USE:
        return EBUSY;
    case ERROR_DIR_NOT_EMPTY:
        return ENOTEMPTY;
    case ERROR_DIRECTORY:
        return EINVAL;
    case ERROR_DISK_FULL:
        return ENOSPC;
    case ERROR_FILE_EXISTS:
        return EEXIST;
    case ERROR_FILE_NOT_FOUND:
        return ENOENT;
    case ERROR_HANDLE_DISK_FULL:
        return ENOSPC;
    case ERROR_INVALID_ACCESS:
        return EACCES;
    case ERROR_INVALID_DRIVE:
        return ENODEV;
    case ERROR_INVALID_FUNCTION:
        return ENOSYS;
    case ERROR_INVALID_HANDLE:
        return EINVAL;
    case ERROR_INVALID_NAME:
        return EINVAL;
    case ERROR_LOCK_VIOLATION:
        return ENOLCK;
    case ERROR_LOCKED:
        return ENOLCK;
    case ERROR_NEGATIVE_SEEK:
        return EINVAL;
    case ERROR_NOACCESS:
        return EACCES;
    case ERROR_NOT_ENOUGH_MEMORY:
        return ENOMEM;
    case ERROR_NOT_READY:
        return EAGAIN;
    case ERROR_NOT_SAME_DEVICE:
        return EXDEV;
    case ERROR_OPEN_FAILED:
        return EIO;
    case ERROR_OPEN_FILES:
        return EBUSY;
    case ERROR_OPERATION_ABORTED:
        return ECANCELED;
    case ERROR_OUTOFMEMORY:
        return ENOMEM;
    case ERROR_PATH_NOT_FOUND:
        return ENOENT;
    case ERROR_READ_FAULT:
        return EIO;
    case ERROR_RETRY:
        return EAGAIN;
    case ERROR_SEEK:
        return EIO;
    case ERROR_SHARING_VIOLATION:
        return EACCES;
    case ERROR_TOO_MANY_OPEN_FILES:
        return EMFILE;
    case ERROR_WRITE_FAULT:
        return EIO;
    case ERROR_WRITE_PROTECT:
        return EACCES;
    case WSAEACCES:
        return EACCES;
    case WSAEADDRINUSE:
        return EADDRINUSE;
    case WSAEADDRNOTAVAIL:
        return EADDRNOTAVAIL;
    case WSAEAFNOSUPPORT:
        return EAFNOSUPPORT;
    case WSAEALREADY:
        return EALREADY;
    case WSAEBADF:
        return EBADF;
    case WSAECONNABORTED:
        return ECONNABORTED;
    case WSAECONNREFUSED:
        return ECONNREFUSED;
    case WSAECONNRESET:
        return ECONNRESET;
    case WSAEDESTADDRREQ:
        return EDESTADDRREQ;
    case WSAEFAULT:
        return EFAULT;
    case WSAEHOSTUNREACH:
        return EHOSTUNREACH;
    case WSAEINPROGRESS:
        return EINPROGRESS;
    case WSAEINTR:
        return EINTR;
    case WSAEINVAL:
        return EINVAL;
    case WSAEISCONN:
        return EISCONN;
    case WSAEMFILE:
        return EMFILE;
    case WSAEMSGSIZE:
        return EMSGSIZE;
    case WSAENETDOWN:
        return ENETDOWN;
    case WSAENETRESET:
        return ENETRESET;
    case WSAENETUNREACH:
        return ENETUNREACH;
    case WSAENOBUFS:
        return ENOBUFS;
    case WSAENOPROTOOPT:
        return ENOPROTOOPT;
    case WSAENOTCONN:
        return ENOTCONN;
    case WSAENOTSOCK:
        return ENOTSOCK;
    case WSAEOPNOTSUPP:
        return EOPNOTSUPP;
    case WSAEPROTONOSUPPORT:
        return EPROTONOSUPPORT;
    case WSAEPROTOTYPE:
        return EPROTOTYPE;
    case WSAETIMEDOUT:
        return ETIMEDOUT;
    case WSAEWOULDBLOCK:
        return EWOULDBLOCK;
    default:
        break;
    }
    return EPERM;
}
#endif
/*
int GetWindowsOrLinuxSocketLastErrorAsPosix(void)
{
#ifdef _WIN32
    return windows_error_to_posix(WSAGetLastError());
#else
    return errno;
#endif
}
*/

