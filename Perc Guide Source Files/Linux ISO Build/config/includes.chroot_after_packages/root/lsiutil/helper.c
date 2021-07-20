/***************************************************************************
 *                                                                         *
 *  Copyright 2012 LSI Corporation.  All rights reserved.                  *
 *                                                                         *
 *  This file is confidential and a trade secret of LSI Corporation.  The  *
 *  receipt of or possession of this file does not convey any rights to    *
 *  reproduce or disclose its contents or to manufacture, use, or sell     *
 *  anything it may describe, in whole, or in part, without the specific   *
 *  written consent of LSI Corporation.                                    *
 *                                                                         *
 ***************************************************************************
*/
/*
 * helper -- EFI helper routines commonly found in a CRTL
 *
 * Written by Stephen F. Shirron, September 15, 2006
 */

EFI_STATUS last_efi_status;
int errno;
int optind;
char *optarg;

EFI_HANDLE gImageHandle;

EFI_LOADED_IMAGE *gLoadedImage;

EFI_EVENT gSleepEvent;

size_t strlen(const char *s)
{
    const char *c = s;

    while (*c++);

    return c - s - 1;
}

char *strcpy(char *s1, const char *s2)
{
    char *c1 = s1;
    const char *c2 = s2;

    while (*c1++ = *c2++);

    return s1;
}

char *strncpy(char *s1, const char *s2, size_t n)
{
    unsigned char *c1 = s1;
    const char *c2 = s2;
    size_t t = 0;

    while (t++ < n && (*c1++ = *c2++));
    while (t++ < n) *c1++ = 0;

    return s1;
}

char *strcat(char *s1, const char *s2)
{
    char *c1 = s1;
    const char *c2 = s2;

    while (*c1) c1++;
    while (*c1++ = *c2++);

    return s1;
}

char *strncat(char *s1, const char *s2, size_t n)
{
    char *c1 = s1;
    const char *c2 = s2;
    size_t t;

    while (*c1) c1++;
    t = 0;
    while (t++ < n && (*c1++ = *c2++));
    if (t >= n && *(c1-1)) *c1 = 0;

    return s1;
}

char *strchr(const char *s1, int t)
{
    while (*s1) if (*s1++ == t) return (char *)--s1;
    return NULL;
}

int strcmp(const char *s1, const char *s2)
{
    while (*s1 && *s1 == *s2) s1++, s2++;
    if (*s1 < *s2) return -1;
    if (*s1 > *s2) return 1;
    return 0;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
    while (n && *s1 && *s1 == *s2) n--, s1++, s2++;
    if (n == 0) return 0;
    if (*s1 < *s2) return -1;
    if (*s1 > *s2) return 1;
    return 0;
}

int toupper(int c)
{
    if (c >= 'a' && c <= 'z') return c - 'a' + 'A';
    return c;
}

int tolower(int c)
{
    if (c >= 'A' && c <= 'Z') return c - 'A' + 'a';
    return c;
}

int strcasecmp(const char *s1, const char *s2)
{
    while (*s1 && tolower(*s1) == tolower(*s2)) s1++, s2++;
    if (tolower(*s1) < tolower(*s2)) return -1;
    if (tolower(*s1) > tolower(*s2)) return 1;
    return 0;
}

int strncasecmp(const char *s1, const char *s2, size_t n)
{
    while (n && *s1 && tolower(*s1) == tolower(*s2)) n--, s1++, s2++;
    if (n == 0) return 0;
    if (tolower(*s1) < tolower(*s2)) return -1;
    if (tolower(*s1) > tolower(*s2)) return 1;
    return 0;
}

void *memset(void *s, int t, size_t n)
{
    char *c = s;

    while (n) n--, *c++ = t;
    return s;
}

void *memcpy(void *s2, const void *s1, size_t n)
{
    const char *c1 = s1;
    char *c2 = s2;

    while (n) n--, *c2++ = *c1++;
    return s2;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
    const char *c1 = s1;
    const char *c2 = s2;

    while (n && *c1 == *c2) n--, c1++, c2++;
    if (n == 0) return 0;
    if (*c1 < *c2) return -1;
    if (*c1 > *c2) return 1;
    return 0;
}

void bzero(void *s, size_t n)
{
    char *c = s;

    while (n)
    {
        *c++ = 0;
        n--;
    }
}

