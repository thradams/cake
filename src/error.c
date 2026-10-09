/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

//#pragma safety enable

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include "error.h"
#include <errno.h>

#ifdef _WIN32
#include <winerror.h>
//#include <winsock2.h>
#endif

void bug()
{
    
}

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
#ifdef EPERM
    case EPERM:
        return "Operation not permitted";
#endif
#ifdef ENOENT
    case ENOENT:
        return "No such file or directory";
#endif
#ifdef ESRCH
    case ESRCH:
        return "No such process";
#endif
#ifdef EINTR
    case EINTR:
        return "Interrupted system call";
#endif
#ifdef EIO
    case EIO:
        return "I/O error";
#endif
#ifdef ENXIO
    case ENXIO:
        return "No such device or address";
#endif
#ifdef E2BIG
    case E2BIG:
        return "Arg list too long";
#endif
#ifdef ENOEXEC
    case ENOEXEC:
        return "Exec format error";
#endif
#ifdef EBADF
    case EBADF:
        return "Bad file number";
#endif
#ifdef ECHILD
    case ECHILD:
        return "No child processes";
#endif
#ifdef EAGAIN
    case EAGAIN:
        return "Try again";
#endif
#ifdef ENOMEM
    case ENOMEM:
        return "Out of memory";
#endif
#ifdef EACCES
    case EACCES:
        return "Permission denied";
#endif
#ifdef EFAULT
    case EFAULT:
        return "Bad address";
#endif
#ifdef EBUSY
    case EBUSY:
        return "Device or resource busy";
#endif
#ifdef EEXIST
    case EEXIST:
        return "File exists";
#endif
#ifdef EXDEV
    case EXDEV:
        return "Cross-device link";
#endif
#ifdef ENODEV
    case ENODEV:
        return "No such device";
#endif
#ifdef ENOTDIR
    case ENOTDIR:
        return "Not a directory";
#endif
#ifdef EISDIR
    case EISDIR:
        return "Is a directory";
#endif
#ifdef EINVAL
    case EINVAL:
        return "Invalid argument";
#endif
#ifdef ENFILE
    case ENFILE:
        return "File table overflow";
#endif
#ifdef EMFILE
    case EMFILE:
        return "Too many open files";
#endif
#ifdef ENOTTY
    case ENOTTY:
        return "Not a typewriter";
#endif
#ifdef ETXTBSY
    case ETXTBSY:
        return "Text file busy";
#endif
#ifdef EFBIG
    case EFBIG:
        return "File too large";
#endif
#ifdef ENOSPC
    case ENOSPC:
        return "No space left on device";
#endif
#ifdef ESPIPE
    case ESPIPE:
        return "Illegal seek";
#endif
#ifdef EROFS
    case EROFS:
        return "Read-only file system";
#endif
#ifdef EMLINK
    case EMLINK:
        return "Too many links";
#endif
#ifdef EPIPE
    case EPIPE:
        return "Broken pipe";
#endif
#ifdef EDOM
    case EDOM:
        return "Math argument out of domain of func";
#endif
#ifdef ERANGE
    case ERANGE:
        return "Math result not representable";
#endif
#ifdef EDEADLK
    case EDEADLK:
        return "Resource deadlock would occur";
#endif
#ifdef ENAMETOOLONG
    case ENAMETOOLONG:
        return "File name too long";
#endif
#ifdef ENOLCK
    case ENOLCK:
        return "No record locks available";
#endif
#ifdef ENOSYS
    case ENOSYS:
        return "Function not implemented";
#endif
#ifdef ENOTEMPTY
    case ENOTEMPTY:
        return "Directory not empty";
#endif
#ifdef ELOOP
    case ELOOP:
        return "Too many symbolic links encountered";
#endif
#ifdef ENOMSG
    case ENOMSG:
        return "No message of desired type";
#endif
#ifdef EIDRM
    case EIDRM:
        return "Identifier removed";
#endif
#ifdef ENOSTR
    case ENOSTR:
        return "Device not a stream";
#endif
#ifdef ENODATA
    case ENODATA:
        return "No data available";
#endif
#ifdef ETIME
    case ETIME:
        return "Timer expired";
#endif
#ifdef ENOSR
    case ENOSR:
        return "Out of streams resources";
#endif
#ifdef ENOLINK
    case ENOLINK:
        return "Link has been severed";
#endif
#ifdef EPROTO
    case EPROTO:
        return "Protocol error";
#endif
#ifdef EBADMSG
    case EBADMSG:
        return "Not a data message";
#endif
#ifdef EOVERFLOW
    case EOVERFLOW:
        return "Value too large for defined data type";
#endif
#ifdef EILSEQ
    case EILSEQ:
        return "Illegal byte sequence";
