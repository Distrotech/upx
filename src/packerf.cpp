/* packerf.cpp -- packer filter handling

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


#include "conf.h"
#include "packer.h"
#include "filter.h"


/*************************************************************************
// filter util
**************************************************************************/

bool Packer::isValidFilter(int filter_id) const
{
    if (!Filter::isValidFilter(filter_id))
        return false;
    if (filter_id == 0)
        return true;
    for (const int *f = getFilters(); f && *f >= 0; f++)
    {
        if (*f == filter_id)
            return true;
    }
    return false;
}


void Packer::tryFilters(Filter *ft, upx_byte *buf, unsigned buf_len,
                        unsigned addvalue) const
{
    // debug
    //scanFilters(ft, buf, buf_len, addvalue);

    ft->init();
    if (opt->filter == 0)
        return;
    for (const int *f = getFilters(); f && *f >= 0; f++)
    {
        if (*f == 0)        // skip no-filter
            continue;
        if (opt->filter < 0 || *f == opt->filter)
        {
            ft->init(*f, addvalue);
            optimizeFilter(ft, buf, buf_len);
            if (ft->filter(buf, buf_len) && ft->calls > 0)
                break;                      // success
            ft->init();
        }
    }
}


void Packer::scanFilters(Filter *ft, const upx_byte *buf, unsigned buf_len,
                         unsigned addvalue) const
{
    ft->init();
    if (opt->filter == 0)
        return;
    for (const int *f = getFilters(); f && *f >= 0; f++)
    {
        if (*f == 0)        // skip no-filter
            continue;
        ft->init(*f, addvalue);
        //static const int ctos[] = { 0xff, 0xfe, 0x80, 0x22, -1 };
        //ft->preferred_ctos = ctos;
        if (ft->scan(buf, buf_len))
        {
            printf("scanFilters: id 0x%02x size: %6d: calls %5d/%5d/%3d, cto 0x%02x\n",
                   ft->id, ft->buf_len, ft->calls, ft->noncalls, ft->wrongcalls, ft->cto);
        }
        ft->init();
    }
}


/*************************************************************************
// addFilter32
**************************************************************************/

#define NOFILT 0  // no filter
#define FNOMRU 1  // filter, but not using mru
#define MRUFLT 2  // mru filter

static inline unsigned f80_call(int fid)
{
    return (1+ (0x0f & fid)) % 3;
}

static inline unsigned f80_jmp1(int fid)
{
    return ((1+ (0x0f & fid)) / 3) % 3;
}

static inline unsigned f80_jcc2(int fid)
{
    return f80_jmp1(fid);
}


