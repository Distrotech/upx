/* help.cpp --

   This file is part of the UPX executable compressor.

   Copyright (C) 1996-2000 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 1996-2000 Laszlo Molnar
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
#include "version.h"


/*************************************************************************
//
**************************************************************************/

static bool head_done = 0;

void show_head(void)
{
    FILE *f = con_term;
    int fg;

    if (head_done)
        return;
    head_done = 1;

    fg = con_fg(f,FG_GREEN);
    con_fprintf(f,
                "                     Ultimate Packer for eXecutables\n"
                "               Copyright (C) 1996, 1997, 1998, 1999, 2000\n"
                "UPX v%-12sMarkus F.X.J. Oberhumer & Laszlo Molnar%21s\n\n",
                UPX_VERSION_STRING, UPX_VERSION_DATE);
    fg = con_fg(f,fg);
}


/*************************************************************************
//
**************************************************************************/

void show_usage(void)
{
    FILE *f = con_term;

    con_fprintf(f,"Usage: %s [-123456788dlthVL] [-qvfk] [-o file] %sfile..\n", progname,
#if defined(__DJGPP__) || defined(__EMX__)
                "[@]");
#else
                "");
#endif
}


/*************************************************************************
//
**************************************************************************/

void show_help(int x)
{
    FILE *f = con_term;
    int fg;

    show_head();
    show_usage();

    fg = con_fg(f,FG_YELLOW);
    con_fprintf(f,"\nCommands:\n");
    fg = con_fg(f,fg);
    con_fprintf(f,
                "  -1     compress faster                   -9    compress better\n"
                "%s"
                "  -d     decompress                        -l    list compressed file\n"
                "  -t     test compressed file              -V    display version number\n"
                "  -h     give %s help                    -L    display software license\n%s",
                x == 0 ? "" : "  --best compress best (can be very slow for big files)\n",
                x == 0 ? "more" : "this", x == 0 ? "" : "\n");

    fg = con_fg(f,FG_YELLOW);
    con_fprintf(f,"Options:\n");
    fg = con_fg(f,fg);

    con_fprintf(f,
                "  -q     be quiet                          -v    be verbose\n"
                //"  -oFILE write output to `FILE'            -c    write output to stdout\n"
                "  -oFILE write output to `FILE'\n"
                //"  -f     force overwrite of output files and compression of suspicious files\n"
                "  -f     force compression of suspicious files\n"
                "%s%s"
                , (x == 0) ? "  -k     keep backup files\n" : ""
#if 1
                , (x > 0) ? "  --no-color, --mono, --color, --no-progress   change look\n" : ""
#else
                , ""
#endif
                );

    if (x > 0)
    {
        fg = con_fg(f,FG_YELLOW);
        con_fprintf(f,"\nBackup options:\n");
        fg = con_fg(f,fg);
        con_fprintf(f,
                    "  -k, --backup        keep backup files\n"
                    "  --no-backup         no backup files [default]\n"
                    "\n");
        fg = con_fg(f,FG_YELLOW);
        con_fprintf(f,"Overlay options:\n");
        fg = con_fg(f,fg);
        con_fprintf(f,
                    "  --overlay=copy      copy any extra data attached to the file [default]\n"
                    "  --overlay=strip     strip any extra data attached to the file [dangerous]\n"
                    "  --overlay=skip      don't compress a file with an overlay\n"
                    "\n");
        fg = con_fg(f,FG_YELLOW);
        con_fprintf(f,"Options for djgpp2/coff:\n");
        fg = con_fg(f,fg);
        con_fprintf(f,
                    "  --coff              produce COFF output [default: EXE]\n"
                    "\n");
        fg = con_fg(f,FG_YELLOW);
        con_fprintf(f,"Options for dos/com:\n");
        fg = con_fg(f,fg);
        con_fprintf(f,
                    "  --8086              make compressed com work on any 8086\n"
                    "\n");
        fg = con_fg(f,FG_YELLOW);
        con_fprintf(f,"Options for dos/exe:\n");
        fg = con_fg(f,fg);
        con_fprintf(f,
                    "  --8086              make compressed exe work on any 8086\n"
                    "  --no-reloc          put no relocations in to the exe header\n"
                    "\n");
        fg = con_fg(f,FG_YELLOW);
        con_fprintf(f,"Options for dos/sys:\n");
        fg = con_fg(f,fg);
        con_fprintf(f,
                    "  --8086              make compressed sys work on any 8086\n"
                    "\n");
        fg = con_fg(f,FG_YELLOW);
        con_fprintf(f,"Options for watcom/le:\n");
        fg = con_fg(f,fg);
        con_fprintf(f,
                    "  --le                produce LE output [default: EXE]\n"
                    "\n");
        fg = con_fg(f,FG_YELLOW);
        con_fprintf(f,"Options for win32/pe & rtm32/pe:\n");
        fg = con_fg(f,fg);
        con_fprintf(f,
                    "  --compress-exports=0    do not compress the export section\n"
                    "  --compress-exports=1    compress the export section [default]\n"
                    "  --compress-icons=0      do not compress any icons\n"
                    "  --compress-icons=1      compress all but the first icon\n"
                    "  --compress-icons=2      compress all but the first icon directory [default]\n"
                    "  --compress-resources=0  do not compress any resources at all\n"
                    "  --strip-relocs=0        do not strip relocations\n"
                    "  --strip-relocs=1        strip relocations [default]\n"
                    "\n");
#if 0
        fg = con_fg(f,FG_YELLOW);
        con_fprintf(f,"Options for linux/386\n");
        fg = con_fg(f,fg);
        con_fprintf(f,
                    "  --script             use /usr/local/lib/upx/upx[bd] as decompressor\n"
                    "  --script=/path/upxX  use path/upxX as decompressor\n"
                    "\n");
#endif
    }

    con_fprintf(f,
                "  file.. executables to (de)compress\n"
                "\n"
                "This version supports:\n    "
                "atari/tos, "
                "bvmlinuz/386, "
                "djgpp2/coff, "
                "dos/com, "
                "dos/exe, "
                "dos/sys,\n    "
                "linux/386, "
                "linux/elf386, "
                "linux/sh386, "
                "rtm32/pe, "
                "tmt/adam, "
                "vmlinuz/386,\n    "
                "watcom/le, "
                "win32/pe"
                "\n\nUPX comes with ABSOLUTELY NO WARRANTY; for details visit http://upx.tsx.org\n"
                //"\n\nUPX comes with ABSOLUTELY NO WARRANTY; for details type `upx -L'.\n"
                "");


#if defined(DEBUG) || defined(TESTING)
    fg = con_fg(f,FG_RED);
    con_fprintf(f,"\nWARNING: this version is compiled with"
#if defined(DEBUG)
                " -DDEBUG"
#endif
#if defined(TESTING)
                " -DTESTING"
#endif
                "\n");
    fg = con_fg(f,fg);
#endif
}


