/* ct.h -- calltrick filter

   This file is part of the UPX executable compressor.

   Copyright (C) 1996-2002 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 1996-2002 Laszlo Molnar
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



/*************************************************************************
// 16-bit calltrick ("naive")
**************************************************************************/

#define CT16(f, cond, addvalue, get, set) \
    upx_byte *b = f->buf; \
    upx_byte *b_end = b + f->buf_len - 3; \
    do { \
        if (cond) \
        { \
            b += 1; \
            unsigned a = (unsigned) (b - f->buf); \
            f->lastcall = a; \
            set(b, get(b) + (addvalue)); \
            f->calls++; \
            b += 2 - 1; \
        } \
    } while (++b < b_end); \
    if (f->lastcall) f->lastcall += 2; \
    return 0;



// filter: e8, e9, e8e9
static int f_ct16_e8(Filter *f)
{
    CT16(f, (*b == 0xe8), a + f->addvalue, get_le16, set_le16)
}

static int f_ct16_e9(Filter *f)
{
    CT16(f, (*b == 0xe9), a + f->addvalue, get_le16, set_le16)
}

static int f_ct16_e8e9(Filter *f)
{
    CT16(f, (*b == 0xe8 || *b == 0xe9), a + f->addvalue, get_le16, set_le16)
}


// unfilter: e8, e9, e8e9
static int u_ct16_e8(Filter *f)
{
    CT16(f, (*b == 0xe8), 0 - a - f->addvalue, get_le16, set_le16)
}

static int u_ct16_e9(Filter *f)
{
    CT16(f, (*b == 0xe9), 0 - a - f->addvalue, get_le16, set_le16)
}

static int u_ct16_e8e9(Filter *f)
{
    CT16(f, (*b == 0xe8 || *b == 0xe9), 0 - a - f->addvalue, get_le16, set_le16)
}


// scan: e8, e9, e8e9
static int s_ct16_e8(Filter *f)
{
    CT16(f, (*b == 0xe8), a + f->addvalue, get_le16, set_dummy)
}

static int s_ct16_e9(Filter *f)
{
    CT16(f, (*b == 0xe9), a + f->addvalue, get_le16, set_dummy)
}

static int s_ct16_e8e9(Filter *f)
{
    CT16(f, (*b == 0xe8 || *b == 0xe9), a + f->addvalue, get_le16, set_dummy)
}


// filter: e8, e9, e8e9 with bswap le->be
static int f_ct16_e8_bswap_le(Filter *f)
{
    CT16(f, (*b == 0xe8), a + f->addvalue, get_le16, set_be16)
}

static int f_ct16_e9_bswap_le(Filter *f)
{
    CT16(f, (*b == 0xe9), a + f->addvalue, get_le16, set_be16)
}

static int f_ct16_e8e9_bswap_le(Filter *f)
{
    CT16(f, (*b == 0xe8 || *b == 0xe9), a + f->addvalue, get_le16, set_be16)
}


// unfilter: e8, e9, e8e9 with bswap le->be
static int u_ct16_e8_bswap_le(Filter *f)
{
    CT16(f, (*b == 0xe8), 0 - a - f->addvalue, get_be16, set_le16)
}

static int u_ct16_e9_bswap_le(Filter *f)
{
    CT16(f, (*b == 0xe9), 0 - a - f->addvalue, get_be16, set_le16)
}

static int u_ct16_e8e9_bswap_le(Filter *f)
{
    CT16(f, (*b == 0xe8 || *b == 0xe9), 0 - a - f->addvalue, get_be16, set_le16)
}


// scan: e8, e9, e8e9 with bswap le->be
static int s_ct16_e8_bswap_le(Filter *f)
{
    CT16(f, (*b == 0xe8), a + f->addvalue, get_be16, set_dummy)
}

static int s_ct16_e9_bswap_le(Filter *f)
{
    CT16(f, (*b == 0xe9), a + f->addvalue, get_be16, set_dummy)
}

