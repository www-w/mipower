all: irtest.bin

irtest.bin:build/irtest.ihx
	makebin -p build/irtest.ihx irtest.bin
#objcopy -I ihex -O binary build/irtest.hex irtest.bin

#build/irtest.hex:build/irtest.ihx
#	packihx build/irtest.ihx > build/irtest.hex

build/irtest.ihx:irtest.c
	sdcc -o build/ irtest.c

clean:
	rm irtest.bin
	rm build/*
