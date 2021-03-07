# The built module crashes because the libnx build scripts do not support
# the required "offset to runtime-generated module object" field
# (see https://switchbrew.org/wiki/NSO and https://github.com/switchbrew/libnx/blob/master/nx/source/runtime/switch_crt0.s#L91)
# This script manually sets it to the start of crt0.

import sys
import struct

with open(sys.argv[1], "r+b") as f:
  f.seek(0)
  bytes = f.read(4)

  # Find the MOD0 header
  if bytes == b"":
    sys.exit(1)

  while bytes != b"MOD0":
     bytes = f.read(4)

  f.read(4)

  # Get the offset of crt0
  crt0Offset = int.from_bytes(f.read(4), byteorder='little')

  # Write to offset of runtime-generated module object
  f.seek(f.tell() + 12)
  f.write(struct.pack('i', crt0Offset))
