import sys
import struct
import re

function_regex = re.compile("DO_APP_FUNC\(0x(\w+), .+?, (.+?),")
symbol_regex = re.compile("(.+) = (\w+)")

with open("include/il2cpp-functions.h", "r") as f:
	function_lines = f.readlines()

with open("linkerscripts/syms102.ld") as f:
  symbol_lines = f.readlines()

duplicate_addresses = {} # address -> array of names
symbols = {} # name -> address

for l in function_lines:
  match = function_regex.match(l)
  if match:
    addr = match.group(1)
    name = match.group(2)
    if addr in duplicate_addresses:
      duplicate_addresses[addr].append(name)
    else:
      duplicate_addresses[addr] = [name]

for l in symbol_lines:
  match = symbol_regex.match(l)
  if match:
    name = match.group(1)
    addr = match.group(2)
    symbols[name] = addr

syms_lines = []
map_lines = []

for key, names in duplicate_addresses.items():
  if len(names) > 1:
    last_name = names[-1]
    last_name_addr = symbols[last_name]
    assert(last_name_addr)

    for name in names:
      if name != last_name:
        syms_lines.append(f"{name} = {last_name_addr.lower()} - 0x5caf000;")

        padded_addr = last_name_addr.replace("0x", "").rjust(16, '0')
        map_lines.append(f" 00000004:{padded_addr}       {name}")
    
print("-------- syms ------------")
print()

for line in syms_lines:
  print(line)

print()
print("-------- maps ------------")
print()

for line in map_lines:
  print(line)
