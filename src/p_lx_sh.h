/* p_lx_sh.h --

   This file is part of the UPX executable compressor.

   Copyright (C) 1996-2000 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 1996-2000 Laszlo Molnar
   Copyright (C) 2000 John F. Reiser.  All rights reserved.

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

   John F. Reiser
   jreiser@BitWagon.com
 */


#ifndef __UPX_P_LX_SH_H  //{
#define __UPX_P_LX_SH_H

class PackLinuxI386sh : public PackLinuxI386
{
    typedef PackLinuxI386 super;
public:
    PackLinuxI386sh(InputFile *f);
    virtual ~PackLinuxI386sh();
    virtual int getVersion() const { return 11; }
    virtual int getFormat() const { return UPX_F_LINUX_SH_i386; }
    virtual const char *getName() const { return "linux/sh386"; }
    virtual const int *getFilters() const { return NULL; }

    virtual bool canPack();
    virtual void pack(OutputFile *fo);
    // virtual void unpack(OutputFile *fo) { super::unpack(fo); }

    virtual bool canUnpackFormat(int format) const;
    virtual bool canUnpackVersion(int version) const
        { return (version >= 11); }

protected:
    virtual const upx_byte *getLoader() const;
    virtual int getLoaderSize() const;
    virtual bool getShellName(char *buf);

    virtual void patchLoader();

    Elf_LE32_Ehdr *ehdri; // from input file

    int o_shname;  // offset to name_of_shell
    int l_shname;  // length of name_of_shell
};

#endif //}__UPX_P_LX_SH_H


/*
vi:ts=4:et
*/

