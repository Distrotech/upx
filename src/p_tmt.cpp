/* p_tmt.cpp --

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

   Markus F.X.J. Oberhumer                   Laszlo Molnar
   markus.oberhumer@jk.uni-linz.ac.at        ml1050@cdata.tvnet.hu
 */

#include "conf.h"
#include "file.h"
#include "filter.h"
#include "packer.h"
#include "p_tmt.h"

static const
#include "stub/l_tmt.h"

#define EXTRA_INFO      4       // original entry point


/*************************************************************************
//
**************************************************************************/

PackTmt::PackTmt(InputFile *f) : super(f)
{
    COMPILE_TIME_ASSERT(sizeof(tmt_header_t) == 44);
}


const int *PackTmt::getCompressionMethods(int method, int level) const
{
    static const int m_nrv2b[] = { M_NRV2B_LE32, M_NRV2D_LE32, -1 };
    static const int m_nrv2d[] = { M_NRV2D_LE32, M_NRV2B_LE32, -1 };

    if (M_IS_NRV2B(method))
        return m_nrv2b;
    if (M_IS_NRV2D(method))
        return m_nrv2d;
    if (level == 1 || file_size <= 512*1024)
        return m_nrv2b;
    return m_nrv2d;
}


const int *PackTmt::getFilters() const
{
    static const int filters[] = { 0x26, 0x24, 0x11, 0x14, 0x13, 0x16,
                                   0x25, 0x12, 0x15, -1 };
    return filters;
}


unsigned PackTmt::findOverlapOverhead(const upx_bytep buf,
                                      unsigned range,
                                      unsigned upper_limit) const
{
    // make sure the decompressor will be paragraph aligned
    unsigned o = super::findOverlapOverhead(buf, range, upper_limit);
    o = ((o + 0x20) &~ 0xf) - (ph.u_len & 0xf);
    return o;
}


int PackTmt::buildLoader(const Filter *ft)
{
    // prepare loader
    initLoader(nrv_loader,sizeof(nrv_loader));
    addLoader("IDENTSTR""TMTMAIN1",
              ft->id ? "TMTCALT1" : "",
              "TMTMAIN2""UPX1HEAD""TMTCUTPO""+0XXXXXX",
              getDecompressor(),
              "TMTMAIN5",
              NULL
             );
    if (ft->id)
    {
        assert(ft->calls > 0);
        addLoader("TMTCALT2",NULL);
        addFilter32(ft->id);
    }
    addLoader("TMTRELOC""RELOC320",
              big_relocs ? "REL32BIG" : "",
              "RELOC32J""TMTJUMP1",
              NULL
             );
    return getLoaderSize();
}


/*************************************************************************
//
**************************************************************************/

int PackTmt::readFileHeader()
{
#define H(x)  get_le16(h,2*(x))
#define H4(x) get_le32(h,x)
    unsigned char h[0x40];
    int ic;
    unsigned exe_offset = 0;
    adam_offset = 0;

    for (ic = 0; ic < 20; ic++)
    {
        fi->seek(adam_offset,SEEK_SET);
        fi->readx(h,sizeof(h));

        if (memcmp(h,"MZ",2) == 0) // dos exe
        {
            exe_offset = adam_offset;
            adam_offset += H(2)*512+H(1);
            if (H(1))
                adam_offset -= 512;
            if (H(0x18/2) == 0x40 && H4(0x3c))
                adam_offset = H4(0x3c);
        }
        else if (memcmp(h,"BW",2) == 0)
            adam_offset += H(2)*512+H(1);
        else if (memcmp(h,"PMW1",4) == 0)
        {
            fi->seek(adam_offset + H4(0x18),SEEK_SET);
            adam_offset += H4(0x24);
            int objs = H4(0x1c);
            while (objs--)
            {
                fi->readx(h,0x18);
                adam_offset += H4(4);
            }
        }
        else if (memcmp(h,"LE",2) == 0)
        {
            // + (memory_pages-1)*memory_page_size+bytes_on_last_page
            unsigned offs = exe_offset + (H4(0x14) - 1) * H4(0x28) + H4(0x2c);
            fi->seek(adam_offset+0x80,SEEK_SET);
            fi->readx(h,4);
            // + data_pages_offset
            adam_offset = offs + H4(0);
        }
        else if (memcmp(h,"Adam",4) == 0)
            break;
        else
            return 0;
    }
    if (ic == 20)
        return 0;

    fi->seek(adam_offset,SEEK_SET);
    fi->readx(&ih,sizeof(ih));
    // FIXME: should add some checks for the values in `ih'

    return UPX_F_TMT_ADAM;
#undef H4
#undef H
}


bool PackTmt::canPack()
{
    if (!readFileHeader())
        return false;
    return true;
}


/*************************************************************************
//
**************************************************************************/

