#! /usr/bin/env python
## vim:set ts=4 sw=4 et: -*- coding: utf-8 -*-
#
#  bin2h.py --
#
#  This file is part of the UPX executable compressor.
#
#  Copyright (C) 1996-2006 Markus Franz Xaver Johannes Oberhumer
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


import getopt, os, re, struct, sys, zlib


class opts:
    dry_run = 0
    ident = None
    methods = [ 0 ]
    mname = "STUB_COMPRESS_METHOD"
    mode = "c"
    verbose = 0


# /***********************************************************************
# // write header
# ************************************************************************/

def w_header_c(w, ifile, ofile, n):
    w("/* %s\n   created from %s, %d (0x%x) bytes\n" % (os.path.basename(ofile), os.path.basename(ifile), n, n))
    w("""\n\
   This file is part of the UPX executable compressor.

   Copyright (C) 1996-2006 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 1996-2006 Laszlo Molnar
   Copyright (C) 2000-2006 John F. Reiser
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
 */\n\n\n""")


# /***********************************************************************
# // write data
# ************************************************************************/

def w_checksum_c(w, s, data):
    w("#define %s_SIZE    %d\n"     % (s, len(data)))
    w("#define %s_ADLER32 0x%08x\n" % (s, 0xffffffffL & zlib.adler32(data)))
    w("#define %s_CRC32   0x%08x\n" % (s, 0xffffffffL & zlib.crc32(data)))
    w("\n")


def w_data_c(w, data):
    def w_eol(w, i):
        if i > 0:
            w("   /* 0x%4x */" % (i - 16))
            w("\n")

    n = len(data)
    for i in range(n):
        if i % 16 == 0:
            w_eol(w, i)
        w("%3d" % ord(data[i]))
        w(", " [i == n - 1])
    i = n
    while i % 16 != 0:
        w("    ")
        i += 1
    w_eol(w, i)


def w_data_gas(w, data):
    def w_eol(w, i):
        if i > 0:
            w("   /* 0x%04x */" % (i - 16))
            w("\n")

    n = len(data)
    for i in range(n):
        if i % 16 == 0:
            w_eol(w, i)
            w(".byte ")
        else:
            w(",")
        w("%3d" % ord(data[i]))
    i = n
    while i % 16 != 0:
        w("    ")
        i += 1
    w_eol(w, i)


def w_data_nasm(w, data):
    def w_eol(w, i):
        if i > 0:
            w("   ; 0x%04x" % (i - 16))
            w("\n")

    n = len(data)
    for i in range(n):
        if i % 16 == 0:
            w_eol(w, i)
            w("db ")
        else:
            w(",")
        w("%3d" % ord(data[i]))
    i = n
    while i % 16 != 0:
        w("    ")
        i += 1
    w_eol(w, i)


def encode_compressed_stub_header(method, idata, odata):
    assert 0 < method <= 255
    if len(idata) <= 65535:
        h = "UPX#" + struct.pack("<BHH", method, len(idata), len(odata))
        assert len(h) == 9
    else:
        h = "UPX#" + "\x00" + struct.pack("<BII", method, len(idata), len(odata))
        assert len(h) == 14
    ##assert len(h) + len(odata) < len(idata), ("stub compression failed", len(h), len(odata), len(idata))
    return h


# /***********************************************************************
# //
# ************************************************************************/