#endif
#ifdef ENOTSOCK
    case ENOTSOCK:
        return "Socket operation on non-socket";
#endif
#ifdef EDESTADDRREQ
    case EDESTADDRREQ:
        return "Destination address required";
#endif
#ifdef EMSGSIZE
    case EMSGSIZE:
        return "Message too long";
#endif
#ifdef EPROTOTYPE
    case EPROTOTYPE:
        return "Protocol wrong type for socket";
#endif
#ifdef ENOPROTOOPT
    case ENOPROTOOPT:
        return "Protocol not available";
#endif
#ifdef EPROTONOSUPPORT
    case EPROTONOSUPPORT:
        return "Protocol not supported";
#endif
#ifdef EOPNOTSUPP
    case EOPNOTSUPP:
        return "Operation not supported on transport endpoint";
#endif
#ifdef EAFNOSUPPORT
    case EAFNOSUPPORT:
        return "Address family not supported by protocol";
#endif
#ifdef EADDRINUSE
    case EADDRINUSE:
        return "Address already in use";
#endif
#ifdef EADDRNOTAVAIL
    case EADDRNOTAVAIL:
        return "Cannot assign requested address";
#endif
#ifdef ENETDOWN
    case ENETDOWN:
        return "Network is down";
#endif
#ifdef ENETUNREACH
    case ENETUNREACH:
        return "Network is unreachable";
#endif
#ifdef ENETRESET
    case ENETRESET:
        return "Network dropped connection because of reset";
#endif
#ifdef ECONNABORTED
    case ECONNABORTED:
        return "Software caused connection abort";
#endif
#ifdef ECONNRESET
    case ECONNRESET:
        return "Connection reset by peer";
#endif
#ifdef ENOBUFS
    case ENOBUFS:
        return "No buffer space available";
#endif
#ifdef EISCONN
    case EISCONN:
        return "Transport endpoint is already connected";
#endif
#ifdef ENOTCONN
    case ENOTCONN:
        return "Transport endpoint is not connected";
#endif
#ifdef ETIMEDOUT
    case ETIMEDOUT:
        return "Connection timed out";
#endif
#ifdef ECONNREFUSED
    case ECONNREFUSED:
        return "Connection refused";
#endif
#ifdef EHOSTUNREACH
    case EHOSTUNREACH:
        return "No route to host";
#endif
#ifdef EALREADY
    case EALREADY:
        return "Operation already in progress";
#endif
#ifdef EINPROGRESS
    case EINPROGRESS:
        return "Operation now in progress";
#endif

        /* Linux-specific errors */
#ifndef _WIN32

#ifdef ENOTBLK
    case ENOTBLK:
        return "Block device required";
#endif
#ifdef ECHRNG
    case ECHRNG:
        return "Channel number out of range";
#endif
#ifdef EL2NSYNC
    case EL2NSYNC:
        return "Level 2 not synchronized";
#endif
#ifdef EL3HLT
    case EL3HLT:
        return "Level 3 halted";
#endif
#ifdef EL3RST
    case EL3RST:
        return "Level 3 reset";
#endif
#ifdef ELNRNG
    case ELNRNG:
        return "Link number out of range";
#endif
#ifdef EUNATCH
    case EUNATCH:
        return "Protocol driver not attached";
#endif
#ifdef ENOCSI
    case ENOCSI:
        return "No CSI structure available";
#endif
#ifdef EL2HLT
    case EL2HLT:
        return "Level 2 halted";
#endif
#ifdef EBADE
    case EBADE:
        return "Invalid exchange";
#endif
#ifdef EBADR
    case EBADR:
        return "Invalid request descriptor";
#endif
#ifdef EXFULL
    case EXFULL:
        return "Exchange full";
#endif
#ifdef ENOANO
    case ENOANO:
        return "No anode";
#endif
#ifdef EBADRQC
    case EBADRQC:
        return "Invalid request code";
#endif
#ifdef EBADSLT
    case EBADSLT:
        return "Invalid slot";
#endif
#ifdef EBFONT
    case EBFONT:
        return "Bad font file format";
#endif
#ifdef ENONET
    case ENONET:
        return "Machine is not on the network";
#endif
#ifdef ENOPKG
    case ENOPKG:
        return "Package not installed";
#endif
#ifdef EREMOTE
    case EREMOTE:
        return "Object is remote";
#endif
#ifdef EMULTIHOP
    case EMULTIHOP:
        return "Multihop attempted";
#endif
#ifdef EDOTDOT
    case EDOTDOT:
        return "RFS specific error";
#endif
#ifdef EADV
    case EADV:
        return "Advertise error";
