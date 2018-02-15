# This Makefile was generated by the made script:
# https://github.com/alx741/made
#
# Replace FILE with your C code filename
# Replace PARTNO with the target chip model
#
# Dependencies:
#	SDCC
#   stm8flash


FILE=main
MMCU=stm8
PARTNO=stm8s103f3	# list valid parts with `stm8flash -l`
PROGRAMMER=stlinkv2

CFLAGS =
CFLAGS += -l$(MMCU)
CFLAGS += -m$(MMCU)
CFLAGS += --out-fmt-ihx


$(FILE).ihx: $(FILE).c
	/home/alx/sdcc/bin/sdcc $(CFLAGS) $< -o $@

clean:
	rm -f $(FILE).map $(FILE).rst $(FILE).ihx $(FILE).cdb $(FILE).asm $(FILE).rel $(FILE).lst $(FILE).lk $(FILE).sym $(FILE).ihx

burn: $(FILE).ihx
	stm8flash -p$(PARTNO) -c$(PROGRAMMER) -w $(FILE).ihx
