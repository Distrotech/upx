/* except.h --

   This file is part of the UPX executable compressor.

   Copyright (C) 1996-2000 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 1996-2000 Laszlo Molnar

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


#ifndef __UPX_EXCEPT_H
#define __UPX_EXCEPT_H

#ifdef __cplusplus

const char *prettyName(const char *n);
const char *prettyName(const type_info &ti);


/*************************************************************************
// exceptions
**************************************************************************/

class Throwable : public exception
{
protected:
    Throwable(const char *m = 0, int e = 0, bool w = false)
        : msg(m), err(e), is_warning(w) { }
public:
    virtual ~Throwable() { }
    const char *getMsg() const { return msg; }
    int getErrno() const { return err; }
    bool isWarning() const { return is_warning; }
private:
    //Throwable(const Throwable &);
private:
//  void * operator new(size_t);        // ...
    const char *msg;
    int err;
protected:
    bool is_warning;                    // can be set by subclasses
};


// Exceptions can/should be caught
class Exception : public Throwable
{
    typedef Throwable super;
public:
    Exception(const char *m = 0, int e = 0, bool w = false) : super(m,e,w) { }
};


// Errors should not be caught (or re-thrown)
class Error : public Throwable
{
    typedef Throwable super;
public:
    Error(const char *m = 0, int e = 0) : super(m,e) { }
};


/*************************************************************************
// system exception
**************************************************************************/

class OutOfMemoryException : public Exception
{
    typedef Exception super;
public:
    OutOfMemoryException(const char *m = 0, int e = 0) : super(m,e) { }
};


class IOException : public Exception
{
    typedef Exception super;
public:
    IOException(const char *m = 0, int e = 0) : super(m,e) { }
};


class EOFException : public IOException
{
    typedef IOException super;
public:
    EOFException(const char *m = 0, int e = 0) : super(m,e) { }
};


class FileNotFoundException : public IOException
{
    typedef IOException super;
public:
    FileNotFoundException(const char *m = 0, int e = 0) : super(m,e) { }
};


class FileAlreadyExistsException : public IOException
{
    typedef IOException super;
public:
    FileAlreadyExistsException(const char *m = 0, int e = 0) : super(m,e) { }
};


/*************************************************************************
// application execptions
**************************************************************************/

class OverlayException : public Exception
{
    typedef Exception super;
public:
    OverlayException(const char *m = 0, bool w = false) : super(m,0,w) { }
};

class CantPackException : public Exception
{
    typedef Exception super;
public:
    CantPackException(const char *m = 0, bool w = false) : super(m,0,w) { }
};

class UnknownExecutableFormatException : public CantPackException
{
    typedef CantPackException super;
public:
    UnknownExecutableFormatException(const char *m = 0, bool w = false) : super(m,w) { }
};

class AlreadyPackedException : public CantPackException
{
    typedef CantPackException super;
public:
    AlreadyPackedException(const char *m = 0) : super(m) { is_warning = true; }
};

class NotCompressibleException : public CantPackException
{
    typedef CantPackException super;
public:
    NotCompressibleException(const char *m = 0) : super(m) { }
};


class CantUnpackException : public Exception
{
    typedef Exception super;
public:
    CantUnpackException(const char *m = 0, bool w = false) : super(m,0,w) { }
};

class NotPackedException : public CantUnpackException
{
    typedef CantUnpackException super;
public:
    NotPackedException(const char *m = 0) : super(m,true) { }
};


/*************************************************************************
// errors
**************************************************************************/

class InternalError : public Error
{
    typedef Error super;
public:
    InternalError(const char *m = 0) : super(m,0) { }
};


/*************************************************************************
// util
**************************************************************************/

#undef NORET
#if 0 && defined(__GNUC__)
// (noreturn) is probably not the correct semantics for throwing exceptions
#define NORET __attribute__((noreturn))
#else
#define NORET
#endif

void throwCantPack(const char *msg) NORET;
void throwUnknownExecutableFormat(const char *msg = 0, bool warn = false) NORET;
void throwNotCompressible(const char *msg = 0) NORET;
void throwAlreadyPacked(const char *msg = 0) NORET;
void throwAlreadyPackedByUPX(const char *msg = 0) NORET;
void throwCantUnpack(const char *msg) NORET;
void throwNotPacked(const char *msg = 0) NORET;
void throwFilterException() NORET;
void throwBadLoader() NORET;
void throwChecksumError() NORET;
void throwCompressedDataViolation() NORET;
void throwInternalError(const char *msg) NORET;
void throwIOException(const char *msg = 0, int e = 0) NORET;
void throwEOFException(const char *msg = 0, int e = 0) NORET;

#undef NORET


#endif /* __cplusplus */

#endif /* already included */


/*
vi:ts=4:et
*/