void bcopy(const void *s1, void *s2, size_t n)
{
    const char *c1 = s1;
    char *c2 = s2;

    while (n)
    {
        *c2++ = *c1++;
        n--;
    }
}

int isdigit(int c)
{
    if (c >= '0' && c <= '9') return 1;
    return 0;
}

int isxdigit(int c)
{
    if (c >= '0' && c <= '9') return 1;
    if (c >= 'a' && c <= 'f') return 1;
    if (c >= 'A' && c <= 'F') return 1;
    return 0;
}

int isalpha(int c)
{
    if (c >= 'a' && c <= 'z') return 1;
    if (c >= 'A' && c <= 'Z') return 1;
    return 0;
}

int isalnum(int c)
{
    if (c >= '0' && c <= '9') return 1;
    if (c >= 'a' && c <= 'z') return 1;
    if (c >= 'A' && c <= 'Z') return 1;
    return 0;
}

int isprint(int c)
{
    if (c >= ' ' && c <= '~') return 1;
    return 0;
}

void *malloc(size_t size)
{
    return AllocatePool(size);
}

void *realloc(void *oldp, size_t size)
{
    void *newp = malloc(size);

    memcpy(newp, oldp, size);

    free(oldp);

    return newp;
}

void free(void *p)
{
    FreePool(p);
}

static void convert_unicode_ascii(const CHAR16 *s1, char *s2, int n)
{
    int i;
    char c;

    for (i = 0; i < n; i++)
    {
        c = (char)*s1++;
        *s2++ = c;

        if (c == 0)
            break;
    }
}

static int convert_dtoi(const char *s, int *r, int w)
{
    int n = 0;
    char c;

    *r = 0;
    while (isdigit(c = *s++))
    {
        *r *= 10;
        *r += c - '0';
        n++;
        if (n == w)
            return n;
    }

    return n;
}

static int convert_xtoi(const char *s, int *r, int w)
{
    int n = 0;
    char c;

    *r = 0;
    while (isxdigit(c = *s++))
    {
        *r *= 16;
        if (isdigit(c))
            *r += c - '0';
        else
            *r += tolower(c) - 'a' + 10;
        n++;
        if (n == w)
            return n;
    }

    return n;
}

static int convert_itod(char *s, int64_t r)
{
    char c;
    char buffer[32];
    int i;
    int n = 0;
    int negative = 0;

    if (r < 0)
    {
        r = -r;
        *s++ = '-';
        negative = 1;
    }

    do
    {
        c = (char)(r % 10);
        r /= 10;
        buffer[n] = c + '0';
        n++;
    }
    while (r);

    for (i = 0; i < n; i++)
        s[i] = buffer[n - i - 1];

    s[n] = 0;

    return n + negative;
}

static int convert_itox(char *s, uint64_t r)
{
    char c;
    char buffer[32];
    int i;
    int n = 0;
    uint64_t r0 = r;

    do
    {
        c = (char)(r % 16);
        r /= 16;
        if (c < 10)
            buffer[n] = c + '0';
        else
            buffer[n] = c + 'a' - 10;
        n++;
    }
    while (r);

    for (i = 0; i < n; i++)
        s[i] = buffer[n - i - 1];

    s[n] = 0;

    return n;
}

char *fgets(char *s, int size, FILE *stream)
{
    CHAR16 buffer[256];

    if (stream == stdin)
    {
        Input(NULL, buffer, sizeof buffer / 2);
        convert_unicode_ascii(buffer, s, size);
        APrint("\n");

        return s;
    }

    printf("fgets from a file not implemented yet!\n");

    return NULL;
}

int sscanf(const char *s, const char *format, ...)
{
    va_list args;
    char c;
    int n = 0;
    int w;
    char *as;
    int *ad;
    int *ax;
    int t;
    const char *b;

    va_start(args, format);

    b = s;
    while ((c = *format++))
    {
        while (*s == ' ')
            *s++;
        if (c == '%')
        {
            c = *format++;
            w = 0;
            if (isdigit(c))
            {
                format += convert_dtoi(--format, &w, 0);
                c = *format++;
            }
            if (!c)
                break;
            switch (c)
            {
            case 's':
                if (!*s)
                    break;
                as = va_arg(args, char *);
                while ((c = *s))
                {
                    s++;
                    if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
                        break;
                    *as++ = c;
                }
                *as = 0;
                n++;
                break;
            case 'c':
                if (!*s)
                    break;
                as = va_arg(args, char *);
                *as = *s++;
                n++;
                break;
            case 'd':
                if (!*s)
                    break;
                ad = va_arg(args, int *);
                t = convert_dtoi(s, ad, w);
                if (t)
                {
                    s += t;
                    n++;
                }
                break;
            case 'x':
                if (!*s)
                    break;
                ax = va_arg(args, int *);
                t = convert_xtoi(s, ax, w);
                if (t)
                {
                    s += t;
                    n++;
                }
                break;
            case 'n':
                ad = va_arg(args, int *);
                *ad = (int)(s - b);
                break;
            }
        }
        else if (c != ' ')
        {
            if (*s++ != c)
                break;
        }
    }

    va_end(args);

    return n;
}

