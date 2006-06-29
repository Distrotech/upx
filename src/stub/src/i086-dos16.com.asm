/*
;  l_com.asm -- loader & decompressor for the dos/com format
;
;  This file is part of the UPX executable compressor.
;
;  Copyright (C) 1996-2006 Markus Franz Xaver Johannes Oberhumer
;  Copyright (C) 1996-2006 Laszlo Molnar
;  All Rights Reserved.
;
;  UPX and the UCL library are free software; you can redistribute them
;  and/or modify them under the terms of the GNU General Public License as
;  published by the Free Software Foundation; either version 2 of
;  the License, or (at your option) any later version.
;
;  This program is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;  GNU General Public License for more details.
;
;  You should have received a copy of the GNU General Public License
;  along with this program; see the file COPYING.
;  If not, write to the Free Software Foundation, Inc.,
;  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
;
;  Markus F.X.J. Oberhumer              Laszlo Molnar
;  <mfx@users.sourceforge.net>          <ml1050@users.sourceforge.net>
;
*/

#define         COM     1
#define         CJT16   1
#include        "arch/i086/macros.ash"

                CPU     8086

/*
; =============
; ============= ENTRY POINT
; =============
*/

section         COMMAIN1
                cmp     sp, offset sp_limit
                ja      mem_ok
                int     0x20
mem_ok:
                mov     cx, offset bytes_to_copy  /* size of decomp + sizeof (data) + 1 */
                mov     si, offset copy_source    /* cx + 0x100 */
                mov     di, offset copy_destination
                mov     bx, 0x8000

                std
                rep
                movsb
                cld

                xchg    di, si
                .byte   0x83, 0xc6, COMCUTPO /* add si, xxx */
section         COMSBBBP
                sbb     bp, bp
section         COMPSHDI
                push    di
section         COMCALLT
                push    di
section         COMMAIN2
                .byte   0xe9
                .word   decompressor /* FIXME decomp_start_n2b */

#include        "include/header2.ash"

section         COMCUTPO

#include        "arch/i086/nrv2b_d16.ash"  /* decompressor & calltrick */

section         CORETURN
                ret
/*
; vi:ts=8:et:nowrap
*/
