# hyperbeam
Environment for linking to PMD: Rescue Team DX executable and implementing hooks.

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

## Starlight Contributors
- [3096](https://github.com/3096)
- [khang06](https://github.com/khang06)
- [OatmealDome](https://github.com/OatmealDome)
- [Random0666](https://github.com/random0666)
- [shadowninja108](https://github.com/shadowninja108)
- [shibbo](https://github.com/shibbo) - Repo based on their work on OdysseyReversed
- [Thog](https://github.com/Thog) - Expertise in how rtld is implemented

## Credits
- djkaty - Il2CppInspector (used to generate headers and extract function offsets)
- brycewithfiveunderscores - [Starlight-SMO-Example](https://github.com/brycewithfiveunderscores/Starlight-SMO-Example)
- devkitA64
- libnx - switch build rules