int vsprintf(char *s1, const char *format, va_list args)
{
    char c;
    char *s;
    char s2[256];
    char b[32];
    int len;
    int n = 0;
    int t = 0;
    int i;
    int w;
    char ac;
    char *as;
    int64_t ad;
    int64_t ax;
#if !EFIEBC
    double af;
#endif
    int left;
    char pad;
    int minw;
    int maxw;
    int longint;

    if (!s1)
        s = s2;
    else
        s = s1;

    while ((c = *format++))
    {
        if (c == '%')
        {
            c = *format++;
            left = 0;
            pad = ' ';
            minw = 1;
            maxw = 100;
            longint = 0;
            if (c == '-')
            {
                left = 1;
                c = *format++;
            }
            if (c == '0')
            {
                pad = '0';
                c = *format++;
            }
            if (isdigit(c))
            {
                format += convert_dtoi(--format, &minw, 0);
                c = *format++;
            }
            if (c == '.')
            {
                c = *format++;
                if (isdigit(c))
                {
                    format += convert_dtoi(--format, &maxw, 0);
                    c = *format++;
                }
            }
            if (c == 'l')
            {
                c = *format++;
                if (c == 'l')
                    c = *format++;
                longint = 1;
            }
            if (!c)
                break;
            switch (c)
            {
            case 'c':
                ac = (char)va_arg(args, int);
                b[0] = ac;
                b[1] = 0;
                as = b;
                break;
            case 's':
                as = va_arg(args, char *);
                break;
            case 'd':
                if (longint)
                    ad = (int64_t)va_arg(args, int64_t);
                else
                    ad = (int)va_arg(args, int);
                convert_itod(b, ad);
                as = b;
                break;
            case 'x':
                if (longint)
                    ax = (uint64_t)va_arg(args, uint64_t);
                else
                    ax = (unsigned int)va_arg(args, unsigned int);
                convert_itox(b, ax);
                as = b;
                break;
            case 'X':
                if (longint)
                    ax = (uint64_t)va_arg(args, uint64_t);
                else
                    ax = (unsigned int)va_arg(args, unsigned int);
                convert_itox(b, ax);
                for (i = 0; i < (int)strlen(b); i++)
                    b[i] = toupper(b[i]);
                as = b;
                break;
            case 'p':
                ax = (uint64_t)va_arg(args, void *);
                convert_itox(b, ax);
                as = b;
                break;
#if !EFIEBC
            case 'f':
                af = (double)va_arg(args, double);
                for (i = 0, ax = 1; i < maxw; i++, ax *= 10);
                ad = (int64_t)(af * ax + 0.5);
                convert_itod(b, ad);
                if (maxw)
                {
                    len = (int)strlen(b);
                    if (len < maxw + 1)
                    {
                        for (i = maxw + 1; i > maxw - len; i--)
                            b[i] = b[i-maxw-1+len];
                        for (i = 0; i < maxw + 1 - len; i++)
                            b[i] = '0';
                        len = maxw + 1;
                    }
                    for (i = len + 1; i > len - maxw; i--)
                        b[i] = b[i-1];
                    b[i] = '.';
                }
                as = b;
                maxw = 100;
                break;
#endif
            default:
                s[n++] = c;
                as = NULL;
                minw = 0;
                break;
            }
            if (minw)
            {
                len = (int)strlen(as);
                if (as != b)
                {
                    if (len > maxw)
                        len = maxw;
                }
                w = minw - len;
                if (!left && w > 0)
                {
                    memset(s + n, pad, w);
                    n += w;
                }
                memcpy(s + n, as, len);
                n += len;
                if (left && w > 0)
                {
                    memset(s + n, pad, w);
                    n += w;
                }
            }
        }
        else
        {
            s[n++] = c;
        }
        if (c == '\n' && !s1)
        {
            s[n] = 0;
            t += n;

            if (n)
                APrint("%a", s);

            n = 0;
        }
    }

    s[n] = 0;
    t += n;

    if (n && !s1)
        APrint("%a", s);

    return n;
}

