/* p_w16ne.h --

   This file is part of the UPX executable compressor.

   Copyright (C) 1996-2004 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 1996-2004 Laszlo Molnar
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


#ifndef __UPX_P_W16NE_H
#define __UPX_P_W16NE_H


/*************************************************************************
// win16/ne
**************************************************************************/

class PackW16Ne : public Packer
{
    typedef Packer super;
public:
    PackW16Ne(InputFile *f);
    virtual int getVersion() const { return 13; }
    virtual int getFormat() const { return UPX_F_WIN16_NE; }
    virtual const char *getName() const { return "win16/ne"; }
    virtual const int *getCompressionMethods(int method, int level) const;
    virtual const int *getFilters() const;

    virtual void pack(OutputFile *fo);
    virtual void unpack(OutputFile *fo);

    virtual bool canPack();
    virtual int canUnpack();

protected:
    virtual int readFileHeader(void);

    virtual int buildLoader(const Filter *ft);
};


#endif /* already included */


/*
vi:ts=4:et
*/

