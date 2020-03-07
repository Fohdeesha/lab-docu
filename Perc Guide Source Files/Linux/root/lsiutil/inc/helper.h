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

#if EFIEBC
#define EFI_SPECIFICATION_VERSION 0x0001000A
#define TIANO_RELEASE_VERSION 0x00080006
#include "efibind.h"
#include "efitypes.h"
#include "eficommon.h"
#include "efiapi.h"
#include "efierror.h"
#include "efistdarg.h"
#include "efishelllib.h"
#define DevicePathToStr LibDevicePathToStr
#define LibFileInfo LibGetFileInfo
#define GenericFileInfo gEfiFileInfoGuid
#define InitializeShellApplication LibInitializeShellApplication
#define LoadedImageProtocol gEfiLoadedImageProtocolGuid
#define EFIERR_OEM(x) EFIERR(x+1000)
#define EFI_LOADED_IMAGE EFI_LOADED_IMAGE_PROTOCOL
#define EFI_FILE_IO_INTERFACE EFI_SIMPLE_FILE_SYSTEM_PROTOCOL
#define SIMPLE_FILE_SYSTEM_PROTOCOL EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID
#define EVT_TIMER EFI_EVENT_TIMER
#define va_start VA_START
#define va_arg VA_ARG
#define va_end VA_END
#define va_list VA_LIST
#else
#include "efi.h"
#include "efilib.h"
#include "efistdarg.h"
#include "shell.h"
#endif
#if EFI32
typedef unsigned int size_t;
#endif
// ctype.h
int isalnum(int c);
int isalpha(int c);
int isascii(int c);
int isblank(int c);
int iscntrl(int c);
int isdigit(int c);
int isgraph(int c);
int islower(int c);
int isprint(int c);
int ispunct(int c);
int isspace(int c);
int isupper(int c);
int isxdigit(int c);
int toupper(int c);
int tolower(int c);
// errno.h
extern int errno;
#define EFAULT 14
#define ENODEV 19
#define EINVAL 22

// fcntl.h
#define O_RDONLY             00
#define O_WRONLY             01
#define O_RDWR               02
#define O_CREAT            0100 /* not fcntl */
#define O_TRUNC           01000 /* not fcntl */
// sttdef.h
#define   offsetof(s, m)  (size_t)(&(((s *)0)->m))
// stdio.h
#define EOF 0
typedef void * FILE;
#define stdin (void *)1
#define stdout (void *)2
#define stderr (void *)3
int fgetc(FILE *stream);
char *fgets(char *s, int size, FILE *stream);
int getc(FILE *stream);
int getchar(void);
char *gets(char *s);
int ungetc(int c, FILE *stream);
int vsprintf(char *s, const char *format, va_list args);
int printf(const char *format, ...);
int fprintf(FILE *stream, const char *format, ...);
int sprintf(char *str, const char *format, ...);
int scanf(const char *format, ...);
int fscanf(FILE *stream, const char *format, ...);
int sscanf(const char *str, const char *format, ...);
FILE *fopen(const char *path, const char *mode);
int fclose(FILE *stream);
int fputs(const char *s, FILE *stream);
int fputc(int c, FILE *stream);
void perror(const char *s);
int fflush(FILE *stream);
// stdlib.h
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
void exit(int status);
int rand(void);
// string.h
void *memset(void *s, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
size_t strlen(const char *s);
char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, size_t n);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
// strings.h
int strcasecmp(const char *s1, const char *s2);
int strncasecmp(const char *s1, const char *s2, size_t n);
void bzero(void *s, size_t n);
void bcopy(const void *src, void *dest, size_t n);
// sys/stat.h
typedef size_t off_t;
struct stat {
    off_t   st_size;     /* total size, in bytes */
};
int stat(const char *file_name, struct stat *buf);
int fstat(int filedes, struct stat *buf);
// time.h
typedef unsigned long time_t;
time_t time(time_t *t);
char *ctime(const time_t *t);
void mdelay(int milliseconds);
void udelay(int microseconds);
// unistd.h
typedef int ssize_t;
typedef int mode_t;
unsigned int sleep(unsigned int seconds);
int open(const char *pathname, int flags, ...);
int close(int fd);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
