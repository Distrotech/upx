/*
;  macros.ash -- macros
;
;  This file is part of the UCL data compression library.
;
;  Copyright (C) 1996-2006 Markus Franz Xaver Johannes Oberhumer
;  All Rights Reserved.
;
;  The UCL library is free software; you can redistribute it and/or
;  modify it under the terms of the GNU General Public License as
;  published by the Free Software Foundation; either version 2 of
;  the License, or (at your option) any later version.
;
;  The UCL library is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;  GNU General Public License for more details.
;
;  You should have received a copy of the GNU General Public License
;  along with the UCL library; see the file COPYING.
;  If not, write to the Free Software Foundation, Inc.,
;  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
;
;  Markus F.X.J. Oberhumer              Jens Medoch
;  <markus@oberhumer.com>               <jssg@users.sourceforge.net>
;  http://www.oberhumer.com/opensource/ucl/
;
*/

.macro  section name
        .section \name
.endm

.set    noat
.altmacro

#define zero    $0
#define at      $1
#define v0      $2
#define v1      $3
#define a0      $4
#define a1      $5
#define a2      $6
#define a3      $7
#define t0      $8
#define t1      $9
#define t2      $10
#define t3      $11
#define t4      $12
#define t5      $13
#define t6      $14
#define t7      $15
#define s0      $16
#define sp      $29
#define ra      $31

#define IF      .if
#define ELSE    .else
#define ENDIF   .endif

#define DW      .long
#define DB      .byte

.macro  subiu   reg, p1, p2
        .ifnb p2
        addiu   \reg, p1, -p2
        .else
        addiu   \reg, -p1
        .endif
.endm