#if sfs
int printf(const char *format, ...)
{
    va_list args;
    int n;

    va_start(args, format);

    n = vsprintf(NULL, format, args);

    va_end(args);

    return n;
}
#endif

int sprintf(char *s, const char *format, ...)
{
    va_list args;
    int n;

    va_start(args, format);

    n = vsprintf(s, format, args);

    va_end(args);

    return n;
}

#define MAX_HANDLES 32

EFI_FILE_HANDLE gFileHandles[MAX_HANDLES];

FILE *fopen(const char *name, const char *mode)
{
    int flags;
    int fd;

    if (strchr(mode, 'r'))
        flags = O_RDONLY;
    else if (strchr(mode, 'w'))
        flags = O_RDWR | O_CREAT | O_TRUNC;
    else if (strchr(mode, 'a'))
        flags = O_RDWR | O_CREAT;

    fd = open(name, flags);
    if (fd < 0)
        return NULL;

    return (FILE *)fd;
}

int fclose(FILE *stream)
{
    return close((int)stream);
}

#if sfs
int fprintf(FILE *stream, const char *format, ...)
{
    va_list args;
    char *s;
    char s2[256];
    int n;

    if (stream == stdout || stream == stderr)
        s = NULL;
    else
        s = s2;

    va_start(args, format);

    n = vsprintf(s, format, args);

    va_end(args);

    if (s)
        n = write((int)stream, s, n);

    return n;
}
#endif

int fflush(FILE *stream)
{
    EFI_FILE_HANDLE file;
    EFI_STATUS status;
    int fd = (int)stream;

    if (fd < 4)
        return -1;
    fd -= 4;
    if (fd > MAX_HANDLES)
        return -1;

    file = gFileHandles[fd];
    if (!file)
        return -1;

    status = file->Flush(file);
    if (EFI_ERROR(status))
    {
        last_efi_status = status;
        printf("Flush failed!\n");
    }

    return 0;
}

int fputs(const char *s, FILE *stream)
{
    int n;

    n = (int)strlen(s);
    if (stream == stdout || stream == stderr)
        APrint("%a", s);
    else
        write((int)stream, s, n);

    return n;
}

int fputc(int c, FILE *stream)
{
    if (stream == stdout || stream == stderr)
        APrint("%c", (UINTN)c);
    else
        write((int)stream, (void *)&c, 1);

    return 1;
}

int open(const char *name, int flags, ...)
{
    EFI_FILE_IO_INTERFACE *fileIo;
    EFI_FILE_HANDLE root;
    EFI_FILE_HANDLE file;
    EFI_FILE_INFO *info;
    EFI_GUID SimpleFileSystemProtocol = SIMPLE_FILE_SYSTEM_PROTOCOL;
    EFI_STATUS status;
    CHAR16 *path;
    CHAR16 filename[128];
    UINTN i;
    UINT64 mode;
    int fd;

    for (fd = 0; fd < MAX_HANDLES; fd++)
        if (!gFileHandles[fd])
            break;
    if (fd == MAX_HANDLES)
        return -1;

    if (*name == '.' || (isalnum(*name) && !strchr(name, ':')))
    {
        path = DevicePathToStr(gLoadedImage->FilePath);

//      APrint("loaded image = <%s>\n", path);

        for (i = StrLen(path) - 1; i > 0; i--)
            if (path[i] == L'/')
                break;
        // if the path contains "/." at the end, remove it
        if (i >= 2 && path[i-2] == '/' && path[i-1] == '.')
            i -= 2;
        // if the path doesn't end with "\", add one
        if (!(i >= 1 && path[i-1] == '\\'))
            path[i++] = '\\';
        // throw away the rest of the string
        path[i] = 0;
        StrCpy(filename, path);

        FreePool(path);
    } else
        filename[0] = 0;

    for (i = StrLen(filename); *name; i++)
        filename[i] = (CHAR16)*name++;
    filename[i] = 0;

//  APrint("filename = <%s>\n", filename);

    status = BS->HandleProtocol(gLoadedImage->DeviceHandle,
                                &SimpleFileSystemProtocol, &fileIo);
    if (EFI_ERROR(status))
    {
        last_efi_status = status;
        printf("HandleProtocol(SimpleFileSystem) failed!\n");
        return -1;
    }

    status = fileIo->OpenVolume(fileIo, &root);
    if (EFI_ERROR(status))
    {
        last_efi_status = status;
        printf("OpenVolume failed!\n");
        return -1;
    }

    mode = EFI_FILE_MODE_READ;
    if (flags & O_RDWR)
        mode |= EFI_FILE_MODE_WRITE;
    if (flags & O_CREAT)
        mode |= EFI_FILE_MODE_CREATE;

    status = root->Open(root, &file, filename, mode, 0);
    if (EFI_ERROR(status))
    {
        last_efi_status = status;
        APrint("Open of <%s> failed!\n", filename);
        return -1;
    }

    if (flags & O_CREAT)
    {
        info = LibFileInfo(file);
        if (info)
        {
            if (flags & O_TRUNC)
            {
                info->FileSize = 0;
                info->PhysicalSize = 0;
                status = file->SetInfo(file, &GenericFileInfo, (UINTN)info->Size, info);
                if (EFI_ERROR(status))
                {
                    last_efi_status = status;
                    APrint("SetInfo of <%s> failed!\n", filename);
                }
            }
            else
            {
                status = file->SetPosition(file, info->FileSize);
                if (EFI_ERROR(status))
                {
                    last_efi_status = status;
                    APrint("SetPosition of <%s> failed!\n", filename);
                }
            }

            FreePool(info);
        }
    }

    gFileHandles[fd] = file;

    fd += 4;

    return fd;
}