def write_stub(w, idata, method_index):
    method = opts.methods[method_index]
    # compress
    if method == 0:
        odata = idata
    elif method == 14: # M_LZMA:
        import pylzma
        odata = pylzma.compress(idata, eos=0)
        ## FIXME: internal pylzma-0.3.0 error
        ##assert pylzma.decompress(odata, maxlength=len(idata)) == idata
        # recode lzma-header
        prop = ord(odata[0])
        pb = (prop / 9) / 5; lp = (prop / 9) % 5; lc = prop % 9
        h = chr(((lc + lp) << 3) | pb) + chr((lp << 4) | lc)
        odata = h + odata[5:]
        # encode upx stub header
        odata = encode_compressed_stub_header(method, idata, odata) + odata
    elif method == 15: # M_DEFLATE:
        odata = zlib.compress(idata, 9)
        # strip zlib-header and zlib-trailer (adler32)
        odata = odata[2:-4]
        assert zlib.decompress(odata, -15) == idata
        # encode upx stub header
        odata = encode_compressed_stub_header(method, idata, odata) + odata
    else:
        raise Exception, ("invalid method", method, opts.methods)
    if 0 and len(odata) >= len(idata):
        odata = idata
        method = 0
    assert len(odata) <= len(idata), "stub compression failed"

    if len(opts.methods) > 1:
        if method_index == 0:
            w("#if (%s == %d)\n\n" % (opts.mname, method))
        elif method_index < len(opts.methods) - 1:
            w("\n#elif (%s == %d)\n\n" % (opts.mname, method))
        else:
            w("\n#else\n\n")

    if opts.ident:
        if opts.mode == "c":
            w_checksum_c(w, opts.ident.upper(), odata)
            w("unsigned char %s[%d] = {\n" % (opts.ident, len(odata)))
    if opts.mode == "c":
        w_data_c(w, odata)
    elif opts.mode == "gas":
        w_data_gas(w, odata)
    elif opts.mode == "nasm":
        w_data_nasm(w, odata)
    else:
        assert 0, opts.mode
    if opts.ident:
        if opts.mode == "c":
            w("};\n")

    if len(opts.methods) > 1:
        if method_index == len(opts.methods) - 1:
            w("\n#endif\n")


# /***********************************************************************
# // main
# ************************************************************************/

def main(argv):
    shortopts, longopts = "qv", [
        "compress=", "dry-run", "ident=", "mode=", "quiet", "verbose"
    ]
    xopts, args = getopt.gnu_getopt(argv[1:], shortopts, longopts)
    for opt, optarg in xopts:
        if 0: pass
        elif opt in ["-q", "--quiet"]: opts.verbose = opts.verbose - 1
        elif opt in ["-v", "--verbose"]: opts.verbose = opts.verbose + 1
        elif opt in ["--compress"]: opts.methods = map(int, optarg.split(","))
        elif opt in ["--dry-run"]: opts.dry_run = opts.dry_run + 1
        elif opt in ["--ident"]: opts.ident = optarg
        elif opt in ["--mode"]: opts.mode = optarg.lower()
        else: assert 0, ("getopt problem:", opt, optarg, xopts, args)

    assert len(args) == 2
    ifile = args[0]
    ofile = args[1]

    # check file size
    st = os.stat(ifile)
    if 1 and st.st_size <= 0:
        print >> sys.stderr, "%s: ERROR: emtpy file" % (ifile)
        sys.exit(1)
    if 1 and st.st_size > 128*1024:
        print >> sys.stderr, "%s: ERROR: file is too big (%d bytes)" % (ifile, st.st_size)
        sys.exit(1)

    # read ifile
    ifile = os.path.normpath(ifile)
    ifp = open(ifile, "rb")
    idata = ifp.read()
    ifp.close()
    assert len(idata) == st.st_size

    # opts.ident
    if opts.ident in ["auto", "auto-stub"]:
        s = os.path.basename(ifile)
        s = re.sub(r"\.(bin|out)$", "", s)
        s = re.sub(r"[-.]", "_", s)
        if opts.ident in ["auto-stub"]:
            s = "stub_"  + s
        opts.ident = s
    if opts.ident:
        assert re.search(r"^[a-zA-Z]", opts.ident), opts.ident
        assert not re.search(r"[^a-zA-Z0-9_]", opts.ident), opts.ident

    # write ofile
    if opts.dry_run:
        ofp = None
        def dummy_write(s): pass
        w = dummy_write
    else:
        if ofile == "-":
            ofp = sys.stdout
        else:
            ofp = open(ofile, "wb")
        w = ofp.write
    if opts.verbose >= 0:
        if opts.mode == "c":
            w_header_c(w, ifile, ofile, len(idata))
    assert len(opts.methods) >= 1
    for i in range(len(opts.methods)):
        write_stub(w, idata, i)
    if ofp:
        if ofp is sys.stdout:
            ofp.flush()
        else:
            ofp.close()


if __name__ == "__main__":
    sys.exit(main(sys.argv))

