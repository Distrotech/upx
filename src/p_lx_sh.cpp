/* p_lx_sh.cpp --

   This file is part of the UPX executable compressor.

   Copyright (C) 1996-2003 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 1996-2003 Laszlo Molnar
   Copyright (C) 2000-2003 John F. Reiser
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

   Markus F.X.J. Oberhumer   Laszlo Molnar           John F. Reiser
   markus@oberhumer.com      ml1050@cdata.tvnet.hu   jreiser@BitWagon.com
 */


#include "conf.h"

#include "file.h"
#include "filter.h"
#include "packer.h"
#include "p_elf.h"
#include "p_unix.h"
#include "p_lx_exc.h"
#include "p_lx_sh.h"

#define PT_LOAD     Elf_LE32_Phdr::PT_LOAD


/*************************************************************************
//
**************************************************************************/

static const
#include "stub/l_lx_sh86.h"
static const
#include "stub/fold_sh86.h"


PackLinuxI386sh::PackLinuxI386sh(InputFile *f) :
    super(f), o_shname(0), l_shname(0)
{
}

PackLinuxI386sh::~PackLinuxI386sh()
{
}

int
PackLinuxI386sh::buildLoader(Filter const *ft)
{
    unsigned const sz_fold = sizeof(linux_i386sh_fold);
    MemBuffer buf(sz_fold);
    memcpy(buf, linux_i386sh_fold, sz_fold);

    checkPatch(NULL, 0, 0, 0);  // reset
    patch_le32(buf,sz_fold,"UPX3",l_shname);
    patch_le32(buf,sz_fold,"UPX2",o_shname);

    // get fresh filter
    Filter fold_ft = *ft;
    fold_ft.init(ft->id, ft->addvalue);
    int preferred_ctos[2] = {ft->cto, -1};
    fold_ft.preferred_ctos = preferred_ctos;

    // filter
    optimizeFilter(&fold_ft, buf, sz_fold);
    bool success = fold_ft.filter(buf + sizeof(cprElfHdr2), sz_fold - sizeof(cprElfHdr2));
    UNUSED(success);

    return buildLinuxLoader(
        linux_i386sh_loader, sizeof(linux_i386sh_loader),
        buf, sz_fold, ft );
}

void PackLinuxI386sh::patchLoader() { }


bool PackLinuxI386sh::getShellName(char *buf)
{
    exetype = -1;
    l_shname = strcspn(buf, " \t\n\v\f\r");
    buf[l_shname] = 0;
    static char const *const shname[] = { // known shells that accept "-c" arg
        "ash", "bash", "bsh", "csh", "ksh", "pdksh", "sh", "tcsh", "zsh",
        //"python", // FIXME: does python work ???
        NULL
    };
    const char *bname = strrchr(buf, '/');
    if (bname == NULL)
        return false;
    for (int j = 0; NULL != shname[j]; ++j) {
        if (0 == strcmp(shname[j], bname + 1)) {
            return super::canPack();
        }
    }
    return false;
}


bool PackLinuxI386sh::canPack()
{
#if defined(__linux__)  //{
    // only compress i386sh scripts when running under Linux
    char buf[512];

    fi->readx(buf, sizeof(buf));
    fi->seek(0, SEEK_SET);
    buf[sizeof(buf) - 1] = 0;
    if (!memcmp(buf, "#!/", 3)) {                       // #!/bin/sh
        o_shname = 2;
        return getShellName(&buf[o_shname]);
    }
    else if (!memcmp(buf, "#! /", 4)) {                 // #! /bin/sh
        o_shname = 3;
        return getShellName(&buf[o_shname]);
    }
#endif  //}
    return false;
}


void
PackLinuxI386sh::pack1(OutputFile *fo, Filter &)
{
    generateElfHdr(fo, linux_i386sh_fold, 0x08048000);
}

/*
vi:ts=4:et
*/

