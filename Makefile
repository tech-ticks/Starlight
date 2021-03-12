# TODO (Khangaroo): Make this process a lot less hacky (no, export did not work)
# See MakefileNSO

.PHONY: all clean starlight send

S2VER ?= 102
S2VERSTR ?= 1.0.2

all: starlight files

starlight:
	$(MAKE) all -f nso.mk S2VER=$(S2VER) S2VERSTR=$(S2VERSTR)
	$(MAKE) starlight_patch_$(S2VER)/*.ips

starlight_patch_$(S2VER)/*.ips: patches/*.slpatch patches/configs/$(S2VER).config patches/maps/$(S2VER)/*.map \
								build$(S2VER)/$(shell basename $(CURDIR))$(S2VER).map scripts/genPatch.py
	@rm -f starlight_patch_$(S2VER)/*.ips
	python3 scripts/genPatch.py $(S2VER)

files:
	mkdir -p custom_data
	python3 scripts/generateCustomFiles.py

send: all
	python3 scripts/sendPatch.py $(IP) $(S2VER) $(USER) $(PW)

listen: all send
	python3 scripts/logClient.py

clean:
	$(MAKE) clean -f nso.mk
	@rm -fr starlight_patch_*
