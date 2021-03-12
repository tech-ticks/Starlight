# Generates an fixed_organization.bin for loading with the custom code.

import sys
import struct

# Extra Pokémon types
TYPE_GUEST = 0
TYPE_WANDERER = 1
TYPE_HELPER_WANDERER = 2

# Warehouse IDs
ID_HERO = 0
ID_PARTNER = 1
ID_ABSOL = 2
ID_GENGAR = 3
ID_ZAPDOS = 10
ID_MEW = 19
ID_ENTEI = 21
ID_SUICUNE = 22
ID_RAIKOU = 23
ID_JIRACHI = 28
ID_GARDEVOIR = 31
ID_CELEBI = 32
ID_NULL = 0xFFFF

LOCKED = True
UNLOCKED = False

ORGANIZATIONS = {
  "FREE":                 { "members": [(ID_NULL, UNLOCKED), (ID_NULL, UNLOCKED),  (ID_NULL, UNLOCKED)], "guests": []},
  "HERO_PARTNER":         { "members": [(ID_HERO, LOCKED),   (ID_PARTNER, LOCKED), (ID_NULL, LOCKED)],   "guests": []},
  "HERO_PARTNER_MEMBER":  { "members": [(ID_HERO, LOCKED),   (ID_PARTNER, LOCKED), (ID_NULL, UNLOCKED)], "guests": []},
  "HERO_PARTNER_ABUSORU": { "members": [(ID_HERO, LOCKED),   (ID_PARTNER, LOCKED), (ID_ABSOL, LOCKED)],  "guests": []},
  "EVENT_GENGAA": {
    "members": [(ID_HERO, LOCKED), (ID_PARTNER, LOCKED), (ID_NULL, LOCKED)],
    "guests": [(ID_GENGAR, TYPE_GUEST)]
  },
  "MAX":                  { "members": [(ID_NULL, UNLOCKED), (ID_NULL, UNLOCKED),  (ID_NULL, UNLOCKED)], "guests": []},
  "TRAINIG":              { "members": [(ID_NULL, UNLOCKED), (ID_NULL, UNLOCKED),  (ID_NULL, UNLOCKED)], "guests": []}, # sic

  # Add custom teams here
  "CUSTOM_EXAMPLE": {
    "members": [(ID_JIRACHI, LOCKED), (ID_GARDEVOIR, LOCKED), (ID_PARTNER, UNLOCKED)],
    "guests": [
      (ID_ZAPDOS, TYPE_GUEST),
      (ID_CELEBI, TYPE_WANDERER),
      (ID_MEW, TYPE_HELPER_WANDERER),
      (ID_ENTEI, TYPE_GUEST),
      (ID_SUICUNE, TYPE_GUEST)
    ]
  }
}

with open("custom_data/fixed_organization.bin", "wb") as f:
  entry_num = len(ORGANIZATIONS)

  f.write(struct.pack("I", entry_num))
  for name, organization in ORGANIZATIONS.items():
    # Write name as 32 byte fixed-length string
    if (len(name) > 32):
      raise Exception(f"Organization name '{name}' too long")
    f.write(struct.pack("32s", name.encode("ascii")))

    # Write members
    if len(organization["members"]) != 3:
      raise Exception("Number of members must be 3")

    for index, locked in organization["members"]:
      f.write(struct.pack("H", index))
      f.write(struct.pack("?", locked))
      f.write(struct.pack("x")) # Padding

    # Write guests
    if len(organization["guests"]) > 8:
      raise Exception("Number of guests must be <= 8")

    for index, guest_type in organization["guests"]:
      f.write(struct.pack("H", index))
      f.write(struct.pack("H", guest_type))
      
    # Fill the other slots with up to 8 with dummy values
    for i in range(len(organization["guests"]), 8):
      f.write(struct.pack("H", ID_NULL))
      f.write(struct.pack("H", 0))