void Packer::addFilter32(int filter_id)
{
    assert(filter_id > 0);
    assert(isValidFilter(filter_id));

    if (filter_id < 0x80) {
        if ((filter_id & 0xf) % 3 == 0) {
            if (filter_id < 0x40) {
                addLoader("CALLTR00",
                        (filter_id > 0x20) ? "CTCLEVE1" : "",
                        "CALLTR01",
                        (filter_id & 0xf) > 3 ? (filter_id > 0x20 ? "CTBSHR01""CTBSWA01" : "CTBROR01""CTBSWA01") : "",
                        "CALLTR02",
                        NULL
                        );
            }
            else if (0x40==(0xF0 & filter_id)) {
                addLoader("CKLLTR00", 0);
                if (9<=(0xf & filter_id)) {
                    addLoader("CKLLTR10", 0);
                }
                addLoader("CKLLTR20", 0);
                if (9<=(0xf & filter_id)) {
                    addLoader("CKLLTR30", 0);
                }
                addLoader("CKLLTR40", 0);
            }
        }
        else
            addLoader("CALLTR10",
                    (filter_id & 0xf) % 3  == 1 ? "CALLTRE8" : "CALLTRE9",
                    "CALLTR11",
                    (filter_id > 0x20) ? "CTCLEVE2" : "",
                    "CALLTR12",
                    (filter_id & 0xf) > 3 ? (filter_id > 0x20 ? "CTBSHR11""CTBSWA11" : "CTBROR11""CTBSWA11") : "",
                    "CALLTR13",
                    NULL
                    );
    }
    if (0x80==(filter_id & 0xF0)) {
        bool const x386 = (opt->cpu <= opt->CPU_386);
        unsigned const n_mru = ph.n_mru ? 1+ ph.n_mru : 0;
        bool const mrupwr2 = (0!=n_mru) && 0==((n_mru -1) & n_mru);
        unsigned const f_call = f80_call(filter_id);
        unsigned const f_jmp1 = f80_jmp1(filter_id);
        unsigned const f_jcc2 = f80_jcc2(filter_id);

        if (NOFILT!=f_jcc2) {
                addLoader("LXJCC010", 0);
                if (n_mru) {
                    addLoader("LXMRU045", 0);
                }
                else {
                    addLoader("LXMRU046", 0);
                }
                if (0==n_mru || MRUFLT!=f_jcc2) {
                    addLoader("LXJCC020", 0);
                }
                else { // 0!=n_mru
                    addLoader("LXJCC021", 0);
                }
                if (NOFILT!=f_jcc2) {
                    addLoader("LXJCC023", 0);
                }
        }
        addLoader("LXUNF037", 0);
        if (x386) {
            if (n_mru) {
                addLoader("LXUNF386", 0);
            }
            addLoader("LXUNF387", 0);
            if (n_mru) {
                addLoader("LXUNF388", 0);
            }
        }
        else {
            addLoader("LXUNF486", 0);
            if (n_mru) {
                addLoader("LXUNF487", 0);
            }
        }
        if (n_mru) {
            addLoader("LXMRU065", 0);
            if (256==n_mru) {
                addLoader("MRUBYTE3", 0);
            }
            else {
                addLoader("MRUARB30", 0);
                if (mrupwr2) {
                    addLoader("MRUBITS3", 0);
                }
                else {
                    addLoader("MRUARB40", 0);
                }
            }
            addLoader("LXMRU070", 0);
            if (256==n_mru) {
                addLoader("MRUBYTE4", 0);
            }
            else if (mrupwr2) {
                addLoader("MRUBITS4", 0);
            }
            else {
                addLoader("MRUARB50", 0);
            }
            addLoader("LXMRU080", 0);
            if (256==n_mru) {
                addLoader("MRUBYTE5", 0);
            }
            else {
                addLoader("MRUARB60", 0);
                if (mrupwr2) {
                    addLoader("MRUBITS5", 0);
                }
                else {
                    addLoader("MRUARB70", 0);
                }
            }
            addLoader("LXMRU090", 0);
            if (256==n_mru) {
                addLoader("MRUBYTE6", 0);
            }
            else {
                addLoader("MRUARB80", 0);
                if (mrupwr2) {
                        addLoader("MRUBITS6", 0);
                }
                else {
                    addLoader("MRUARB90", 0);
                }
            }
            addLoader("LXMRU100", 0);
        }
        addLoader("LXUNF040", 0);
        if (n_mru) {
            addLoader("LXMRU110", 0);
        }
        else {
            addLoader("LXMRU111", 0);
        }
        addLoader("LXUNF041", 0);
        addLoader("LXUNF042", 0);
        if (n_mru) {
            addLoader("LXMRU010", 0);
            if (NOFILT!=f_jmp1 && NOFILT==f_call) {
                addLoader("LXJMPA00", 0);
            }
            else {
                addLoader("LXCALLB0", 0);
            }
            addLoader("LXUNF021", 0);
        }
        else {
            addLoader("LXMRU022", 0);
            if (NOFILT!=f_jmp1 && NOFILT==f_call) {
                addLoader("LXJMPA01", 0);
            }
            else {
                addLoader("LXCALLB1", 0);
            }
        }
        if (n_mru) {
            if (256!=n_mru && mrupwr2) {
                addLoader("MRUBITS1", 0);
            }
            addLoader("LXMRU030", 0);
            if (256==n_mru) {
                addLoader("MRUBYTE1", 0);
            }
            else {
                addLoader("MRUARB10", 0);
            }
            addLoader("LXMRU040", 0);
        }

        addLoader("LXUNF030", 0);
        if (NOFILT!=f_jcc2) {
            addLoader("LXJCC000", 0);
        }
        if (NOFILT!=f_call || NOFILT!=f_jmp1) { // at least one is filtered
            // shift opcode origin to zero
            if (0==n_mru) {
                addLoader("LXCJ0MRU", 0);
            }
            else {
                addLoader("LXCJ1MRU", 0);
            }

            // determine if in range
            if ((NOFILT!=f_call) && (NOFILT!=f_jmp1)) { // unfilter both
                addLoader("LXCALJMP", 0);
            }
            if ((NOFILT==f_call) ^  (NOFILT==f_jmp1)) { // unfilter just one
                if (0==n_mru) {
                    addLoader("LXCALL00", 0);
                }
                else {
                    addLoader("LXCALL01", 0);
                }
            }

            // determine if mru applies
            if (0==n_mru || ! ((FNOMRU==f_call) || (FNOMRU==f_jmp1)) ) {
                addLoader("LXCJ2MRU", 0);  // no mru, or no exceptions
            }
            else {  // mru on one, but not the other
                addLoader("LXCJ4MRU", 0);
                if (MRUFLT==f_jmp1) { // JMP only
                    addLoader("LXCJ6MRU", 0);
                } else
                if (MRUFLT==f_call) { // CALL only
                    addLoader("LXCJ7MRU", 0);
                }
                addLoader("LXCJ8MRU", 0);
            }
        }
        addLoader("LXUNF034", 0);
        if (n_mru) {
            addLoader("LXMRU055", 0);
            if (256==n_mru) {
                addLoader("MRUBYTE2", 0);
            }
            else if (mrupwr2) {
                addLoader("MRUBITS2", 0);
            }
            else if (n_mru) {
                addLoader("MRUARB20", 0);
            }
            addLoader("LXMRU057", 0);
        }
    }
}