static int s_ct16_e8e9_bswap_le(Filter *f)
{
    CT16(f, (*b == 0xe8 || *b == 0xe9), a + f->addvalue, get_be16, set_dummy)
}


// filter: e8, e9, e8e9 with bswap be->le
static int f_ct16_e8_bswap_be(Filter *f)
{
    CT16(f, (*b == 0xe8), a + f->addvalue, get_be16, set_le16)
}

static int f_ct16_e9_bswap_be(Filter *f)
{
    CT16(f, (*b == 0xe9), a + f->addvalue, get_be16, set_le16)
}

static int f_ct16_e8e9_bswap_be(Filter *f)
{
    CT16(f, (*b == 0xe8 || *b == 0xe9), a + f->addvalue, get_be16, set_le16)
}


// unfilter: e8, e9, e8e9 with bswap be->le
static int u_ct16_e8_bswap_be(Filter *f)
{
    CT16(f, (*b == 0xe8), 0 - a - f->addvalue, get_le16, set_be16)
}

static int u_ct16_e9_bswap_be(Filter *f)
{
    CT16(f, (*b == 0xe9), 0 - a - f->addvalue, get_le16, set_be16)
}

static int u_ct16_e8e9_bswap_be(Filter *f)
{
    CT16(f, (*b == 0xe8 || *b == 0xe9), 0 - a - f->addvalue, get_le16, set_be16)
}


// scan: e8, e9, e8e9 with bswap be->le
static int s_ct16_e8_bswap_be(Filter *f)
{
    CT16(f, (*b == 0xe8), a + f->addvalue, get_le16, set_dummy)
}

static int s_ct16_e9_bswap_be(Filter *f)
{
    CT16(f, (*b == 0xe9), a + f->addvalue, get_le16, set_dummy)
}

static int s_ct16_e8e9_bswap_be(Filter *f)
{
    CT16(f, (*b == 0xe8 || *b == 0xe9), a + f->addvalue, get_le16, set_dummy)
}


#undef CT16


/*************************************************************************
// 32-bit calltrick ("naive")
**************************************************************************/

#define CT32(f, cond, addvalue, get, set) \
    upx_byte *b = f->buf; \
    upx_byte *b_end = b + f->buf_len - 5; \
    do { \
        if (cond) \
        { \
            b += 1; \
            unsigned a = (unsigned) (b - f->buf); \
            f->lastcall = a; \
            set(b, get(b) + (addvalue)); \
            f->calls++; \
            b += 4 - 1; \
        } \
    } while (++b < b_end); \
    if (f->lastcall) f->lastcall += 4; \
    return 0;


// filter: e8, e9, e8e9
static int f_ct32_e8(Filter *f)
{
    CT32(f, (*b == 0xe8), a + f->addvalue, get_le32, set_le32)
}

static int f_ct32_e9(Filter *f)
{
    CT32(f, (*b == 0xe9), a + f->addvalue, get_le32, set_le32)
}

static int f_ct32_e8e9(Filter *f)
{
    CT32(f, (*b == 0xe8 || *b == 0xe9), a + f->addvalue, get_le32, set_le32)
}


// unfilter: e8, e9, e8e9
static int u_ct32_e8(Filter *f)
{
    CT32(f, (*b == 0xe8), 0 - a - f->addvalue, get_le32, set_le32)
}

static int u_ct32_e9(Filter *f)
{
    CT32(f, (*b == 0xe9), 0 - a - f->addvalue, get_le32, set_le32)
}

static int u_ct32_e8e9(Filter *f)
{
    CT32(f, (*b == 0xe8 || *b == 0xe9), 0 - a - f->addvalue, get_le32, set_le32)
}


// scan: e8, e9, e8e9
static int s_ct32_e8(Filter *f)
{
    CT32(f, (*b == 0xe8), a + f->addvalue, get_le32, set_dummy)
}