#endif
#ifdef ESRMNT
    case ESRMNT:
        return "Srmount error";
#endif
#ifdef ECOMM
    case ECOMM:
        return "Communication error on send";
#endif
#ifdef ERESTART
    case ERESTART:
        return "Interrupted system call should be restarted";
#endif
#ifdef ESTRPIPE
    case ESTRPIPE:
        return "Streams pipe error";
#endif
#ifdef EUSERS
    case EUSERS:
        return "Too many users";
#endif
#ifdef ENOTUNIQ
    case ENOTUNIQ:
        return "Email not unique on network";
#endif
#ifdef EBADFD
    case EBADFD:
        return "File descriptor in bad state";
#endif
#ifdef EREMCHG
    case EREMCHG:
        return "Remote address changed";
#endif
#ifdef ELIBACC
    case ELIBACC:
        return "Cannot access a needed shared library";
#endif
#ifdef ELIBBAD
    case ELIBBAD:
        return "Accessing a corrupted shared library";
#endif
#ifdef ELIBSCN
    case ELIBSCN:
        return ".lib section in a.out corrupted";
#endif
#ifdef ELIBMAX
    case ELIBMAX:
        return "Attempting to link in too many shared libraries";
#endif
#ifdef ELIBEXEC
    case ELIBEXEC:
        return "Cannot exec a shared library directly";
#endif
#ifdef EUCLEAN
    case EUCLEAN:
        return "Structure needs cleaning";
#endif
#ifdef ENOTNAM
    case ENOTNAM:
        return "Not a XENIX named type file";
#endif
#ifdef ENAVAIL
    case ENAVAIL:
        return "No XENIX semaphores available";
#endif
#ifdef EISNAM
    case EISNAM:
        return "Is a named type file";
#endif
#ifdef EREMOTEIO
    case EREMOTEIO:
        return "Remote I/O error";
#endif
#ifdef EDQUOT
    case EDQUOT:
        return "Quota exceeded";
#endif
#ifdef ENOMEDIUM
    case ENOMEDIUM:
        return "No medium found";
#endif
#ifdef EMEDIUMTYPE
    case EMEDIUMTYPE:
        return "Wrong medium type";
#endif

#endif /* _WIN32 */

#ifdef ESOCKTNOSUPPORT
    case ESOCKTNOSUPPORT:
        return "Socket type not supported";
#endif
#ifdef EPFNOSUPPORT
    case EPFNOSUPPORT:
        return "Protocol family not supported";
#endif
#ifdef EHOSTDOWN
    case EHOSTDOWN:
        return "Host is down";
#endif
#ifdef ESHUTDOWN
    case ESHUTDOWN:
        return "Cannot send after transport endpoint shutdown";
#endif
#ifdef ETOOMANYREFS
    case ETOOMANYREFS:
        return "Too many references: cannot splice";
#endif
#ifdef ESTALE
    case ESTALE:
        return "Stale NFS file handle";
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
#ifdef EACCES
    case ERROR_ACCESS_DENIED:
        return EACCES;
#endif

#ifdef EEXIST
    case ERROR_ALREADY_EXISTS:
        return EEXIST;
#endif

#ifdef ENODEV
    case ERROR_BAD_UNIT:
        return ENODEV;
#endif

#ifdef ENAMETOOLONG
    case ERROR_BUFFER_OVERFLOW:
        return ENAMETOOLONG;
#endif

#ifdef EBUSY
    case ERROR_BUSY:
        return EBUSY;

    case ERROR_BUSY_DRIVE:
        return EBUSY;
#endif

#ifdef EACCES
    case ERROR_CANNOT_MAKE:
        return EACCES;

    case ERROR_CURRENT_DIRECTORY:
        return EACCES;

    case ERROR_INVALID_ACCESS:
        return EACCES;

    case ERROR_NOACCESS:
        return EACCES;

    case ERROR_SHARING_VIOLATION:
        return EACCES;

    case ERROR_WRITE_PROTECT:
        return EACCES;
#endif

#ifdef EIO
    case ERROR_CANTOPEN:
        return EIO;

    case ERROR_CANTREAD:
        return EIO;

    case ERROR_CANTWRITE:
        return EIO;

    case ERROR_OPEN_FAILED:
        return EIO;

    case ERROR_READ_FAULT:
        return EIO;

    case ERROR_SEEK:
        return EIO;

    case ERROR_WRITE_FAULT:
        return EIO;
#endif

#ifdef ENOTEMPTY
    case ERROR_DIR_NOT_EMPTY:
        return ENOTEMPTY;
#endif

#ifdef EINVAL
    case ERROR_DIRECTORY:
        return EINVAL;

    case ERROR_INVALID_HANDLE:
        return EINVAL;

    case ERROR_INVALID_NAME:
        return EINVAL;

    case ERROR_NEGATIVE_SEEK:
        return EINVAL;