int close(int fd)
{
    EFI_FILE_HANDLE file;
    EFI_STATUS status;

    if (fd < 4)
        return -1;
    fd -= 4;
    if (fd > MAX_HANDLES)
        return -1;

    file = gFileHandles[fd];
    if (!file)
        return -1;

    status = file->Close(file);
    if (EFI_ERROR(status))
    {
        last_efi_status = status;
        printf("Close failed!\n");
    }

    return 0;
}

ssize_t read(int fd, void *buf, size_t count)
{
    EFI_FILE_HANDLE file;
    EFI_STATUS status;
    UINTN size = count;

    if (fd < 4)
        return -1;
    fd -= 4;
    if (fd > MAX_HANDLES)
        return -1;

    file = gFileHandles[fd];
    if (!file)
        return -1;

    status = file->Read(file, &size, buf);
    if (EFI_ERROR(status))
    {
        last_efi_status = status;
        printf("Read failed!\n");
        return -1;
    }

    return (ssize_t)size;
}

ssize_t write(int fd, const void *buf, size_t count)
{
    EFI_FILE_HANDLE file;
    EFI_STATUS status;
    UINTN size = count;

    if (fd < 4)
        return -1;
    fd -= 4;
    if (fd > MAX_HANDLES)
        return -1;

    file = gFileHandles[fd];
    if (!file)
        return -1;

    status = file->Write(file, &size, (void *)buf);
    if (EFI_ERROR(status))
    {
        last_efi_status = status;
        printf("Write failed!\n");
        return -1;
    }

    return (ssize_t)size;
}

int fstat(int fd, struct stat *buf)
{
    EFI_FILE_HANDLE file;
    EFI_FILE_INFO *info;

    if (fd < 4)
        return -1;
    fd -= 4;
    if (fd > MAX_HANDLES)
        return -1;

    file = gFileHandles[fd];
    if (!file)
        return -1;

    info = LibFileInfo(file);
    if (!info)
    {
        printf("GetInfo failed!\n");
        return -1;
    }

    buf->st_size = (size_t)info->FileSize;

    FreePool(info);

    return 0;
}

static int rand_seed = 152L;

int rand(void)
{
    rand_seed = rand_seed * 69069L + 1;

    return ((rand_seed >> 16) + (rand_seed << 16));
}

void perror(const char *s)
{
    if (s && *s)
        APrint("%a: ", s);
    APrint("%r\n", last_efi_status);
}

static int dpm[] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
};

#ifndef DISABLE_TIMEOUTS
time_t time(time_t *t)
{
    EFI_TIME et;
    time_t local_t;
    int days;

    RT->GetTime(&et, NULL);

    days = dpm[et.Month - 1] + et.Day - 1;

    if (et.Year >= 1970)
    {
        days += (et.Year - 1970) * 365;

        // all modulo-4 years since 1970 are leap years, up until 2100
        days += (et.Year - 1969) / 4;
    }

    local_t = ((days * 24 + et.Hour) * 60 + et.Minute) * 60 + et.Second;
    if (t)
        *t = local_t;

    return local_t;
}
#endif

