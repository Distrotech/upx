/* mem.cpp --

   This file is part of the UPX executable compressor.

   Copyright (C) 1996-2003 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 1996-2003 Laszlo Molnar
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

   Markus F.X.J. Oberhumer              Laszlo Molnar
   <mfx@users.sourceforge.net>          <ml1050@users.sourceforge.net>
 */


#include "conf.h"
#include "mem.h"


/*************************************************************************
//
**************************************************************************/

MemBuffer::MemBuffer(unsigned size) :
    ptr(NULL), alloc_ptr(NULL), alloc_size(0)
{
    if (size > 0)
        alloc(size, 0);
}


MemBuffer::~MemBuffer()
{
    this->dealloc();
}

void MemBuffer::dealloc()
{
    if (alloc_ptr)
        ::free(alloc_ptr);
    alloc_ptr = ptr = NULL;
    alloc_size = 0;
}


unsigned MemBuffer::getSize() const
{
    if (!alloc_ptr)
        return 0;
    unsigned size = alloc_size - (ptr - alloc_ptr);
    assert((int)size > 0);
    return size;
}


void MemBuffer::alloc(unsigned size, unsigned base_offset)
{
    // NOTE: we don't automaticlly free a used buffer
    assert(alloc_ptr == NULL);
    assert((int)size > 0);
    size = base_offset + size;
    alloc_ptr = (unsigned char *) malloc(size);
    if (!alloc_ptr)
    {
        //throw bad_alloc();
        throwCantPack("out of memory");
        //exit(1);
    }
    alloc_size = size;
    ptr = alloc_ptr + base_offset;
}


void MemBuffer::alloc(unsigned size)
{
    alloc(size, 0);
}


void MemBuffer::allocForCompression(unsigned uncompressed_size)
{
    assert((int)uncompressed_size > 0);
    alloc(uncompressed_size + uncompressed_size/8 + 256, 0);
}


void MemBuffer::allocForUncompression(unsigned uncompressed_size)
{
    assert((int)uncompressed_size > 0);
    //alloc(uncompressed_size + 3 + 512, 0);  // 512 safety bytes
    alloc(uncompressed_size + 3, 0);  // 3 bytes for asm_fast decompresion
}


/*
vi:ts=4:et
*/

