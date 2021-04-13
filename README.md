# Sentry Duty minigame

This branch contains code for the Sentry Duty minigame implementation. For instructions on how to build this,
see the hyperbeam README below.

This ROM hack was possible thanks to [Sky Editor Deluxe](https://github.com/tech-ticks/SkyEditor.RomEditor.Rtdx) and [SkyTemple](https://github.com/SkyTemple) contributors. Special thanks to 
[irdkwia](https://github.com/irdkwia) who found the location of the Sentry Duty data in Explorers of Sky.

## Generating custom data files

Generating the data files is optional since the generated files are provided in the `data` directory. To build the files in `data/custom_data` manually, install [skytemple-files](https://pypi.org/project/skytemple-files/), [ndspy](https://pypi.org/project/ndspy/) and [Sky Editor Deluxe](https://github.com/tech-ticks/SkyEditor.RomEditor.Rtdx).

Run `python scripts/extractEosSentryDutyData.py [path to EoS rom]` to generate a .csv file with data from EoS, then run `[path to Sky Editor Deluxe .dll] [path to directory with RTDX romfs and exefs] scripts/GenerateBinaryFiles.csx`. The base file `data/custom_data/town_npc_database.json` can be generated with a code hook at runtime (see `hookGroundManagerOnEnable` in `main.cpp`). 

The version in this project was edited to include Loudred in every story scenario. Loudred was added to `m02_010` with an empty `refStepSymbol` and to all the other story scenarios with a `refStepSymbol` of `m02_010`, talking to him will always execute the script at `data/native_data/script/event/usual/talk_action_gr/talk_gr@m02_010.lua`.

## Unity asset files

`UnityAssets.unitypackage` contains all the Unity asset files that were used to generate the custom asset bundles inside `data/ab`.

# hyperbeam
Environment for linking to the PMD: Rescue Team DX executable and implementing hooks.

## Setup
Install Python 3, [devkitPro](https://devkitpro.org/) and run `pip install keystone-engine`.

## Examples
Examples for manipulating party members are included, see `source/main.cpp` and `patches/codehook.slpatch`

## Building
- Make sure that the `DEVKITPRO` environment variable is set to the path where devkitPro is installed.
- Run `make`.
- Copy the built `Starlight102.nso` to `/atmosphere/contents/01003D200BAA2000/exefs/subsdk1`
- Copy `starlight_patch_102/3AB632DEE82D59448599B2291F30994A.ips` to `/atmosphere/exefs_patches/starlight_patch_102/3AB632DEE82D59448599B2291F30994A.ips`
(if you're using Yuzu, copy the .ips to the `exefs` directory inside your mod directory, see their [game modding guide](https://yuzu-emu.org/help/feature/game-modding/)).

Alternatively, run `IP=[Switch IP] USER=[FTP User] PW=[FTP password] make listen` to build the 
NSO, send the built files to your Switch via FTP and connect the TCP logger.

The files in `custom_data` can be generated with [Sky Editor Deluxe](https://github.com/tech-ticks/SkyEditor.RomEditor.Rtdx).

## Starlight Contributors
- [3096](https://github.com/3096)
- [khang06](https://github.com/khang06)
- [OatmealDome](https://github.com/OatmealDome)
- [Random0666](https://github.com/random0666)
- [shadowninja108](https://github.com/shadowninja108)
- [shibbo](https://github.com/shibbo) - Repo based on their work on OdysseyReversed
- [Thog](https://github.com/Thog) - Expertise in how rtld is implemented

## Credits
- [Skyline](https://github.com/skyline-dev/skyline) contributors - Additional headers, logging
- djkaty - Il2CppInspector (used to generate headers and extract function offsets)
- brycewithfiveunderscores - [Starlight-SMO-Example](https://github.com/brycewithfiveunderscores/Starlight-SMO-Example)
- devkitA64
- libnx - switch build rules