/*************************************************************************
//
**************************************************************************/

void show_license(void)
{
    FILE *f = con_term;

    show_head();

con_fprintf(f,
        "   This program may be used freely, and you are welcome to\n"
        "   redistribute it under certain conditions.\n"
        "\n"
        "   This program is distributed in the hope that it will be useful,\n"
        "   but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
        "   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
        "   UPX License Agreement for more details.\n"
        "\n"
        "   You should have received a copy of the UPX License Agreement\n"
        "   along with this program; see the file LICENSE.\n"
        "   If not, visit one of the following pages:\n"
        "\n"
    );
    int fg = con_fg(f,FG_CYAN);
con_fprintf(f,
        "        http://upx.tsx.org\n"
        "        http://wildsau.idv.uni-linz.ac.at/mfx/upx.html\n"
    );
    (void)con_fg(f,FG_ORANGE);
con_fprintf(f,
        "\n"
        "   Markus F.X.J. Oberhumer                   Laszlo Molnar\n"
        "   markus.oberhumer@jk.uni-linz.ac.at        ml1050@cdata.tvnet.hu\n"
    );
    fg = con_fg(f,fg);
}


/*************************************************************************
//
**************************************************************************/

void show_version(int x)
{
    FILE *f = stdout;
    UNUSED(x);

    fprintf(f,"upx %s\n",UPX_VERSION_STRING);
#if defined(WITH_NRV)
    fprintf(f,"NRV data compression library %s\n", nrv_version_string());
#endif
#if defined(WITH_UCL)
    fprintf(f,"UCL data compression library %s\n", ucl_version_string());
#endif
    fprintf(f,"Copyright (C) 1996,1997,1998,1999,2000 Markus Franz Xaver Johannes Oberhumer\n");
    fprintf(f,"Copyright (C) 1996,1997,1998,1999,2000 Laszlo Molnar\n");
    fprintf(f,"Copyright (C) 2000 John F. Reiser\n");
    fprintf(f,"UPX comes with ABSOLUTELY NO WARRANTY; for details type `%s -L'.\n", progname);
}


/*
vi:ts=4:et:nowrap
*/

