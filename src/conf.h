/* conf.h --

   This file is part of the UPX executable compressor.

   Copyright (C) 1996-2000 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 1996-2000 Laszlo Molnar
   All Rights Reserved.

   UPX and the UCL library are free software; you can redistribute them
   and/or modify them under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.
   If not, write to the Free Software Foundation, Inc.,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   Markus F.X.J. Oberhumer                   Laszlo Molnar
   markus.oberhumer@jk.uni-linz.ac.at        ml1050@cdata.tvnet.hu
 */


#ifndef __UPX_CONF_H
#define __UPX_CONF_H

#if 0 && defined(__EMX__)
#  include <sys/emx.h>
#endif

#if defined(UPX_CONFIG_H)
#  include UPX_CONFIG_H
#endif
#include <limits.h>

#include "version.h"
#include "tailor.h"

#if !defined(__i386__)
#  if defined(__386__) || defined(_M_IX86)
#    define __i386__ 1
#  endif
#endif
#if defined(__linux__) && !defined(__unix__)
#  define __unix__ 1
#endif

// just in case
#undef NDEBUG
#undef dos
#undef linux
#undef small
#undef tos
#undef unix


#if defined(WITH_UCL)
#  include <ucl/uclconf.h>
#  include <ucl/ucl.h>
#  if !defined(UPX_UINT_MAX)
#    define UPX_UINT_MAX  UCL_UINT_MAX
#    define upx_uint      ucl_uint
#    define upx_voidp     ucl_voidp
#    define upx_uintp     ucl_uintp
#    define upx_byte      ucl_byte
#    define upx_bytep     ucl_bytep
#    define upx_bool      ucl_bool
#    define upx_progress_callback_t ucl_progress_callback_t
#    define upx_adler32   ucl_adler32
#    define UPX_E_OK      UCL_E_OK
#    define UPX_E_ERROR   UCL_E_ERROR
#    define UPX_E_OUT_OF_MEMORY UCL_E_OUT_OF_MEMORY
#    define __UPX_ENTRY   __UCL_ENTRY
#  endif
#endif
#if defined(WITH_NRV)
#  include <nrv/nrvconf.h>
#  if !defined(UPX_UINT_MAX)
#    define UPX_UINT_MAX  NRV_UINT_MAX
#    define upx_uint      nrv_uint
#    define upx_voidp     nrv_voidp
#    define upx_uintp     nrv_uintp
#    define upx_byte      nrv_byte
#    define upx_bytep     nrv_bytep
#    define upx_bool      nrv_bool
#    define upx_progress_callback_t nrv_progress_callback_t
#    define upx_adler32   nrv_adler32
#    define UPX_E_OK      NRV_E_OK
#    define UPX_E_ERROR   NRV_E_ERROR
#    define UPX_E_OUT_OF_MEMORY NRV_E_OUT_OF_MEMORY
#    define __UPX_ENTRY   __NRV_ENTRY
#  endif
#endif
#if !defined(__UPX_CHECKER)
#  if defined(__UCL_CHECKER) || defined(__NRV_CHECKER)
#    define __UPX_CHECKER
#  endif
#endif
#if !defined(UPX_UINT_MAX) || (UINT_MAX < 0xffffffffL)
#  error "you lose"
#endif
#if !defined(WITH_UCL)
#  error "you lose"
#endif
#if !defined(UCL_VERSION) || (UCL_VERSION < 0x009200L)
#  error "please upgrade your UCL installation"
#endif

#ifndef WITH_ZLIB
#  define WITH_ZLIB 1
#endif


/*************************************************************************
// system includes
**************************************************************************/

#if !defined(NO_SYS_TYPES_H)
#  include <sys/types.h>
#endif

#define NDEBUG
#undef NDEBUG
#include <assert.h>

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
#if !defined(NO_FCNTL_H)
#  include <fcntl.h>
#endif
#if !defined(NO_SYS_STAT_H)
#  include <sys/stat.h>
#endif
#if defined(HAVE_IO_H) && !defined(NO_IO_H)
#  include <io.h>
#endif
#if defined(HAVE_DOS_H) && !defined(NO_DOS_H)
#  include <dos.h>
#endif
#if defined(HAVE_MALLOC_H) && !defined(NO_MALLOC_H)
#  include <malloc.h>
#endif
#if defined(HAVE_ALLOCA_H) && !defined(NO_ALLOCA_H)
#  include <alloca.h>
#endif
#if defined(HAVE_SIGNAL_H)
#  include <signal.h>
#endif
#if defined(HAVE_UNISTD_H)
#  include <unistd.h>
#endif
#if defined(TIME_WITH_SYS_TIME)
#  include <sys/time.h>
#  include <time.h>
#else
#  include <time.h>
#endif
#if defined(HAVE_UTIME_H)
#  include <utime.h>
#elif defined(HAVE_SYS_UTIME_H)
#  include <sys/utime.h>
#endif
#if defined(HAVE_SHARE_H)
#  include <share.h>
#endif


