import sys

with open(sys.argv[1],"rb") as f:
    data = f.read()

print(",".join(("0x%02x" % d for d in data)))