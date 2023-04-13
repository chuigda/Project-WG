import sys
from bin_rw import read_binseq
from compdec import decompress

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("usage: python tckdcmprs.py tck-file", file=sys.stderr)

    try:
        with open(sys.argv[1], "rb") as f_in, open(sys.argv[1] + ".dec", "w") as f_out:
            while True:
                data = read_binseq(f_in)
                if data is None:
                    break
                data = decompress(data)
                f_out.write(data + "\n")
    except Exception as e:
        print(" *** ERROR: %s: %s" % (e.__class__.__name__, e), file=sys.stderr)
        sys.exit(1)