static int s_ct32_e9(Filter *f)
{
    CT32(f, (*b == 0xe9), a + f->addvalue, get_le32, set_dummy)
}

static int s_ct32_e8e9(Filter *f)
{
    CT32(f, (*b == 0xe8 || *b == 0xe9), a + f->addvalue, get_le32, set_dummy)
}


// filter: e8, e9, e8e9 with bswap le->be
static int f_ct32_e8_bswap_le(Filter *f)
{
    CT32(f, (*b == 0xe8), a + f->addvalue, get_le32, set_be32)
}

static int f_ct32_e9_bswap_le(Filter *f)
{
    CT32(f, (*b == 0xe9), a + f->addvalue, get_le32, set_be32)
}

static int f_ct32_e8e9_bswap_le(Filter *f)
{
    CT32(f, (*b == 0xe8 || *b == 0xe9), a + f->addvalue, get_le32, set_be32)
}


// unfilter: e8, e9, e8e9 with bswap le->be
static int u_ct32_e8_bswap_le(Filter *f)
{
    CT32(f, (*b == 0xe8), 0 - a - f->addvalue, get_be32, set_le32)
}

static int u_ct32_e9_bswap_le(Filter *f)
{
    CT32(f, (*b == 0xe9), 0 - a - f->addvalue, get_be32, set_le32)
}

static int u_ct32_e8e9_bswap_le(Filter *f)
{
    CT32(f, (*b == 0xe8 || *b == 0xe9), 0 - a - f->addvalue, get_be32, set_le32)
}


// scan: e8, e9, e8e9 with bswap le->be
static int s_ct32_e8_bswap_le(Filter *f)
{
    CT32(f, (*b == 0xe8), a + f->addvalue, get_be32, set_dummy)
}

static int s_ct32_e9_bswap_le(Filter *f)
{
    CT32(f, (*b == 0xe9), a + f->addvalue, get_be32, set_dummy)
}

static int s_ct32_e8e9_bswap_le(Filter *f)
{
    CT32(f, (*b == 0xe8 || *b == 0xe9), a + f->addvalue, get_be32, set_dummy)
}


// filter: e8, e9, e8e9 with bswap be->le
static int f_ct32_e8_bswap_be(Filter *f)
{
    CT32(f, (*b == 0xe8), a + f->addvalue, get_be32, set_le32)
}

static int f_ct32_e9_bswap_be(Filter *f)
{
    CT32(f, (*b == 0xe9), a + f->addvalue, get_be32, set_le32)
}

static int f_ct32_e8e9_bswap_be(Filter *f)
{
    CT32(f, (*b == 0xe8 || *b == 0xe9), a + f->addvalue, get_be32, set_le32)
}


// unfilter: e8, e9, e8e9 with bswap be->le
static int u_ct32_e8_bswap_be(Filter *f)
{
    CT32(f, (*b == 0xe8), 0 - a - f->addvalue, get_le32, set_be32)
}

static int u_ct32_e9_bswap_be(Filter *f)
{
    CT32(f, (*b == 0xe9), 0 - a - f->addvalue, get_le32, set_be32)
}

static int u_ct32_e8e9_bswap_be(Filter *f)
{
    CT32(f, (*b == 0xe8 || *b == 0xe9), 0 - a - f->addvalue, get_le32, set_be32)
}


// scan: e8, e9, e8e9 with bswap be->le
static int s_ct32_e8_bswap_be(Filter *f)
{
    CT32(f, (*b == 0xe8), a + f->addvalue, get_le32, set_dummy)
}

static int s_ct32_e9_bswap_be(Filter *f)
{
    CT32(f, (*b == 0xe9), a + f->addvalue, get_le32, set_dummy)
}

static int s_ct32_e8e9_bswap_be(Filter *f)
{
    CT32(f, (*b == 0xe8 || *b == 0xe9), a + f->addvalue, get_le32, set_dummy)
}


#undef CT32


/*
vi:ts=4:et:nowrap
*/

