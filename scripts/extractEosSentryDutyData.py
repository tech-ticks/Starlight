# Generates a CSV file with data from EoS

import sys
import struct

from ndspy.rom import NintendoDSRom
from skytemple_files.common.types.file_types import FileType

SENTRY_DUTY_DATA_OFFSET_US = 0xA1BB0
GROVYLE_NUMBER = 253 # Grovyle is excluded in EoS although it has an entry
LAST_GEN3_POKEMON = 386 # RTDX only contains Pokémon up to gen 3, but there are some exceptions
ALLOWED_GEN4_POKEMON = [406, 407, 424, 429, 430, 433, 438, 439, 440, 446, 447, 448, 458, 461, 462, 463, 464, \
  465, 466, 467, 468, 469, 470, 471, 472, 473, 474, 475, 476, 477, 478]

rom = NintendoDSRom.fromFile(sys.argv[1])

arm9 = rom.loadArm9()
data = arm9.sections[0].data
monster_md = FileType.MD.deserialize(rom.getFileByName("BALANCE/monster.md"))
strings = FileType.STR.deserialize(rom.getFileByName("MESSAGE/text_e.str"))
pokemon_descriptions = strings.strings[1782:2186]

with open("pokedex_numbers_and_descriptions.csv", "w") as f:
  for i in range(0, 101):
    pokemon_id, = struct.unpack_from('h', data, SENTRY_DUTY_DATA_OFFSET_US + i * 2)
    pokedex_num = monster_md.get_by_index(pokemon_id).national_pokedex_number

    if (pokedex_num > LAST_GEN3_POKEMON or pokedex_num == GROVYLE_NUMBER) and not pokedex_num in ALLOWED_GEN4_POKEMON:
      continue

    descriptions = pokemon_descriptions[i*4:i*4 + 4]
    line = str(pokedex_num) + ";" + ";".join(descriptions)
    # Get rid of newlines in descriptions
    line = line.replace("\n", " ")
    f.write(line + "\n")
