#! /usr/bin/perl -w
#
#  bin2h.pl --
#
#  This file is part of the UPX executable compressor.
#
#  Copyright (C) 1996-2002 Markus Franz Xaver Johannes Oberhumer
#  Copyright (C) 1996-2002 Laszlo Molnar
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
#  Markus F.X.J. Oberhumer              Laszlo Molnar
#  <mfx@users.sourceforge.net>          <ml1050@users.sourceforge.net>
#


use Compress::Zlib;


$delim = $/;
undef $/;       # undef input record separator - read file as a whole

$ifile = shift || die;
$ident = shift || die;
$ofile = shift || die;

$opt_q = "";
$opt_q = shift if ($#ARGV >= 0);

open(INFILE,$ifile) || die "$ifile\n";
binmode(INFILE);
open(OUTFILE,">$ofile") || die "$ofile\n";
binmode(OUTFILE);

# read whole file
$data = <INFILE>;
close(INFILE);
$n = length($data);

# print
select(OUTFILE);

$if = $ifile;
$if =~ s/.*[\/\\]//;
$of = $ofile;
$of =~ s/.*[\/\\]//;

if ($opt_q ne "-q") {
printf ("/* %s -- created from %s, %d (0x%x) bytes\n", $of, $if, $n, $n);
print <<"EOF";

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
   <mfx\@users.sourceforge.net>          <ml1050\@users.sourceforge.net>
 */


EOF
}

$s = $ident;
$s =~ tr/a-z/A-Z/;
printf("#define %s_ADLER32 0x%08x\n", $s, &adler32($data));
printf("#define %s_CRC32   0x%08x\n", $s, &crc32($data));
printf("\n");

printf("unsigned char %s[%d] = {", $ident, $n);
for ($i = 0; $i < $n; $i++) {
    if ($i % 16 == 0) {
        printf("   /* 0x%4x */", $i - 16) if $i > 0;
        print "\n";
    }
    printf("%3d", ord(substr($data, $i, 1)));
    print "," if ($i != $n - 1);
}

while (($i % 16) != 0) {
    $i++;
    print "    ";
}
printf("    /* 0x%4x */", $i - 16);

print "\n};\n";

close(OUTFILE) || die;
select(STDOUT);

undef $delim;
exit(0);


# /***********************************************************************
# //
# ************************************************************************/

sub adler32_OLD {
    local($d) = @_;
    local($n) = length($d);
    local($i);
    local($s1) = 1;
    local($s2) = 0;

    for ($i = 0; $i < $n; $i++) {
        $s1 += ord(substr($d, $i, 1));
        $s2 += $s1;
        $s1 %= 65521;
        $s2 %= 65521;
    }

    return ($s2 << 16) | $s1;
}

# vi:ts=4:et