#undef NOFILT
#undef FNOMRU
#undef MRUFLT


/*************************************************************************
// patchFilter32
**************************************************************************/

bool Packer::patchFilter32(void *loader, int lsize, const Filter *ft)
{
    if (ft->id == 0)
        return false;
    assert(ft->calls > 0);

    if (ft->id < 0x80) {
        if (0x40 <= ft->id && ft->id < 0x50
        && (  UPX_F_LINUX_i386   ==ph.format
           || UPX_F_VMLINUX_i386 ==ph.format
           || UPX_F_VMLINUZ_i386 ==ph.format
           || UPX_F_BVMLINUZ_i386==ph.format ) ) {
            // "push byte '?'"
            patch_le16(loader, lsize, "\x6a?", 0x6a + (ft->cto << 8));
            checkPatch(NULL, 0, 0, 0);  // reset
        }
        if (0x20 <= ft->id && ft->id < 0x40) {
            // 077==modr/m of "cmp [edi], byte '?'" (compare immediate 8 bits)
            patch_le16(loader, lsize, "\077?", 077 + (ft->cto << 8));
        }
        if (ft->id < 0x40) {
            patch_le32(loader, lsize, "TEXL", (ft->id & 0xf) % 3 == 0 ? ft->calls :
                    ft->lastcall - ft->calls * 4);
        }
    }
    if (0x80==(ft->id & 0xF0)) {
        int const mru = ph.n_mru ? 1+ ph.n_mru : 0;
        if (mru && mru!=256) {
            unsigned const is_pwr2 = (0==((mru -1) & mru));
            patch_le32(0x80 + (char *)loader, lsize - 0x80, "NMRU", mru - is_pwr2);
        }
    }
    return true;
}


/*
vi:ts=4:et:nowrap
*/

