;  fold_exec86.asm -- linkage to C code to process Elf binary
;
;  This file is part of the UPX executable compressor.
;
;  Copyright (C) 2000-2003 John F. Reiser
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
;  John F. Reiser
;  <jreiser@users.sourceforge.net>
;


                BITS    32
                SECTION .text

;; control just falls through, after this part and compiled C code
;; are uncompressed.

%define szElf32_Ehdr 0x34
%define szElf32_Phdr 8*4
%define e_entry  (16 + 2*2 + 4)
%define p_vaddr  2*4
%define p_memsz  5*4
%define szl_info 12
%define szp_info 12

fold_begin:  ; enter: %ebx= &Elf32_Ehdr of this program
        pop eax  ; discard &dstlen
        pop eax  ; discard  dstlen

                pop     eax                 ; Pop the argument count
                mov     ecx, esp            ; argv starts just at the current stack top
                lea     edx, [esp+eax*4+4]  ; envp = &argv[argc + 1]
                mov esi, [e_entry + ebx]
                add ebx, szElf32_Ehdr + szElf32_Phdr + szl_info
                sub esi, ebx  ; length
                lea edi, [2 + ebp]  ; f_unfilter, maybe
                pusha  ; (f_unf, cprLen, f_decpr, xx, cprSrc, envp, argv, argc)
EXTERN upx_main
                call    upx_main            ; Call the UPX main function
                hlt                         ; Crash if somehow upx_main does return

; vi:ts=8:et:nowrap