#endif

#ifdef ENOSPC
    case ERROR_DISK_FULL:
        return ENOSPC;

    case ERROR_HANDLE_DISK_FULL:
        return ENOSPC;
#endif

#ifdef ENOENT
    case ERROR_FILE_NOT_FOUND:
        return ENOENT;

    case ERROR_PATH_NOT_FOUND:
        return ENOENT;
#endif

#ifdef ENOSYS
    case ERROR_INVALID_FUNCTION:
        return ENOSYS;
#endif

#ifdef ENOLCK
    case ERROR_LOCK_VIOLATION:
        return ENOLCK;

    case ERROR_LOCKED:
        return ENOLCK;
#endif

#ifdef ENOMEM
    case ERROR_NOT_ENOUGH_MEMORY:
        return ENOMEM;

    case ERROR_OUTOFMEMORY:
        return ENOMEM;
#endif

#ifdef EAGAIN
    case ERROR_NOT_READY:
        return EAGAIN;

    case ERROR_RETRY:
        return EAGAIN;
#endif

#ifdef EXDEV
    case ERROR_NOT_SAME_DEVICE:
        return EXDEV;
#endif

#ifdef EBUSY
    case ERROR_OPEN_FILES:
        return EBUSY;
#endif

#ifdef ECANCELED
    case ERROR_OPERATION_ABORTED:
        return ECANCELED;
#endif

#ifdef EMFILE
    case ERROR_TOO_MANY_OPEN_FILES:
        return EMFILE;
#endif

#ifdef EADDRINUSE
    case WSAEADDRINUSE:
        return EADDRINUSE;
#endif

#ifdef EADDRNOTAVAIL
    case WSAEADDRNOTAVAIL:
        return EADDRNOTAVAIL;
#endif

#ifdef EAFNOSUPPORT
    case WSAEAFNOSUPPORT:
        return EAFNOSUPPORT;
#endif

#ifdef EALREADY
    case WSAEALREADY:
        return EALREADY;
#endif

#ifdef EBADF
    case WSAEBADF:
        return EBADF;
#endif

#ifdef ECONNABORTED
    case WSAECONNABORTED:
        return ECONNABORTED;
#endif

#ifdef ECONNREFUSED
    case WSAECONNREFUSED:
        return ECONNREFUSED;
#endif

#ifdef ECONNRESET
    case WSAECONNRESET:
        return ECONNRESET;
#endif

#ifdef EDESTADDRREQ
    case WSAEDESTADDRREQ:
        return EDESTADDRREQ;
#endif

#ifdef EFAULT
    case WSAEFAULT:
        return EFAULT;
#endif

#ifdef EHOSTUNREACH
    case WSAEHOSTUNREACH:
        return EHOSTUNREACH;
#endif

#ifdef EINPROGRESS
    case WSAEINPROGRESS:
        return EINPROGRESS;
#endif

#ifdef EINTR
    case WSAEINTR:
        return EINTR;
#endif

#ifdef EISCONN
    case WSAEISCONN:
        return EISCONN;
#endif

#ifdef EMSGSIZE
    case WSAEMSGSIZE:
        return EMSGSIZE;
#endif

#ifdef ENETDOWN
    case WSAENETDOWN:
        return ENETDOWN;
#endif

#ifdef ENETRESET
    case WSAENETRESET:
        return ENETRESET;
#endif

#ifdef ENETUNREACH
    case WSAENETUNREACH:
        return ENETUNREACH;
#endif

#ifdef ENOBUFS
    case WSAENOBUFS:
        return ENOBUFS;
#endif

#ifdef ENOPROTOOPT
    case WSAENOPROTOOPT:
        return ENOPROTOOPT;
#endif

#ifdef ENOTCONN
    case WSAENOTCONN:
        return ENOTCONN;
#endif

#ifdef ENOTSOCK
    case WSAENOTSOCK:
        return ENOTSOCK;
#endif

#ifdef EOPNOTSUPP
    case WSAEOPNOTSUPP:
        return EOPNOTSUPP;
#endif

#ifdef EPROTONOSUPPORT
    case WSAEPROTONOSUPPORT:
        return EPROTONOSUPPORT;
#endif

#ifdef EPROTOTYPE
    case WSAEPROTOTYPE:
        return EPROTOTYPE;
#endif

#ifdef ETIMEDOUT
    case WSAETIMEDOUT:
        return ETIMEDOUT;
#endif

#ifdef EWOULDBLOCK
    case WSAEWOULDBLOCK:
        return EWOULDBLOCK;
#endif
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