// malloc debuggers
#if defined(WITH_DMALLOC)
#  define DMALLOC_FUNC_CHECK
#  include <dmalloc.h>
#elif defined(WITH_GC)
#  define GC_DEBUG
#  include <gc/gc.h>
#  undef malloc
#  undef realloc
#  undef free
#  define malloc            GC_MALLOC
#  define realloc           GC_REALLOC
#  define free              GC_FREE
#elif defined(WITH_MSS)
#  define MSS
#  include <mss.h>
#endif


/*************************************************************************
// portab
**************************************************************************/

#if defined(NO_BOOL)
typedef int bool;
enum { false, true };
#endif

#if !defined(PATH_MAX)
#  define PATH_MAX          512
#elif (PATH_MAX < 512)
#  undef PATH_MAX
#  define PATH_MAX          512
#endif


#ifndef RETSIGTYPE
#  define RETSIGTYPE        void
#endif
#ifndef SIGTYPEENTRY
#  define SIGTYPEENTRY
#endif
typedef RETSIGTYPE (SIGTYPEENTRY *sig_type)(int);

#undef MODE_T
#if defined(HAVE_MODE_T)
#  define MODE_T            mode_t
#else
#  define MODE_T            int
#endif

#if !defined(HAVE_STRCHR)
#  if defined(HAVE_INDEX)
#    define strchr          index
#  endif
#endif
#if !defined(HAVE_STRCASECMP)
#  if defined(HAVE_STRICMP)
#    define strcasecmp      stricmp
#  else
#    define strcasecmp      strcmp
#  endif
#endif
#if !defined(HAVE_STRNCASECMP)
#  if defined(HAVE_STRNICMP)
#    define strncasecmp     strnicmp
#  else
#    define strncasecmp     strncmp
#  endif
#endif


#ifndef STDIN_FILENO
#  define STDIN_FILENO      (fileno(stdin))
#endif
#ifndef STDOUT_FILENO
#  define STDOUT_FILENO     (fileno(stdout))
#endif
#ifndef STDERR_FILENO
#  define STDERR_FILENO     (fileno(stderr))
#endif


#if !defined(S_IWUSR) && defined(_S_IWUSR)
#  define S_IWUSR           _S_IWUSR
#elif !defined(S_IWUSR) && defined(_S_IWRITE)
#  define S_IWUSR           _S_IWRITE
#endif

#if !defined(S_IFMT) && defined(_S_IFMT)
#  define S_IFMT            _S_IFMT
#endif
#if !defined(S_IFREG) && defined(_S_IFREG)
#  define S_IFREG           _S_IFREG
#endif
#if !defined(S_IFDIR) && defined(_S_IFDIR)
#  define S_IFDIR           _S_IFDIR
#endif
#if !defined(S_IFCHR) && defined(_S_IFCHR)
#  define S_IFCHR           _S_IFCHR
#endif

#if !defined(S_ISREG)
#  if defined(S_IFMT) && defined(S_IFREG)
#    define S_ISREG(m)      (((m) & S_IFMT) == S_IFREG)
#  else
#    error S_ISREG
#  endif
#endif
#if !defined(S_ISDIR)
#  if defined(S_IFMT) && defined(S_IFDIR)
#    define S_ISDIR(m)      (((m) & S_IFMT) == S_IFDIR)
#  else
#    error S_ISDIR
#  endif
#endif
#if !defined(S_ISCHR)
#  if defined(S_IFMT) && defined(S_IFCHR)
#    define S_ISCHR(m)      (((m) & S_IFMT) == S_IFCHR)
#  endif
#endif


// avoid warnings about shadowing that obsolete index() function
#define index   upx_index

// a dummy statement
#define nop     ((void)0)


/*************************************************************************
// file io
**************************************************************************/