void PackTmt::pack(OutputFile *fo)
{
    big_relocs = 0;

    Packer::handleStub(fi,fo,adam_offset);

    const unsigned usize = ih.imagesize;
    const unsigned rsize = ih.relocsize;

    ibuf.alloc(usize+rsize+128);
    obuf.allocForCompression(usize+rsize+128);

    MemBuffer wrkmem;
    wrkmem.alloc(rsize+EXTRA_INFO); // relocations

    fi->seek(adam_offset+sizeof(ih),SEEK_SET);
    fi->readx(ibuf,usize);
    fi->readx(wrkmem+4,rsize);
    const unsigned overlay = file_size - fi->tell();

    if (find_le32(ibuf,128,get_le32("UPX ")) >= 0)
        throwAlreadyPacked();
    if (rsize == 0)
        throwCantPack("file is already compressed with another packer");

    checkOverlay(overlay);

    unsigned relocsize = 0;
    //if (rsize)
    {
        for (unsigned ic=4; ic<=rsize; ic+=4)
            set_le32(wrkmem+ic,get_le32(wrkmem+ic)-4);
        relocsize = optimizeReloc32(wrkmem+4,rsize/4,wrkmem,ibuf,1,&big_relocs)- wrkmem;
    }

    wrkmem[relocsize++] = 0;
    set_le32(wrkmem+relocsize,ih.entry); // save original entry point
    relocsize += 4;
    set_le32(wrkmem+relocsize,relocsize+4);
    relocsize += 4;
    memcpy(ibuf+usize,wrkmem,relocsize);

    // prepare packheader
    ph.u_len = usize + relocsize;
    // prepare filter
    Filter ft(ph.level);
    ft.buf_len = usize;
    // compress
    compressWithFilters(&ft, 512);

    const unsigned lsize = getLoaderSize();
    MemBuffer loader(lsize);
    memcpy(loader,getLoader(),lsize);

    const unsigned s_point = getLoaderSection("TMTMAIN1");
    int e_len = getLoaderSectionStart("TMTCUTPO");
    const unsigned d_len = lsize - e_len;
    assert(e_len > 0  && s_point > 0);

    // patch loader
    patch_le32(loader,lsize,"JMPO",ih.entry-(ph.u_len+ph.overlap_overhead+d_len));
    patchFilter32(loader, lsize, &ft);
    patchPackHeader(loader,e_len);

    const unsigned jmp_pos = find_le32(loader,e_len,get_le32("JMPD"));
    patch_le32(loader,e_len,"JMPD",ph.u_len+ph.overlap_overhead-jmp_pos-4);

    patch_le32(loader,e_len,"ECX0",ph.c_len+d_len);
    patch_le32(loader,e_len,"EDI0",ph.u_len+ph.overlap_overhead+d_len-1);
    patch_le32(loader,e_len,"ESI0",ph.c_len+e_len+d_len-1);
    //fprintf(stderr,"\nelen=%x dlen=%x copy_len=%x  copy_to=%x  oo=%x  jmp_pos=%x  ulen=%x  clen=%x \n\n",
    //                e_len,d_len,copy_len,copy_to,ph.overlap_overhead,jmp_pos,ph.u_len,ph.c_len);

    memcpy(&oh,&ih,sizeof(oh));
    oh.imagesize = ph.c_len+e_len+d_len; // new size
    oh.entry = s_point; // new entry point
    oh.relocsize = 4;

    // write loader + compressed file
    fo->write(&oh,sizeof(oh));
    fo->write(loader,e_len);
    fo->write(obuf,ph.c_len);
    fo->write(loader+lsize-d_len,d_len); // decompressor
    char rel_entry[4];
    set_le32(rel_entry,5 + s_point);
    fo->write(rel_entry,sizeof (rel_entry));

    // verify
    verifyOverlappingDecompression(&obuf, ph.overlap_overhead);

    // copy the overlay
    copyOverlay(fo, overlay, &obuf);

    // finally check the compression ratio
    if (!checkFinalCompressionRatio(fo))
        throwNotCompressible();
}


int PackTmt::canUnpack()
{
    if (!PackTmt::readFileHeader())
        return false;
    fi->seek(adam_offset, SEEK_SET);
    return readPackHeader(512) ? 1 : -1;
}


void PackTmt::unpack(OutputFile *fo)
{
    Packer::handleStub(fi,fo,adam_offset);

    ibuf.alloc(ph.c_len);
    obuf.allocForUncompression(ph.u_len);

    fi->seek(adam_offset + ph.buf_offset + ph.getPackHeaderSize(),SEEK_SET);
    fi->readx(ibuf,ph.c_len);

    // decompress
    decompress(ibuf,obuf);

    // decode relocations
    const unsigned osize = ph.u_len - get_le32(obuf+ph.u_len-4);
    upx_byte *relocs = obuf + osize;
    const unsigned origstart = get_le32(obuf+ph.u_len-8);

    // unfilter
    if (ph.filter)
    {
        Filter ft(ph.level);
        ft.init(ph.filter, 0);
        ft.cto = (unsigned char) (ph.version < 11 ? (get_le32(obuf+ph.u_len-12) >> 24) : ph.filter_cto);
        ft.unfilter(obuf, relocs-obuf);
    }

    // decode relocations
    MemBuffer wrkmem;
    unsigned relocn = unoptimizeReloc32(&relocs,obuf,&wrkmem,1);
    for (unsigned ic = 0; ic < relocn; ic++)
        set_le32(wrkmem+ic*4,get_le32(wrkmem+ic*4)+4);

    memcpy(&oh,&ih,sizeof(oh));
    oh.imagesize = osize;
    oh.entry = origstart;
    oh.relocsize = relocn*4;

    const unsigned overlay = file_size - adam_offset - ih.imagesize
        - ih.relocsize - sizeof(ih);
    checkOverlay(overlay);

    // write decompressed file
    if (fo)
    {
        fo->write(&oh,sizeof(oh));
        fo->write(obuf,osize);
        fo->write(wrkmem,relocn*4);
    }

    // copy the overlay
    copyOverlay(fo, overlay, &obuf);
}


/*
vi:ts=4:et
*/

