/* file.cpp --

   This file is part of the UPX executable compressor.

   Copyright (C) 1996-2001 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 1996-2001 Laszlo Molnar
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

   Markus F.X.J. Oberhumer   Laszlo Molnar
   markus@oberhumer.com      ml1050@cdata.tvnet.hu
 */


#include "conf.h"
#include "file.h"


/*************************************************************************
//
**************************************************************************/

void File::chmod(const char *name, int mode)
{
#if defined(HAVE_CHMOD)
    if (::chmod(name,mode) != 0)
        throwIOException(name,errno);
#endif
}


void File::rename(const char *old_, const char *new_)
{
#if 1 && defined(__DJGPP__)
    if (::_rename(old_,new_) != 0)
#else
    if (::rename(old_,new_) != 0)
#endif
        throwIOException("rename error",errno);
}


void File::unlink(const char *name)
{
    if (::unlink(name) != 0)
        throwIOException(name,errno);
}


/*************************************************************************
//
**************************************************************************/

FileBase::FileBase() :
    _fd(-1), _flags(0), _shflags(0), _mode(0), _name(NULL)
{
    memset(&st,0,sizeof(st));
}


FileBase::~FileBase()
{
#if 0 && defined(__GNUC__)    // debug
    if (isOpen())
        fprintf(stderr,"%s: %s\n", _name, __PRETTY_FUNCTION__);
#endif

    // FIXME: we should use close() during exception unwinding but
    //        closex() otherwise
    closex();
}


bool FileBase::close()
{
    bool ok = true;
    if (isOpen() && _fd != STDIN_FILENO && _fd != STDOUT_FILENO && _fd != STDERR_FILENO)
        if (::close(_fd) == -1)
            ok = false;
    _fd = -1;
    _flags = 0;
    _mode = 0;
    _name = NULL;
    return ok;
}


void FileBase::closex()
{
    if (!close())
        throwIOException("close failed",errno);
}


int FileBase::read(void *buf, int len)
{
    int l;
    if (!isOpen() || len < 0)
        throwIOException("bad read");
    if (len == 0)
        return 0;
    for (;;)
    {
#if 1 && defined(__DJGPP__)
        l = ::_read(_fd,buf,len);
#else
        l = ::read(_fd,buf,len);
#endif
        if (l < 0)
        {
#if defined(EINTR)
            if (errno == EINTR)
                continue;
#endif
            throwIOException("read error",errno);
        }
        break;
    }
    return l;
}


int FileBase::readx(void *buf, int len)
{
    int l = this->read(buf,len);
    if (l != len)
        throwEOFException();
    return l;
}


void FileBase::write(const void *buf, int len)
{
    int l;
    if (!isOpen() || len < 0)
        throwIOException("bad write");
    if (len == 0)
        return;
    for (;;)
    {
#if 1 && defined(__DJGPP__)
        l = ::_write(_fd,buf,len);
#else
        l = ::write(_fd,buf,len);
#endif
#if defined(EINTR)
        if (l < 0 && errno == EINTR)
            continue;
#endif
        if (l != len)
            throwIOException("write error",errno);
        break;
    }
}


void FileBase::seek(off_t off, int whence)
{
    if (!isOpen())
        throwIOException("bad seek 1");
    if (whence == SEEK_SET && off < 0)
        throwIOException("bad seek 2");
    if (whence == SEEK_END && off > 0)
        throwIOException("bad seek 3");
    if (::lseek(_fd,off,whence) < 0)
        throwIOException("seek error",errno);
}


off_t FileBase::tell()
{
    if (!isOpen())
        throwIOException("bad tell");
    off_t l = ::lseek(_fd,0,SEEK_CUR);
    if (l < 0)
        throwIOException("tell error",errno);
    return l;
}


/*************************************************************************
//
**************************************************************************/

InputFile::InputFile()
{
}


InputFile::~InputFile()
{
}


void InputFile::sopen(const char *name, int flags, int shflags)
{
    close();
    _name = name;
    _flags = flags;
    _shflags = shflags;
    _mode = 0;
    if (shflags < 0)
        _fd = ::open(_name,_flags);
    else
#if defined(__DJGPP__)
        _fd = ::open(_name,_flags | _shflags);
#elif defined(SH_DENYRW)
        _fd = ::sopen(_name,_flags,_shflags);
#else
        assert(0);
#endif
    if (!isOpen())
    {
        if (errno == ENOENT)
            throw FileNotFoundException(_name,errno);
        else if (errno == EEXIST)
            throw FileAlreadyExistsException(_name,errno);
        else
            throwIOException(_name,errno);
    }
}


int InputFile::read(void *buf, int len)
{
    return super::read(buf,len);
}


int InputFile::readx(void *buf, int len)
{
    return super::readx(buf,len);
}


void InputFile::seek(off_t off, int whence)
{
    super::seek(off,whence);
}


off_t InputFile::tell()
{
    return super::tell();
}


/*************************************************************************
//
**************************************************************************/

OutputFile::OutputFile() :
    bytes_written(0)
{
}


OutputFile::~OutputFile()
{
}


void OutputFile::sopen(const char *name, int flags, int shflags, int mode)
{
    close();
    _name = name;
    _flags = flags;
    _shflags = shflags;
    _mode = mode;
    if (shflags < 0)
        _fd = ::open(_name,_flags,_mode);
    else
#if defined(__DJGPP__)
        _fd = ::open(_name,_flags | _shflags, _mode);
#elif defined(SH_DENYRW)
        _fd = ::sopen(_name,_flags,_shflags,_mode);
#else
        assert(0);
#endif
    if (!isOpen())
    {
#if 0
        // don't throw FileNotFound here -- this is confusing
        if (errno == ENOENT)
            throw FileNotFoundException(_name,errno);
        else
#endif
        if (errno == EEXIST)
            throw FileAlreadyExistsException(_name,errno);
        else
            throwIOException(_name,errno);
    }
}


bool OutputFile::openStdout(int flags, bool force)
{
    close();
    if (!force)
    {
        if (!isafile(STDOUT_FILENO))
            return false;
    }
    _fd = STDOUT_FILENO;
    _name = "<stdout>";
    _flags = flags;
    _shflags = -1;
    _mode = 0;
    if (flags != 0)
    {
        assert(flags == O_BINARY);
#if defined(HAVE_SETMODE) && defined(USE_SETMODE)
        if (setmode(_fd, O_BINARY) == -1)
            throwIOException(_name,errno);
#if defined(__DJGPP__)
        __djgpp_set_ctrl_c(1);
#endif
#endif
    }
    return true;
}


void OutputFile::write(const void *buf, int len)
{
    super::write(buf,len);
    bytes_written += len;
}


void OutputFile::dump(const char *name, const void *buf, int len, int flags)
{
    if (flags < 0)
         flags = O_CREAT | O_BINARY | O_TRUNC;
    flags |= O_WRONLY;
    OutputFile f;
    f.open(name, flags, 0666);
    f.write(buf, len);
    f.closex();
}


/*************************************************************************
//
**************************************************************************/

MemoryOutputFile::MemoryOutputFile() :
    b(NULL), b_size(0), b_pos(0), bytes_written(0)
{
}


void MemoryOutputFile::write(const void *buf, int len)
{
    if (!isOpen() || len < 0)
        throwIOException("bad write");
    if (len == 0)
        return;
    if (b_pos + len > b_size)
        throwIOException("write error",ENOSPC);
    memcpy(b + b_pos, buf, len);
    b_pos += len;
    bytes_written += len;
}


/*
vi:ts=4:et
*/