#if defined(HAVE_SETMODE)
#  if !defined(O_BINARY)
#    error "setmode without O_BINARY"
#  endif
#  define USE_SETMODE 1
#endif

#if !defined(O_BINARY)
#  define O_BINARY  0
#endif

#if defined(__DJGPP__)
#  undef sopen
#  undef USE_SETMODE
#endif


/*************************************************************************
// memory util
**************************************************************************/

#undef FREE
#define FREE(ptr)           if (ptr) { free(ptr); ptr = NULL; }

#undef UNUSED
#if defined(__BORLANDC__)
#define UNUSED(parm)        ((void)(parm))
#else
#define UNUSED(parm)        (parm = parm)
#endif

#define HIGH(array)         ((unsigned) (sizeof(array)/sizeof((array)[0])))

#define ALIGN_DOWN(a,b)     (((a) / (b)) * (b))
#define ALIGN_UP(a,b)       ALIGN_DOWN((a) + ((b) - 1), b)

#define UPX_MAX(a,b)        ((a) >= (b) ? (a) : (b))
#define UPX_MIN(a,b)        ((a) <= (b) ? (a) : (b))
#define UPX_MAX3(a,b,c)     ((a) >= (b) ? UPX_MAX(a,c) : UPX_MAX(b,c))
#define UPX_MIN3(a,b,c)     ((a) <= (b) ? UPX_MIN(a,c) : UPX_MIN(b,c))


#if 0 && defined(__cplusplus)
// global operators - debug
inline void *operator new(size_t l)
{
    void *p = malloc(l);
    printf("new %6ld %p\n",(long)l,p);
    fflush(stdout);
    return p;
}
inline void *operator new[](size_t l)
{
    void *p = malloc(l);
    printf("new %6ld %p\n",(long)l,p);
    fflush(stdout);
    return p;
}
inline void operator delete(void *p)
{
    printf("delete     %p\n",p);
    fflush(stdout);
    if (p) free(p);
}
inline void operator delete[](void *p)
{
    printf("delete     %p\n",p);
    fflush(stdout);
    if (p) free(p);
}
#endif


// A autoheap_array allocates memory on the heap, but automatically
// gets destructed when leaving scope or on exceptions.
// "var" is declared as a read-only reference to a pointer
// and behaves exactly like an array "var[]".
#if 0
# define autoheap_array(type, var, size) \
    assert((int)(size) > 0); \
    vector<type> var ## _autoheap_vec((size)); \
    type * const & var = & var ## _autoheap_vec[0]
#else
# define autoheap_array(type, var, size) \
    assert((int)(size) > 0); \
    MemBuffer var ## _autoheap_buf((size)*(sizeof(type))); \
    type * const & var = (type *) (unsigned char *) var ## _autoheap_buf
#endif


/*************************************************************************
//
**************************************************************************/

/* exit codes of this program: 0 ok, 1 error, 2 warning */
#define EXIT_OK         0
#define EXIT_ERROR      1
#define EXIT_WARN       2

#define EXIT_USAGE      1
#define EXIT_FILE_READ  1
#define EXIT_FILE_WRITE 1
#define EXIT_MEMORY     1
#define EXIT_CHECKSUM   1
#define EXIT_INIT       1
#define EXIT_INTERNAL   1


// compression methods - DO NOT CHANGE
#define M_NRV2B_LE32    2
#define M_NRV2B_8       3
#define M_NRV2B_LE16    4
#define M_NRV2D_LE32    5
#define M_NRV2D_8       6
#define M_NRV2D_LE16    7

#define M_IS_NRV2B(x)   ((x) >= M_NRV2B_LE32 && (x) <= M_NRV2B_LE16)
#define M_IS_NRV2D(x)   ((x) >= M_NRV2D_LE32 && (x) <= M_NRV2D_LE16)


/*************************************************************************
// globals
**************************************************************************/

#include "unupx.h"


#if defined(__cplusplus)

#include "stdcxx.h"
#include "except.h"
#include "bele.h"
#include "util.h"
#include "console.h"

// options
enum {
    CMD_NONE,
    CMD_COMPRESS, CMD_DECOMPRESS, CMD_TEST, CMD_LIST, CMD_FILEINFO,
    CMD_HELP, CMD_LICENSE, CMD_VERSION
};

struct options_t {
    int cmd;

