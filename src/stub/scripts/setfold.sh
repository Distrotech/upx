#! /bin/sh
set -e

#
#  setfold.sh --
#
#  This file is part of the UPX executable compressor.
#
#  Copyright (C) 1996-2001 Markus Franz Xaver Johannes Oberhumer
#  Copyright (C) 1996-2001 Laszlo Molnar
#  All Rights Reserved.
#
#  UPX and the UCL library are free software; you can redistribute them
#  and/or modify them under the terms of the GNU General Public License as
#  published by the Free Software Foundation; either version 2 of
#  the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; see the file COPYING.
#  If not, write to the Free Software Foundation, Inc.,
#  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
#  Markus F.X.J. Oberhumer   Laszlo Molnar
#  markus@oberhumer.com      ml1050@cdata.tvnet.hu
#


file="$1"

# get directory of this script
bindir=`echo "$0" | sed -e 's|[^/][^/]*$||'`
bindir=`cd "$bindir" && pwd`

sstrip="./util/sstrip/sstrip"
test -x "$sstrip" || sstrip="$bindir/../util/sstrip/sstrip"

# get address of symbol "fold_begin"
fold=`nm -f bsd "$file" | grep fold_begin | sed 's/^0*\([0-9a-fA-F]*\).*/0x\1/'`

# strip
objcopy -S -R .comment -R .note "$file"
"$sstrip" "$file"

# patch address
perl -w "$bindir/setfold.pl" "$file" "$fold"

exit 0