static const char *day_name[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static const char *mon_name[] = {
    "Mar", "Apr", "May", "Jun", "Jul", "Aug",
    "Sep", "Oct", "Nov", "Dec", "Jan", "Feb"
};

static const int days_per[12] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static char ctime_result[26];

char *ctime(const time_t *seconds)
{
    int f, k, m, D, C, d;  // going to use Zeller's Rule to derive the weekday
    int days, leap, year, month, day, hours, mins, secs, t;

    days = *seconds / 86400;
    t = days;
    year = 1970;
    // if it's already past 2004, get that out of the way quick
    if (t >= 12418)
    {
        t -= 12418;
        year += 34;
    }
    for (; ; year++)
    {
        // this simple rule is good until 2100
        leap = (year % 4) == 0;
        if (t < 365 + leap)
            break;
        t -= 365 + leap;
    }
    month = 0;
    // if it's already past June, get that out of the way quick
    if (t >= 181 + leap)
    {
        t -= 181 + leap;
        month += 6;
    }
    for (; ; month++)
    {
        if (t < days_per[month])
            break;
        t -= days_per[month];
        if (month == 1)
        {
            if (t < leap)
                break;
            t -= leap;
        }
    }
    day = t + 1;
    t = *seconds - days * 86400;
    hours = t / 3600;
    t -= hours * 3600;
    mins = t / 60;
    t -= mins * 60;
    secs = t;

    k = day;
    m = month - 1;
    D = year % 100;
    C = year / 100;

    if (m < 1)
    {
        m += 12;
        D -= 1;
    }

    f = k + (13*m-1)/5 + D + D/4 + C/4 - 2*C;

    d = f % 7;
    if (d < 0)
        d += 7;

    sprintf(ctime_result, "%.3s %.3s%3d %02d:%02d:%02d %d\n",
            day_name[d], mon_name[m-1],
            day, hours, mins, secs, year);

    return ctime_result;
}

VOID EfiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_TIME et;
    EFI_STATUS status;
    int i;
    int argc;
    char **argv;
    int n;

    InitializeShellApplication(ImageHandle, SystemTable);

    gImageHandle = ImageHandle;

    status = BS->HandleProtocol(gImageHandle, &LoadedImageProtocol, &gLoadedImage);
    if (EFI_ERROR(status))
    {
        last_efi_status = status;
        printf("HandleProtocol(LoadedImage) failed!\n");
        gLoadedImage = NULL;
    }

    status = BS->CreateEvent(EVT_TIMER, 0, NULL, NULL, &gSleepEvent);
    if (EFI_ERROR(status))
        gSleepEvent = NULL;

    RT->GetTime(&et, NULL);
    if (et.Year % 4 == 0 && et.Year % 100 != 0 && et.Year % 400 == 0)
        dpm[3]++;

    argc = (int)SI->Argc;
    argv = malloc(sizeof argv * argc);

    for (i = 0; i < argc; i++)
    {
        n = (int)StrLen(SI->Argv[i]) + 1;
        argv[i] = malloc(n);
        convert_unicode_ascii(SI->Argv[i], argv[i], n);
    }

    main(argc, argv);
}

void exit(int status)
{
    if (gSleepEvent)
        BS->CloseEvent(gSleepEvent);

    if (status == 0)
        BS->Exit(gImageHandle, EFI_SUCCESS, 0, NULL);
    else
        BS->Exit(gImageHandle, EFIERR_OEM(status), 0, NULL);
}

unsigned int sleep(unsigned int seconds)
{
    UINTN unused;

    if (gSleepEvent)
    {
        BS->SetTimer(gSleepEvent, TimerRelative, seconds * 10000000);
        BS->WaitForEvent(1, &gSleepEvent, &unused);
    }
    else
    {
        BS->Stall(seconds * 1000000);
    }

    return 0;
}

void mdelay(int milliseconds)
{
    BS->Stall(milliseconds * 1000);
}

void udelay(int microseconds)
{
    BS->Stall(microseconds);
}

int _fltused;

#include "getopt.c"

/* vi: set sw=4 ts=4 sts=4 et :iv */