    // compression options
    int method;
    int level;          // compression level 1..10
    int mem_level;      // memory level 1..9
    int filter;         // preferred filter from Packer::getFilters()
    bool all_filters;   // try all filters ?

    // other options
    int backup;
    int console;
    int debug;
    int force;
    int info_mode;
    bool ignorewarn;
    bool no_env;
    bool no_progress;
    const char *output_name;
    int small;
    int verbose;
    bool to_stdout;

    // overlay handling
    enum {
        SKIP_OVERLAY      = 0,
        COPY_OVERLAY      = 1,
        STRIP_OVERLAY     = 2
    };
    int overlay;

    // compression runtime parameters - see struct ucl_compress_config_t
    struct {
        upx_uint max_offset;
        upx_uint max_match;
        int s_level;
        int h_level;
        int p_level;
        int c_flags;
        upx_uint m_size;
    } crp;

    // CPU
    enum {
        CPU_DEFAULT = 0,
        CPU_8086    = 1,
        CPU_286     = 2,
        CPU_386     = 3,
        CPU_486     = 4,
        CPU_586     = 5,
        CPU_686     = 6
    };
    int cpu;

    // options for various executable formats
    struct {
        bool force_stub;
        bool no_reloc;
    } dos;
    struct {
        bool coff;
    } djgpp2;
    struct {
        bool split_segments;
    } tos;
    struct {
        unsigned blocksize;
        enum { SCRIPT_MAX = 32 };
        const char *script_name;
    } unix;
    struct {
        bool le;
    } wcle;
    struct {
        int compress_exports;
        int compress_icons;
        bool compress_resources;
        int strip_relocs;
    } w32pe;
};

extern struct options_t * volatile opt;


// main.cpp
extern const char *progname;
void init_options(struct options_t *o);
bool set_ec(int ec);
#if defined(__GNUC__)
void e_exit(int ec) __attribute__((noreturn));
#else
void e_exit(int ec);
#endif


// msg.cpp
void printSetNl(int need_nl);
void printClearLine(FILE *f = NULL);
void printErr(const char *iname, const Throwable *e);
void printUnhandledException(const char *iname, const exception *e);
#if defined(__GNUC__)
void printErr(const char *iname, const char *format, ...)
        __attribute__((format(printf,2,3)));
void printWarn(const char *iname, const char *format, ...)
        __attribute__((format(printf,2,3)));
#else
void printErr(const char *iname, const char *format, ...);
void printWarn(const char *iname, const char *format, ...);
#endif

#if defined(__GNUC__)
void infoWarning(const char *format, ...)
        __attribute__((format(printf,1,2)));
void infoHeader(const char *format, ...)
        __attribute__((format(printf,1,2)));
void info(const char *format, ...)
        __attribute__((format(printf,1,2)));
#else
void infoWarning(const char *format, ...);
void infoHeader(const char *format, ...);
void info(const char *format, ...);
#endif
void infoHeader();
void infoWriting(const char *what, long size);


// work.cpp
void do_one_file(const char *iname, char *oname);
void do_files(int i, int argc, char *argv[]);


// help.cpp
void show_head(void);
void show_help(int x = 0);
void show_license(void);
void show_usage(void);
void show_version(int);


// compress.cpp
#if defined(WITH_UCL)
#define upx_compress_config_t   ucl_compress_config_t
#elif defined(WITH_NRV)
struct nrv_compress_config_t;
struct nrv_compress_config_t
{
    int bb_endian;
    int bb_size;
    nrv_uint max_offset;
    nrv_uint max_match;
    int s_level;
    int h_level;
    int p_level;
    int c_flags;
    nrv_uint m_size;
};
#define upx_compress_config_t   nrv_compress_config_t
#endif

int upx_compress           ( const upx_byte *src, upx_uint  src_len,
                                   upx_byte *dst, upx_uint *dst_len,
                                   upx_progress_callback_t *cb,
                                   int method, int level,
                             const struct upx_compress_config_t *conf,
                                   upx_uintp result);
int upx_decompress         ( const upx_byte *src, upx_uint  src_len,
                                   upx_byte *dst, upx_uint *dst_len,
                                   int method );
int upx_test_overlap       ( const upx_byte *buf, upx_uint src_off,
                                   upx_uint  src_len, upx_uint *dst_len,
                                   int method );

#endif /* __cplusplus */


#endif /* already included */


/*
vi:ts=4:et
*/

